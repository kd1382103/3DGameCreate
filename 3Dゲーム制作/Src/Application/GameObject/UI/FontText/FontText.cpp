#include "FontText.h"
#include <Application/main.h>
#include <Application/GameObject/Camera/CameraBase.h>

//3D空間上の座標と表示する数値を受け取り、FontTextを初期化する関数

//==================================================
// ダメージフライテキスト
//==================================================
void FontText::Init(const Math::Vector3& worldPos, int value)
{
	m_worldPos = worldPos;
	m_value = value;

	m_isHeal = false
		;
	// ダメージ：赤
	m_color =
	{
		1.0f,
		0.15f,
		0.15f,
		1.0f
	};

	m_life = LifeTime;
	m_offsetY = 0.0f;
	m_offsetX = ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
	m_velocityX = ((rand() % 100) / 100.0f - 0.5f) * 0.15f;
	m_alpha = 255.0f;
	m_scale = 0.0f;

	m_drawType = eDrawTypeUI;
}

//==================================================
// 回復フライテキスト
//==================================================
void FontText::InitHeal(const Math::Vector3& worldPos, int value)
{
	m_worldPos = worldPos;
	m_value = value;

	m_isHeal = true;

	// 回復：緑
	m_color =
	{
		0.15f,
		1.0f,
		0.15f,
		1.0f
	};

	m_life = LifeTime;
	m_offsetY = 0.0f;
	m_offsetX = ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
	m_velocityX = ((rand() % 100) / 100.0f - 0.5f) * 0.15f;
	m_alpha = 255.0f;
	m_scale = 0.0f;

	m_drawType = eDrawTypeUI;
}


// 文字列のみを受け取り、FontTextを初期化する関数
void FontText::InitMessage(
	const std::string& text,
	const Math::Vector2& pos,
	float scale)
{
	m_text = text;
	m_screenPos = pos;
	m_messageScale = scale;

	m_isMessage = true;

	m_color =
	{
		1.0f,
		1.0f,
		1.0f,
		1.0f
	};

	m_drawType = eDrawTypeUI;
}

void FontText::Update()
{
	//==================================================
	// GAME CLEARなどのメッセージ
	//==================================================
	if (m_isMessage) { return; }

	//==================================================
	// フライテキスト
	//==================================================
	float dt = Application::Instance().GetDeltaTime();

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
		m_scale = 1.0f;
	}
	else
	{
		float t = (elapsed - shrinkStart) / ScaleTime;
		t = std::clamp(t, 0.0f, 1.0f);

		m_scale = 1.0f - t;
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

void FontText::DrawSprite()
{
	if (!m_visible)
	{
		return;
	}

	auto& sprite =
		KdShaderManager::Instance().m_spriteShader;

	KdSpriteShader::FontParam param;

	param.color = m_color;
	param.scale = m_scale;
	param.pivot = { 0.5f, 0.5f };

	//---------------------------------------
	// GAME CLEARなどのメッセージ
	//---------------------------------------
	if (m_isMessage)
	{
		param.pos = m_screenPos;
		param.scale = m_messageScale;
		param.color = m_color;
		param.pivot = { 0.5f, 0.5f };

		// メッセージの縁取り色：黒
		Math::Color outlineColor =
		{
			0.0f, 0.0f, 0.0f, 1.0f
		};

		const float outlineSize = 1.5f;

		const Math::Vector2 offsets[] =
		{
			{ -outlineSize,  0.0f },
			{  outlineSize,  0.0f },
			{  0.0f, -outlineSize },
			{  0.0f,  outlineSize },
			{ -outlineSize, -outlineSize },
			{  outlineSize, -outlineSize },
			{ -outlineSize,  outlineSize },
			{  outlineSize,  outlineSize }
		};

		// 先に縁取りを描画
		for (const auto& offset : offsets)
		{
			param.pos =
			{
				m_screenPos.x + offset.x,
				m_screenPos.y + offset.y
			};

			param.color = outlineColor;

			sprite.DrawFontEx(
				param,
				"%s",
				m_text.c_str()
			);
		}

		// 本来の文字を描画
		param.pos = m_screenPos;
		param.color = m_color;

		sprite.DrawFontEx(
			param,
			"%s",
			m_text.c_str()
		);

		return;
	}

	//---------------------------------------
	// フライテキスト
	//---------------------------------------
	auto cam = m_wpCamera.lock();

	if (!cam)
	{
		return;
	}

	Math::Vector3 camForward =
		cam->GetCameraDir();

	Math::Vector3 toText =
		m_worldPos - cam->GetCameraPos();

	toText.Normalize();

	if (camForward.Dot(toText) < 0.0f)
	{
		return;
	}

	Math::Vector3 pos = m_worldPos;

	pos.x += m_offsetX;
	pos.y += m_offsetY;

	Math::Vector2 screen =
		cam->WorldToScreen(pos);

	param.pos = screen;

	//-------------------------------
	// 縁取り色
	//-------------------------------
	Math::Color outlineColor;

	if (m_isHeal)
	{
		// 回復：暗い緑
		outlineColor = { 0.0f, 0.25f, 0.0f, 1.0f };
	}
	else
	{
		// ダメージ：暗い赤
		outlineColor = { 0.25f, 0.0f, 0.0f, 1.0f };
	}

	const float outlineSize = 1.0f;

	const Math::Vector2 offsets[] =
	{
		{ -outlineSize,  0.0f },
		{  outlineSize,  0.0f },
		{  0.0f, -outlineSize },
		{  0.0f,  outlineSize },
		{ -outlineSize, -outlineSize },
		{  outlineSize, -outlineSize },
		{ -outlineSize,  outlineSize },
		{  outlineSize,  outlineSize }
	};

	// 先に縁取りを描画
	for (const auto& offset : offsets)
	{
		param.pos =
		{
			screen.x + offset.x,
			screen.y + offset.y
		};

		param.color = outlineColor;

		if (m_isHeal)
		{
			sprite.DrawFontEx(param, "+%d", m_value);
		}
		else
		{
			sprite.DrawFontEx(param, "%d", m_value);
		}
	}

	// 本来の文字を最後に描画
	param.pos = screen;
	param.color = m_color;

	if (m_isHeal)
	{
		sprite.DrawFontEx(param, "+%d", m_value);
	}
	else
	{
		sprite.DrawFontEx(param, "%d", m_value);
	}
}