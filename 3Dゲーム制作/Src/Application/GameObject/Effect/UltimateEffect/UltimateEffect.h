#pragma once

#include <Application/GameObject/Effect/EffectBase/EffectBase.h>

class UltimateEffect : public EffectBase
{
public:

	UltimateEffect() = default;
	~UltimateEffect() override = default;

	void Init(
		const Math::Vector3& pos,
		const Math::Vector3& direction,
		float hitInterval,
		int maxHitCount,
		float travelDistance
	);

	void Update() override;

	void DrawEffect() override;

private:

	//==========================================================
	// 斬撃ポリゴン
	//==========================================================
	std::shared_ptr<KdSquarePolygon> m_polygon;

	//==========================================================
	// 飛んでいく方向
	//==========================================================
	Math::Vector3 m_direction = { 0, 0, 1 };

	//==========================================================
	// 大きさ
	//==========================================================
	float m_width = 2.5f;
	float m_height = 4.0f;

	//==========================================================
	// 移動速度
	//==========================================================
	float m_moveSpeed = 0.0f;

	//==========================================================
	// 表示時間
	//==========================================================
	float m_effectDuration = 0.0f;

	//==========================================================
	// 残像
	//==========================================================
	std::vector<Math::Vector3> m_trailPositions;

	// 残像を残す数
	static constexpr int kTrailCount = 5;

	// 残像を記録する間隔
	float m_trailTimer = 0.0f;

	// 何秒ごとに残像を記録するか
	static constexpr float kTrailInterval = 0.025f;
};