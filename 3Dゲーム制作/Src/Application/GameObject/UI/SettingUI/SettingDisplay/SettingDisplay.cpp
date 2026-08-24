#include "SettingDisplay.h"

#include <Application/main.h>

//============================================================
// 静的変数
//============================================================
int SettingDisplay::s_fpsIndex = 1;

//============================================================
// 初期化
//============================================================
void SettingDisplay::Init()
{
	//s_fpsIndex = 1;

	//Application::Instance()
	//	.GetFPSController()
	//	.SetMaxFPS(kFPSList[s_fpsIndex]);
}

//============================================================
// 更新
//============================================================
void SettingDisplay::Update()
{
	//---------------------------------------
	// FPS 左
	//---------------------------------------
	bool fpsLeftOver =
		IsMouseOver(
			m_fpsLeftX,
			m_fpsY,
			m_buttonWidth,
			m_buttonHeight
		);

	//---------------------------------------
	// FPS 右
	//---------------------------------------
	bool fpsRightOver =
		IsMouseOver(
			m_fpsRightX,
			m_fpsY,
			m_buttonWidth,
			m_buttonHeight
		);

	//---------------------------------------
	// FPS 左クリック
	//---------------------------------------
	if (fpsLeftOver &&
		(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		if (!m_fpsLeftClick)
		{
			ChangeFPS(-1);

			m_fpsLeftClick = true;
		}
	}
	else
	{
		m_fpsLeftClick = false;
	}

	//---------------------------------------
	// FPS 右クリック
	//---------------------------------------
	if (fpsRightOver &&
		(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		if (!m_fpsRightClick)
		{
			ChangeFPS(1);

			m_fpsRightClick = true;
		}
	}
	else
	{
		m_fpsRightClick = false;
	}
}

//============================================================
// 描画
//============================================================
void SettingDisplay::DrawSprite()
{
	auto& shader =
		KdShaderManager::Instance().m_spriteShader;

	//========================================================
	// タイトル
	//========================================================
	{
		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_titleX,
			m_titleY
		};

		param.color = kWhiteColor;
		param.scale = 1.0f;
		param.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			param,
			"< ディスプレイ設定 >"
		);
	}

	//========================================================
	// 解像度
	//========================================================
	{
		KdSpriteShader::FontParam label;

		label.pos =
		{
			-20.0f,
			m_resolutionY
		};

		label.color = kWhiteColor;
		label.scale = 1.0f;
		label.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			label,
			"解像度"
		);
	}

	//---------------------------------------
	// 解像度 左
	//---------------------------------------
	{
		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_resolutionLeftX,
			m_resolutionY
		};

		param.color = kWhiteColor;
		param.scale = 1.0f;
		param.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			param,
			"<"
		);
	}

	//---------------------------------------
	// 解像度
	//---------------------------------------
	{
		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_resolutionX,
			m_resolutionY
		};

		param.color = kWhiteColor;
		param.scale = 1.0f;
		param.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			param,
			"1280 x 720"
		);
	}

	//---------------------------------------
	// 解像度 右
	//---------------------------------------
	{
		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_resolutionRightX,
			m_resolutionY
		};

		param.color = kWhiteColor;
		param.scale = 1.0f;
		param.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			param,
			">"
		);
	}

	//========================================================
	// FPS
	//========================================================
	{
		KdSpriteShader::FontParam label;

		label.pos =
		{
			-20.0f,
			m_fpsY
		};

		label.color = kWhiteColor;
		label.scale = 1.0f;
		label.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			label,
			"FPS"
		);
	}

	//---------------------------------------
	// FPS 左
	//---------------------------------------
	{
		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_fpsLeftX,
			m_fpsY
		};

		param.color = kWhiteColor;
		param.scale = 1.0f;
		param.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			param,
			"<"
		);
	}

	//---------------------------------------
	// FPS 現在値
	//---------------------------------------
	{
		std::string fpsText;

		if (kFPSList[s_fpsIndex] == 0)
		{
			fpsText = "Unlimited";
		}
		else
		{
			fpsText =
				std::to_string(
					kFPSList[s_fpsIndex]
				);
		}

		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_fpsX,
			m_fpsY
		};

		param.color = kWhiteColor;
		param.scale = 1.0f;
		param.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			param,
			"%s",
			fpsText.c_str()
		);
	}

	//---------------------------------------
	// FPS 右
	//---------------------------------------
	{
		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_fpsRightX,
			m_fpsY
		};

		param.color = kWhiteColor;
		param.scale = 1.0f;
		param.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			param,
			">"
		);
	}
}

//============================================================
// FPS変更
//============================================================
void SettingDisplay::ChangeFPS(int direction)
{
	s_fpsIndex += direction;

	//---------------------------------------
	// ループ
	//---------------------------------------
	if (s_fpsIndex < 0)
	{
		s_fpsIndex =
			kFPSCount - 1;
	}

	if (s_fpsIndex >= kFPSCount)
	{
		s_fpsIndex = 0;
	}

	//---------------------------------------
	// FPSControllerへ反映
	//---------------------------------------
	Application::Instance()
		.GetFPSController()
		.SetMaxFPS(
			kFPSList[s_fpsIndex]
		);
}

//============================================================
// マウス判定
//============================================================
bool SettingDisplay::IsMouseOver(
	float x,
	float y,
	float width,
	float height
) const
{
	POINT mousePos;

	GetCursorPos(&mousePos);

	HWND hWnd =
		Application::Instance()
		.GetWindowHandle();

	ScreenToClient(
		hWnd,
		&mousePos
	);

	float mouseX =
		static_cast<float>(mousePos.x)
		- 640.0f;

	float mouseY =
		360.0f
		- static_cast<float>(mousePos.y);

	return
		mouseX >= x - width * 0.5f &&
		mouseX <= x + width * 0.5f &&
		mouseY >= y - height * 0.5f &&
		mouseY <= y + height * 0.5f;
}