#include"EnemyState.h"
#include <Application/GameObject/Enemy/EnemyBase.h>
#include <Application/GameObject/Player/Player/Player.h>
#include <Application/Scene/SceneManager.h>
#include <Application/main.h>

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 待機ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void EnemyBaseStateIdle::Enter(EnemyBase& owner)
{
	owner.PlayAnimationAuto("", owner.animIdleIndex);
}

void EnemyBaseStateIdle::Update(EnemyBase& owner)
{
	auto player = owner.m_wpPlayer.lock();
	if (!player) return;

	float dist = (player->GetPos() - owner.m_nowPos).Length();

	// プレイヤーが近づいたら移動へ
	if (dist < owner.detectRange)
	{
		owner.stateMachine->ChangeState(std::make_unique<EnemyBaseStateWalk>());
		return;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 歩行ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void EnemyBaseStateMove::Update(EnemyBase& owner)
{
	float dt = SceneManager::Instance().GetTimeScale() * owner.GetTimeScale();

	auto player = owner.m_wpPlayer.lock();
	if (!player) return;

	// プレイヤー方向
	Math::Vector3 dir = player->GetPos() - owner.m_nowPos;
	dir.y = 0;
	dir.Normalize();

	// キャラ回転処理
	{
		Math::Vector3 nowDir = owner.GetForward();
		nowDir.Normalize();

		float dot = std::clamp(nowDir.Dot(dir), -1.0f, 1.0f);
		float angle = acos(dot);

		Math::Vector3 cross = nowDir.Cross(dir);
		if (cross.y < 0) angle = -angle;

		float rotSpeed = DirectX::XMConvertToRadians(owner.m_rotationSpeedDeg) * dt;
		angle = std::clamp(angle, -rotSpeed, rotSpeed);

		owner.m_angleY += angle;
	}

	// 移動
	owner.m_nowPos += dir * owner.m_moveSpeed * dt;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 歩きステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void EnemyBaseStateWalk::Enter(EnemyBase& owner)
{
	owner.PlayAnimationAuto("", owner.animWalkIndex);
}

void EnemyBaseStateWalk::Update(EnemyBase& owner)
{
	EnemyBaseStateMove::Update(owner);

	// 攻撃距離なら攻撃予知へ
	auto player = owner.m_wpPlayer.lock();
	if (!player) return;

	float dist = (player->GetPos() - owner.m_nowPos).Length();
	if (dist < owner.m_attackDist)
	{
		owner.stateMachine->ChangeState(std::make_unique<EnemyBaseStateAttack>());
		return;
	}
}


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 走りステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void EnemyBaseStateDash::Enter(EnemyBase& owner)
{
	owner.PlayAnimationAuto("", owner.animDashIndex);
}

void EnemyBaseStateDash::Update(EnemyBase& owner)
{
	EnemyBaseStateMove::Update(owner);
}


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 攻撃系ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void EnemyBaseStateAttack::Enter(EnemyBase& owner)
{
	owner.m_isAttacking = true;

	//---------------------------------------
	// 攻撃アニメーション開始 & フラグリセット
	//---------------------------------------
	owner.PlayAnimationAuto("", owner.animAttackIndex, false);
	owner.m_attackHitOnce = false;
	owner.m_attackSEPlayed = false;

	//---------------------------------------
	// 攻撃予知開始
	//---------------------------------------
	owner.m_preAttackActive = true;
	owner.m_preAttackAlpha = 1.0f;
	owner.m_preAttackTimer = 0.0f;
	owner.m_preAttackPos = owner.m_nowPos + Math::Vector3(0, 2.0f, 0);

	//---------------------------------------
	// 回避可能
	//---------------------------------------
	auto player = owner.m_wpPlayer.lock();

	if (player)
	{
		player->m_canDodge = true;
	}
}

void EnemyBaseStateAttack::Update(EnemyBase& owner)
{
	float dt =
		Application::Instance().GetDeltaTime() *
		SceneManager::Instance().GetTimeScale() *
		owner.GetTimeScale();

	float t = owner.m_animator.GetAnimeCurrentTime();

	//---------------------------------------
	// 攻撃予知演出
	// 今までのPreAttackと同じ処理
	//---------------------------------------
	if (owner.m_preAttackActive)
	{
		owner.m_preAttackTimer += dt;

		// 敵の頭上に追従
		owner.m_preAttackPos =
			owner.m_nowPos + Math::Vector3(0, 2.0f, 0);

		//---------------------------------------
		// 攻撃判定直前まで徐々にフェードアウト
		//---------------------------------------

		const float predictionEndTime = 14.0f;

		float progress = t / predictionEndTime;

		// 0.0 ～ 1.0 に制限
		progress = std::clamp(progress, 0.0f, 1.0f);

		// 1 → 0
		owner.m_preAttackAlpha = 1.0f - progress;

		//---------------------------------------
		// Alphaに合わせて拡大
		//---------------------------------------
		owner.m_preAttackScale =
			1.0f +
			(1.0f - owner.m_preAttackAlpha) * 0.5f;
	}

	//---------------------------------------
	// ダメージ判定直前で予知終了
	//---------------------------------------
	if (t >= 14.0f && owner.m_preAttackActive)
	{
		owner.m_preAttackActive = false;

		auto player = owner.m_wpPlayer.lock();
		if (player)
		{
			player->m_canDodge = false;
		}
	}

	//---------------------------------------
	// 攻撃判定
	//---------------------------------------
	if (t > 15.0f && t < 25.0f)
	{
		//-----------------------------------
		// 攻撃SE
		//-----------------------------------
		if (!owner.m_attackSEPlayed)
		{
			if (SceneManager::Instance().GetTimeScale() >= 0.99f)
			{
				owner.m_attackSound =
					KdAudioManager::Instance().Play(
						"Asset/Sounds/SE/Attack.wav",
						SoundType::SE
					);
				owner.m_attackSEPlayed = true;
			}
		}		
		owner.DoAttackHitCheck(owner.m_attackDist);
	}

	//==============================
	// 攻撃終了
	//==============================
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.m_isAttacking = false;

		owner.StopAttackSound();
		
		//========================================
		// チュートリアル攻撃の場合
		//========================================
		if (owner.m_isTutorialAttack)
		{
			auto player = owner.m_wpPlayer.lock();

			if (player)
			{
				// 回避成功
				if (player->m_justDodgeSuccess)
				{
					owner.m_tutorialAttackFinished = true;
					owner.m_isTutorialAttack = false;

					player->m_canDodge = false;

					owner.stateMachine->ChangeState(
						std::make_unique<EnemyBaseStateIdle>()
					);

					return;
				}
			}

			//---------------------------------------
			// 回避失敗
			// → もう一度Attack1
			//---------------------------------------
			owner.stateMachine->ChangeState(
				std::make_unique<EnemyBaseStateAttack>()
			);

			return;
		}

		//========================================
		// 通常敵
		//========================================
		owner.stateMachine->ChangeState(
			std::make_unique<EnemyBaseStateIdle>()
		);
	}
};