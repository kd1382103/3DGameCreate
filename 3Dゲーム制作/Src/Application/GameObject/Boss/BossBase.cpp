#include "BossBase.h"

#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/GameObject/UI/FontText/FontText.h>

#include <Application/Scene/SceneManager.h>
#include <Application/main.h>

#include <Application/GameObject/Boss/BossState/BossState.h>
#include <Application/Scene/GameScene/GameScene.h>
#include <Application/GameObject/Effect/EffectManager.h>


void BossBase::Init()
{
	// モデルはBossが設定する
	m_animator = KdAnimator();

	// ワールド上で表示するUIの初期化
	m_preAttackPoly = std::make_shared<KdSquarePolygon>();
	m_lockOnIcon = std::make_shared<KdSquarePolygon>();
	SetupWorldPolygon(*m_preAttackPoly, "Asset/Textures/Effect/PreAttack.png", 1.0f);
	SetupWorldPolygon(*m_lockOnIcon, "Asset/Textures/Effect/LookOn.png", 0.8);

	// HPゲージ
	m_hpGauge = std::make_shared<HPGauge>();
	m_hpGauge->Init();
	m_hpGauge->SetMode(HPGauge::GaugeMode::World);

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
	// Death（死亡演出）
	//========================================
	if (m_isDying)
	{
		const float frameScale =
			Application::Instance()
			.GetFPSController()
			.GetFrameScale();

		UpdateDeath(frameScale);
		return;
	}

	//========================================
	// Resolve（出現演出）
	//========================================
	if (m_isResolving)
	{
		const float frameScale =
			Application::Instance()
			.GetFPSController()
			.GetFrameScale();

		UpdateResolve(frameScale);

		// リゾルブ中もIdleアニメーションを再生
		if (m_model)
		{
			// リゾルブ中もIdleアニメーションを再生
			AdvanceModelAnimation(
				m_animator,
				m_model,
				frameScale
			);
		}

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
	// スロー更新
	//========================================
	UpdateSlowTimer(
		m_isSlow,
		m_slowTimer,
		Application::Instance().GetDeltaTime()
	);

	//========================================
	// 重力
	//========================================
	ApplyGravity(m_gravity, m_nowPos, dt);

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
	if (m_model)
	{
		// アニメーション
		AdvanceModelAnimation(
			m_animator,
			m_model,
			dt
		);
	}

	//========================================
	// HPゲージ
	//========================================
	UpdateWorldHPGauge(
		m_hpGauge,
		m_wpCamera,
		m_wpPlayer,
		m_nowPos,
		m_hp,
		m_hpMax
	);

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
// 地面判定・壁判定・ワールド行列
//==============================================================
void BossBase::PostUpdate()
{
	GroundCollisionCheck();
	WallCollisionCheck();

	//==========================================================
	// ノックバック移動（障害物に当たったら壁沿いに滑る）
	//==========================================================
	NockBackCheck();

	//==========================================================
	// ワールド行列
	//==========================================================
	Math::Matrix rotMat =
		Math::Matrix::CreateRotationY(m_angleY);

	Math::Matrix transMat =
		Math::Matrix::CreateTranslation(m_nowPos);

	m_mWorld =
		rotMat * transMat;

	//==========================================================
	// モデル原点
	//==========================================================
	Math::Vector3 modelPos =
		m_mWorld.Translation();

	UpdatePreAttackPosition();

	//==========================================================
	// ロックオンアイコン位置
	//==========================================================
	m_lockOnPos =
		m_nowPos +
		Math::Vector3(0, 1.0f, 0);

	//==========================================================
	// HPゲージ位置
	//==========================================================
	if (m_hpGauge)
	{
		Math::Vector3 worldPos =
			m_mWorld.Translation();

		m_hpGauge->SetWorldPos(
			worldPos +
			Math::Vector3(0, 2.0f, 0)
		);
	}
}

void BossBase::DrawLit()
{
	if (!m_model) return;

	//==========================================================
	// 視錐台カリング
	//==========================================================
	if (!m_isDying)
	{
		auto cam = m_wpCamera.lock();

		if (cam)
		{
			Math::Vector3 center =
				m_nowPos + Math::Vector3(0, 1.0f, 0);

			if (!cam->IsInCameraView(center, 2.0f))
			{
				return;
			}
		}
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
// DrawSprite
//==============================================================
void BossBase::DrawSprite()
{
	if (m_isDying || m_isResolving)
	{
		return;
	}

	if (m_hpGauge)
	{
		m_hpGauge->DrawSprite();
	}

	auto cam = m_wpCamera.lock();
	if (!cam) return;

	//==========================================================
	// 攻撃予知
	//==========================================================
	if (m_preAttackActive &&
		m_preAttackPoly)
	{
		Math::Vector3 camForward =
			cam->GetCameraDir();

		Math::Vector3 toBoss =
			m_preAttackPos -
			cam->GetCameraPos();

		toBoss.Normalize();

		float dot =
			camForward.Dot(toBoss);

		if (dot < 0.0f)
		{
			return;
		}

		Math::Vector2 screen =
			cam->WorldToScreen(
				m_preAttackPos
			);

		float scale =
			m_preAttackScale;

		float w = 128.0f * scale;
		float h = 128.0f * scale;

		float x =
			screen.x - w * 0.5f;

		float y =
			screen.y - h * 0.5f;

		auto& sprite =
			KdShaderManager::Instance()
			.m_spriteShader;

		Math::Color color =
		{
			1, 1, 1,
			m_preAttackAlpha
		};

		sprite.DrawTex(
			m_preAttackPoly
			->GetMaterial()
			->m_baseColorTex.get(),
			x,
			y,
			w,
			h,
			nullptr,
			&color,
			{ 0, 0 }
		);
	}

	//==========================================================
	// ロックオン
	//==========================================================
	if (m_lockOnActive &&
		m_lockOnIcon)
	{
		Math::Vector3 camForward =
			cam->GetCameraDir();

		Math::Vector3 toBoss =
			m_lockOnPos -
			cam->GetCameraPos();

		toBoss.Normalize();

		float dot =
			camForward.Dot(toBoss);

		if (dot < 0.0f)
		{
			return;
		}

		Math::Vector2 screen =
			cam->WorldToScreen(
				m_lockOnPos
			);

		float scale =
			m_lockOnScale;

		float iconWidth =
			64.0f * scale;

		float iconHeight =
			64.0f * scale;

		float x =
			screen.x -
			iconWidth * 0.5f;

		float y =
			screen.y -
			iconHeight * 0.5f;

		auto& sprite =
			KdShaderManager::Instance()
			.m_spriteShader;

		Math::Color color =
		{
			1, 1, 1, 1
		};

		sprite.DrawTex(
			m_lockOnIcon
			->GetMaterial()
			->m_baseColorTex.get(),
			x,
			y,
			iconWidth,
			iconHeight,
			nullptr,
			&color,
			{ 0, 0 }
		);
	}
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
	// ダメージ表示
	//==========================================================
	auto fly = std::make_shared<FontText>();

	fly->Init(
		m_nowPos + Math::Vector3(0, 2.0f, 0),
		(int)dmg
	);

	fly->SetCamera(m_wpCamera.lock());

	SceneManager::Instance().AddObject(fly);

	//==========================================================
	// 被弾エフェクト
	//==========================================================
	EffectManager::Instance().Play(
		EffectType::Hit,
		m_nowPos + Math::Vector3(0, 1.0f, 0)
	);

	//==========================================================
	// HP更新
	//==========================================================
	float before = m_hp;

	float after = std::max(
		0.0f,
		before - dmg
	);

	if (m_hpGauge)
	{
		m_hpGauge->OnDamage(before, after);
		m_hpGauge->SetGauge(after, m_hpMax);
	}

	m_hp = after;

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
	// 通常攻撃・スキル・必殺技で実行
	//==========================================================
	if (auto player = m_wpPlayer.lock())
	{
		Math::Vector3 knockDir =
			m_nowPos - player->GetPos();

		knockDir.y = 0.0f;

		if (knockDir.LengthSquared() > 0.00001f)
		{
			knockDir.Normalize();

			// 既存のノックバック強度を維持
			m_knockBackMove +=
				knockDir *
				(m_knockBackPower * knockBackRate);

			m_hasPendingKnockBack = true;
		}

		// 必殺技の攻撃時は回避受付を終了
		if (isUltimate)
		{
			player->m_canDodge = false;
		}
	}

	//==========================================================
	// 強攻撃・必殺技でボスの攻撃を中断
	//==========================================================
	if (!interruptAttack && !isUltimate)
	{
		return;
	}

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
	m_preAttackActive = false;
	m_preAttackAlpha = 0.0f;
	m_preAttackTimer = 0.0f;
}
//==============================================================
// 攻撃判定
//==============================================================
void BossBase::DoAttackHitCheck(float range)
{
	if (m_attackHitOnce) return;

	auto player = m_wpPlayer.lock();
	if (!player) return;

	if (player->m_isInvincible) return;

	Math::Vector3 toPlayer =
		player->GetPos() -
		m_nowPos;

	toPlayer.y = 0;

	float dist =
		toPlayer.Length();

	if (dist > range) return;
	if (dist < 0.0001f) return;

	toPlayer.Normalize();

	// Bossの正面
	Math::Vector3 forward =
		GetForward();

	forward.y = 0;

	if (dist < 0.0001f) return;

	forward.Normalize();

	// 正面判定
	float dot =
		std::clamp(
			forward.Dot(toPlayer),
			-1.0f,
			1.0f
		);

	float angle =
		acos(dot);

	if (angle >
		DirectX::XMConvertToRadians(90.0f))
	{
		return;
	}

	player->Damage(m_attackDamage, false, false, 1.0f);

	m_attackHitOnce = true;
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
	m_isDeathEffectPlayed = false;

	m_isDying = true;

	// 死亡演出の開始時は爆発・消滅演出を停止
	m_isDeathEffectPaused = true;

	m_isDeathDissolving = false;
	m_deathTimer = 0.0f;
	m_deathDissolve = 0.0f;

	m_isDeathEffectWaiting = false;
	m_deathEffectDelayTimer = 0.0f;

	// 攻撃関連を停止
	m_preAttackActive = false;
	m_preAttackAlpha = 0.0f;
	m_preAttackTimer = 0.0f;

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
		// Deathアニメーション
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

		// 爆発エフェクトを一度だけ再生
		if (!m_isDeathEffectPlayed)
		{
			EffectManager::Instance().Play(
				EffectType::Explosion,
				GetPos()
			);

			m_isDeathEffectPlayed = true;
		}

		// 爆発に合わせてディゾルブ開始
		m_isDeathDissolving = true;
		m_deathTimer = 0.0f;
		m_deathDissolve = 0.0f;

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