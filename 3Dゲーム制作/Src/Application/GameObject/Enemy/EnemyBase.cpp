#include "EnemyBase.h"
#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>

#include <Application/Scene/SceneManager.h>
#include <Application/GameObject/Enemy/EnemyState/EnemyState.h>

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
}

//==============================================================
// Update
//==============================================================
void EnemyBase::Update()
{
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
	m_animator.AdvanceTime(m_model->WorkNodes(), 1.0f);

	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}

	// HPゲージ
	if (auto cam = m_wpCamera.lock())
	{
		m_hpGauge->SetCamera(cam);
	}

	m_hpGauge->SetGauge(m_hp, m_hpMax);
	Math::Vector3 worldPos = m_mWorld.Translation();
	m_hpGauge->SetWorldPos(worldPos + Math::Vector3(0, 2.0f, 0));
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

void EnemyBase::DrawUnLit()
{
	if (m_preAttackActive && m_preAttackPoly)
	{
		auto mat = m_preAttackPoly->GetMaterial();

		// 発光（Pulse）
		float pulse = (sin(m_preAttackTimer * 20.0f) + 1.0f) * 0.5f;
		mat->m_emissiveRate = { 1.0f * pulse, 0.2f * pulse, 0.2f * pulse };

		mat->m_baseColorRate = { 1, 1, 1, m_preAttackAlpha };

		Math::Vector3 pos = m_preAttackPos;

		Math::Matrix billboardRot = Math::Matrix::Identity;
		if (auto cam = m_wpCamera.lock())
		{
			billboardRot = cam->GetBillboardMatrix();
		}

		Math::Matrix scale = Math::Matrix::CreateScale(m_preAttackScale);
		Math::Matrix trans = Math::Matrix::CreateTranslation(pos);
		Math::Matrix world = scale * billboardRot * trans;

		KdShaderManager::Instance().m_StandardShader.BeginUnLit();
		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

		KdShaderManager::Instance().m_StandardShader.DrawPolygon(
			*m_preAttackPoly,
			world,
			Math::Color(1, 1, 1, 1),
			Math::Vector3::Zero
		);

		KdShaderManager::Instance().UndoBlendState();
		KdShaderManager::Instance().m_StandardShader.EndUnLit();
	}
}

void EnemyBase::DrawSprite()
{
	if (m_hpGauge)
		m_hpGauge->DrawSprite();
}

//==============================================================
// Damage
//==============================================================
void EnemyBase::Damage(float dmg)
{
	float before = m_hp;
	float after = before - dmg;
	if (after < 0) after = 0;

	if (m_hpGauge)
	{
		m_hpGauge->OnDamage(before, after);
		m_hpGauge->SetGauge(after, m_hpMax);
	}

	m_hp = after;

	if (m_hp <= 0)
	{
		m_isExpired = true;
	}

	m_hitStopTimer = 0.35f;

	// ノックバック
	Math::Vector3 back = -m_mWorld.Forward();
	back.y = 0;
	back.Normalize();

	m_nowPos += back * 0.65f;   // 距離は調整可能

}

//==============================================================
// 攻撃判定
//==============================================================
void EnemyBase::DoAttackHitCheck(float range)
{
	auto player = m_wpPlayer.lock();
	if (!player) return;

	float dist = (player->GetPos() - m_nowPos).Length();
	if (dist < range)
	{
		player->Damage(m_attackDamage);
	}
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
