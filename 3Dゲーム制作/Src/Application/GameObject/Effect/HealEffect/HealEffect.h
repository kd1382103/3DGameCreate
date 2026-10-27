#pragma once

#include <Application/GameObject/Effect/EffectBase/EffectBase.h>

class Player;

class HealEffect : public EffectBase
{
public:
	HealEffect() = default;
	~HealEffect() override = default;

	void Init(const Math::Vector3& pos);
	void SetPlayer(const std::shared_ptr<Player>& player);

	void Update() override;
	void DrawEffect() override;

private:
	std::weak_ptr<Player> m_wpPlayer;
	std::weak_ptr<KdEffekseerObject> m_wpEffect;
};

