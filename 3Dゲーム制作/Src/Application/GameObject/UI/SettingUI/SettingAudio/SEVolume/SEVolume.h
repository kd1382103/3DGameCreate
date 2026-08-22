#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class SEVolume : public BaseObject
{
public:

	void Init() override;
	void Update() override;
	void DrawSprite() override;

	//---------------------------------------
	// 音量取得
	//---------------------------------------
	float GetVolume() const
	{
		return s_volume;
	}

private:

	//---------------------------------------
	// マウス判定
	//---------------------------------------
	bool IsMouseOverBar() const;

private:

	//---------------------------------------
	// 音量
	//---------------------------------------
	static float s_volume;

	//---------------------------------------
	// バー位置
	//---------------------------------------
	float m_barX = -200.0f;
	float m_barY = -100.0f;

	//---------------------------------------
	// バーサイズ
	//---------------------------------------
	float m_barWidth = 400.0f;
	float m_barHeight = 20.0f;

	//---------------------------------------
	// 操作中
	//---------------------------------------
	bool m_dragging = false;
};