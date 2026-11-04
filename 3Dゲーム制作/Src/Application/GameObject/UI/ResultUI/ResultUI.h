#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class FontText;
class UIBackground;

class ResultUI : public BaseObject
{
public:
	ResultUI() {}
	~ResultUI() override {}

	void Init() override;
	void Update() override;
	void DrawSprite() override;

	// スコア情報を設定
	void SetScoreData(
		int baseScore,
		int timePenalty,
		int damagePenalty,
		int finalScore,
		const std::string& rank
	);

	// リザルト演出開始
	void Start();

	// 演出終了判定
	bool IsFinished() const
	{
		return m_finished;
	}

private:

	// 文字生成
	std::shared_ptr<FontText> CreateText(
		const std::string& text,
		const Math::Vector2& pos,
		float scale,
		const Math::Color& color
	);

	// 数値表示更新
	void UpdateScoreText();

	// 数値をカンマ区切りにする
	std::string FormatNumber(int value) const;

private:

	// 背景
	std::shared_ptr<UIBackground> m_mainBackground;
	std::shared_ptr<UIBackground> m_rankBackground;

	// 見出し
	std::shared_ptr<FontText> m_titleText;

	// 左側ラベル
	std::shared_ptr<FontText> m_baseLabel;
	std::shared_ptr<FontText> m_timeLabel;
	std::shared_ptr<FontText> m_damageLabel;
	std::shared_ptr<FontText> m_finalLabel;

	// 左側数値
	std::shared_ptr<FontText> m_baseValue;
	std::shared_ptr<FontText> m_timeValue;
	std::shared_ptr<FontText> m_damageValue;
	std::shared_ptr<FontText> m_finalValue;

	// ランク
	std::shared_ptr<FontText> m_rankLabel;
	std::shared_ptr<FontText> m_rankValue;

	// スコア
	int m_baseScore = 0;
	int m_timePenalty = 0;
	int m_damagePenalty = 0;
	int m_finalScore = 0;

	std::string m_rank;

	// アニメーション
	int m_currentStep = 0;
	float m_animationTimer = 0.0f;

	const float m_countDuration = 0.7f;
	const float m_stepInterval = 0.2f;

	bool m_started = false;
	bool m_finished = false;
};