
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

	const int iconX = -550;
	const int iconY = 265;
	const int iconSize = 40;

	// ゲージの充填率（0.0～1.0）
	const float chargeRate = std::clamp(
		m_gauge / m_gaugeMax,
		0.0f,
		1.0f
	);

	// 元画像のサイズ
	const int texWidth = m_iconTex->GetInfo().Width;
	const int texHeight = m_iconTex->GetInfo().Height;

	if (texWidth <= 0 || texHeight <= 0) { return; }

	// アイコン全体を白色で描画
	Math::Color whiteColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	sprite.DrawTex(
		m_iconTex.get(),
		iconX,
		iconY,
		iconSize,
		iconSize,
		nullptr,
		&whiteColor,
		{ 0.5f, 0.5f }
	);

	// 必殺技ゲージが満タンなら、全体をピンク色で点滅
	if (isMax)
	{
		Math::Color pinkColor = {
			1.0f, 0.35f, 0.9f, m_blinkAlpha
		};

		sprite.DrawTex(
			m_iconTex.get(),
			iconX,
			iconY,
			iconSize,
			iconSize,
			nullptr,
			&pinkColor,
			{ 0.5f, 0.5f }
		);

		return;
	}

	// ゲージが空なら色を重ねない
	if (chargeRate <= 0.0f) { return; }

	// 下から上に向かって色を付ける範囲を計算
	const int fillHeight = static_cast<int>(texHeight * chargeRate);

	if (fillHeight <= 0) { return; }

	// 元画像の下側を切り抜く
	Math::Rectangle srcRect(
		0,
		texHeight - fillHeight,
		texWidth,
		fillHeight
	);

	// 画面上での描画高さ
	const int drawHeight = static_cast<int>(iconSize * chargeRate);

	if (drawHeight <= 0) { return; }

	// アイコンの下端を固定する。
	// pivot の Y を 1.0f にすることで、drawY が下端の座標になる。
	const int bottomY = iconY - iconSize / 2;
	const int drawY = bottomY + drawHeight;

	// チャージ部分をピンク色で重ねる
	Math::Color chargeColor = {
		1.0f, 0.35f, 0.9f, 1.0f
	};

	sprite.DrawTex(
		m_iconTex.get(),
		iconX,
		drawY,
		iconSize,
		drawHeight,
		&srcRect,
		&chargeColor,
		{ 0.5f, 1.0f }
	);
}