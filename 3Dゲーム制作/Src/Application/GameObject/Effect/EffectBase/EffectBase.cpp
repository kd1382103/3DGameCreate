#include "EffectBase.h"

#include <Application/main.h>
#include <Application/GameObject/Camera/CameraBase.h>

//==============================================================
// 初期化
//==============================================================
void EffectBase::Init(
	const Math::Vector3& pos)
{
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