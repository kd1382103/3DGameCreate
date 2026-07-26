#include "SkillGauge.h"

void SkillGauge::Init()
{
	// ゲージ本体
	m_barTex = std::make_shared<KdTexture>();
	m_barTex->Load("Asset/Textures/UI/Gauge/Skill/SkillGauge1.png");
}

void SkillGauge::SetGauge(float gauge, float gaugeMax)
{
	m_gauge = gauge;
	m_gaugeMax = gaugeMax;
}

void SkillGauge::DrawSprite()
{
	float rate = m_gauge / m_gaugeMax;

	// ★ ゼロならパッと消える
	if (rate <= 0.0f)
	{
		return;
	}

	float fullWidth = 200.0f;     // ゲージ最大幅
	float nowWidth = fullWidth * rate;
	float x = -500.0f;  
	float y = 250.0f; 

	Math::Color color = { 1, 1, 1, 1 };
	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	// ゲージ本体（左から右に伸びる）
	sprite.DrawTex(
		m_barTex.get(),
		x,y,              // 画面位置
		nowWidth, 20,     // ← 幅だけ変える
		nullptr,
		&color,
		{ 0, 0 }          // pivot 左上固定
	);
}
