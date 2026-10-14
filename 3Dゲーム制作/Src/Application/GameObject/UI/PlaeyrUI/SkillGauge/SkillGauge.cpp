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
	if (!m_visible) { return; }

	// 最大値が0以下なら描画しない
	if (m_gaugeMax <= 0.0f) { return; }

	// ゲージ割合を0～1に制限
	float rate = m_gauge / m_gaugeMax;
	rate = std::clamp(rate, 0.0f, 1.0f);

	float fullWidth = 225.0f;  // ゲージ最大幅
	float nowWidth = fullWidth * rate;
	float barHeight = 20.0f;

	float x = -575.0f;
	float y = 290.0f;

	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	//===========================
	// ① 最大ゲージ背景（黒）
	//===========================
	Math::Color backColor = { 0, 0, 0, 1 };

	sprite.DrawTex(
		m_barTex.get(),
		x, y,
		fullWidth, barHeight,
		nullptr,
		&backColor,
		{ 0, 0 }
	);

	//===========================
	// ② 現在のスキルゲージ
	//===========================
	if (rate > 0.0f)
	{
		Math::Color color = { 1.0f, 0.2f, 0.8f, 1.0f };

		sprite.DrawTex(
			m_barTex.get(),
			x, y,
			nowWidth, barHeight,
			nullptr,
			&color,
			{ 0, 0 }
		);
	}

	//===========================
	// ③ MPラベル
	//===========================
	auto& spriteShader =
		KdShaderManager::Instance().m_spriteShader;

	KdSpriteShader::FontParam param;

	param.pos = { x - 35.0f, y };
	param.scale = 1.25f;
	param.pivot = { 0.5f, 0.5f };

	// 黒い縁取り
	Math::Color outlineColor = { 0, 0, 0, 1 };
	const float outlineSize = 1.0f;

	const Math::Vector2 offsets[] =
	{
		{ -outlineSize,  0 },
		{  outlineSize,  0 },
		{  0, -outlineSize },
		{  0,  outlineSize }
	};

	for (const auto& offset : offsets)
	{
		param.pos =
		{
			x - 20.0f + offset.x,
			y + 10.0f + offset.y
		};

		param.color = outlineColor;

		spriteShader.DrawFontEx(
			param,
			"%s",
			"MP"
		);
	}

	param.pos = { x - 20.0f, y + 10.0f };
	param.color = { 1.0f, 0.2f, 0.8f, 1.0f };

	spriteShader.DrawFontEx(
		param,
		"%s",
		"MP"
	);
}