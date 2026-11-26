#include "EnemyCharacterBase.h"

#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/Scene/SceneManager.h>
//==============================================================
// アニメーション
//==============================================================

void EnemyCharacterBase::PlayAnimationAuto(
	const std::string& animName,
	bool loop)
{
	if (!m_model) return;

	auto anim = m_model->GetAnimation(animName);

	if (anim)
	{
		m_animator.SetAnimation(anim, loop);
	}
}

//==============================================================
// HPゲージ表示
//==============================================================

void EnemyCharacterBase::SetHPGaugeVisible(bool visible)
{
	if (m_hpGauge)
	{
		m_hpGauge->SetVisible(visible);
	}
}

//==============================================================
// 深度マップ描画
//==============================================================

void EnemyCharacterBase::GenerateDepthMapFromLightCommon(bool isDying)
{
	if (!m_model) return;

	//==========================================================
	// 視錐台カリング（影）
	//==========================================================
	if (!isDying)
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

	KdShaderManager::Instance()
		.m_StandardShader.DrawModel(
			*m_model,
			m_mWorld
		);
}

void EnemyCharacterBase::UpdatePreAttackPosition()
{
	if (!m_model) return;

	const auto* headNode = m_model->FindNode("head");

	if (headNode)
	{
		Math::Vector3 headPos =
			Math::Vector3::Transform(
				Math::Vector3::Zero,
				headNode->m_worldTransform * m_mWorld);

		headPos.y += 0.2f;

		m_preAttackPos = headPos;
	}
	else
	{
		// Headノードがない場合は従来の位置
		m_preAttackPos =
			m_mWorld.Translation() +
			Math::Vector3(0, 1.8f, 0);
	}
}

void EnemyCharacterBase::GroundCollisionCheck()
{
	// 地面判定
	Math::Vector3 groundPos = m_nowPos;
	groundPos.y += 0.2f;

	Math::Vector3 rayDir = Math::Vector3(0, -1, 0);
	float rayRange = 0.2f + m_gravity;

	KdCollider::RayInfo rayInfo;
	rayInfo.m_pos = groundPos;
	rayInfo.m_dir = rayDir;
	rayInfo.m_range = rayRange;
	rayInfo.m_type = KdCollider::TypeGround;

	std::list<KdCollider::CollisionResult> results;

	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		obj->Intersects(rayInfo, &results);
	}

	float maxOverlap = 0.0f;
	Math::Vector3 hitPos;

	for (auto& result : results)
	{
		if (maxOverlap < result.m_overlapDistance)
		{
			maxOverlap = result.m_overlapDistance;
			hitPos = result.m_hitPos;
		}
	}

	if (maxOverlap > 0.0f)
	{
		m_nowPos.y = hitPos.y;
		m_gravity = 0.0f;
	}
}
