#include "HPGauge.h"
#include <Application/GameObject/Camera/CameraBase.h>

void HPGauge::Init()
{
	if (!m_barTex)
	{
		m_barTex = std::make_shared<KdTexture>();
		m_barTex->Load("Asset/Textures/UI/Gauge/HP/HPGauge1.png");
	}
}

void HPGauge::DrawSprite()
{
	if (!m_visible) { return; }

	if (m_mode == GaugeMode::World)
	{
		auto cam = m_wpCamera.lock();
		if (!cam) return;

		// カメラの前方ベクトルと敵方向ベクトルを比較
		Math::Vector3 camForward = cam->GetCameraDir();
		Math::Vector3 toEnemy = m_worldPos - cam->GetCameraPos();
		toEnemy.Normalize();

		float dot = camForward.Dot(toEnemy);

		// 背面なら描画しない
		if (dot < 0.0f)
		{
			return;
		}
	}

	//===========================
	// ゲージサイズ計算
	//===========================
	float fullWidth = 200.0f;

	float rate = m_hp / m_hpMax;
	rate = std::clamp(rate, 0.0f, 1.0f);

	float scale = 1.0f;

	if (m_mode == GaugeMode::World)
	{
		scale = m_scale;
	}

	// 最大幅（距離による縮小込み）
	float scaledFullWidth = fullWidth * scale;

	// 現在HPの幅
	float nowWidth = scaledFullWidth * rate;

	// 高さ
	float barHeight = 20.0f * scale;


	//===========================
	// 描画位置
	//===========================
	float x = 0.0f;
	float y = 0.0f;
	
	if (m_mode == GaugeMode::Screen)
	{
		// プレイヤー用固定座標
		x = -500.0f;
		y = 275.0f;
	}
	else
	{
		auto cam = m_wpCamera.lock();
		if (!cam) return;

		Math::Vector2 screen =
			cam->WorldToScreen(m_worldPos);

		// 敵の位置をゲージ中央にする
		x = screen.x - scaledFullWidth * 0.5f;
		y = screen.y;
	}

	auto& sprite = KdShaderManager::Instance().m_spriteShader;

	//===========================
		// ① 最大HP背景（黒）
		// 常に最大幅
		//===========================
	Math::Color backColor = { 0, 0, 0, 1 };

	sprite.DrawTex(
		m_barTex.get(),
		x,
		y,
		scaledFullWidth,
		barHeight,
		nullptr,
		&backColor,
		{ 0, 0 }
	);


	//===========================
	// ② ダメージバー（赤）
	//===========================
	if (m_showDamageEffect && m_damageBarRate > 0.0f)
	{
		if (m_damageDelayTimer < m_damageDelay)
		{
			m_damageDelayTimer += 0.016f;
		}
		else
		{
			// 1秒間に何％縮むか
			m_damageBarRate -=
				(m_damageShrinkSpeed / fullWidth) * 0.016f;

			if (m_damageBarRate <= 0.0f)
			{
				m_damageBarRate = 0.0f;
				m_showDamageEffect = false;
			}
		}

		//===========================
		// 割合 → 実際の幅に変換
		//===========================
		float damageBarWidth =
			scaledFullWidth * m_damageBarRate;

		// 緑＋赤が最大幅を超えないようにする
		if (damageBarWidth + nowWidth > scaledFullWidth)
		{
			damageBarWidth =
				scaledFullWidth - nowWidth;
		}

		Math::Color dmgColor = { 1, 0, 0, 1 };

		sprite.DrawTex(
			m_barTex.get(),
			x + nowWidth,
			y,
			damageBarWidth,
			barHeight,
			nullptr,
			&dmgColor,
			{ 0, 0 }
		);
	}


	//===========================
	// ③ 現在HP（緑）
	//===========================
	Math::Color hpColor = { 0, 1, 0, 1 };

	sprite.DrawTex(
		m_barTex.get(),
		x,
		y,
		nowWidth,
		barHeight,
		nullptr,
		&hpColor,
		{ 0, 0 }
	);
}