#pragma once
#include <Application/GameObject/BaseObject/BaseObject.h>

class HPGauge : public BaseObject
{
public:
	void Init() override;
	void DrawSprite() override;

	void SetGauge(float hp, float hpMax);

private:
	std::shared_ptr<KdTexture> m_barTex;
	std::shared_ptr<KdTexture> m_frameTex;

	float m_hp = 0.0f;
	float m_hpMax = 1.0f;
};
