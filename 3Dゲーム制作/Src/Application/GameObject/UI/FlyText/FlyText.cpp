#include "FlyText.h"
#include <Application/main.h>
#include <Application/GameObject/Camera/CameraBase.h>

//3D空間上の座標と表示する数値を受け取り、FlyTextを初期化する関数
void FlyText::Init(const Math::Vector3& worldPos, int value)
{
	m_worldPos = worldPos;
	m_value = value;

	m_life = LifeTime;
	m_offsetY = 0.0f;
	m_offsetX = ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
	m_velocityX = ((rand() % 100) / 100.0f - 0.5f) * 0.15f;
	m_alpha = 255.0f;
	m_scale = 0.0f;

	m_drawType = eDrawTypeUI;
}

// 文字列のみを受け取り、FlyTextを初期化する関数
void FlyText::InitMessage(const std::string& text)
{
	m_text = text;
	m_isMessage = true;

	m_life = LifeTime;

	m_offsetY = 0.0f;
	m_offsetX = 0.0f;
	m_velocityX = 0.0f;

	m_alpha = 255.0f;
	m_scale = 0.0f;

	m_drawType = eDrawTypeUI;
}

void FlyText::Update()
{
	float dt = Application::Instance().GetDeltaTime();

	//==================================================
	// GAME CLEARなどのメッセージ
	//==================================================
	if (m_isMessage)
	{
		// 消えないようにする
		m_alpha = 255.0f;
		m_scale = m_maxScale;

		return;
	}

	//==================================================
	// ダメージ数字
	//==================================================
	m_life -= dt;

	m_offsetY += 0.25f * dt;
	m_offsetX += m_velocityX * dt;
	m_velocityX *= 0.96f;

	float elapsed = LifeTime - m_life;
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

	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	Math::Color color = { 1,1,1,m_alpha / 255.0f };

	KdSpriteShader::FontParam param;

	param.color = color;
	param.scale = m_scale;
	param.pivot = { 0.5f, 0.5f };

	//---------------------------------------
	// GAME CLEARなどのメッセージ
	//---------------------------------------
	if (m_isMessage)
	{
		// 画面中央
		param.pos = { 0.0f,0.0f };
		param.scale = 2.0f;
		param.color = { 1.0f,1.0f,1.0f,1.0f };
		param.pivot = { 0.5f,0.5f };

		const char* text = m_text.c_str();
		float spacing = 40.0f;
		float startX = -((strlen(text) - 1) * spacing) * 0.5f;

		for (int i = 0; text[i] != '\0'; i++)
		{
			param.pos.x = startX + i * spacing;

			sprite.DrawFontEx(
				param,
				"%c",
				text[i]
			);
		}

		return;
	}

	//---------------------------------------
	// ダメージ数字
	//---------------------------------------
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


	param.pos = screen;

	sprite.DrawFontEx(
		param,
		"%d",
		m_value
	);
}