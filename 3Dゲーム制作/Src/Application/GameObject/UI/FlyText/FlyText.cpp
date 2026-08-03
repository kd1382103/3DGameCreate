#include "FlyText.h"
#include <Application/main.h>
#include <Application/GameObject/Camera/CameraBase.h>

void FlyText::Init(const Math::Vector3& worldPos, int value)
{
	m_worldPos = worldPos;
	m_value = value;

	m_life = 1.0f;
	m_offsetY = 0.0f;
	m_alpha = 255.0f;

	m_drawType = eDrawTypeUI;
}

void FlyText::Update()
{
	float dt = Application::Instance().GetDeltaTime();
	m_life -= dt;
	m_offsetY += 0.75f * dt;

	// 残り半分になったらフェードアウト
	if (m_life < 0.5f)
	{
		m_alpha -= 510.0f * dt; 
		if (m_alpha < 0.0f)
		{
			m_alpha = 0.0f;
		}
	}

	// 寿命終了
	if (m_life <= 0.0f)
	{
		m_isExpired = true;
	}
}

void FlyText::DrawSprite()
{
	auto cam = m_wpCamera.lock();
	if (!cam) return;
	Math::Vector3 camForward = cam->GetCameraDir();
	Math::Vector3 toText = m_worldPos - cam->GetCameraPos();
	toText.Normalize();

	if (camForward.Dot(toText) < 0.0f)
	{
		return;
	}

	Math::Vector3 pos = m_worldPos;
	pos.y += m_offsetY;

	Math::Vector2 screen = cam->WorldToScreen(pos);

	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	Math::Color color = { 1, 1, 1, m_alpha / 255.0f };

	// 文字を描画
	sprite.DrawFont(
		screen,
		&color,
		"%d",
		m_value
	);
}