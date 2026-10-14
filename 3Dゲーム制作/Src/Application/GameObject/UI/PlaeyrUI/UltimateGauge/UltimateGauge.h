#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class UltimateGauge : public BaseObject
{
public:
	void Init() override;
	void Update() override;
	void DrawSprite() override;

	// 必殺技ポイントを設定
	void SetGauge(float gauge, float gaugeMax);

	// 表示切り替え
	void SetVisible(bool visible)
	{
		m_visible = visible;
	}

private:
	// 必殺技アイコン
	std::shared_ptr<KdTexture> m_iconTex = nullptr;

	// 現在のポイント
	float m_gauge = 0.0f;
	float m_gaugeMax = 500.0f;

	// 満タン時の点滅
	float m_blinkTimer = 0.0f;
	float m_blinkAlpha = 1.0f;

	bool m_visible = true;
};