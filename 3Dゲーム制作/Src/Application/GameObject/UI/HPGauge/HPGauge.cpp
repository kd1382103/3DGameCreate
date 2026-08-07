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

	float fullWidth = 200.0f;

	float rate = m_hp / m_hpMax;
	if (rate < 0.0f) rate = 0.0f;
	if (rate > 1.0f) rate = 1.0f;

	float scale = 1.0f;
	if (m_mode == GaugeMode::World)
	{
		scale = m_scale;
	}

	float nowWidth = fullWidth * rate * scale;
	float barHeight = 20.0f * scale;

	float x, y;

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

		Math::Vector3 pos = m_worldPos;

		Math::Vector2 screen = cam->WorldToScreen(pos);

		x = screen.x - (fullWidth * 0.5f);
		y = screen.y;

		// 描画サイズは scale を掛ける
		float nowWidth = fullWidth * rate * scale;
		float barHeight = 20.0f * scale;
	}

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
		barHeight,
		nullptr,
		&color,
		{ 0, 0 }
	);

	//===========================
	// ② 赤バー（滑らかに縮む）
	//===========================
	if (m_showDamageEffect && m_damageBarWidth > 0.0f)
	{
		if (m_damageDelayTimer < m_damageDelay)
		{
			m_damageDelayTimer += 0.016f;
		}
		else
		{
			m_damageBarWidth -= m_damageShrinkSpeed * 0.016f;
			if (m_damageBarWidth <= 0.0f)
			{
				m_damageBarWidth = 0.0f;
				m_showDamageEffect = false;
			}
		}

		// 赤バーが緑バーを超えないように制限
		if (m_damageBarWidth + nowWidth > fullWidth)
		{
			m_damageBarWidth = fullWidth - nowWidth;
		}

		Math::Color dmgColor = { 1, 0, 0, 1 };
		sprite.DrawTex(
			m_barTex.get(),
			x + nowWidth,
			y,
			m_damageBarWidth,
			barHeight,
			nullptr,
			&dmgColor,
			{ 0, 0 }
		);
	}

	//KdDebugGUI::Instance().AddLog(
	//	"mode=%d hp=%.1f max=%.1f rate=%.3f nowWidth=%.1f\n",
	//	(int)m_mode, m_hp, m_hpMax, rate, nowWidth
	//);
}