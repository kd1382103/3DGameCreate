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
};