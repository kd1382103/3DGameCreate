#include "ScreenFade.h"

#include <Application/main.h>

//==================================================
// 初期化
//==================================================
void ScreenFade::Init()
{
	m_alpha = 0.0f;

	m_timer = 0.0f;
	m_duration = 1.0f;

	m_isFadeIn = false;
	m_isFinished = true;
}

//==================================================
// 暗転開始
//==================================================
void ScreenFade::StartFadeIn(float duration)
{
	StartFade(true, duration);
}

//==================================================
// 暗転解除
//==================================================
void ScreenFade::StartFadeOut(float duration)
{
	StartFade(false, duration);
}

//==================================================
// 暗転開始・解除の共通処理
//==================================================
void ScreenFade::StartFade(bool isFadeIn, float duration)
{
	m_timer = 0.0f;
	m_duration = (std::max)(duration, 0.01f);

	m_isFadeIn = isFadeIn;
	m_isFinished = false;
}

//==================================================
// 更新
//==================================================
void ScreenFade::Update()
{
	if (m_isFinished)
	{
		return;
	}

	m_timer += Application::Instance().GetDeltaTime();

	float t = std::clamp(
		m_timer / m_duration,
		0.0f,
		1.0f
	);

	// 滑らかな変化
	float smoothT = t * t * (3.0f - 2.0f * t);

	m_alpha = m_isFadeIn ? smoothT : 1.0f - smoothT;

	// 演出終了
	if (t >= 1.0f)
	{
		m_isFinished = true;
	}
}

//==================================================
// 描画
//==================================================
void ScreenFade::DrawSprite()
{
	if (m_alpha <= 0.0f)
	{
		return;
	}

	auto& shaderManager = KdShaderManager::Instance();
	auto& shader = shaderManager.m_spriteShader;

	Math::Color color = {
		0.0f,
		0.0f,
		0.0f,
		m_alpha
	};

	shaderManager.ChangeBlendState(KdBlendState::Alpha);

	// 画面全体を覆う
	shader.DrawBox(
		0,
		0,
		640,
		360,
		&color,
		true
	);

	shaderManager.UndoBlendState();
}