#include "BGMVolume.h"

#include <Application/main.h>

//---------------------------------------
// BGM音量
// ※ゲームシーンをまたいでも保持
//---------------------------------------
float BGMVolume::s_volume = 1.0f;

void BGMVolume::Init()
{
}

void BGMVolume::Update()
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
		s_volume =(mouseX - static_cast<float>(m_barX)) / static_cast<float>(m_barWidth);

		//---------------------------------------
		// 0～1に制限
		//---------------------------------------
		if (s_volume < 0.0f)
		{
			s_volume = 0.0f;
		}

		if (s_volume > 1.0f)
		{
			s_volume = 1.0f;
		}
	}
}
void BGMVolume::DrawSprite()
{
	auto& sprite =
		KdShaderManager::Instance().m_spriteShader;

	//---------------------------------------
	// バー背景
	//---------------------------------------
	Math::Color backColor =
	{ 0.2f, 0.2f, 0.2f, 1.0f };

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
		m_barWidth * s_volume;

	Math::Color volumeColor =
	{ 0.0f, 0.8f, 1.0f, 1.0f };

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
	// BGM文字
	//---------------------------------------
	KdSpriteShader::FontParam param;

	param.pos =
	{
		static_cast<float>(m_barX),
		static_cast<float>(m_barY) + 60.0f 
	};

	param.scale = 1.0f;

	param.pivot =
	{ 0.0f, 0.5f };

	sprite.DrawFontEx(
		param,
		"背景音楽 音量"
	);

	//---------------------------------------
	// BGM音量パーセント
	//---------------------------------------
	KdSpriteShader::FontParam percentParam;

	percentParam.pos =
	{
		static_cast<float>(m_barX + m_barWidth + 30.0f),
		static_cast<float>(m_barY + m_barHeight * 0.5f)
	};

	percentParam.scale = 1.0f;

	percentParam.pivot =
	{
		0.0f,
		0.5f
	};

	sprite.DrawFontEx(
		percentParam,
		"%d%%",
		static_cast<int>(s_volume * 100.0f + 0.5f)
	);
}

bool BGMVolume::IsMouseOverBar() const
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
