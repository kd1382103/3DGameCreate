#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class SEVolume : public BaseObject
{
public:

	void Init() override;
	void Update() override;
	void DrawSprite() override;

private:

	float m_volume = 1.0f;
};