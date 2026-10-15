#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class SkillGauge : public BaseObject
{
public:
	void Init() override;
	void Update() override;
	void DrawSprite() override;

	void SetGauge(float gauge, float gaugeMax);

	void SetVisible(bool visible)
	{
		m_visible = visible;
	}

private:
	// MPゲージのテクスチャ
	std::shared_ptr<KdTexture> m_barTex = nullptr;
	std::shared_ptr<KdTexture> m_frameTex = nullptr;

	// MP
	float m_gauge = 0.0f;
	float m_gaugeMax = 100.0f;

	// 満タン時の点滅
	float m_blinkTimer = 0.0f;
	float m_blinkAlpha = 1.0f;

	bool m_visible = true;
};