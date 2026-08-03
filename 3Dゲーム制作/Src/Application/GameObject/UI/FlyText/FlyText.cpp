#include "FlyText.h"
#include <Application/main.h>
#include <Application/GameObject/Camera/CameraBase.h>


void FlyText::Init(const Math::Vector3& worldPos, int value)
{
	m_worldPos = worldPos;
	m_value = value;

	m_life = LifeTime;
	m_offsetY = 0.0f;
	m_offsetX = 0.0f;
	m_velocityX = ((rand() % 100) / 100.0f - 0.5f) * 0.03f;
	m_alpha = 255.0f;
	m_scale = 0.0f;

	m_drawType = eDrawTypeUI;
}

void FlyText::Update()
{
	float dt = Application::Instance().GetDeltaTime();
	m_life -= dt;
	m_offsetY += 0.25f * dt;
	// 左右へ少しはじける
	m_offsetX += m_velocityX * dt;

	// 徐々に減速
	m_velocityX *= 0.96f;

	float elapsed = LifeTime - m_life;

	if (elapsed > 0.05f)
	{
		m_offsetX += m_velocityX * dt;
		m_velocityX *= 0.96f;
	}

	const float shrinkStart = LifeTime - ScaleTime;

	//---------------------------------------
	// 拡大縮小
	//---------------------------------------
	if (elapsed < ScaleTime)
	{
		m_scale = elapsed / ScaleTime;
	}
	else if (elapsed < shrinkStart)
	{
		// 等倍維持
		m_scale = m_maxScale;
	}
	else
	{
		float t = (elapsed - shrinkStart) / ScaleTime;
		t = std::clamp(t, 0.0f, 1.0f);
		m_scale = m_maxScale * (1.0f - t);		
	}

	//---------------------------------------
	// フェードアウト
	//---------------------------------------
	if (elapsed >= shrinkStart)
	{
		float t = (elapsed - shrinkStart) / ScaleTime;
		t = std::clamp(t, 0.0f, 1.0f);

		m_alpha = 255.0f * (1.0f - t);
	}

	//---------------------------------------
	// 消滅
	//---------------------------------------
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
	pos.x += m_offsetX;
	pos.y += m_offsetY;

	Math::Vector2 screen = cam->WorldToScreen(pos);

	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	Math::Color color = { 1, 1, 1, m_alpha / 255.0f };

	KdSpriteShader::FontParam param;
	param.pos = screen;
	param.color = color;
	param.scale = m_scale;
	param.pivot = { 0.5f, 0.5f };

	sprite.DrawFontEx(
		param,
		"%d",
		m_value
	);
}