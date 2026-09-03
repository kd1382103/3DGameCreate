#include "PlayerState.h"

#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Effect/EffectManager.h>

#include <Application/Scene/SceneManager.h>
#include <Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include <Application/main.h>

//==============================================================
// Idle
//==============================================================
void PlayerStateIdle::Enter(Player& owner)
{
	owner.SetAnim(9, true); // Idle
}

void PlayerStateIdle::Update(Player& owner)
{
	//========================================
	// スキル
	//========================================
	if (owner.IsSkillInput() &&
		owner.m_skillGauge >= owner.m_skillCost)
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateSkill>()
		);
		return;
	}

	//========================================
	// 必殺技
	//========================================
	if (owner.IsUltimateInput() &&
		owner.m_ultimateEnergy >= owner.m_ultimateEnergyMax)
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerUltimate>()
		);
		return;
	}

	//========================================
	// 攻撃
	//========================================
	if (owner.IsAttackInput())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateAttack1>()
		);
		return;
	}

	//========================================
	// 回避
	//========================================
	if (owner.IsDodgeInput())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateDodge>()
		);
		return;
	}

	//========================================
	// 移動
	//========================================
	if (owner.IsMoveInput())
	{
		if (owner.m_running)
		{
			owner.stateMachine->ChangeState(
				std::make_unique<PlayerStateDash>()
			);
		}
		else
		{
			owner.stateMachine->ChangeState(
				std::make_unique<PlayerStateWalk>()
			);
		}

		return;
	}
}

//==============================================================
// Move 基底
//==============================================================
void PlayerStateMove::Update(Player& owner)
{
	//========================================
	// 移動入力が消えたら Idle
	//========================================
	if (!owner.IsMoveInput())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);
		return;
	}

	//========================================
	// スキル
	//========================================
	if (owner.IsSkillInput() &&
		owner.m_skillGauge >= owner.m_skillCost)
	{
		owner.stateMachine->ChangeStateImmediate(
			std::make_unique<PlayerStateSkill>(),
			owner
		);
		return;
	}

	//========================================
	// 必殺技
	//========================================
	if (owner.IsUltimateInput() &&
		owner.m_ultimateEnergy >= owner.m_ultimateEnergyMax)
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerUltimate>()
		);
		return;
	}

	//========================================
	// 攻撃
	//========================================
	if (owner.IsAttackInput())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateAttack1>()
		);
		return;
	}

	//========================================
	// 回避
	//========================================
	if (owner.IsDodgeInput())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateDodge>()
		);
		return;
	}

	//========================================
	// 60FPS基準のフレーム倍率
	//========================================
	const float frameScale =
		Application::Instance()
		.GetFPSController()
		.GetFrameScale();

	//========================================
	// ゲーム全体の速度
	//========================================
	const float timeScale =
		SceneManager::Instance().GetTimeScale();

	const float scaledFrameScale =
		frameScale * timeScale;

	//========================================
	// キャラ回転
	//========================================
	{
		Math::Vector3 nowDir = owner.GetForward();
		Math::Vector3 targetDir = owner.m_dir;

		nowDir.y = 0.0f;
		targetDir.y = 0.0f;

		if (nowDir.LengthSquared() > 0.0001f &&
			targetDir.LengthSquared() > 0.0001f)
		{
			nowDir.Normalize();
			targetDir.Normalize();

			float dot =
				std::clamp(
					nowDir.Dot(targetDir),
					-1.0f,
					1.0f
				);

			float angle = acos(dot);

			Math::Vector3 cross =
				nowDir.Cross(targetDir);

			if (cross.y < 0)
			{
				angle = -angle;
			}

			float rotSpeed =
				DirectX::XMConvertToRadians(
					owner.m_rotationSpeedDeg
				);

			// FPSに依存しないようにする
			angle =
				std::clamp(
					angle,
					-rotSpeed * scaledFrameScale,
					rotSpeed * scaledFrameScale
				);

			owner.m_angleY += angle;
		}
	}

	//========================================
	// 移動
	//========================================
	float moveSpeed =
		owner.m_running
		? owner.m_runSpeed
		: owner.m_walkSpeed;

	owner.m_nowPos +=
		owner.m_dir *
		moveSpeed *
		scaledFrameScale;
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
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateDash>()
		);
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
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateWalk>()
		);
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

	EnterAttack(owner);
}

void PlayerStateAttack1::Update(Player& owner)
{
	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//========================================
	// 攻撃処理
	//========================================
	UpdateAttack(owner, t, 15);

	//========================================
	// 次の攻撃
	//========================================
	if (t > 40.0f && t < 85.0f)
	{
		if (owner.IsAttackInput())
		{
			owner.m_canGainUltimate = false;

			owner.stateMachine->ChangeState(
				std::make_unique<PlayerStateAttack2>()
			);

			return;
		}
	}

	//========================================
	// アニメーション終了
	//========================================
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);
	}
}

//==============================================================
// Attack2
//==============================================================
void PlayerStateAttack2::Enter(Player& owner)
{
	owner.SetAnim(40, false);

	EnterAttack(owner);
}

void PlayerStateAttack2::Update(Player& owner)
{
	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//========================================
	// 攻撃処理
	//========================================
	UpdateAttack(owner, t, 20);

	//========================================
	// 次の攻撃
	//========================================
	if (t > 40.0f && t < 85.0f)
	{
		if (owner.IsAttackInput())
		{
			owner.m_canGainUltimate = false;

			owner.stateMachine->ChangeState(
				std::make_unique<PlayerStateAttack3>()
			);

			return;
		}
	}

	//========================================
	// アニメーション終了
	//========================================
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);
	}
}

//==============================================================
// Attack3
//==============================================================
void PlayerStateAttack3::Enter(Player& owner)
{
	owner.SetAnim(41, false);

	EnterAttack(owner);

	owner.m_comboFinished = true;
}

void PlayerStateAttack3::Update(Player& owner)
{
	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//========================================
	// 攻撃処理
	//========================================
	UpdateAttack(owner, t, 30);

	//========================================
	// 次の攻撃
	//========================================
	if (t > 40.0f && t < 85.0f)
	{
		if (owner.IsAttackInput())
		{
			owner.m_canGainUltimate = false;

			owner.stateMachine->ChangeState(
				std::make_unique<PlayerStateAttack1>()
			);

			return;
		}
	}

	//========================================
	// アニメーション終了
	//========================================
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.m_canGainUltimate = false;

		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);
	}
}

//==============================================================
// Skill
//==============================================================
void PlayerStateSkill::Enter(Player& owner)
{
	owner.SetAnim(20, false);

	// スキルゲージ消費
	owner.m_skillGauge -= owner.m_skillCost;

	// 初期化
	owner.m_dir = Math::Vector3::Zero;
	owner.m_attackHitOnce = false;
	owner.m_attackContact = false;
	owner.m_canGainUltimate = true;
}

void PlayerStateSkill::Update(Player& owner)
{
	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//========================================
	// スキル攻撃判定
	//========================================
	if (t > 3.0f && t < 8.0f)
	{
		owner.DoSkillHitCheck(
			2.5f,
			40
		);
	}
	else
	{
		owner.m_attackContact = false;
	}

	//========================================
	// アニメーション終了
	//========================================
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.m_canGainUltimate = false;

		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);
	}
}
//==============================================================
// Dodge
//==============================================================
void PlayerStateDodge::Enter(Player& owner)
{
	owner.SetAnim(27, false);

	//========================================
	// スロー
	//========================================
	SceneManager::Instance().SetTimeScale(0.2f);

	owner.m_slowTimer = 0.5f;

	//========================================
	// 無敵
	//========================================
	owner.m_isInvincible = true;

	//========================================
	// 回避カメラ
	//========================================
	if (auto cam =
		std::dynamic_pointer_cast<TPSCamera>(
			owner.m_wpCamera.lock()))
	{
		cam->StartDodgeCamera();
	}

	//========================================
	// 回避方向
	//========================================
	Math::Vector3 dodgeDir =
		owner.m_dir;

	if (dodgeDir.LengthSquared() < 0.0001f)
	{
		dodgeDir =
			owner.GetForward();
	}

	dodgeDir.Normalize();

	owner.m_dodgeDir =
		dodgeDir;

	owner.m_dir =
		Math::Vector3::Zero;
}

void PlayerStateDodge::Update(Player& owner)
{
	//========================================
	// 回避中は無敵
	//========================================
	owner.m_isInvincible = true;

	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//========================================
	// 60FPS基準のフレーム倍率
	//========================================
	const float frameScale =
		Application::Instance()
		.GetFPSController()
		.GetFrameScale();

	//========================================
	// ゲーム全体の速度
	//========================================
	const float timeScale =
		SceneManager::Instance().GetTimeScale();

	const float scaledFrameScale =
		frameScale * timeScale;

	//========================================
	// 回避移動
	//========================================
	if (t > 0.0f && t < 40.0f)
	{
		owner.m_nowPos +=
			owner.m_dodgeDir *
			0.1f *
			scaledFrameScale;
	}

	//========================================
	// 回避終了
	//========================================
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.m_isInvincible = false;

		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);
	}
}

//==============================================================
// Ultimate
//==============================================================
void PlayerUltimate::Enter(Player& owner)
{
	// 現状、必殺技アニメーションがないため
	// 攻撃1段目のアニメーションを使用
	owner.SetAnim(39, false);

	owner.m_dir = Math::Vector3::Zero;
	owner.m_attackHitOnce = false;
	owner.m_attackContact = false;
	owner.m_canGainUltimate = false;
	owner.m_ultimateActivated = true;

	//========================================
	// ゲージ消費
	//========================================
	owner.m_ultimateEnergy = 0;

	//========================================
	// 必殺技ヒット情報リセット
	//========================================
	owner.m_ultimateHitTimer = 0;
	owner.m_ultimateHitCount = 0;

	//========================================
	// 必殺技エフェクト
	//========================================
	owner.m_ultimateEffectPlayed = false;

	//========================================
	// 必殺技のカメラ
	//========================================
	if (auto cam =
		std::dynamic_pointer_cast<TPSCamera>(
			owner.m_wpCamera.lock()))
	{
		cam->StartUltimateCamera();
	}
}

void PlayerUltimate::Update(Player& owner)
{
	float t = owner.m_animator.GetAnimeCurrentTime();

	//========================================
	// 必殺技攻撃
	//========================================
	if (t > 30.0f && t < 60.0f)
	{
		//====================================
		// 必殺技エフェクト生成
		//====================================
		if (!owner.m_ultimateEffectPlayed)
		{
			EffectManager::Instance().Play(
				EffectType::Ultimate,
				owner.m_nowPos,
				owner.GetForward(),
				owner.m_ultimateHitInterval,
				owner.m_ultimateMaxHitCount,
				4.0f
			);

			owner.m_ultimateEffectPlayed = true;
		}

		//========================================
		// 60FPS基準のフレーム倍率
		//========================================
		const float frameScale =
			Application::Instance()
			.GetFPSController()
			.GetFrameScale();

		//========================================
		// 必殺技ヒットタイマー
		//========================================
		owner.m_ultimateHitTimer += frameScale;

		if (owner.m_ultimateHitCount < 5 &&
			owner.m_ultimateHitTimer >=
			owner.m_ultimateHitInterval)
		{
			owner.m_ultimateHitTimer -=
				owner.m_ultimateHitInterval;

			owner.DoUltimateHitCheck(
				4.0f,   // 正面方向の長さ
				1.75f,   // 横幅
				40
			);
		}
	}

	//========================================
	// アニメーション終了
	//========================================
	if (owner.m_animator.IsAnimationEnd())
	{

		//========================================
		// 必殺技カメラ終了
		//========================================
		if (auto cam =
			std::dynamic_pointer_cast<TPSCamera>(
				owner.m_wpCamera.lock()))
		{
			cam->EndUltimateCamera();
		}

		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);
	}
}

//==============================================================
// Attack State 共通 Enter
//==============================================================
void PlayerAttackStateBase::EnterAttack(Player& owner)
{
	owner.m_dir = Math::Vector3::Zero;

	owner.m_canNextAttack = false;

	owner.m_attackHitOnce = false;

	owner.m_attackContact = false;

	owner.m_canGainUltimate = true;

	owner.m_attackSEPlayed = false;
}

//==============================================================
// Attack State 共通 Update
//==============================================================
void PlayerAttackStateBase::UpdateAttack(
	Player& owner,
	float t,
	int damage
)
{
	//========================================
	// 60FPS基準のフレーム倍率
	//========================================
	const float frameScale =
		Application::Instance()
		.GetFPSController()
		.GetFrameScale();

	//========================================
	// ゲーム全体の速度
	//========================================
	const float timeScale =
		SceneManager::Instance().GetTimeScale();

	const float scaledFrameScale =
		frameScale * timeScale;

	//========================================
	// 踏み込み
	//========================================
	if (t > 20.0f && t < 30.0f)
	{
		Math::Vector3 f =
			owner.GetForward();

		f.Normalize();

		owner.m_nowPos +=
			f *
			0.05f *
			scaledFrameScale;
	}

	//========================================
	// 攻撃判定
	//========================================
	if (t > 35.0f && t < 40.0f)
	{
		//====================================
		// 攻撃SE
		//====================================
		if (!owner.m_attackSEPlayed)
		{
			KdAudioManager::Instance().Play(
				"Asset/Sounds/SE/Attack.wav",
				SoundType::SE
			);

			owner.m_attackSEPlayed = true;
		}

		//====================================
		// 攻撃判定
		//====================================
		owner.DoAttackHitCheckMulti(
			owner.m_attackDist,
			90.0f,
			damage
		);
	}
	else
	{
		owner.m_attackContact = false;
	}
}