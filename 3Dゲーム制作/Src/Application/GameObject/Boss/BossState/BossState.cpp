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
	float dt = SceneManager::Instance().GetTimeScale();

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

	// 攻撃距離なら攻撃予知へ
	auto player = owner.m_wpPlayer.lock();
	if (!player) return;

	float dist = (player->GetPos() - owner.m_nowPos).Length();

	if (dist < owner.m_attackDist)
	{
		owner.stateMachine->ChangeState(
			std::make_unique<BossStatePreAttack>()
		);

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
// 攻撃予知ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStatePreAttack::Enter(BossBase& owner)
{
	owner.m_preAttackActive = true;
	owner.m_preAttackAlpha = 1.0f;
	owner.m_preAttackTimer = 0.0f;
	owner.m_preAttackPos =
		owner.m_nowPos + Math::Vector3(0, 2.0f, 0);

	auto player = owner.m_wpPlayer.lock();
	if (player)
	{
		player->m_canDodge = true;
	}
}

void BossStatePreAttack::Update(BossBase& owner)
{
	float dt = Application::Instance().GetDeltaTime() * SceneManager::Instance().GetTimeScale();

	owner.m_preAttackTimer += dt;

	// ボスの頭上に追従
	owner.m_preAttackPos =
		owner.m_nowPos + Math::Vector3(0, 2.0f, 0);

	owner.m_preAttackScale =
		1.0f + (1.0f - owner.m_preAttackAlpha) * 0.5f;

	// フェードアウト
	owner.m_preAttackAlpha -= dt * 2.0f;

	if (owner.m_preAttackAlpha < 0.0f)
	{
		owner.m_preAttackAlpha = 0.0f;
	}

	// フェードアウト完了でAttack1へ
	if (owner.m_preAttackTimer > 0.8f)
	{
		auto player = owner.m_wpPlayer.lock();

		if (player)
		{
			if (player->m_justDodgeSuccess)
			{
				owner.StartSlow(2.0f);
				player->m_justDodgeSuccess = false;
			}

			player->m_canDodge = false;
		}

		owner.m_preAttackActive = false;

		owner.stateMachine->ChangeState(
			std::make_unique<BossStateAttack1>()
		);

		return;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 攻撃1ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStateAttack1::Enter(BossBase& owner)
{
	owner.PlayAnimationAuto("", owner.animAttack1Index, false);
	owner.m_attackHitOnce = false;
}

void BossStateAttack1::Update(BossBase& owner)
{
	float t = owner.m_animator.GetAnimeCurrentTime();

	// 踏み込み
	// if (t > 20 && t < 30)
	// {
	//     Math::Vector3 f = owner.GetForward();
	//     f.Normalize();
	//     owner.m_nowPos += f * 0.05f;
	// }

	// 攻撃判定
	if (t > 15.0f && t < 25.0f)
	{
		owner.DoAttackHitCheck(owner.m_attackDist);
	}

	// 攻撃終了
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<BossStateIdle>()
		);

		return;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 攻撃2ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStateAttack2::Enter(BossBase& owner)
{
	owner.PlayAnimationAuto("", owner.animAttack2Index, false);
	owner.m_attackHitOnce = false;
}

void BossStateAttack2::Update(BossBase& owner)
{
	float t = owner.m_animator.GetAnimeCurrentTime();

	// 踏み込み
	// if (t > 20 && t < 30)
	// {
	//     Math::Vector3 f = owner.GetForward();
	//     f.Normalize();
	//     owner.m_nowPos += f * 0.05f;
	// }

	// 攻撃判定
	if (t > 15.0f && t < 25.0f)
	{
		owner.DoAttackHitCheck(owner.m_attackDist);
	}

	// 攻撃終了
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<BossStateIdle>()
		);

		return;
	}
}
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 攻撃3ステート
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void BossStateAttack3::Enter(BossBase& owner)
{
	owner.PlayAnimationAuto("", owner.animAttack3Index, false);
	owner.m_attackHitOnce = false;
}

void BossStateAttack3::Update(BossBase& owner)
{
	float t = owner.m_animator.GetAnimeCurrentTime();

	// 踏み込み
	// if (t > 20 && t < 30)
	// {
	//     Math::Vector3 f = owner.GetForward();
	//     f.Normalize();
	//     owner.m_nowPos += f * 0.05f;
	// }

	// 攻撃判定
	if (t > 15.0f && t < 25.0f)
	{
		owner.DoAttackHitCheck(owner.m_attackDist);
	}

	// 攻撃終了
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<BossStateIdle>()
		);

		return;
	}
}