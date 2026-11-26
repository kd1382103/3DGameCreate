#include "EnemyCharacterBase.h"

#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>

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