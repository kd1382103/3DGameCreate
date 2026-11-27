#include "BossBase.h"

#include <Application/GameObject/Player/Player/Player.h>
#include <Application/Scene/SceneManager.h>
#include <Application/main.h>

#include <Application/GameObject/Boss/BossState/BossState.h>
#include <Application/GameObject/Effect/EffectManager.h>


void BossBase::Init()
{
	// モデルはBossが設定する
	m_animator = KdAnimator();

	// HPゲージ
	InitHPGauge();

	// 3D空間上に出すUI
	InitWorldUI();

	// Boss専用ステートマシン
	stateMachine =
		std::make_shared<StateMachine<BossBase>>();

	stateMachine->ChangeStateImmediate(
		std::make_unique<BossStateIdle>(),
		*this
	);
}

//==============================================================
// Update
//==============================================================
void BossBase::Update()
{
	if (m_isGameEnd)
	{
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
	// Death（死亡演出）
	//========================================
	if (m_isDying)
	{
		UpdateDeath(frameScale);
		return;
	}

	//========================================
	// Resolve（出現演出）
	//========================================
	if (m_isResolving)
	{
		UpdateResolve(frameScale);

		// リゾルブ中もIdleアニメーションを再生
		AdvanceModelAnimation(
			m_animator,
			m_model,
			frameScale
		);

		return;
	}

	//========================================
	// ゲーム全体の時間倍率
	//========================================
	const float timeScale =
		SceneManager::Instance()
		.GetTimeScale();

	//========================================
	// Boss自身の時間倍率
	//========================================
	const float bossTimeScale =
		GetTimeScale();

	//========================================
	// 最終フレーム倍率
	//========================================
	const float dt =
		frameScale *
		timeScale *
		bossTimeScale;

	//========================================
	// スロー・重力
	//========================================
	UpdateSlowAndGravity(dt);

	//========================================
	// ステート
	//========================================
	if (stateMachine)
	{
		stateMachine->Update(*this);
	}

	//========================================
	// ヒットストップ
	//========================================
	if (m_hitStopTimer > 0.0f)
	{
		m_hitStopTimer -= dt;

		if (m_hitStopTimer > 0.0f)
		{
			return;
		}

		m_hitStopTimer = 0.0f;
	}

	//========================================
	// アニメーション
	//========================================
	AdvanceModelAnimation(
		m_animator,
		m_model,
		dt
	);

	//========================================
	// HPゲージ
	//========================================
	UpdateHPGauge();

	//========================================
	// デバッグ
	//========================================
	if (GetAsyncKeyState('3') & 0x8000)
	{
		m_hp = 1;
	}
}

//==============================================================
// PostUpdate
// ワールド行列
//==============================================================
void BossBase::PostUpdate()
{
	//==========================================================
	// ノックバック移動（障害物に当たったら壁沿いに滑る）
	//==========================================================
	NockBackCheck();

	//==========================================================
	// 地面判定
	//==========================================================
	GroundCollisionCheck();

	//==========================================================
	// 壁判定
	//==========================================================
	WallCollisionCheck();

	//==========================================================
	// ワールド行列・UI位置更新
	//==========================================================
	UpdateWorldMatrixAndUI();

	//==========================================================
	// 攻撃予知位置
	// ※親が頭ノードの位置に設定しても、ボスは固定位置にする
	//==========================================================
	m_preAttackPos =
		m_mWorld.Translation() +
		Math::Vector3(0, 1.8f, 0);
}

void BossBase::DrawLit()
{
	if (!m_model) return;

	//==========================================================
	// 視錐台カリング
	//==========================================================
	if (!m_isDying && !IsInCameraView())
	{
		return;
	}

	//========================================
	// 死亡演出
	//========================================
	if (m_isDying)
	{
		KdShaderManager::Instance()
			.m_StandardShader.SetDissolve(
				m_deathDissolve,
				&m_deathEdgeRange,
				&m_deathEmissive
			);

		KdShaderManager::Instance()
			.m_StandardShader.DrawModel(
				*m_model,
				m_mWorld,
				{ 0.6f, 0.35f, 1.0f, 1.0f }
			);

		return;
	}

	//========================================
	// 出現演出
	//========================================
	if (m_isResolving)
	{
		KdShaderManager::Instance()
			.m_StandardShader.SetDissolve(
				m_resolveDissolve,
				&m_resolveEdgeRange,
				&m_resolveEmissive
			);

		KdShaderManager::Instance()
			.m_StandardShader.DrawModel(
				*m_model,
				m_mWorld,
				{ 0.6f, 0.35f, 1.0f, 1.0f }
			);

		return;
	}

	//========================================
	// 通常描画
	//========================================
	KdShaderManager::Instance()
		.m_StandardShader.DrawModel(
			*m_model,
			m_mWorld,
			{ 0.6f, 0.35f, 1.0f, 1.0f }
		);
}

//==============================================================
// 描画
//==============================================================
void BossBase::DrawSprite()
{
	if (m_isDying || m_isResolving) return;

	DrawWorldUI();
}

void BossBase::GenerateDepthMapFromLight()
{
	GenerateDepthMapFromLightCommon(m_isDying);
}

//==============================================================
// ボスへのダメージ処理
//==============================================================
void BossBase::DamageBoss(
	float dmg,
	bool isUltimate,
	bool finalHit,
	float knockBackRate,
	bool interruptAttack
)
{
	// 死亡中・出現中はダメージを受け付けない
	if (m_isDying || m_isResolving)
	{
		return;
	}

	//==========================================================
	// ダメージ関係（HP更新、フライテキスト、Hitエフェクト描画）
	//==========================================================
	CommonDamage(dmg);

	//==========================================================
	// 死亡演出開始
	// GameScene側でカメラ演出を行う場合は開始を保留
	//==========================================================
	if (m_hp <= 0.0f)
	{
		if (!m_isDeathPending)
		{
			StartDeath();
		}

		return;
	}

	//==========================================================
	// ノックバック
	// 通常攻撃・強攻撃・スキルで実行
	//==========================================================
	if (auto player = m_wpPlayer.lock())
	{
		Math::Vector3 knockDir =
			m_nowPos - player->GetPos();

		knockDir.y = 0.0f;

		if (knockDir.LengthSquared() > 0.00001f)
		{
			knockDir.Normalize();

			float knockPower =
				StateMachineParameter::KnockBackPower * knockBackRate;

			// スキルの途中ヒットは弱くする
			if (isUltimate && !finalHit)
			{
				knockPower *= 0.2f;
			}

			// ノックバックの移動量を記録
			// 実際の移動はPostUpdateで障害物を確認してから行う
			m_knockBackMove += knockDir * knockPower;
			m_hasPendingKnockBack = true;
		}
	}

	//==========================================================
	// 強攻撃・スキルでボスの攻撃を中断
	//==========================================================
	if (!interruptAttack && !isUltimate)
	{
		return;
	}

	//==========================================================
	// 攻撃を中断するので、ジャスト回避の受付を終了
	//==========================================================
	ReleaseDodge();

	//==========================================================
	// 被弾ステートへ移行
	//==========================================================
	if (stateMachine)
	{
		stateMachine->ChangeState(
			std::make_unique<BossStateHit>()
		);
	}

	//==========================================================
	// ヒットストップ
	//==========================================================
	m_hitStopTimer = finalHit ? 0.5f : 0.35f;

	//==========================================================
	// 攻撃予知解除
	//==========================================================
	ClearPreAttack();
}

//==============================================================
// 攻撃予知の解除
//==============================================================
void BossBase::ClearPreAttack()
{
	m_preAttackActive = false;
	m_preAttackAlpha = 0.0f;
	m_preAttackTimer = 0.0f;
}

//==============================================================
// プレイヤーのジャスト回避受付を終了する
//==============================================================
void BossBase::ReleaseDodge()
{
	if (auto player = m_wpPlayer.lock())
	{
		player->m_canDodge = false;
	}
}

//==============================================================
// Resolve開始
//==============================================================
void BossBase::StartResolve()
{
	m_isResolving = true;

	m_resolveTimer = 0.0f;

	m_resolveDissolve = 1.0f;

	// 出現中は演出を非表示
	m_preAttackActive = false;
	m_lockOnActive = false;

	// HPゲージ非表示
	SetHPGaugeVisible(false);

	PlayAnimationAuto("Idel", true);
}

//==============================================================
// Death開始
//==============================================================
void BossBase::StartDeath()
{
	// 二重開始を防ぐ
	if (m_isDying)
	{
		return;
	}

	// 死亡演出の待機を解除
	m_isDeathPending = false;

	m_isDying = true;

	// 死亡演出の開始時は爆発・消滅演出を停止
	m_isDeathEffectPaused = true;

	m_isDeathDissolving = false;
	m_deathTimer = 0.0f;
	m_deathDissolve = 0.0f;

	m_isDeathEffectWaiting = false;
	m_deathEffectDelayTimer = 0.0f;

	// 攻撃関連を停止
	ClearPreAttack();

	// プレイヤーのジャスト回避受付を終了
	ReleaseDodge();

	m_attackHitOnce = true;
	m_attackSEPlayed = false;

	// ロックオンを解除
	m_lockOnActive = false;

	// HPゲージを非表示
	SetHPGaugeVisible(false);

	// 死亡アニメーションを再生
	PlayAnimationAuto("Death", false);
}

//==============================================================
// Death更新
//==============================================================
void BossBase::UpdateDeath(float frameScale)
{
	if (!m_model)
	{
		m_isExpired = true;
		return;
	}

	//========================================
	// Deathアニメーション
	//========================================
	if (!m_isDeathDissolving && !m_isDeathEffectWaiting)
	{
		AdvanceModelAnimation(
			m_animator,
			m_model,
			frameScale
		);

		// アニメーション終了後、爆発まで待機
		if (m_animator.IsAnimationEnd())
		{
			m_isDeathEffectWaiting = true;
			m_deathEffectDelayTimer = 0.0f;
		}

		return;
	}

	//========================================
	// 爆発までの待機
	//========================================
	if (m_isDeathEffectWaiting)
	{
		// プレイヤーが退避するまで爆発を待つ
		if (m_isDeathEffectPaused)
		{
			return;
		}

		m_deathEffectDelayTimer += frameScale;

		if (m_deathEffectDelayTimer < m_deathEffectDelay)
		{
			return;
		}

		m_isDeathEffectWaiting = false;

		// 爆発エフェクトを再生
		EffectManager::Instance().Play(
			EffectType::Explosion,
			GetPos()
		);

		// 爆発に合わせてディゾルブ開始
		m_isDeathDissolving = true;

		return;
	}

	//========================================
	// ディゾルブで消滅
	//========================================
	m_deathTimer += frameScale;

	float t = m_deathTimer / m_deathDuration;
	t = std::clamp(t, 0.0f, 1.0f);

	m_deathDissolve = t;

	//========================================
	// 演出終了
	//========================================
	if (m_deathTimer >= m_deathDuration)
	{
		m_deathTimer = m_deathDuration;
		m_deathDissolve = 1.0f;
		m_isExpired = true;
	}
}

//==============================================================
// Resolve更新
//==============================================================
void BossBase::UpdateResolve(float frameScale)
{
	m_resolveTimer += frameScale;

	float t =
		m_resolveTimer /
		m_resolveDuration;

	t = std::clamp(t, 0.0f, 1.0f);

	// 1.0 → 0.0
	m_resolveDissolve = 1.0f - t;

	// 完了
	if (m_resolveTimer >= m_resolveDuration)
	{
		m_resolveTimer = m_resolveDuration;

		m_resolveDissolve = 0.0f;

		m_isResolving = false;

		// HPゲージ表示
		SetHPGaugeVisible(true);
	}
}