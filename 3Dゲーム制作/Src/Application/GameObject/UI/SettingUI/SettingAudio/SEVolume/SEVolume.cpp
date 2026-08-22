#include "SEVolume.h"

#include <Application/main.h>

void SEVolume::Init()
{
	m_volume = 1.0f;
}

void SEVolume::Update()
{
	//---------------------------------------
	// マウスがバーの上にあるか
	//---------------------------------------
	bool mouseOver = IsMouseOverBar();

	//---------------------------------------
	// 左クリック開始
	//---------------------------------------
	if (mouseOver &&
		(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		m_dragging = true;
	}

	//---------------------------------------
	// 左クリックを離した
	//---------------------------------------
	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		m_dragging = false;
	}

	//---------------------------------------
	// ドラッグ中
	//---------------------------------------
	if (m_dragging)
	{
		POINT mousePos;

		GetCursorPos(&mousePos);

		HWND hWnd = GetActiveWindow();

		if (!hWnd)
		{
			return;
		}

		ScreenToClient(hWnd, &mousePos);

		RECT rect;

		GetClientRect(hWnd, &rect);

		float screenWidth =
			static_cast<float>(rect.right - rect.left);

		//---------------------------------------
		// 画面中央基準のX座標
		//---------------------------------------
		float mouseX =
			static_cast<float>(mousePos.x)
			- screenWidth * 0.5f;

		//---------------------------------------
		// 音量計算
		//---------------------------------------
		m_volume =
			(mouseX - m_barX) / m_barWidth;

		//---------------------------------------
		// 0～1に制限
		//---------------------------------------
		if (m_volume < 0.0f)
		{
			m_volume = 0.0f;
		}

		if (m_volume > 1.0f)
		{
			m_volume = 1.0f;
		}
	}
}

void SEVolume::DrawSprite()
{
	auto& sprite =
		KdShaderManager::Instance().m_spriteShader;

	//---------------------------------------
	// バー背景
	//---------------------------------------
	Math::Color backColor =
	{
		0.2f,
		0.2f,
		0.2f,
		1.0f
	};

	sprite.DrawBox(
		static_cast<int>(m_barX + m_barWidth * 0.5f),
		static_cast<int>(m_barY + m_barHeight * 0.5f),
		static_cast<int>(m_barWidth * 0.5f),
		static_cast<int>(m_barHeight * 0.5f),
		&backColor,
		true
	);

	//---------------------------------------
	// 現在の音量部分
	//---------------------------------------
	float volumeWidth =
		m_barWidth * m_volume;

	Math::Color volumeColor =
	{
		0.0f,
		0.8f,
		1.0f,
		1.0f
	};

	sprite.DrawBox(
		static_cast<int>(
			m_barX + volumeWidth * 0.5f
			),
		static_cast<int>(
			m_barY + m_barHeight * 0.5f
			),
		static_cast<int>(
			volumeWidth * 0.5f
			),
		static_cast<int>(
			m_barHeight * 0.5f
			),
		&volumeColor,
		true
	);

	//---------------------------------------
	// SE文字
	//---------------------------------------
	KdSpriteShader::FontParam param;

	param.pos =
	{
		m_barX,
		m_barY + 60.0f
	};

	param.scale = 1.0f;

	param.pivot =
	{
		0.0f,
		0.5f
	};

	sprite.DrawFontEx(
		param,
		"SE VOLUME"
	);
}

bool SEVolume::IsMouseOverBar() const
{
	POINT mousePos;

	if (!GetCursorPos(&mousePos))
	{
		return false;
	}

	HWND hWnd = GetActiveWindow();

	if (!hWnd)
	{
		return false;
	}

	//---------------------------------------
	// スクリーン座標 → ウィンドウ座標
	//---------------------------------------
	ScreenToClient(hWnd, &mousePos);

	RECT rect;

	GetClientRect(hWnd, &rect);

	float screenWidth =
		static_cast<float>(rect.right - rect.left);

	float screenHeight =
		static_cast<float>(rect.bottom - rect.top);

	//---------------------------------------
	// 画面中央を (0,0) にする
	//---------------------------------------
	float mouseX =
		static_cast<float>(mousePos.x)
		- screenWidth * 0.5f;

	float mouseY =
		screenHeight * 0.5f
		- static_cast<float>(mousePos.y);

	//---------------------------------------
	// バーの範囲
	//---------------------------------------
	float left = m_barX;
	float right = m_barX + m_barWidth;

	float bottom = m_barY;
	float top = m_barY + m_barHeight;

	return
		mouseX >= left &&
		mouseX <= right &&
		mouseY >= bottom &&
		mouseY <= top;
}