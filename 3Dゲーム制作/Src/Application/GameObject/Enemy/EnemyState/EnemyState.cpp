#include"EnemyState.h"
#include <Application/GameObject/Enemy/EnemyBase.h>
#include <Application/GameObject/Player/Player/Player.h>
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

		float rotSpeed = DirectX::XMConvertToRadians(owner.m_rotationSpeedDeg);
		angle = std::clamp(angle, -rotSpeed, rotSpeed);

		owner.m_angleY += angle;
	}

	// 移動
	owner.m_nowPos += dir * owner.m_moveSpeed;
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
		owner.stateMachine->ChangeState(std::make_unique<EnemyBaseStatePreAttack>());
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
void EnemyBaseStateAttack1::Enter(EnemyBase& owner)
{
	owner.PlayAnimationAuto("", owner.animAttack1Index, false);
}

void EnemyBaseStateAttack1::Update(EnemyBase& owner)
{
	float t = owner.m_animator.GetAnimeCurrentTime();

	// 踏み込み（攻撃1専用）
	//if (t > 20 && t < 30)
	//{
	//	Math::Vector3 f = owner.GetForward();
	//	f.Normalize();
	//	owner.m_nowPos += f * 0.05f;
	//}

	// 攻撃判定（攻撃1専用）
	if (t > 15.0f && t < 25.0f)
	{
		owner.DoAttackHitCheck(owner.m_attackDist);
	}

	// 終了判定（攻撃1専用）
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.stateMachine->ChangeState(std::make_unique<EnemyBaseStateIdle>());
	}

};

void EnemyBaseStatePreAttack::Enter(EnemyBase& owner)
{
	owner.m_preAttackActive = true;
	owner.m_preAttackAlpha = 1.0f; 
	owner.m_preAttackPos = owner.m_nowPos + Math::Vector3(0, 2.0f, 0);
}

void EnemyBaseStatePreAttack::Update(EnemyBase& owner)
{
	m_time += Application::Instance().GetDeltaTime();
	if (m_time > 0.5f)
	{
		owner.m_preAttackActive = false;
		owner.m_preAttackAlpha -= Application::Instance().GetDeltaTime() * 2.0f;
		owner.m_preAttackAlpha = std::max(owner.m_preAttackAlpha, 0.0f);
		owner.stateMachine->ChangeState(std::make_unique<EnemyBaseStateAttack1>());
	}
}