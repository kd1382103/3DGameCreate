#include "BossState.h"
#include <Application/GameObject/Boss/BossBase.h>
#include <Application/GameObject/Player/Player/Player.h>
#include <Application/Scene/SceneManager.h>
#include <Application/main.h>

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 待機ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStateIdle::Enter(BossBase& owner)
{
	owner.PlayAnimationAuto("", owner.animIdleIndex, true);
}

void BossStateIdle::Update(BossBase& owner)
{
	auto player = owner.m_wpPlayer.lock();
	if (!player) return;

	float dist = (player->GetPos() - owner.m_nowPos).Length();

	// プレイヤーが近づいたら移動へ
	if (dist < owner.detectRange)
	{
		owner.stateMachine->ChangeState(std::make_unique<BossStateWalk>());
		return;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 移動ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStateMove::Update(BossBase& owner)
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

		float rotSpeed =
			DirectX::XMConvertToRadians(owner.m_rotationSpeedDeg) * dt;

		angle = std::clamp(angle, -rotSpeed, rotSpeed);

		owner.m_angleY += angle;
	}

	// 移動
	owner.m_nowPos += dir * owner.m_moveSpeed * dt;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 歩きステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStateWalk::Enter(BossBase& owner)
{
	owner.PlayAnimationAuto("", owner.animWalkIndex, true);
}

void BossStateWalk::Update(BossBase& owner)
{
	BossStateMove::Update(owner);

	auto player = owner.m_wpPlayer.lock();
	if (!player) return;

	float dist =
		(player->GetPos() - owner.m_nowPos).Length();

	if (dist < owner.m_attackDist)
	{
		//---------------------------------------
		// 攻撃パターンがまだ作られていない場合
		//---------------------------------------
		if (!owner.m_isComboStarted)
		{
			int pattern = rand() % 4;

			switch (pattern)
			{
			case 0:
				owner.m_attackPattern =
				{
					BossBase::BossAttackType::Attack1,
					BossBase::BossAttackType::Attack2,
					BossBase::BossAttackType::Attack3
				};
				break;

			case 1:
				owner.m_attackPattern =
				{
					BossBase::BossAttackType::Attack1,
					BossBase::BossAttackType::Attack1,
					BossBase::BossAttackType::Attack2
				};
				break;

			case 2:
				owner.m_attackPattern =
				{
					BossBase::BossAttackType::Attack2,
					BossBase::BossAttackType::Attack1,
					BossBase::BossAttackType::Attack3
				};
				break;

			case 3:
				owner.m_attackPattern =
				{
					BossBase::BossAttackType::Attack1,
					BossBase::BossAttackType::Attack3
				};
				break;
			}

			owner.m_attackPatternIndex = 0;
			owner.m_isComboStarted = true;
		}

		//---------------------------------------
		// 最初の攻撃へ
		//---------------------------------------
		switch (
			owner.m_attackPattern[
				owner.m_attackPatternIndex
			]
			)
		{
		case BossBase::BossAttackType::Attack1:
			owner.stateMachine->ChangeState(
				std::make_unique<BossStateAttack1>()
			);
			break;

		case BossBase::BossAttackType::Attack2:
			owner.stateMachine->ChangeState(
				std::make_unique<BossStateAttack2>()
			);
			break;

		case BossBase::BossAttackType::Attack3:
			owner.stateMachine->ChangeState(
				std::make_unique<BossStateAttack3>()
			);
			break;
		}
		return;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 走りステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStateDash::Enter(BossBase& owner)
{
	owner.PlayAnimationAuto("", owner.animDashIndex, true);
}

void BossStateDash::Update(BossBase& owner)
{
	BossStateMove::Update(owner);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 攻撃1ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStateAttack1::Enter(BossBase& owner)
{
	//---------------------------------------
	// 攻撃アニメーション開始
	//---------------------------------------

	owner.PlayAnimationAuto("", owner.animAttack1Index, false);
	owner.m_attackHitOnce = false;
	owner.m_attackSEPlayed = false;

	//---------------------------------------
	// 攻撃予知開始
	//---------------------------------------
	owner.m_preAttackActive = true;
	owner.m_preAttackAlpha = 1.0f;
	owner.m_preAttackTimer = 0.0f;

	owner.m_preAttackPos =
		owner.m_nowPos +
		Math::Vector3(0, 2.0f, 0);

	//---------------------------------------
	// 回避可能
	//---------------------------------------
	auto player = owner.m_wpPlayer.lock();

	if (player)
	{
		player->m_canDodge = true;
	}
}

void BossStateAttack1::Update(BossBase& owner)
{
	float dt =
		Application::Instance().GetDeltaTime() *
		SceneManager::Instance().GetTimeScale() *
		owner.GetTimeScale();

	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//---------------------------------------
	// 攻撃予知
	//---------------------------------------
	if (owner.m_preAttackActive)
	{
		owner.m_preAttackTimer += dt;

		owner.m_preAttackPos =
			owner.m_nowPos +
			Math::Vector3(0, 2.0f, 0);

		const float predictionEndTime = 14.0f;

		float progress =
			t / predictionEndTime;

		progress =
			std::clamp(
				progress,
				0.0f,
				1.0f
			);

		//---------------------------------------
		// 1 → 0
		//---------------------------------------
		owner.m_preAttackAlpha =
			1.0f - progress;

		//---------------------------------------
		// 徐々に拡大
		//---------------------------------------
		owner.m_preAttackScale =
			1.0f +
			(1.0f - owner.m_preAttackAlpha)
			* 0.5f;
	}

	if (t >= 14.0f && owner.m_preAttackActive)
	{
		//---------------------------------------
		// 予知終了
		//---------------------------------------
		owner.m_preAttackActive = false;

		auto player =
			owner.m_wpPlayer.lock();

		if (player)
		{
			//---------------------------------------
			// ジャスト回避成功
			//---------------------------------------
			if (player->m_justDodgeSuccess)
			{
				owner.StartSlow(2.0f);

				// 成功フラグをリセット
				player->m_justDodgeSuccess = false;
			}

			//---------------------------------------
			// 回避受付終了
			//---------------------------------------
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
				KdAudioManager::Instance().Play(
					"Asset/Sounds/SE/Attack.wav"
				);
			}

			owner.m_attackSEPlayed = true;
		}
		owner.DoAttackHitCheck(owner.m_attackDist);
	}

	//---------------------------------------
	// 攻撃終了
	//---------------------------------------
	if (
		owner.m_animator.IsAnimationEnd()
		)
	{
		GoNextAttack(owner);
		return;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 攻撃2ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStateAttack2::Enter(BossBase& owner)
{
	//---------------------------------------
	// 攻撃アニメーション開始
	//---------------------------------------
	owner.PlayAnimationAuto("", owner.animAttack2Index, false);
	owner.m_attackHitOnce = false;
	owner.m_attackSEPlayed = false;

	//---------------------------------------
	// 攻撃予知開始
	//---------------------------------------
	owner.m_preAttackActive = true;
	owner.m_preAttackAlpha = 1.0f;
	owner.m_preAttackTimer = 0.0f;

	owner.m_preAttackPos =
		owner.m_nowPos +
		Math::Vector3(0, 2.0f, 0);

	//---------------------------------------
	// 回避可能
	//---------------------------------------
	auto player = owner.m_wpPlayer.lock();

	if (player)
	{
		player->m_canDodge = true;
	}
}

void BossStateAttack2::Update(BossBase& owner)
{
	float dt =
		Application::Instance().GetDeltaTime() *
		SceneManager::Instance().GetTimeScale() *
		owner.GetTimeScale();

	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//---------------------------------------
	// 攻撃予知
	//---------------------------------------
	if (owner.m_preAttackActive)
	{
		owner.m_preAttackTimer += dt;

		owner.m_preAttackPos =
			owner.m_nowPos +
			Math::Vector3(0, 2.0f, 0);

		const float predictionEndTime = 14.0f;

		float progress =
			t / predictionEndTime;

		progress =
			std::clamp(
				progress,
				0.0f,
				1.0f
			);

		owner.m_preAttackAlpha =
			1.0f - progress;

		owner.m_preAttackScale =
			1.0f +
			(1.0f - owner.m_preAttackAlpha)
			* 0.5f;
	}

	if (t >= 14.0f && owner.m_preAttackActive)
	{
		//---------------------------------------
		// 予知終了
		//---------------------------------------
		owner.m_preAttackActive = false;

		auto player =
			owner.m_wpPlayer.lock();

		if (player)
		{
			//---------------------------------------
			// ジャスト回避成功
			//---------------------------------------
			if (player->m_justDodgeSuccess)
			{
				owner.StartSlow(2.0f);

				// 成功フラグをリセット
				player->m_justDodgeSuccess = false;
			}

			//---------------------------------------
			// 回避受付終了
			//---------------------------------------
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
				KdAudioManager::Instance().Play(
					"Asset/Sounds/SE/Attack.wav"
				);
			}

			owner.m_attackSEPlayed = true;
		}
		owner.DoAttackHitCheck(owner.m_attackDist);
	}

	//---------------------------------------
	// 攻撃終了
	//---------------------------------------
	if (
		owner.m_animator.IsAnimationEnd()
		)
	{
		GoNextAttack(owner);
		return;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 攻撃3ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStateAttack3::Enter(BossBase& owner)
{
	//---------------------------------------
	// 攻撃アニメーション開始
	//---------------------------------------
	owner.PlayAnimationAuto("", owner.animAttack3Index, false);
	owner.m_attackHitOnce = false;
	owner.m_attackSEPlayed = false;

	//---------------------------------------
	// 攻撃予知開始
	//---------------------------------------
	owner.m_preAttackActive = true;
	owner.m_preAttackAlpha = 1.0f;
	owner.m_preAttackTimer = 0.0f;

	owner.m_preAttackPos =
		owner.m_nowPos +
		Math::Vector3(0, 2.0f, 0);

	//---------------------------------------
	// 回避可能
	//---------------------------------------
	auto player = owner.m_wpPlayer.lock();

	if (player)
	{
		player->m_canDodge = true;
	}
}

void BossStateAttack3::Update(BossBase& owner)
{
	float dt =
		Application::Instance().GetDeltaTime() *
		SceneManager::Instance().GetTimeScale() *
		owner.GetTimeScale();

	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//---------------------------------------
	// 攻撃予知
	//---------------------------------------
	if (owner.m_preAttackActive)
	{
		owner.m_preAttackTimer += dt;

		owner.m_preAttackPos =
			owner.m_nowPos +
			Math::Vector3(0, 2.0f, 0);

		const float predictionEndTime = 14.0f;

		float progress =
			t / predictionEndTime;

		progress =
			std::clamp(
				progress,
				0.0f,
				1.0f
			);

		owner.m_preAttackAlpha =
			1.0f - progress;

		owner.m_preAttackScale =
			1.0f +
			(1.0f - owner.m_preAttackAlpha)
			* 0.5f;
	}

	if (t >= 14.0f && owner.m_preAttackActive)
	{
		//---------------------------------------
		// 予知終了
		//---------------------------------------
		owner.m_preAttackActive = false;

		auto player =
			owner.m_wpPlayer.lock();

		if (player)
		{
			//---------------------------------------
			// ジャスト回避成功
			//---------------------------------------
			if (player->m_justDodgeSuccess)
			{
				owner.StartSlow(2.0f);

				// 成功フラグをリセット
				player->m_justDodgeSuccess = false;
			}

			//---------------------------------------
			// 回避受付終了
			//---------------------------------------
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
				KdAudioManager::Instance().Play(
					"Asset/Sounds/SE/Attack.wav"
				);
			}

			owner.m_attackSEPlayed = true;
		}
		owner.DoAttackHitCheck(owner.m_attackDist);
	}

	//---------------------------------------
	// 攻撃終了
	//---------------------------------------
	if (
		owner.m_animator.IsAnimationEnd()
		)
	{
		GoNextAttack(owner);
		return;
	}
}

void BossStateAttackBase::GoNextAttack(
	BossBase& owner
)
{
	//---------------------------------------
	// 次の攻撃番号
	//---------------------------------------
	owner.m_attackPatternIndex++;

	//---------------------------------------
	// コンボ終了
	//---------------------------------------
	if (
		owner.m_attackPatternIndex >=
		static_cast<int>(
			owner.m_attackPattern.size()
			)
		)
	{
		owner.m_isComboStarted = false;

		owner.stateMachine->ChangeState(
			std::make_unique<BossStateIdle>()
		);

		return;
	}

	//---------------------------------------
	// 次の攻撃へ直接移動
	//---------------------------------------
	switch (
		owner.m_attackPattern[
			owner.m_attackPatternIndex
		]
		)
	{
	case BossBase::BossAttackType::Attack1:

		owner.stateMachine->ChangeState(
			std::make_unique<BossStateAttack1>()
		);
		break;

	case BossBase::BossAttackType::Attack2:

		owner.stateMachine->ChangeState(
			std::make_unique<BossStateAttack2>()
		);
		break;

	case BossBase::BossAttackType::Attack3:

		owner.stateMachine->ChangeState(
			std::make_unique<BossStateAttack3>()
		);
		break;
	}
}
