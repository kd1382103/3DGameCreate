#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class ScreenFade : public BaseObject
{
public:

	void Init() override;
	void Update() override;
	void DrawSprite() override;

	// 暗転開始
	void StartFadeIn(float duration = 1.0f);

	// 暗転解除
	void StartFadeOut(float duration = 1.0f);

	// 演出状態
	bool IsFinished() const
	{
		return m_isFinished;
	}

	// 完全に暗転しているか
	bool IsBlack() const
	{
		return m_alpha >= 1.0f;
	}

private:

	// 暗転開始・解除の共通処理
	void StartFade(bool isFadeIn, float duration);

	float m_alpha = 0.0f;

	float m_timer = 0.0f;
	float m_duration = 1.0f;

	bool m_isFadeIn = false;    // true:暗転 false:暗転解除
	bool m_isFinished = true;
};