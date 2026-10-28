
#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class Player;

class SwordTrail : public BaseObject
{
public:

	void Init() override;
	void Update() override;
	void DrawEffect() override;

	void SetPlayer(const std::shared_ptr<Player>& player)
	{
		m_player = player;
	}

private:

	//==================================================
	// 剣の位置からPointLightを登録
	//==================================================
	void UpdateSwordPointLights(
		const Math::Vector3& basePos,
		const Math::Vector3& tipPos,
		float intensity
	);

private:

	std::weak_ptr<Player> m_player;

	//==================================================
	// 剣の軌跡
	//==================================================
	std::shared_ptr<KdTrailPolygon> m_tPoly;

	//==================================================
	// 軌跡を表示するか
	//==================================================
	bool m_wasTrailActive = false;

	//==================================================
	// 軌跡ポイント追加用タイマー
	//==================================================
	float m_trailTimer = 0.0f;

	//==================================================
	// 軌跡ポイント追加間隔
	//==================================================
	static constexpr float TrailInterval = 1.0f / 60.0f;

	//==================================================
	// 剣の発光強度（0.0～1.0）
	//==================================================
	float m_lightIntensity = 0.0f;

	//==================================================
	// 発光フェード速度
	//==================================================
	static constexpr float LightFadeInSpeed = 3.0f;
	static constexpr float LightFadeOutSpeed = 4.0f;
};
