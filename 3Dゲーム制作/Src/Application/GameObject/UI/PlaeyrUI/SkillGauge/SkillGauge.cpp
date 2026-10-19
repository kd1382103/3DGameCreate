#include "SkillGauge.h"

#include <algorithm>
#include <cmath>

void SkillGauge::Init()
{
	// MPゲージ本体
	m_barTex = std::make_shared<KdTexture>();
	m_barTex->Load(
		"Asset/Textures/UI/Gauge/Skill/SkillGauge1.png"
	);

	m_gauge = 0.0f;
	m_gaugeMax = 500.0f;

	m_blinkTimer = 0.0f;
	m_blinkAlpha = 1.0f;
	m_visible = true;
}

void SkillGauge::Update()
{
	// MP満タン時だけ点滅
	if (m_gaugeMax > 0.0f && m_gauge >= m_gaugeMax)
	{
		m_blinkTimer += 1.0f / 60.0f;

		m_blinkAlpha =
			0.675f + 0.325f * std::sin(m_blinkTimer * 6.0f);
	}
	else
	{
		m_blinkTimer = 0.0f;
		m_blinkAlpha = 1.0f;
	}
}

void SkillGauge::SetGauge(float gauge, float gaugeMax)
{
	m_gaugeMax = (gaugeMax > 0.0f) ? gaugeMax : 1.0f;
	m_gauge = std::clamp(gauge, 0.0f, m_gaugeMax);
}

void SkillGauge::DrawSprite()
{
	if (!m_visible) { return; }
	if (m_gaugeMax <= 0.0f) { return; }
	if (!m_barTex) { return; }

	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	// MPの割合
	const float rate = std::clamp(
		m_gauge / m_gaugeMax,
		0.0f,
		1.0f
	);

	const bool isMax = (m_gauge >= m_gaugeMax);

	// ゲージの位置とサイズ
	const float x = -575.0f;
	const float y = -335.0f;
	const float fullWidth = 225.0f;
	const float barHeight = 20.0f;
	const float nowWidth = fullWidth * rate;

	// 背景ゲージ
	Math::Color backColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	sprite.DrawTex(
		m_barTex.get(),
		x, y,
		fullWidth, barHeight,
		nullptr,
		&backColor,
		{ 0, 0 }
	);

	// 現在のMPゲージ
	if (rate > 0.0f)
	{
		Math::Color gaugeColor;

		if (isMax)
		{
			// 満タン時はピンク色で点滅
			gaugeColor = {
				1.0f,
				0.35f,
				0.9f,
				m_blinkAlpha
			};
		}
		else
		{
			// 通常時はピンク色
			gaugeColor = {
				1.0f,
				0.2f,
				0.8f,
				1.0f
			};
		}

		// 左から右へ伸びるゲージ
		sprite.DrawTex(
			m_barTex.get(),
			x, y,
			nowWidth, barHeight,
			nullptr,
			&gaugeColor,
			{ 0, 0 }
		);
	}

	// MPラベル
	auto& spriteShader =
		KdShaderManager::Instance().m_spriteShader;

	KdSpriteShader::FontParam param;
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
		param.pos = {
			x - 20.0f + offset.x,
			y +10.0f+ offset.y
		};

		param.color = outlineColor;
		spriteShader.DrawFontEx(param, "%s", "SP");
	}

	// MP文字本体
	param.pos = { x - 20.0f, y + 10.0f };
	param.color = isMax
		? Math::Color(
			1.0f, 0.35f, 0.9f, m_blinkAlpha
		)
		: Math::Color(
			1.0f, 0.2f, 0.8f, 1.0f
		);

	spriteShader.DrawFontEx(param, "%s", "SP");
}