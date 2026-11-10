#include "HitEffect.h"

//==============================================================
// 初期化
//==============================================================
void HitEffect::Init(const Math::Vector3& pos)
{
	EffectBase::Init(pos);

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