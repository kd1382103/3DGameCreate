#pragma once

#include <Application/GameObject/Effect/EffectBase/EffectBase.h>

//==============================================================
// 火花エフェクト用ポリゴン
//==============================================================
class SparkEffectPolygon : public KdPolygon
{
public:

	void SetVertices(
		float length,
		float width);
};


//==============================================================
// 火花1粒
//==============================================================
struct SparkParticle
{
	Math::Vector3 pos;
	Math::Vector3 velocity;

	// 火花の長さ
	float length = 0.3f;

	// 火花の太さ
	float width = 0.03f;
};


//==============================================================
// 火花エフェクト
//==============================================================
class SparkEffect : public EffectBase
{
public:

	SparkEffect() = default;
	~SparkEffect() override = default;

	void Init(
		const Math::Vector3& pos,
		const Math::Vector3& direction);

	void Update() override;

	void DrawEffect() override;

private:

	//==========================================================
	// 粒子生成
	//==========================================================
	void CreateParticles();

private:

	//==========================================================
	// ポリゴン
	//==========================================================
	std::shared_ptr<SparkEffectPolygon> m_polygon;

	//==========================================================
	// 火花粒子
	//==========================================================
	std::vector<SparkParticle> m_particles;

	//========================================================== 
	// 攻撃方向 
	//==========================================================
	Math::Vector3 m_direction =
		Math::Vector3::Forward;
};