#pragma once

//============================================================
// Display設定
//============================================================
class SettingDisplay
{
public:

	void Init();
	void Update();
	void DrawSprite();

private:

	//---------------------------------------
	// FPS変更
	//---------------------------------------
	void ChangeFPS(int direction);

	//---------------------------------------
	// 表示モード変更
	//---------------------------------------
	void ChangeDisplayMode(int direction);

	//---------------------------------------
	// マウス判定
	//---------------------------------------
	bool IsMouseOver(
		float x,
		float y,
		float width,
		float height
	) const;

private:

	//============================================================
	// FPS
	//============================================================
	static int s_fpsIndex;

	static constexpr int kFPSCount = 5;

	static constexpr int kFPSList[kFPSCount] =
	{
		30,
		60,
		120,
		144,
		0
	};

	//============================================================
	// 表示モード
	//============================================================
	enum class DisplayMode
	{
		Window1280x720,
		Window1920x1080,
		Fullscreen
	};

	static int s_displayModeIndex;

	static constexpr int kDisplayModeCount = 3;

	static constexpr DisplayMode kDisplayModeList[kDisplayModeCount] =
	{
		DisplayMode::Window1280x720,
		DisplayMode::Window1920x1080,
		DisplayMode::Fullscreen
	};

	//============================================================
	// 表示位置
	//============================================================

	float m_titleX = 100.0f;
	float m_titleY = 200.0f;

	float m_fpsX = 210.0f;
	float m_fpsY = 20.0f;

	float m_fpsLeftX = 80.0f;
	float m_fpsRightX = 340.0f;

	float m_displayModeX = 210.0f;
	float m_displayModeY = 140.0f;

	float m_displayModeLeftX = 80.0f;
	float m_displayModeRightX = 340.0f;

	//---------------------------------------
	// ボタンサイズ
	//---------------------------------------
	float m_buttonWidth = 80.0f;
	float m_buttonHeight = 60.0f;

	//---------------------------------------
	// クリック判定
	//---------------------------------------
	bool m_fpsLeftClick = false;
	bool m_fpsRightClick = false;

	bool m_displayModeLeftClick = false;
	bool m_displayModeRightClick = false;
};