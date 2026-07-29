#include "PlayerState.h"
#include <Application/GameObject/Player/Player/Player.h>

//==============================================================
// Idle
//==============================================================
void PlayerStateIdle::Enter(Player& owner)
{
	owner.SetAnim(9, true); // Idle
}

void PlayerStateIdle::Update(Player& owner)
{
	if (owner.IsSkillInput() && owner.m_skillGauge >= owner.m_skillCost)
	{
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateSkill>());
		return;
	}

	if (owner.IsAttackInput())
	{
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateAttack1>());
		return;
	}

	if (owner.IsDodgeInput())
	{
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateDedge>());
		return;
	}

	if (owner.IsMoveInput())
	{
		if (owner.m_running)
			owner.stateMachine->ChangeState(std::make_unique<PlayerStateDash>());
		else
			owner.stateMachine->ChangeState(std::make_unique<PlayerStateWalk>());
		return;
	}
}

//==============================================================
// Move 基底
//==============================================================
void PlayerStateMove::Update(Player& owner)
{
	// 移動入力が消えたら Idle
	if (!owner.IsMoveInput())
	{
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateIdle>());
		return;
	}


	// スキル割り込み
	if (owner.IsSkillInput() && owner.m_skillGauge >= owner.m_skillCost)
	{
		owner.stateMachine->ChangeStateImmediate(std::make_unique<PlayerStateSkill>(), owner);
		return;
	}

	// 攻撃
	if (owner.IsAttackInput())
	{
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateAttack1>());
		return;
	}

	// 回避
	if (owner.IsDodgeInput())
	{
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateDedge>());
		return;
	}

	// キャラ回転処理（そのまま）
	{
		Math::Vector3 nowDir = owner.GetForward();
		Math::Vector3 targetDir = owner.m_dir;

		nowDir.Normalize();
		targetDir.Normalize();

		float dot = std::clamp(nowDir.Dot(targetDir), -1.0f, 1.0f);
		float angle = acos(dot);

		Math::Vector3 cross = nowDir.Cross(targetDir);
		if (cross.y < 0) angle = -angle;

		float rotSpeed = DirectX::XMConvertToRadians(owner.m_rotationSpeedDeg);
		angle = std::clamp(angle, -rotSpeed, rotSpeed);

		owner.m_angleY += angle;
	}

	// 移動
	float moveSpeed = owner.m_running ? 0.15f : 0.05f;
	owner.m_nowPos += owner.m_dir * moveSpeed;
}

//==============================================================
// Walk
//==============================================================
void PlayerStateWalk::Enter(Player& owner)
{
	owner.SetAnim(41, true); // Walk
}

void PlayerStateWalk::Update(Player& owner)
{

	if (owner.m_running)
	{
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateDash>());
		return;
	}

	PlayerStateMove::Update(owner);
}

//==============================================================
// Dash
//==============================================================
void PlayerStateDash::Enter(Player& owner)
{
	owner.SetAnim(36, true); // Dash
}

void PlayerStateDash::Update(Player& owner)
{
	if (!owner.m_running)
	{
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateWalk>());
		return;
	}

	PlayerStateMove::Update(owner);
}

//==============================================================
// Attack1
//==============================================================
void PlayerStateAttack1::Enter(Player& owner)
{
	owner.SetAnim(39, false);
	owner.m_dir = Math::Vector3::Zero;
	owner.m_canNextAttack = false;
	owner.m_attackHitOnce = false;
}

void PlayerStateAttack1::Update(Player& owner)
{
	float t = owner.m_animator.GetAnimeCurrentTime();

	// 踏み込み
	if (t > 20 && t < 30)
	{
		Math::Vector3 f = owner.GetForward();
		f.Normalize();
		owner.m_nowPos += f * 0.05f;
	}

	// 攻撃判定
	if (t > 35.0f && t < 45.0f)
	{
		owner.DoAttackHitCheck(1.5f);
	}

	// Attack2 受付
	if (t > 30 && t < 85)
	{
		if (owner.IsAttackInput())
			owner.m_canNextAttack = true;
	}

	// 終了
	if (owner.m_animator.IsAnimationEnd())
	{
		if (owner.m_canNextAttack)
		{
			owner.stateMachine->ChangeState(std::make_unique<PlayerStateAttack2>());
			return;
		}

		owner.stateMachine->ChangeState(std::make_unique<PlayerStateIdle>());
	}
}

//==============================================================
// Attack2
//==============================================================
void PlayerStateAttack2::Enter(Player& owner)
{
	owner.SetAnim(40, false);
	owner.m_dir = Math::Vector3::Zero;
	owner.m_canNextAttack = false;
	owner.m_attackHitOnce = false;

}

void PlayerStateAttack2::Update(Player& owner)
{
	float t = owner.m_animator.GetAnimeCurrentTime();

	if (t > 20 && t < 30)
	{
		Math::Vector3 f = owner.GetForward();
		f.Normalize();
		owner.m_nowPos += f * 0.05f;
	}

	if (t > 35.0f && t < 45.0f)
	{
		owner.DoAttackHitCheck(1.5f);
	}

	if (t > 30 && t < 85)
	{
		if (owner.IsAttackInput())
			owner.m_canNextAttack = true;
	}

	if (owner.m_animator.IsAnimationEnd())
	{
		if (owner.m_canNextAttack)
		{
			owner.stateMachine->ChangeState(std::make_unique<PlayerStateAttack3>());
			return;
		}

		owner.stateMachine->ChangeState(std::make_unique<PlayerStateIdle>());
	}
}

//==============================================================
// Attack3
//==============================================================
void PlayerStateAttack3::Enter(Player& owner)
{
	owner.SetAnim(41, false);
	owner.m_dir = Math::Vector3::Zero;
	owner.m_attackHitOnce = false;

}

void PlayerStateAttack3::Update(Player& owner)
{
	float t = owner.m_animator.GetAnimeCurrentTime();

	if (t > 20 && t < 30)
	{
		Math::Vector3 f = owner.GetForward();
		f.Normalize();
		owner.m_nowPos += f * 0.05f;
	}

	if (t > 35.0f && t < 45.0f)
	{
		owner.DoAttackHitCheck(1.5f);
	}

	if (owner.m_animator.IsAnimationEnd())
	{
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateIdle>());
	}
}

//==============================================================
// Skill
//==============================================================
void PlayerStateSkill::Enter(Player& owner)
{
	owner.SetAnim(20, false);
	owner.m_skillGauge -= owner.m_skillCost;
	owner.m_dir = Math::Vector3::Zero;
}

void PlayerStateSkill::Update(Player& owner)
{
	float t = owner.m_animator.GetAnimeCurrentTime();

	if (t > 10.0f && t < 30.0f)
	{
		owner.DoAttackHitCheck(1.2f);
	}

	if (owner.m_animator.IsAnimationEnd())
	{
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateIdle>());
	}
}

//==============================================================
// Dodge
//==============================================================
void PlayerStateDedge::Enter(Player& owner)
{
	owner.SetAnim(36, false);

	owner.m_isInvincible = true;

	Math::Vector3 dodgeDir = owner.m_dir;   

	if (dodgeDir.LengthSquared() < 0.0001f)
	{
		dodgeDir = owner.GetForward();
	}

	dodgeDir.Normalize();
	owner.m_dodgeDir = dodgeDir;

	owner.m_dir = Math::Vector3::Zero;

}

void PlayerStateDedge::Update(Player& owner)
{
	float t = owner.m_animator.GetAnimeCurrentTime();

	if (t > 0.0f && t < 40.0f)
	{
		owner.m_nowPos += owner.m_dodgeDir * 0.1f;
	}

	owner.m_isInvincible = (t < 40.0f);

	if (owner.m_animator.IsAnimationEnd())
	{
		owner.m_isInvincible = false;
		owner.stateMachine->ChangeState(std::make_unique<PlayerStateIdle>());
	}
}
