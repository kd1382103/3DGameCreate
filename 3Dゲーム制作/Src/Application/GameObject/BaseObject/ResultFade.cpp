#include "ResultFade.h"

#include <Application/main.h>

//==================================================
// 初期化
//==================================================
void ResultFade::Init()
{
	m_alpha = 0.0f;

	m_timer = 0.0f;
	m_duration = 1.0f;

	m_isFadeIn = false;
	m_isFadeOut = false;
	m_isFinished = true;
}

//==================================================
// 暗転開始
//==================================================
void ResultFade::StartFadeIn(float duration)
{
	m_timer = 0.0f;
	m_duration = (std::max)(duration, 0.01f);

	m_isFadeIn = true;
	m_isFadeOut = false;
	m_isFinished = false;
}

//==================================================
// 暗転解除
//==================================================
void ResultFade::StartFadeOut(float duration)
{
	m_timer = 0.0f;
	m_duration = (std::max)(duration, 0.01f);

	m_isFadeIn = false;
	m_isFadeOut = true;
	m_isFinished = false;
}

//==================================================
// 更新
//==================================================
void ResultFade::Update()
{
	if (!m_isFadeIn && !m_isFadeOut)
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

	if (m_isFadeIn)
	{
		m_alpha = smoothT;
	}
	else
	{
		m_alpha = 1.0f - smoothT;
	}

	// 演出終了
	if (t >= 1.0f)
	{
		m_isFadeIn = false;
		m_isFadeOut = false;
		m_isFinished = true;

		if (m_alpha < 0.5f)
		{
			m_alpha = 0.0f;
		}
		else
		{
			m_alpha = 1.0f;
		}
	}
}

//==================================================
// 描画
//==================================================
void ResultFade::DrawSprite()
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