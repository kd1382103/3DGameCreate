#include "HitEffect.h"

#include <Application/Scene/SceneManager.h>
#include <random>

//==============================================================
// 初期化
//==============================================================
void HitEffect::Init(const Math::Vector3& pos)
{
	EffectBase::Init(EffectType::Hit, pos);

	//==========================================================
	// ランダムな拡大率（元の大きさの90～110％）
	//==========================================================
	static std::mt19937 randomEngine(std::random_device{}());
	static std::uniform_real_distribution<float> scaleDistribution(
		0.9f,
		1.1f
	);

	float randomScale = 0.2f * scaleDistribution(randomEngine);

	//==========================================================
	// Effekseerエフェクト再生
	//==========================================================
	KdEffekseerManager::GetInstance().Play(
		"EffekseerSample/02_Tktk03/ToonHit.efkefc",
		pos,
		randomScale,
		2.0f,
		false
	);
}

//==============================================================
// 更新
//==============================================================
void HitEffect::Update()
{
	// Effekseer側で更新されるため、
	// ここでは特に処理しない
}

//==============================================================
// 描画
//==============================================================
void HitEffect::DrawEffect()
{
	// Effekseer側で描画されるため、
	// ここでは特に処理しない
}