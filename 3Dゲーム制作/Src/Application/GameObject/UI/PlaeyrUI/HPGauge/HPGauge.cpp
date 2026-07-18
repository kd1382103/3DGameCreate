#include "HPGauge.h"

void HPGauge::Init()
{
	m_barTex = std::make_shared<KdTexture>();
	m_barTex->Load("Asset/Textures/UI/Player/HP/HPGauge1.png");

}

void HPGauge::SetGauge(float hp, float hpMax)
{
	m_hp = hp;
	m_hpMax = hpMax;
}

void HPGauge::DrawSprite()
{
	float rate = m_hp / m_hpMax;
	if (rate < 0.0f) rate = 0.0f;

	float fullWidth = 200.0f;
	float nowWidth = fullWidth * rate;

	float x = -500.0f;
	float y = 275.0f;

	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	//===========================
	// ① 通常のHPバー（緑）
	//===========================
	Math::Color color = { 0, 1, 0, 1 };
	sprite.DrawTex(
		m_barTex.get(),
		x,
		y,
		nowWidth,
		20,
		nullptr,
		&color,
		{ 0, 0 }
	);

	//===========================
	// ② 赤バー（滑らかに縮む）
	//===========================
	if (m_showDamageEffect && m_damageBarWidth > 0.0f)
	{
		//===========================
		// ① 一定時間そのまま表示
		//===========================
		if (m_damageDelayTimer < m_damageDelay)
		{
			m_damageDelayTimer += 0.016f; // 1フレーム
		}
		else
		{
			//===========================
			// ② 遅延後にゆっくり縮む
			//===========================
			m_damageBarWidth -= m_damageShrinkSpeed * 0.016f;

			if (m_damageBarWidth <= 0.0f)
			{
				m_damageBarWidth = 0.0f;
				m_showDamageEffect = false;
			}
		}

		Math::Color dmgColor = { 1, 0, 0, 1 };

		sprite.DrawTex(
			m_barTex.get(),
			x + nowWidth,
			y,
			m_damageBarWidth,
			20,
			nullptr,
			&dmgColor,
			{ 0, 0 }
		);
	}

}
