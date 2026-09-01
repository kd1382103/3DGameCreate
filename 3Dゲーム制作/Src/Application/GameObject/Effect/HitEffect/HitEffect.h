#pragma once

#include <Application/GameObject/Effect/EffectBase/EffectBase.h>

//==============================================================
// 被弾エフェクト用ポリゴン
//==============================================================
class HitEffectPolygon : public KdPolygon
{
public:

	void SetVertices(float size);
};


//==============================================================
// 血しぶき1粒
//==============================================================
struct HitParticle
{
	Math::Vector3 pos;
	Math::Vector3 velocity;

	float size = 0.1f;
};


//==============================================================
// 被弾エフェクト
//==============================================================
class HitEffect : public EffectBase
{
public:

	HitEffect() = default;
	~HitEffect() override = default;

	void Init(const Math::Vector3& pos);

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
	std::shared_ptr<HitEffectPolygon> m_polygon;

	//==========================================================
	// 血しぶき粒子
	//==========================================================
	std::vector<HitParticle> m_particles;

	//==========================================================
	// 基準サイズ
	//==========================================================
	float m_size = 0.2f;
};