
#include "UltimateGauge.h"

#include <algorithm>
#include <cmath>

void UltimateGauge::Init()
{
	//=======================================
	// 必殺技アイコン
	//=======================================
	m_iconTex = std::make_shared<KdTexture>();
	m_iconTex->Load(
		"Asset/Textures/UI/Gauge/Ultimate/UltimateIcon.png"
	);

	//=======================================
	// 初期値
	//=======================================
	m_gauge = 0.0f;
	m_gaugeMax = 500.0f;

	m_blinkTimer = 0.0f;
	m_blinkAlpha = 1.0f;
	m_visible = true;
}

void UltimateGauge::Update()
{
	//=======================================
	// 満タン時だけ点滅
	//=======================================
	if (m_gaugeMax > 0.0f && m_gauge >= m_gaugeMax)
	{
		m_blinkTimer += 1.0f / 60.0f;

		// アルファ値を0.35～1.0の間で変化させる
		m_blinkAlpha =
			0.675f + 0.325f * std::sin(m_blinkTimer * 6.0f);
	}
	else
	{
		m_blinkTimer = 0.0f;
		m_blinkAlpha = 1.0f;
	}
}

void UltimateGauge::SetGauge(float gauge, float gaugeMax)
{
	m_gaugeMax = (gaugeMax > 0.0f) ? gaugeMax : 1.0f;
	m_gauge = std::clamp(gauge, 0.0f, m_gaugeMax);
}

void UltimateGauge::DrawSprite()
{
	if (!m_visible) { return; }
	if (m_gaugeMax <= 0.0f) { return; }
	if (!m_iconTex) { return; }

	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	const bool isMax = (m_gauge >= m_gaugeMax);

	// アイコンの位置とサイズ
	const float iconX = -550.0f;
	const float iconY = 265.0f;
	const float iconSize = 40.0f;

	// 通常時の色
	Math::Color iconColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 必殺技ポイントが満タンならピンク色に点滅
	if (isMax)
	{
		iconColor = {
			1.0f,
			0.35f,
			0.9f,
			m_blinkAlpha
		};
	}

	// アイコンを描画
	sprite.DrawTex(
		m_iconTex.get(),
		iconX,
		iconY,
		iconSize,
		iconSize,
		nullptr,
		&iconColor,
		{ 0.5f, 0.5f }
	);
}