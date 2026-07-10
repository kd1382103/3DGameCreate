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
	if (rate <= 0.0f) rate = 0.0f;

	float fullWidth = 200.0f;
	float nowWidth = fullWidth * rate;

	float x = -500.0f;   // ← 画面左上に固定
	float y = 275.0f;

	auto& sprite = KdShaderManager::Instance().m_spriteShader;
	Math::Color color = { 1, 1, 1, 1 };

	// ゲージ本体
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
}
