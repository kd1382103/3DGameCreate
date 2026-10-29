#pragma once

#include <Application/GameObject/Effect/EffectBase/EffectBase.h>

class BossDeathEffect : public EffectBase
{
public:
	BossDeathEffect() = default;
	~BossDeathEffect() override = default;

	void Init(const Math::Vector3& pos);

	void Update() override;
	void DrawEffect() override;
};