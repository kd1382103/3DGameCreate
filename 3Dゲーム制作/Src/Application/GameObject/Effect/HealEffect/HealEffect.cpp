#include "HealEffect.h"

#include <Application/Scene/SceneManager.h>
#include <Application/GameObject/Player/Player/Player.h>

//==============================================================
// Player設定
//==============================================================
void HealEffect::SetPlayer(const std::shared_ptr<Player>& player)
{
	m_wpPlayer = player;
}

//==============================================================
// 初期化
//==============================================================
void HealEffect::Init(const Math::Vector3& pos)
{
	EffectBase::Init(EffectType::Heal, pos);

	m_wpEffect = KdEffekseerManager::GetInstance().Play(
		"effekt_stock/Healing/Heal02.efkefc",
		pos,
		0.2f,
		0.75f,
		false
	);
}

//==============================================================
// 更新
//==============================================================
void HealEffect::Update()
{
	auto player = m_wpPlayer.lock();
	auto effect = m_wpEffect.lock();

	if (!player || !effect)
	{
		return;
	}

	// Playerの足元に追従
	Math::Vector3 pos = player->GetPos() + Math::Vector3(0, 1.0f, 0);

	effect->SetPos(pos);
}

//==============================================================
// 描画
//==============================================================
void HealEffect::DrawEffect()
{
	// Effekseer側で描画
}