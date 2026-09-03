#pragma once

#include <Application/GameObject/Effect/EffectBase/EffectBase.h>

//==============================================================
// 必殺技エフェクト用ポリゴン
//==============================================================
class UltimateEffectPolygon : public KdPolygon
{
public:

	void SetVertices(float width, float height);
};


//==============================================================
// 必殺技エフェクト
//==============================================================
class UltimateEffect : public EffectBase
{
public:

	UltimateEffect() = default;
	~UltimateEffect() override = default;

	void Init(
		const Math::Vector3& pos,
		const Math::Vector3& direction
	);

	void Update() override;

	void DrawEffect() override;

private:

	std::shared_ptr<UltimateEffectPolygon> m_polygon;

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
	float m_moveSpeed = 0.25f;
};