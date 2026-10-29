#include "BossDeathEffect.h"

//==============================================================
// 初期化
//==============================================================
void BossDeathEffect::Init(const Math::Vector3& pos)
{
	EffectBase::Init(EffectType::Explosion, pos);

	KdEffekseerManager::GetInstance().Play(
		"EffekseerSample/BossDeath/Explosion.efkefc",
		pos,
		0.5f,
		0.5f,
		false
	);
}

//==============================================================
// 更新
//==============================================================
void BossDeathEffect::Update()
{
	// Effekseer側で更新
}

//==============================================================
// 描画
//==============================================================
void BossDeathEffect::DrawEffect()
{
	// Effekseer側で描画
}