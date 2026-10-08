#include "EnemyBase.h"
#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/GameObject/UI/FontText/FontText.h>

#include <Application/Scene/SceneManager.h>
#include <Application/Scene/GameScene/GameScene.h>
#include <Application/main.h>
#include <Application/GameObject/Enemy/EnemyState/EnemyState.h>
#include <Application/GameObject/Effect/EffectManager.h>
#include <Application/GameObject/Effect/EffectType/EffectType.h>

//==============================================================
// Init
//==============================================================
void EnemyBase::Init()
{
	// モデルは各敵が設定する
	m_animator = KdAnimator();

	InitAttackPrediction();
	InitHPGauge();
	InitStateMachine();
	InitLockOnIcon();
	StartResolve();
}

void EnemyBase::Update()
{
	if (m_isGameEnd) return;

	//========================================
	// 60FPS基準のフレーム倍率
	//========================================
	const float frameScale =
		Application::Instance().GetFPSController().GetFrameScale();

	//==========================================================
	// Resolve中は出現演出だけを進める
	//==========================================================
	if (m_isResolving)
	{
		const float frameScale =
			Application::Instance()
			.GetFPSController()
			.GetFrameScale();

		UpdateResolve(frameScale);

		//========================================
		// リゾルブ中もIdleアニメーションを再生
		//========================================
		if (m_model)
		{
			m_animator.AdvanceTime(
				m_model->WorkNodes(),
				frameScale
			);

			if (m_model->NeedCalcNodeMatrices())
			{
				m_model->CalcNodeMatrices();
			}
		}

		return;
	}

	//==========================================================
	// 死亡中
	//==========================================================
	if (m_isDying)
	{
		UpdateDeath(frameScale);
		return;
	}

	//========================================
	// ゲーム全体の速度
	//========================================
	const float timeScale =
		SceneManager::Instance().GetTimeScale();

	//========================================
	// Enemy自身の速度
	//========================================
	const float enemyTimeScale =
		GetTimeScale();

	//========================================
	// 最終的なフレーム倍率
	//========================================
	const float scaledFrameScale =
		frameScale *
		timeScale *
		enemyTimeScale;

	//========================================
	// スロー更新
	//========================================
	UpdateSlow();

	//========================================
	// 重力
	//========================================
	UpdateGravity(scaledFrameScale);

	//========================================
	// ヒットストップ
	//========================================
	if (m_hitStopTimer > 0.0f)
	{
		m_hitStopTimer -= scaledFrameScale;

		if (m_hitStopTimer > 0.0f)
		{
			return;
		}

		m_hitStopTimer = 0.0f;
	}

	//========================================
	// ステート
	//========================================
	if (stateMachine)
	{
		stateMachine->Update(*this);
	}

	//========================================
	// アニメーション
	//========================================
	UpdateAnimation(scaledFrameScale);

	//========================================
	// HPゲージ
	//========================================
	UpdateHPGauge();

	//========================================
	// デバッグ
	//========================================
	UpdateDebug();
}

//==============================================================
// PostUpdate（地面判定・壁判定）
//==============================================================
void EnemyBase::PostUpdate()
{
	UpdateGroundCollision();

	//========================
	// 壁判定（カプセル）
	//========================
	{
		float maxOverlap = 0.0f;
		Math::Vector3 bestDir = Math::Vector3::Zero;
		bool hit = false;

		KdCollider::CapsuleInfo capsule;
		capsule.m_type = KdCollider::TypeBump;
		capsule.m_radius = 0.3f;
		capsule.m_start = m_nowPos + Math::Vector3(0, 0.5f, 0);
		capsule.m_end = m_nowPos + Math::Vector3(0, 1.5f, 0);
		capsule.m_ownerWorld = m_mWorld;

		std::list<KdCollider::CollisionResult> retCapsuleList;
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects(capsule, &retCapsuleList);
		}

		for (auto& ret : retCapsuleList)
		{
			if (ret.m_overlapDistance > maxOverlap)
			{
				maxOverlap = ret.m_overlapDistance;

				Math::Vector3 dir = ret.m_hitNDir;
				dir.y = 0;

				if (dir.LengthSquared() > 0.00001f)
				{
					dir.Normalize();
				}
				else
				{
					Math::Vector3 fallback = ret.m_hitDir;
					fallback.y = 0;
					fallback.Normalize();
					dir = fallback;
				}

				bestDir = dir;
				hit = true;
			}
		}

		if (hit)
		{
			m_nowPos += bestDir * (maxOverlap * 0.9f);
		}
	}

	//========================
	// ワールド行列更新
	//========================
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(m_angleY);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_nowPos);

	m_mWorld = rotMat * transMat;

	// モデルの原点（ワールド座標）
	Math::Vector3 modelPos = m_mWorld.Translation();

	// 攻撃予知の位置（モデルの頭上）
	m_preAttackPos = modelPos + Math::Vector3(0, 1.8f, 0);

	// ロックオンアイコンの位置（敵の前面）
	m_lockOnPos = m_nowPos + Math::Vector3(0, 1.0f, 0);

	// HPゲージ位置
	if (m_hpGauge)
	{
		Math::Vector3 worldPos = m_mWorld.Translation();
		m_hpGauge->SetWorldPos(worldPos + Math::Vector3(0, 2.0f, 0));
	}
}

//==============================================================
// Draw系
//==============================================================
void EnemyBase::DrawLit()
{
	if (!m_model)
	{
		return;
	}

	//==========================================================
	// Resolve中 または 死亡中
	//==========================================================
	if (m_isResolving || m_isDying)
	{
		KdShaderManager::Instance().m_StandardShader.SetDissolve(
			m_resolveDissolve,
			&m_resolveEdgeRange,
			&m_resolveEmissive
		);

		KdShaderManager::Instance().m_StandardShader.DrawModel(
			*m_model,
			m_mWorld,
			{ 0.6f, 0.8f, 1.0f, 1.0f }
		);

		return;
	}

	//==========================================================
	// 通常描画
	//==========================================================
	KdShaderManager::Instance().m_StandardShader.DrawModel(
		*m_model,
		m_mWorld,
		{ 0.6f, 0.8f, 1.0f, 1.0f }
	);
}

void EnemyBase::GenerateDepthMapFromLight()
{
	if (m_model)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
	}
}

void EnemyBase::DrawSprite()
{
	if (m_isResolving || m_isDying)return;

	if (m_hpGauge) m_hpGauge->DrawSprite();

	auto cam = m_wpCamera.lock();
	if (!cam) return;

	//==========================================================
	// 攻撃予知
	//==========================================================	
		
	if (m_preAttackActive &&
		m_preAttackPoly &&
		(!m_pGameScene || !m_pGameScene->IsSettingOpen())) 
	{
		// 背面判定（HPと同じ）
		Math::Vector3 camForward = cam->GetCameraDir();
		Math::Vector3 toEnemy = m_preAttackPos - cam->GetCameraPos();
		toEnemy.Normalize();

		float dot = camForward.Dot(toEnemy);
		if (dot < 0.0f) return;

		// ワールド → スクリーン座標
		Math::Vector2 screen = cam->WorldToScreen(m_preAttackPos);

		float scale = m_preAttackScale;
		float w = 128.0f * scale;
		float h = 128.0f * scale;

		float x = screen.x - w * 0.5f;
		float y = screen.y - h * 0.5f;

		auto& sprite = KdShaderManager::Instance().m_spriteShader;

		Math::Color color = { 1, 1, 1, m_preAttackAlpha };

		sprite.DrawTex(
			m_preAttackPoly->GetMaterial()->m_baseColorTex.get(),
			x,
			y,
			w,
			h,
			nullptr,
			&color,
			{ 0, 0 }
		);
	}

	//ロックオン
	if (m_lockOnActive && m_lockOnIcon &&
		(!m_pGameScene || !m_pGameScene->IsSettingOpen()))
	{
		//===========================
		// ① 背面判定（HPGauge と同じ）
		//===========================
		Math::Vector3 camForward = cam->GetCameraDir();
		Math::Vector3 toEnemy = m_lockOnPos - cam->GetCameraPos();
		toEnemy.Normalize();

		float dot = camForward.Dot(toEnemy);

		if (dot < 0.0f)
		{
			return; // 背面なら描画しない
		}

		//===========================
		// ② ワールド → スクリーン座標変換
		//===========================
		Math::Vector2 screen = cam->WorldToScreen(m_lockOnPos);

		float scale = m_lockOnScale;

		float iconWidth = 64.0f * scale;
		float iconHeight = 64.0f * scale;

		float x = screen.x - iconWidth * 0.5f;
		float y = screen.y - iconHeight * 0.5f;

		auto& sprite = KdShaderManager::Instance().m_spriteShader;

		Math::Color color = { 1, 1, 1, 1 };

		//===========================
		// ③ HPGauge と同じ DrawTex 形式
		//===========================
		sprite.DrawTex(
			m_lockOnIcon->GetMaterial()->m_baseColorTex.get(),
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

//==============================================================
// Damage
//==============================================================
void EnemyBase::Damage(float dmg, bool isUltimate, bool finalHit, float knockBackRate)
{
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
	// 被弾エフェクト（VS人想定）
	//==========================================================
	EffectManager::Instance().Play(
		EffectType::Hit,
		m_nowPos + Math::Vector3(0, 1.0f, 0)
	);

	//==========================================================
	// 火花エフェクト(VS機械想定)
	//==========================================================

	//	EffectManager::Instance().Play(
	//		EffectType::Spark,
	//		m_nowPos + Math::Vector3(0, 1.0f, 0),
	//		attackDir
	//	);
	

	//==========================================================
	// HP
	//==========================================================
	float before = m_hp;
	float after = std::max(0.0f, before - dmg);

	if (m_hpGauge)
	{
		m_hpGauge->OnDamage(before, after);
		m_hpGauge->SetGauge(after, m_hpMax);
	}

	m_hp = after;

	//==========================================================
	// 死亡処理
	//==========================================================
	if (m_hp <= 0)
	{
		StartDeath();
		return;
	}

	//==========================================================
	// ヒットストップ
	//==========================================================
	m_hitStopTimer = 0.35f;

	//==========================================================
	// 被弾ステート
	//==========================================================
	if (stateMachine)
	{
		stateMachine->ChangeState(
			std::make_unique<EnemyStateHit>()
		);
	}

	//==========================================================
	// 攻撃予知解除
	//==========================================================
	m_preAttackActive = false;
	m_preAttackAlpha = 0.0f;
	m_preAttackTimer = 0.0f;

	//==========================================================
	// プレイヤーのジャスト回避受付も終了
	//==========================================================
	if (auto player = m_wpPlayer.lock())
	{
		player->m_canDodge = false;

		//======================================================
		// ノックバック
		//======================================================
		if (m_canKnockBack)
		{
			Math::Vector3 knockDir =
				m_nowPos - player->GetPos();

			knockDir.y = 0.0f;

			if (knockDir.LengthSquared() > 0.00001f)
			{
				knockDir.Normalize();

				if (isUltimate)
				{
					if (finalHit)
					{
						m_nowPos +=
							knockDir * StateMachineParameter::KnockBackPower * knockBackRate;
					}
					else
					{
						m_nowPos +=
							knockDir *
							(StateMachineParameter::KnockBackPower * 0.2f * knockBackRate);
					}
				}
				else
				{
					m_nowPos +=
						knockDir * StateMachineParameter::KnockBackPower * knockBackRate;
				}
			}
		}
	}
}

//==============================================================
// 攻撃判定
//==============================================================
void EnemyBase::DoAttackHitCheck(float range)
{
	// ヒットストップ中は攻撃判定を行わない
	if (m_hitStopTimer > 0.0f)
	{
		return;
	}

	if (m_attackHitOnce) return;

	auto player = m_wpPlayer.lock();
	if (!player) return;
	if (player->m_isInvincible) return;

	Math::Vector3 toPlayer = player->GetPos() - m_nowPos;
	toPlayer.y = 0;

	float dist = toPlayer.Length();
	if (dist > range) return;
	if (dist < 0.0001f) return;
	toPlayer.Normalize();

	//敵の正面ベクトル
	Math::Vector3 forward = GetForward();
	forward.y = 0;
	if (dist < 0.0001f)return;
	forward.Normalize();

	//正面判定
	float dot = std::clamp(forward.Dot(toPlayer), -1.0f, 1.0f);
	float angle =acos(dot);

	//正面からの攻撃ならヒット
	if (angle > DirectX::XMConvertToRadians(90.0f))return;
	player->Damage(m_attackDamage, false, false, 1.0f);
	m_attackHitOnce = true;
}

//==============================================================
// アニメ再生
//==============================================================
void EnemyBase::PlayAnimationAuto(const std::string& animName, bool loop)
{
	if (!m_model) return;

	auto anim = m_model->GetAnimation(animName);

	if (anim)
	{
		m_animator.SetAnimation(anim, loop);
	}
}

void EnemyBase::SetHPGaugeVisible(bool visible)
{
	if (m_hpGauge)
	{
		m_hpGauge->SetVisible(visible);
	}
}

void EnemyBase::StartTutorialAttack()
{
	if (!stateMachine) return;

	m_isTutorialAttack = true;
	m_tutorialAttackFinished = false;

	stateMachine->ChangeState(
		std::make_unique<EnemyBaseStateAttack>()
	);
}

void EnemyBase::StopTutorialAttack()
{
	m_isTutorialAttack = false;

	if (stateMachine)
	{
		stateMachine->ChangeState(
			std::make_unique<EnemyBaseStateIdle>()
		);
	}
}

void EnemyBase::StopAttackSound()
{
	if (m_attackSound)
	{
		m_attackSound->Stop();
		m_attackSound.reset();
	}
}

void EnemyBase::LookAtPlayer()
{
	auto player = m_wpPlayer.lock();
	if (!player) return;

	Math::Vector3 dir = player->GetPos() - m_nowPos;
	dir.y = 0.0f;

	if (dir.LengthSquared() < 0.0001f) return;

	dir.Normalize();

	m_angleY = std::atan2f(dir.x, dir.z) + DirectX::XM_PI;
}

//==============================================================
// Resolve開始
//==============================================================
void EnemyBase::StartResolve()
{
	m_isResolving = true;

	m_resolveTimer = 0.0f;
	m_resolveDissolve = 1.0f;

	m_preAttackActive = false;
	m_lockOnActive = false;

	// リゾルブ中もIdleアニメーション
	PlayAnimationAuto("Idel", true);
}

//==============================================================
// 死亡開始
//==============================================================
void EnemyBase::StartDeath()
{
	// すでに死亡中なら何もしない
	if (m_isDying)
	{
		return;
	}

	m_isDying = true;
	m_isDeathAnimationEnd = false;

	//==========================================================
	// 既存のResolve用ディゾルブを再利用
	//==========================================================
	m_resolveTimer = 0.0f;
	m_resolveDissolve = 0.0f;

	//==========================================================
	// 戦闘状態をリセット
	//==========================================================
	ResetBattleState();

	//==========================================================
	// ロックオン解除
	//==========================================================
	m_lockOnActive = false;

	//==========================================================
	// 攻撃予知解除
	//==========================================================
	m_preAttackActive = false;
	m_preAttackAlpha = 0.0f;
	m_preAttackTimer = 0.0f;

	//==========================================================
	// HPゲージ非表示
	//==========================================================
	SetHPGaugeVisible(false);

	//==========================================================
	// 死亡アニメーション
	//==========================================================
	PlayAnimationAuto("Death", false);
}

void EnemyBase::InitAttackPrediction()
{
	m_preAttackPoly = std::make_shared<KdSquarePolygon>();

	m_preAttackPoly->SetMaterial(
		"Asset/Textures/Effect/PreAttack.png"
	);

	m_preAttackPoly->Set2DObject(false);
	m_preAttackPoly->SetScale(1.0f);
}

void EnemyBase::InitHPGauge()
{
	m_hpGauge = std::make_shared<HPGauge>();

	m_hpGauge->Init();
	m_hpGauge->SetMode(HPGauge::GaugeMode::World);
}

void EnemyBase::InitStateMachine()
{
	stateMachine = std::make_shared<StateMachine<EnemyBase>>();

	stateMachine->ChangeStateImmediate(
		std::make_unique<EnemyBaseStateIdle>(),
		*this
	);
}

void EnemyBase::InitLockOnIcon()
{
	m_lockOnIcon = std::make_shared<KdSquarePolygon>();

	m_lockOnIcon->SetMaterial(
		"Asset/Textures/Effect/LookOn.png"
	);

	m_lockOnIcon->Set2DObject(false);
	m_lockOnIcon->SetScale(0.8f);
}


//==============================================================
// スロー更新
//==============================================================
void EnemyBase::UpdateSlow()
{
	if (!m_isSlow)
	{
		return;
	}

	m_slowTimer -= Application::Instance().GetDeltaTime();

	if (m_slowTimer <= 0.0f)
	{
		m_slowTimer = 0.0f;
		m_isSlow = false;
	}
}

void EnemyBase::UpdateGravity(float dt)
{
	m_gravity += 0.005f * dt;
	m_nowPos.y -= m_gravity * dt;
}

void EnemyBase::UpdateAnimation(float dt)
{
	if (!m_model) return;

	m_animator.AdvanceTime(m_model->WorkNodes(), dt);

	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}
}

void EnemyBase::UpdateHPGauge()
{
	if (!m_hpGauge) return;

	//---------------------------------------
	// カメラ
	//---------------------------------------
	if (auto cam = m_wpCamera.lock())
	{
		m_hpGauge->SetCamera(cam);
	}

	//---------------------------------------
	// 距離によるスケール
	//---------------------------------------
	float scale = 1.0f;

	if (auto player = m_wpPlayer.lock())
	{
		float dist =
			(player->GetPos() - m_nowPos).Length();

		const float minDist = 3.0f;
		const float maxDist = 15.0f;

		float t =
			(dist - minDist) /
			(maxDist - minDist);

		t = std::clamp(t, 0.0f, 1.0f);

		scale = 1.0f - t * 0.6f;
	}

	m_hpGauge->SetScale(scale);
	m_hpGauge->SetGauge(m_hp, m_hpMax);
}

void EnemyBase::UpdateDebug()
{
	//KdDebugGUI::Instance().ClearLog();

	////========================================
	//// アニメーション一覧
	////========================================
	//for (int i = 0; ; i++)
	//{
	//	auto anim = m_model->GetAnimation(i);

	//	if (!anim)
	//	{
	//		break;
	//	}

	//	KdDebugGUI::Instance().AddLog(
	//		"%d : %s\n",
	//		i,
	//		anim->m_name.c_str()
	//	);
	//}


	if (GetAsyncKeyState('3') & 0x8000)
	{
		Damage(m_hpMax, false, false, 1.0f);
	}
}

//==============================================================
// Resolve更新
//==============================================================
void EnemyBase::UpdateResolve(float frameScale)
{
	m_resolveTimer += frameScale;

	float t = m_resolveTimer / m_resolveDuration;
	t = std::clamp(t, 0.0f, 1.0f);

	// 少し荒くする
	t = std::floor(t * 12.0f) / 12.0f;

	// 1.0 → 0.0 にディゾルブ
	m_resolveDissolve = 1.0f - t;

	// 完了
	if (m_resolveTimer >= m_resolveDuration)
	{
		m_resolveTimer = m_resolveDuration;
		m_resolveDissolve = 0.0f;
		m_isResolving = false;

		// 念のため通常のIdle状態へ
		if (stateMachine)
		{
			stateMachine->ChangeStateImmediate(
				std::make_unique<EnemyBaseStateIdle>(),
				*this
			);
		}
	}
}

//==============================================================
// 死亡演出更新
//==============================================================
void EnemyBase::UpdateDeath(float frameScale)
{
	//==========================================================
	// 死亡アニメーション再生中
	//==========================================================
	if (!m_isDeathAnimationEnd)
	{
		if (m_model)
		{
			m_animator.AdvanceTime(
				m_model->WorkNodes(),
				frameScale
			);

			if (m_model->NeedCalcNodeMatrices())
			{
				m_model->CalcNodeMatrices();
			}
		}

		//======================================================
		// 死亡アニメーション終了
		//======================================================
		if (m_animator.IsAnimationEnd())
		{
			m_isDeathAnimationEnd = true;

			//==================================================
			// 既存Resolve用ディゾルブを最初から開始
			//==================================================
			m_resolveTimer = 0.0f;
			m_resolveDissolve = 0.0f;
		}

		return;
	}

	//==========================================================
	// 死亡アニメーション終了後
	// 最終ポーズを維持したままディゾルブ
	//==========================================================
	m_resolveTimer += frameScale;

	float t = m_resolveTimer / m_resolveDuration;

	t = std::clamp(t, 0.0f, 1.0f);

	// 1.0 → 0.0
	m_resolveDissolve =  t;

	//==========================================================
	// ディゾルブ終了
	//==========================================================
	if (m_resolveTimer >= m_resolveDuration)
	{
		m_resolveTimer = m_resolveDuration;
		m_resolveDissolve = 1.0f;

		// 完全に消えたら削除
		m_isExpired = true;
	}
}

void EnemyBase::UpdateGroundCollision()
{
	KdCollider::RayInfo ray;
	ray.m_type = KdCollider::TypeGround;

	ray.m_pos = m_nowPos;

	static const float enableStepHigh = 0.2f;
	ray.m_pos.y += enableStepHigh;

	ray.m_dir = { 0, -1, 0 };
	ray.m_range = enableStepHigh + m_gravity;

	std::list<KdCollider::CollisionResult> retRayList;

	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		obj->Intersects(ray, &retRayList);
	}

	bool hit = false;
	float maxOverLap = 0.0f;
	Math::Vector3 hitPos = Math::Vector3::Zero;

	for (auto& ret : retRayList)
	{
		if (maxOverLap < ret.m_overlapDistance)
		{
			maxOverLap = ret.m_overlapDistance;
			hitPos = ret.m_hitPos;
			hit = true;
		}
	}

	if (hit)
	{
		m_nowPos.y = hitPos.y;
		m_gravity = 0.0f;
		m_isGround = true;
	}
	else
	{
		m_isGround = false;
	}
}

void EnemyBase::ResetBattleState()
{
	//---------------------------------------
	// 攻撃関連
	//---------------------------------------
	m_isAttacking = false;
	m_attackHitOnce = false;
	m_attackSEPlayed = false;

	//---------------------------------------
	// 攻撃予知
	//---------------------------------------
	m_preAttackActive = false;
	m_preAttackAlpha = 0.0f;
	m_preAttackTimer = 0.0f;

	//---------------------------------------
	// チュートリアル攻撃
	//---------------------------------------
	m_isTutorialAttack = false;
	m_tutorialAttackFinished = false;
	m_canAttack = false;

	//---------------------------------------
	// ロックオン
	//---------------------------------------
	m_lockOnActive = false;

	//---------------------------------------
	// スロー
	//---------------------------------------
	m_isSlow = false;
	m_slowTimer = 0.0f;

	//---------------------------------------
	// 攻撃音
	//---------------------------------------
	StopAttackSound();

	//---------------------------------------
	// プレイヤー側のジャスト回避受付を解除
	//---------------------------------------
	if (auto player = m_wpPlayer.lock())
	{
		player->m_canDodge = false;
	}
}
