#include "SettingDisplay.h"

#include <Application/main.h>

//============================================================
// 静的変数
//============================================================
int SettingDisplay::s_fpsIndex = 1;

// 1280 x 720
int SettingDisplay::s_displayModeIndex = 0;

void SettingDisplay::Init()
{
	//---------------------------------------
	// FPS
	//---------------------------------------
	Application::Instance()
		.GetFPSController()
		.SetMaxFPS(
			kFPSList[s_fpsIndex]
		);

	//---------------------------------------
	// 表示モード
	//---------------------------------------
	KdCSVData windowData(
		"Asset/Data/WindowSettings.csv"
	);

	const auto& modeData =
		windowData.GetLine(0);

	//---------------------------------------
	// CSVから表示モードを取得
	//---------------------------------------
	if (!modeData.empty())
	{
		int modeIndex =
			atoi(modeData[0].c_str());

		if (modeIndex >= 0 &&
			modeIndex < kDisplayModeCount)
		{
			s_displayModeIndex = modeIndex;
		}
	}

	//---------------------------------------
	// Applicationへ反映
	//---------------------------------------
	switch (kDisplayModeList[s_displayModeIndex])
	{
	case DisplayMode::Window1280x720:

		Application::Instance()
			.SetDisplayMode(
				Application::DisplayMode::Window1280x720
			);

		break;

	case DisplayMode::Window1920x1080:

		Application::Instance()
			.SetDisplayMode(
				Application::DisplayMode::Window1920x1080
			);

		break;

	case DisplayMode::Fullscreen:

		Application::Instance()
			.SetDisplayMode(
				Application::DisplayMode::Fullscreen
			);

		break;
	}
}
//============================================================
// 更新
//============================================================
void SettingDisplay::Update()
{
	//========================================================
	// FPS
	//========================================================

	bool fpsLeftOver =
		IsMouseOver(
			m_fpsLeftX,
			m_fpsY,
			m_buttonWidth,
			m_buttonHeight
		);

	bool fpsRightOver =
		IsMouseOver(
			m_fpsRightX,
			m_fpsY,
			m_buttonWidth,
			m_buttonHeight
		);

	//---------------------------------------
	// FPS 左
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
	// FPS 右
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


	//========================================================
	// 表示モード
	//========================================================

	bool displayModeLeftOver =
		IsMouseOver(
			m_displayModeLeftX,
			m_displayModeY,
			m_buttonWidth,
			m_buttonHeight
		);

	bool displayModeRightOver =
		IsMouseOver(
			m_displayModeRightX,
			m_displayModeY,
			m_buttonWidth,
			m_buttonHeight
		);

	//---------------------------------------
	// 表示モード 左
	//---------------------------------------
	if (displayModeLeftOver &&
		(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		if (!m_displayModeLeftClick)
		{
			ChangeDisplayMode(-1);

			m_displayModeLeftClick = true;
		}
	}
	else
	{
		m_displayModeLeftClick = false;
	}

	//---------------------------------------
	// 表示モード 右
	//---------------------------------------
	if (displayModeRightOver &&
		(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		if (!m_displayModeRightClick)
		{
			ChangeDisplayMode(1);

			m_displayModeRightClick = true;
		}
	}
	else
	{
		m_displayModeRightClick = false;
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
	// 表示モード
	//========================================================
	{
		KdSpriteShader::FontParam label;

		label.pos =
		{
			-20.0f,
			m_displayModeY
		};

		label.color = kWhiteColor;
		label.scale = 1.0f;
		label.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			label,
			"表示モード"
		);
	}

	//---------------------------------------
	// 左
	//---------------------------------------
	{
		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_displayModeLeftX,
			m_displayModeY
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
	// 現在値
	//---------------------------------------
	{
		std::string displayModeText;

		switch (kDisplayModeList[s_displayModeIndex])
		{
		case DisplayMode::Window1280x720:
			displayModeText = "1280 x 720";
			break;

		case DisplayMode::Window1920x1080:
			displayModeText = "1920 x 1080";
			break;

		case DisplayMode::Fullscreen:
			displayModeText = "フルスクリーン";
			break;
		}

		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_displayModeX,
			m_displayModeY
		};

		param.color = kWhiteColor;
		param.scale = 1.0f;
		param.pivot = { 0.5f, 0.5f };

		shader.DrawFontEx(
			param,
			"%s",
			displayModeText.c_str()
		);
	}

	//---------------------------------------
	// 右
	//---------------------------------------
	{
		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_displayModeRightX,
			m_displayModeY
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
			fpsText = "無制限";
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
		s_fpsIndex = kFPSCount - 1;
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
// 表示モード変更
//============================================================
void SettingDisplay::ChangeDisplayMode(int direction)
{
	//---------------------------------------
	// Index変更
	//---------------------------------------
	s_displayModeIndex += direction;

	//---------------------------------------
	// ループ
	//---------------------------------------
	if (s_displayModeIndex < 0)
	{
		s_displayModeIndex =
			kDisplayModeCount - 1;
	}

	if (s_displayModeIndex >= kDisplayModeCount)
	{
		s_displayModeIndex = 0;
	}

	//---------------------------------------
	// 表示モード取得
	//---------------------------------------
	DisplayMode mode =
		kDisplayModeList[s_displayModeIndex];

	//---------------------------------------
	// Applicationへ反映
	//---------------------------------------
	switch (mode)
	{
	case DisplayMode::Window1280x720:

		Application::Instance()
			.SetDisplayMode(
				Application::DisplayMode::Window1280x720
			);

		break;

	case DisplayMode::Window1920x1080:

		Application::Instance()
			.SetDisplayMode(
				Application::DisplayMode::Window1920x1080
			);

		break;

	case DisplayMode::Fullscreen:

		Application::Instance()
			.SetDisplayMode(
				Application::DisplayMode::Fullscreen
			);

		break;
	}

	//---------------------------------------
	// CSVへ保存
	//---------------------------------------
	KdCSVData windowData(
		"Asset/Data/WindowSettings.csv"
	);

	windowData.SetData(
		0,
		0,
		std::to_string(s_displayModeIndex)
	);

	windowData.Save();
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

	//---------------------------------------
	// 現在の画面サイズ
	//---------------------------------------
	float screenWidth =
		static_cast<float>(
			Application::Instance()
			.GetResolutionWidth()
			);

	float screenHeight =
		static_cast<float>(
			Application::Instance()
			.GetResolutionHeight()
			);

	//---------------------------------------
	// 実画面 → 1280x720の論理座標
	//---------------------------------------
	float scaleX =
		screenWidth / 1280.0f;

	float scaleY =
		screenHeight / 720.0f;

	float mouseX =
		static_cast<float>(mousePos.x)
		/ scaleX
		- 640.0f;

	float mouseY =
		360.0f
		- static_cast<float>(mousePos.y)
		/ scaleY;

	return
		mouseX >= x - width * 0.5f &&
		mouseX <= x + width * 0.5f &&
		mouseY >= y - height * 0.5f &&
		mouseY <= y + height * 0.5f;
}