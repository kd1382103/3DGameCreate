#include "EnemyBase.h"
#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/GameObject/UI/FontText/FontText.h>

#include <Application/Scene/SceneManager.h>
#include <Application/main.h>
#include <Application/GameObject/Enemy/EnemyState/EnemyState.h>
#include <Application/Scene/GameScene/GameScene.h>
//==============================================================
// Init
//==============================================================
void EnemyBase::Init()
{
	// モデルは各敵が設定する
	m_animator = KdAnimator();

	// 攻撃予知ポリゴン
	m_preAttackPoly = std::make_shared<KdSquarePolygon>();
	m_preAttackPoly->SetMaterial("Asset/Textures/Effect/PreAttack.png");
	m_preAttackPoly->Set2DObject(false);
	m_preAttackPoly->SetScale(1.0f);

	// HPゲージ
	m_hpGauge = std::make_shared<HPGauge>();
	m_hpGauge->Init();
	m_hpGauge->SetMode(HPGauge::GaugeMode::World);

	// ステートマシン生成
	stateMachine = std::make_shared<StateMachine<EnemyBase>>();
	stateMachine->ChangeStateImmediate(std::make_unique<EnemyBaseStateIdle>(), *this);

	// ロックオンアイコン
	m_lockOnIcon = std::make_shared<KdSquarePolygon>();
	m_lockOnIcon->SetMaterial("Asset/Textures/Effect/LookOn.png"); 
	m_lockOnIcon->Set2DObject(false);
	m_lockOnIcon->SetScale(0.8f);

}

//==============================================================
// Update
//==============================================================
void EnemyBase::Update()
{
	if(m_isGameEnd) return;

	//------------------------------------
	// スロー時間管理
	//------------------------------------
	if (m_isSlow)
	{
		m_slowTimer -= Application::Instance().GetDeltaTime();

		if (m_slowTimer <= 0.0f)
		{
			m_slowTimer = 0.0f;
			m_isSlow = false;
		}
	}

	float dt = SceneManager::Instance().GetTimeScale() * GetTimeScale();

	m_gravity += 0.005f * dt;
	m_nowPos.y -= m_gravity * dt;

	if (stateMachine)
	{
		stateMachine->Update(*this);
	}

	if (m_hitStopTimer > 0.0f)
	{
		m_hitStopTimer -= 0.016f;
		return;
	}

	// アニメ更新
	m_animator.AdvanceTime(m_model->WorkNodes(), dt);

	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}

	// HPゲージ
	if (auto cam = m_wpCamera.lock())
	{
		m_hpGauge->SetCamera(cam);
	}

	// 距離によるスケール計算
	auto player = m_wpPlayer.lock();
	float scale = 1.0f;

	if (player)
	{
		float dist = (player->GetPos() - m_nowPos).Length();

		float minDist = 3.0f;   // これ以下なら等倍
		float maxDist = 15.0f;  // これ以上なら最小サイズ

		float t = (dist - minDist) / (maxDist - minDist);
		t = std::clamp(t, 0.0f, 1.0f);

		scale = 1.0f - t * 0.6f;   // 1.0 → 0.4 まで縮む
	}

	m_hpGauge->SetScale(scale);

	m_hpGauge->SetGauge(m_hp, m_hpMax);
	//Math::Vector3 worldPos = m_mWorld.Translation();
	//m_hpGauge->SetWorldPos(worldPos + Math::Vector3(0, 2.0f, 0));
}

//==============================================================
// PostUpdate（地面判定・壁判定）
//==============================================================
void EnemyBase::PostUpdate()
{
	//========================
	// 地面判定（レイ）
	//========================
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
		float maxOverLap = 0;
		Math::Vector3 hitPos = {};

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
	if (m_model)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
	}
}

void EnemyBase::DrawSprite()
{
	if (m_hpGauge) m_hpGauge->DrawSprite();

	auto cam = m_wpCamera.lock();
	if (!cam) return;

	//攻撃予知
	if (m_preAttackActive && m_preAttackPoly)
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
	if (m_lockOnActive && m_lockOnIcon)
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
void EnemyBase::Damage(float dmg, bool isUltimate = false, bool finalHit = false)
{
	auto fly = std::make_shared<FontText>();
	fly->Init(m_nowPos + Math::Vector3(0, 2.0f, 0), (int)dmg);
	fly->SetCamera(m_wpCamera.lock());

	SceneManager::Instance().AddObject(fly);

	float before = m_hp;
	float after = std::max(0.0f, before - dmg);	

	if (m_hpGauge)
	{
		m_hpGauge->OnDamage(before, after);
		m_hpGauge->SetGauge(after, m_hpMax);
	}

	m_hp = after;

	if (m_hp <= 0)
	{
		m_pGameScene->AddKillCount();
		m_isExpired = true;
		
		return;
	}

	//ヒットストップ
	m_hitStopTimer = 0.35f;

	//攻撃予知解除
	m_preAttackActive = false;
	m_preAttackAlpha = 0.0f;
	m_preAttackTimer = 0.0f;

	// プレイヤーのジャスト回避受付も終了
	if (auto player = m_wpPlayer.lock())
	{
		player->m_canDodge = false;

		//=========================
		// ノックバック
		//=========================
		if (m_canKnockBack)
		{
			Math::Vector3 knockDir = m_nowPos - player->GetPos();
			knockDir.y = 0.0f;

			if (knockDir.LengthSquared() > 0.00001f)
			{
				knockDir.Normalize();

				if (isUltimate)
				{
					if (finalHit)
					{
						// 最後の一撃だけ大きく吹き飛ばす
						m_nowPos += knockDir * m_knockBackPower;
					}
					else
					{
						// 多段中はほとんど動かさない
						m_nowPos += knockDir * (m_knockBackPower * 0.2f);
					}
				}
				else
				{
					// 通常攻撃
					m_nowPos += knockDir * m_knockBackPower;
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
	player->Damage(m_attackDamage);
	m_attackHitOnce = true;
}

//==============================================================
// アニメ再生
//==============================================================
void EnemyBase::PlayAnimationAuto(const std::string& animName, int animIndex, bool loop)
{
	if (!m_model) return;

	if (animIndex >= 0)
	{
		auto anim = m_model->GetAnimation(animIndex);
		if (anim) m_animator.SetAnimation(anim, loop);
		return;
	}

	if (!animName.empty())
	{
		auto anim = m_model->GetAnimation(animName);
		if (anim) m_animator.SetAnimation(anim, loop);
		return;
	}
}

void EnemyBase::StartTutorialAttack()
{
	if (!stateMachine) return;

	m_isTutorialAttack = true;
	m_tutorialAttackFinished = false;

	stateMachine->ChangeState(
		std::make_unique<EnemyBaseStateAttack1>()
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
