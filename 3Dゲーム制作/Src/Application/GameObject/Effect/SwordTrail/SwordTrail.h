#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class Player;

//=======================================
// 剣軌跡用ポリゴン
//=======================================
class SwordTrailPolygon : public KdPolygon
{
public:

	void SetVertices(
		const Math::Vector3& prevBase,
		const Math::Vector3& prevTip,
		const Math::Vector3& nowBase,
		const Math::Vector3& nowTip);
};


class SwordTrail : public BaseObject
{
public:

	void Init() override;
	void Update() override;
	void DrawEffect() override;

	//---------------------------------------
	// プレイヤー設定
	//---------------------------------------
	void SetPlayer(const std::shared_ptr<Player>& player)
	{
		m_player = player;
	}

private:

	std::weak_ptr<Player> m_player;

	//---------------------------------------
	// 前フレームの剣位置
	//---------------------------------------
	Math::Vector3 m_prevBasePos;
	Math::Vector3 m_prevTipPos;

	//---------------------------------------
	// 現在の剣位置
	//---------------------------------------
	Math::Vector3 m_nowBasePos;
	Math::Vector3 m_nowTipPos;

	//---------------------------------------
	// 軌跡ポリゴン
	//---------------------------------------
	std::shared_ptr<SwordTrailPolygon> m_trailPolygon;

	//---------------------------------------
	// 軌跡状態
	//---------------------------------------
	bool m_wasTrailActive = false;
};