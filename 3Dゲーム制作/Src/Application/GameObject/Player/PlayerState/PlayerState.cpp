#include "PlayerState.h"

#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Effect/EffectManager.h>

#include <Application/GameObject/Enemy/EnemyBase.h>
#include <Application/GameObject/Boss/BossBase.h>

#include <Application/Scene/SceneManager.h>
#include <Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include <Application/main.h>
//==============================================================
// Idle
//==============================================================
void PlayerStateIdle::Enter(Player& owner)
{
	owner.PlayAnimationAuto("Idle");
}

void PlayerStateIdle::Update(Player& owner)
{
	//========================================
	// 強通常攻撃（Eキー）
	//========================================
	if (owner.IsSkillInput())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateSkill>()
		);
		return;
	}

	//========================================
	// スキル（Spaceキー）
	// MPが足りている場合のみ発動
	//========================================
	if (owner.IsUltimateInput() &&
		owner.m_skillGauge >= owner.m_skillCost)
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerUltimate>()
		);
		return;
	}

	//========================================
	// 弱通常攻撃（左クリック）
	//========================================
	if (owner.IsAttackInput())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateAttack1>()
		);
		return;
	}

	//========================================
	// 回避（右クリック）
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
	// 強通常攻撃（Eキー）
	//========================================
	if (owner.IsSkillInput())
	{
		owner.stateMachine->ChangeStateImmediate(
			std::make_unique<PlayerStateSkill>(),
			owner
		);
		return;
	}

	//========================================
	// スキル（Spaceキー）
	//========================================
	if (owner.IsUltimateInput() &&
		owner.m_skillGauge >= owner.m_skillCost)
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerUltimate>()
		);
		return;
	}

	//========================================
	// 弱通常攻撃（左クリック）
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
	// 移動入力がなくなったらIdle
	//========================================
	if (!owner.IsMoveInput())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);
		return;
	}

	//========================================
	// フレーム倍率
	//========================================
	const float frameScale =
		Application::Instance()
		.GetFPSController()
		.GetFrameScale();

	const float timeScale =
		SceneManager::Instance().GetTimeScale();

	const float scaledFrameScale =
		frameScale * timeScale;

	//========================================
	// キャラクター回転
	//========================================
	Math::Vector3 nowDir = owner.GetForward();
	Math::Vector3 targetDir = owner.m_dir;

	nowDir.y = 0.0f;
	targetDir.y = 0.0f;

	if (nowDir.LengthSquared() > 0.0001f &&
		targetDir.LengthSquared() > 0.0001f)
	{
		nowDir.Normalize();
		targetDir.Normalize();

		const float dot = std::clamp(
			nowDir.Dot(targetDir),
			-1.0f,
			1.0f
		);

		float angle = acos(dot);

		const Math::Vector3 cross =
			nowDir.Cross(targetDir);

		if (cross.y < 0.0f)
		{
			angle = -angle;
		}

		const float rotSpeed =
			DirectX::XMConvertToRadians(
				owner.m_rotationSpeedDeg
			);

		angle = std::clamp(
			angle,
			-rotSpeed * scaledFrameScale,
			rotSpeed * scaledFrameScale
		);

		owner.m_angleY += angle;
	}

	//========================================
	// 移動
	//========================================
	const float moveSpeed =
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
	owner.PlayAnimationAuto("Walk",true);
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
	owner.PlayAnimationAuto("Run",true);
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
	owner.FinishLockOnAfterAction();
	owner.PlayAnimationAuto("Attack1", false);
	EnterAttack(owner);
}

void PlayerStateAttack1::Update(Player& owner)
{
	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//========================================
	// 攻撃処理
	//========================================
	UpdateAttack(owner, t);

	//========================================
	// 攻撃判定
	//========================================
	if (t > 20.0f && t < 27.0f) {
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
			30,
			1.0f
		);
	}
	else
	{
		owner.m_attackContact = false;
	}

	//========================================
	// 次の攻撃
	//========================================
	if (t > 27.0f && t < 40.0f)
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
		FinishAttack(owner);
		return;
	}
}

//==============================================================
// Attack2
//==============================================================
void PlayerStateAttack2::Enter(Player& owner)
{
	owner.FinishLockOnAfterAction();

	owner.PlayAnimationAuto("Attack2", false);

	EnterAttack(owner);

}

void PlayerStateAttack2::Update(Player& owner)
{
	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//========================================
	// 攻撃処理
	//========================================
	UpdateAttack(owner, t);

	//========================================
	// 攻撃判定
	//========================================
	if (t > 13.0f && t < 21.0f)
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
			40,
			1.0f
		);
	}
	else
	{
		owner.m_attackContact = false;
	}

	//========================================
	// 次の攻撃
	//========================================
	if (t > 23.0f && t < 35.0f)
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
		FinishAttack(owner);
		return;
	}
}

//==============================================================
// Attack3
//==============================================================
void PlayerStateAttack3::Enter(Player& owner)
{
	owner.FinishLockOnAfterAction();

	owner.PlayAnimationAuto("Attack3", false);

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
	UpdateAttack(owner, t);

	//========================================
	// 攻撃判定
	//========================================
	if (t > 26.0f && t < 30.0f)
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
		// Attack3
		owner.DoAttackHitCheckMulti(
			owner.m_attackDist,
			90.0f,
			60,
			1.0f
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
		FinishAttack(owner);
		return;
	}
}

//==============================================================
// Skill
//==============================================================
void PlayerStateSkill::Enter(Player& owner)
{
	owner.FinishLockOnAfterAction();

	//==========================================================
	// Skillアニメーション開始
	//==========================================================
	owner.PlayAnimationAuto("Skill", false);

	//==========================================================
	// 初期化
	//==========================================================
	owner.m_dir = Math::Vector3::Zero;

	owner.m_attackHitOnce = false;
	owner.m_attackContact = false;

	owner.m_canGainUltimate = true;

	owner.m_attackSEPlayed = false;

	//==========================================================
	// 剣軌跡開始
	//==========================================================
	owner.StartSwordTrail();
}

void PlayerStateSkill::Update(Player& owner)
{
	const float frameScale =
		Application::Instance()
		.GetFPSController()
		.GetFrameScale();

	float t =
		owner.m_animator.GetAnimeCurrentTime();

	//==========================================================
	// 0～5F
	// 構え
	//==========================================================
	if (t < 5.0f)
	{
		owner.m_dir = Math::Vector3::Zero;
		owner.m_attackContact = false;
	}

	//==========================================================
	// 5～28F
	// 突進
	//==========================================================

	else if (t < 28.0f)
	{
		Math::Vector3 forward = owner.GetForward();
		forward.y = 0.0f;

		if (forward.LengthSquared() > 0.001f)
		{
			forward.Normalize();
		}

		//========================================
		// ロックオン対象を取得
		//========================================
		bool hasTarget = false;
		Math::Vector3 targetPos = Math::Vector3::Zero;

		if (owner.IsLockOn() && owner.m_lockOnTarget)
		{
			if (auto enemy =
				dynamic_cast<EnemyBase*>(owner.m_lockOnTarget))
			{
				if (enemy->IsAlive())
				{
					targetPos = enemy->GetHitCenter();
					hasTarget = true;
				}
			}
			else if (auto boss =
				dynamic_cast<BossBase*>(owner.m_lockOnTarget))
			{
				if (boss->IsAlive())
				{
					targetPos = boss->GetHitCenter();
					hasTarget = true;
				}
			}
		}

		//========================================
		// ロックオン対象へ旋回
		//========================================
		if (hasTarget)
		{
			Math::Vector3 toTarget =
				targetPos - owner.m_nowPos;

			toTarget.y = 0.0f;

			if (toTarget.LengthSquared() > 0.001f)
			{
				toTarget.Normalize();

				const float dot = std::clamp(
					forward.Dot(toTarget),
					-1.0f,
					1.0f
				);

				const float crossY =
					forward.z * toTarget.x -
					forward.x * toTarget.z;

				const float angleDiff =
					atan2f(crossY, dot);

				const float maxTurn =
					DirectX::XMConvertToRadians(6.5f)
					* frameScale;

				const float turnAngle = std::clamp(
					angleDiff,
					-maxTurn,
					maxTurn
				);

				owner.m_angleY += turnAngle;

				// 旋回後の方向を取得
				forward = owner.GetForward();
				forward.y = 0.0f;

				if (forward.LengthSquared() > 0.001f)
				{
					forward.Normalize();
				}
			}
		}

		//========================================
		// 突進
		//========================================
		if (forward.LengthSquared() > 0.001f)
		{
			forward.Normalize();

			const float dashSpeed = 0.20f;

			owner.m_nowPos +=
				forward * dashSpeed * frameScale;
		}

		owner.m_dir = Math::Vector3::Zero;
		owner.m_attackContact = false;
	}


	//==========================================================
	// 28～43F
	// 薙ぎ払い攻撃
	// 30～38Fで攻撃判定
	//==========================================================
	else if (t < 38.0f)
	{
		owner.m_dir = Math::Vector3::Zero;

		if (!owner.m_attackSEPlayed)
		{
			KdAudioManager::Instance().Play(
				"Asset/Sounds/SE/SkillAttack.wav",
				SoundType::SE
			);

			owner.m_attackSEPlayed = true;
		}

		// Skill
		owner.DoAttackHitCheckMulti(
			2.5f,
			100.0f,
			20,
			3.0f,
			true
		);
	}

	//==========================================================
	// 43～50F
	// 剣を戻す
	//==========================================================
	else if (t < 50.0f)
	{
		owner.m_dir = Math::Vector3::Zero;
		owner.m_attackContact = false;
	}

	//==========================================================
	// アニメーション終了
	//==========================================================
	if (owner.m_animator.IsAnimationEnd())
	{
		//======================================================
		// Skill終了時の後始末
		//======================================================

		// 移動停止
		owner.m_dir = Math::Vector3::Zero;

		// 攻撃関連解除
		owner.m_attackContact = false;
		owner.m_canGainUltimate = false;

		// 攻撃フラグ初期化
		owner.m_attackHitOnce = false;
		owner.m_attackSEPlayed = false;

		// 剣軌跡停止
		owner.StopSwordTrail();

		// 敵死亡後の再ロックオン
		owner.FinishLockOnAfterAction();

		// Idleへ遷移
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);

		return;
	}
}
//==============================================================
// Dodge
//==============================================================
void PlayerStateDodge::Enter(Player& owner)
{
	owner.PlayAnimationAuto("Dodge", false);

	//========================================
	// スロー
	//========================================
	SceneManager::Instance().SetTimeScale(0.2f);

	owner.m_slowTimer = 10.0f;

	// 回避カメラ
	if (auto cam =
		std::dynamic_pointer_cast<TPSCamera>(
			owner.m_wpCamera.lock()))
	{
		cam->StartDodgeCamera();
	}

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
	//
	// 入力方向は使用しない。
	// 現在のプレイヤーの向いている方向の
	// 逆方向へ回避する。
	//========================================
	Math::Vector3 dodgeDir =
		-owner.GetForward();

	if (dodgeDir.LengthSquared() < 0.0001f)
	{
		dodgeDir =
			-Math::Vector3::UnitZ;
	}

	dodgeDir.Normalize();

	// Dodge開始時の方向を固定
	owner.m_dodgeDir =
		dodgeDir;

	//========================================
	// Dodge中は通常の移動方向をリセット
	//========================================
	owner.m_dir =
		Math::Vector3::Zero;

	//========================================
	// 回避時のMP加算
	//========================================
	owner.m_skillGauge += 10.0f;

	if (owner.m_skillGauge > owner.m_skillGaugeMax)
	{
		owner.m_skillGauge = owner.m_skillGaugeMax;
	}
}


//==============================================================
// Dodge Update
//==============================================================
void PlayerStateDodge::Update(Player& owner)
{
	//========================================
	// アニメーション時間
	//========================================
	const float t =
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
	//
	// 0～5F    : 溜め
	// 5～10F   : 回避開始
	// 10～20F  : 大きく後退
	// 20～30F  : 減速
	// 30～40F  : 着地
	// 40F以降  : 移動なし
	//========================================
	float dodgeSpeed = 0.0f;

	if (t >= 5.0f && t < 10.0f)
	{
		// 回避開始
		dodgeSpeed = 0.04f;
	}
	else if (t >= 10.0f && t < 20.0f)
	{
		// 一気に後退
		dodgeSpeed = 0.12f;
	}
	else if (t >= 20.0f && t < 30.0f)
	{
		// 減速
		dodgeSpeed = 0.08f;
	}
	else if (t >= 30.0f && t < 40.0f)
	{
		// 着地
		dodgeSpeed = 0.03f;
	}


	//========================================
	// 実際の回避移動
	//========================================
	if (dodgeSpeed > 0.0f)
	{
		owner.m_nowPos +=
			owner.m_dodgeDir *
			dodgeSpeed *
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
	if (owner.IsLockOn() && owner.m_lockOnTarget)
	{
		Math::Vector3 targetPos = Math::Vector3::Zero;
		bool hasTarget = false;

		// 敵へのロックオン
		if (auto enemy = dynamic_cast<EnemyBase*>(
			owner.m_lockOnTarget))
		{
			if (enemy->IsAlive())
			{
				targetPos = enemy->GetHitCenter();
				hasTarget = true;
			}
		}

		// ボスへのロックオン
		if (!hasTarget)
		{
			if (auto boss = dynamic_cast<BossBase*>(
				owner.m_lockOnTarget))
			{
				if (boss->IsAlive())
				{
					targetPos = boss->GetHitCenter();
					hasTarget = true;
				}
			}
		}

		// 対象の方向へ即座に向く
		if (hasTarget)
		{
			Math::Vector3 dir = targetPos - owner.m_nowPos;
			dir.y = 0.0f;

			if (dir.LengthSquared() > 0.001f)
			{
				dir.Normalize();
				owner.m_angleY = atan2f(-dir.x, -dir.z);
			}
		}
	}

	owner.FinishLockOnAfterAction();

	owner.PlayAnimationAuto("Ultimate", false);

	owner.m_isInvincible = true;

	owner.m_dir = Math::Vector3::Zero;
	owner.m_attackHitOnce = false;
	owner.m_attackContact = false;
	owner.m_canGainUltimate = false;
	owner.m_ultimateActivated = true;
	owner.m_attackSEPlayed = false;
	owner.StartSwordTrail();

	// ULT 消費
	owner.m_skillGauge -= owner.m_skillCost;

	// 0未満にならないようにする
	owner.m_skillGauge =
		std::max(0.0f, owner.m_skillGauge);

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
		// 攻撃SE
		//====================================
		if (!owner.m_attackSEPlayed)
		{
			KdAudioManager::Instance().Play(
				"Asset/Sounds/SE/UltimateAttack.wav",
				SoundType::SE
			);

			owner.m_attackSEPlayed = true;
		}

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
				5.5f
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
				6.0f,   // 正面方向の長さ
				7.0f,   // 横幅
				60,	  // ダメージ
				1.0f   // ノックバック率
			);
		}
	}

	if (owner.m_animator.IsAnimationEnd())
	{
		//========================================
		// 無敵解除
		//========================================
		owner.m_isInvincible = false;

		//========================================
		// 必殺技カメラ終了
		//========================================
		if (auto cam =
			std::dynamic_pointer_cast<TPSCamera>(
				owner.m_wpCamera.lock()))
		{
			cam->EndUltimateCamera();
		}

		//========================================
		// 剣軌跡停止
		//========================================
		owner.StopSwordTrail();

		//========================================
		// 敵死亡後の再ロックオン
		//========================================
		owner.FinishLockOnAfterAction();

		//========================================
		// Idleへ
		//========================================
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);

		return;
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

	owner.StartSwordTrail();
}

//==============================================================
// Attack State 共通 Update
//==============================================================
void PlayerAttackStateBase::UpdateAttack(Player& owner, float t)
{
	//========================================
	// フレーム倍率
	//========================================
	const float frameScale =
		Application::Instance()
		.GetFPSController()
		.GetFrameScale();

	const float timeScale =
		SceneManager::Instance().GetTimeScale();

	const float scaledFrameScale =
		frameScale * timeScale;

	//========================================
	// 踏み込み時間
	//========================================
	const float lungeStartFrame = 20.0f;
	const float lungeEndFrame = 30.0f;

	if (t > lungeStartFrame && t < lungeEndFrame)
	{
		//========================================
		// 現在のプレイヤー正面方向
		//========================================
		Math::Vector3 forward = owner.GetForward();
		forward.y = 0.0f;

		if (forward.LengthSquared() > 0.001f)
		{
			forward.Normalize();
		}

		//========================================
		// ロックオン対象の取得
		//========================================
		bool hasTarget = false;
		Math::Vector3 targetPos = Math::Vector3::Zero;

		if (owner.IsLockOn() && owner.m_lockOnTarget)
		{
			if (auto enemy =
				dynamic_cast<EnemyBase*>(owner.m_lockOnTarget))
			{
				if (enemy->IsAlive())
				{
					targetPos = enemy->GetHitCenter();
					hasTarget = true;
				}
			}
			else if (auto boss =
				dynamic_cast<BossBase*>(owner.m_lockOnTarget))
			{
				if (boss->IsAlive())
				{
					targetPos = boss->GetHitCenter();
					hasTarget = true;
				}
			}
		}

		//========================================
		// ロックオン中だけ対象方向へ旋回
		//========================================
		float assistDistance = 0.0f;

		if (hasTarget)
		{
			Math::Vector3 toTarget =
				targetPos - owner.m_nowPos;

			toTarget.y = 0.0f;

			const float distance = toTarget.Length();

			if (distance > 0.001f)
			{
				toTarget.Normalize();

				// 現在の正面と対象方向の角度差
				const float dot = std::clamp(
					forward.Dot(toTarget),
					-1.0f,
					1.0f
				);

				const float crossY =
					forward.z * toTarget.x -
					forward.x * toTarget.z;

				const float angleDiff =
					atan2f(crossY, dot);

				// 1フレーム最大10度旋回
				// 横にいる敵にも向きやすくする
				const float maxTurn =
					DirectX::XMConvertToRadians(10.0f)
					* scaledFrameScale;

				const float turnAngle = std::clamp(
					angleDiff,
					-maxTurn,
					maxTurn
				);

				owner.m_angleY += turnAngle;

				// 旋回後の正面方向を取得
				forward = owner.GetForward();
				forward.y = 0.0f;

				if (forward.LengthSquared() > 0.001f)
				{
					forward.Normalize();
				}

				//====================================
				// 対象との距離に応じた追加踏み込み
				//====================================
				const float assistRate = std::clamp(
					(distance - 1.0f) / 3.0f,
					0.0f,
					1.0f
				);

				assistDistance = 0.08f * assistRate;
			}
		}

		//========================================
		// 踏み込み移動
		//========================================
		// ロックオン中：旋回後の対象方向
		// ロックオンなし：プレイヤーの正面方向
		if (forward.LengthSquared() > 0.001f)
		{
			forward.Normalize();

			const float lungeDistance =
				StateMachineParameter::KnockBackPower;

			const float lungeFrameCount =
				lungeEndFrame - lungeStartFrame;

			owner.m_nowPos +=
				forward *
				(
					lungeDistance / lungeFrameCount
					+ assistDistance
					) *
				scaledFrameScale;
		}
	}
}

//==============================================================
// 攻撃終了 共通処理
//==============================================================
void PlayerAttackStateBase::FinishAttack(Player& owner)
{
	//========================================
	// 剣軌跡停止
	//========================================
	owner.StopSwordTrail();

	//========================================
	// 敵死亡後の再ロックオン
	//========================================
	owner.FinishLockOnAfterAction();

	//========================================
	// Idleへ
	//========================================
	owner.stateMachine->ChangeState(
		std::make_unique<PlayerStateIdle>()
	);
}

//==============================================================
// Start Jump
//==============================================================
void PlayerStateStartJump::Enter(Player& owner)
{
	//========================================
	// ゲーム開始ジャンプ
	//========================================
	owner.PlayAnimationAuto("Jump", false);

	//========================================
	// 開始演出中は移動しない
	//========================================
	owner.m_dir = Math::Vector3::Zero;
}

void PlayerStateStartJump::Update(Player& owner)
{
	//========================================
	// ジャンプ終了
	//========================================
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateLanding>()
		);

		return;
	}
}

//==============================================================
// Landing
//==============================================================
void PlayerStateLanding::Enter(Player& owner)
{
	//========================================
	// 着地アニメーション
	//========================================
	owner.PlayAnimationAuto("Land", false);

	//========================================
	// 開始演出中は移動しない
	//========================================
	owner.m_dir = Math::Vector3::Zero;
}

void PlayerStateLanding::Update(Player& owner)
{
	//========================================
	// 着地終了
	//========================================
	if (owner.m_animator.IsAnimationEnd())
	{
		owner.stateMachine->ChangeState(
			std::make_unique<PlayerStateIdle>()
		);

		return;
	}
}

//==============================================================
// Hit
//==============================================================
void PlayerStateHit::Enter(Player& owner)
{
	// 攻撃などを解除
	owner.m_dir = Math::Vector3::Zero;

	owner.m_moving = false;
	owner.m_running = false;

	owner.m_attackOnce = false;
	owner.m_skillOnce = false;
	owner.m_dodgeing = false;
	owner.m_ultimateOnce = false;

	owner.m_attackHitOnce = false;
	owner.m_attackSEPlayed = false;
	owner.m_attackContact = false;

	// 剣の軌跡を停止
	owner.StopSwordTrail();

	// ロックオン後の処理はここでは行わない
	// 被弾アニメーション
	owner.PlayAnimationAuto("Hit", false);
}

void PlayerStateHit::Update(Player& owner)
{
	// Hitアニメーションが終わるまで待つ
	if (!owner.m_animator.IsAnimationEnd())
	{
		return;
	}

	// 被弾終了
	owner.stateMachine->ChangeState(
		std::make_unique<PlayerStateIdle>()
	);
}