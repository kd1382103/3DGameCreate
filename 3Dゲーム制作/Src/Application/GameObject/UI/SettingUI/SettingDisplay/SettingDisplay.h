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
	// 表示位置
	//============================================================

	// 設定タイトル
	float m_titleX = 100.0f;
	float m_titleY = 220.0f;

	// FPS
	float m_fpsX = 180.0f;
	float m_fpsY = 100.0f;

	// FPS 左右
	float m_fpsLeftX = 20.0f;
	float m_fpsRightX = 340.0f;

	// 解像度
	float m_resolutionX = 180.0f;
	float m_resolutionY = 180.0f;

	// 解像度 左右
	float m_resolutionLeftX = 20.0f;
	float m_resolutionRightX = 340.0f;

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

	bool m_resolutionLeftClick = false;
	bool m_resolutionRightClick = false;
};