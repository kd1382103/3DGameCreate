#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class BGMVolume : public BaseObject
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
	int m_barX = -200;
	int m_barY = 50;

	//---------------------------------------
	// バーサイズ
	//---------------------------------------
	int m_barWidth = 400;
	int m_barHeight = 20;

	//---------------------------------------
	// 操作中
	//---------------------------------------
	bool m_dragging = false;
};