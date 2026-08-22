#pragma once
#include <Application/GameObject/BaseObject/BaseObject.h>

class SkillGauge : public BaseObject
{
public:
	void Init() override;
	void DrawSprite() override;

	void SetGauge(float gauge, float gaugeMax);
	void SetVisible(bool visible) { m_visible = visible; }
private:
	float m_gauge = 0.0f;
	float m_gaugeMax = 100.0f;

	std::shared_ptr<KdTexture> m_barTex;
	std::shared_ptr<KdTexture> m_frameTex;

	bool m_visible = true;

};
