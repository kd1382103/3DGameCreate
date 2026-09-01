#include "EffectBase.h"

#include <Application/main.h>
#include <Application/GameObject/Camera/CameraBase.h>

//==============================================================
// 初期化
//==============================================================
void EffectBase::Init(
	EffectType type,
	const Math::Vector3& pos)
{
	m_type = type;

	SetPos(pos);

	m_nowTime = 0.0f;

	m_isFinished = false;
	m_isExpired = false;
}

//==============================================================
// 更新
//==============================================================
void EffectBase::Update()
{
	if (m_isFinished)
	{
		return;
	}

	m_nowTime += Application::Instance().GetDeltaTime();

	if (m_nowTime >= m_lifeTime)
	{
		m_isFinished = true;
		m_isExpired = true;
	}
}

//==============================================================
// ビルボード行列取得
//==============================================================
Math::Matrix EffectBase::GetBillboardMatrix() const
{
	auto camera = m_wpCamera.lock();

	if (!camera)
	{
		return Math::Matrix::Identity;
	}

	return camera->GetBillboardMatrix();
}