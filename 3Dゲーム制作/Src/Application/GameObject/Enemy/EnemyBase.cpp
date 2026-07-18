#include "EnemyBase.h"
#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Camera/CameraBase.h>

#include <Application/Scene/SceneManager.h>
#include <Application/GameObject/Enemy/EnemyState/EnemyState.h>

//==============================================================
// Init
//==============================================================
void EnemyBase::Init()
{
	// モデルは各敵が設定する
	// アニメーター初期化
	m_animator = KdAnimator();
	m_preAttackPoly = std::make_shared<KdSquarePolygon>();
	m_preAttackPoly->SetMaterial("Asset/Textures/Effect/PreAttack.png");
	m_preAttackPoly->Set2DObject(false);
	m_preAttackPoly->SetScale(1.0f);

	// ★テスト用：常に表示
	m_preAttackActive = true;
	m_preAttackAlpha = 1.0f;

	// ステートマシン生成
	stateMachine = std::make_shared<StateMachine<EnemyBase>>();
	stateMachine->ChangeStateImmediate(std::make_unique<EnemyBaseStateIdle>(), *this);
}

//==============================================================
// Update（プレイヤーと同じ構造）
//==============================================================
void EnemyBase::Update()
{
	// ステート更新
	if (stateMachine)
	{
		stateMachine->Update(*this);
	}

	// アニメ更新（プレイヤーと同じ）
	m_animator.AdvanceTime(m_model->WorkNodes(), 1.0f);

	// ノード行列更新（プレイヤーと同じ）
	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}
}

//==============================================================
// PostUpdate（プレイヤーと同じ地面判定・壁判定）
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
	// ワールド行列更新（プレイヤーと同じ）
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

		mat->m_emissiveTex = nullptr;
		mat->m_emissiveRate = { 0.0f, 0.0f, 0.0f };   // ← 発光ゼロ
		mat->m_baseColorRate = { 1, 1, 1, m_preAttackAlpha };

		Math::Vector3 pos = m_preAttackPos;

		Math::Matrix billboardRot = Math::Matrix::Identity;
		if (auto cam = m_wpCamera.lock())
		{
			billboardRot = cam->GetBillboardMatrix();
		}

		Math::Matrix scale = Math::Matrix::CreateScale(1.0f);
		Math::Matrix trans = Math::Matrix::CreateTranslation(pos);
		Math::Matrix world = scale * billboardRot * trans;

		// ★UnLit に切り替え
		KdShaderManager::Instance().m_StandardShader.BeginUnLit();

		KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);

		// ★emissive = Zero → 画像そのまま
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

//==============================================================
// Damage（プレイヤーと同じ構造）
//==============================================================
void EnemyBase::Damage(float dmg)
{
	m_hp -= dmg;
	if (m_hp <= 0)
	{
		m_isExpired=true;
		//stateMachine->ChangeState(std::make_unique<EnemyBaseStateDead>());
	}
}

//==============================================================
// 攻撃判定（プレイヤーと同じ構造）
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
// アニメ再生（プレイヤーの SetAnim と同じ役割）
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
