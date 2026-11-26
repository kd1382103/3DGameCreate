#include "EnemyCharacterBase.h"

#include <Application/GameObject/UI/HPGauge/HPGauge.h>

//==============================================================
// アニメーション再生
//==============================================================
void EnemyCharacterBase::PlayAnimationAuto(
	const std::string& animName,
	bool loop)
{
	if (!m_model) return;

	auto anim = m_model->GetAnimation(animName);

	if (anim)
	{
		m_animator.SetAnimation(anim, loop);
	}
}

//==============================================================
// HPゲージ表示設定
//==============================================================
void EnemyCharacterBase::SetHPGaugeVisible(bool visible)
{
	if (m_hpGauge)
	{
		m_hpGauge->SetVisible(visible);
	}
}