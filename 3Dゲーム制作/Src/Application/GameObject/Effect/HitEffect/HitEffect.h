#pragma once

#include <Application/GameObject/Effect/EffectBase/EffectBase.h>

class HitEffect : public EffectBase
{
public:
	HitEffect() = default;
	~HitEffect() override = default;

	void Init(const Math::Vector3& pos);

	void Update() override;

	void DrawEffect() override;
};