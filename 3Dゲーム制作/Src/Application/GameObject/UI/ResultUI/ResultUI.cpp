//#include "ResultUI.h"
//
//#include <Application/GameObject/UI/FontText/FontText.h>
//#include <Application/GameObject/UI/PlaeyrUI/UIBackground/UIBackground.h>
//#include <Application/main.h>
//
////==============================================================
//// 初期化
////==============================================================
//void ResultUI::Init()
//{
//	m_started = false;
//	m_finished = false;
//
//	//==========================================================
//	// 背景パネル
//	//==========================================================
//	m_mainBackground =
//		std::make_shared<UIBackground>();
//
//	m_mainBackground->InitTutorial(
//		{ -250.0f, 0.0f },
//		700.0f,
//		450.0f
//	);
//
//	m_rankBackground =
//		std::make_shared<UIBackground>();
//
//	m_rankBackground->InitTutorial(
//		{ 310.0f, 0.0f },
//		280.0f,
//		450.0f
//	);
//
//	//==========================================================
//	// タイトル
//	//==========================================================
//	m_titleText = CreateText(
//		"RESULT",
//		{ 0.0f, 280.0f },
//		2.5f,
//		{ 1.0f, 1.0f, 1.0f, 1.0f }
//	);
//
//	//==========================================================
//	// 左側：項目名
//	//==========================================================
//	const Math::Color labelColor =
//	{
//		0.75f, 0.8f, 0.9f, 1.0f
//	};
//
//	m_baseLabel = CreateText(
//		"BASE SCORE",
//		{ -430.0f, 150.0f },
//		1.5f,
//		labelColor
//	);
//
//	m_timeLabel = CreateText(
//		"TIME PENALTY",
//		{ -430.0f, 50.0f },
//		1.5f,
//		labelColor
//	);
//
//	m_damageLabel = CreateText(
//		"DAMAGE PENALTY",
//		{ -430.0f, -50.0f },
//		1.5f,
//		labelColor
//	);
//
//	m_finalLabel = CreateText(
//		"FINAL SCORE",
//		{ -430.0f, -160.0f },
//		1.8f,
//		{ 1.0f, 0.85f, 0.4f, 1.0f }
//	);
//
//	//==========================================================
//	// 左側：数値
//	//==========================================================
//	const Math::Vector2 valuePos =
//	{
//		20.0f, 150.0f
//	};
//
//	const Math::Color valueColor =
//	{
//		1.0f, 1.0f, 1.0f, 1.0f
//	};
//
//	m_baseValue = CreateText(
//		"0",
//		valuePos,
//		1.5f,
//		valueColor
//	);
//
//	m_timeValue = CreateText(
//		"-0",
//		{ 20.0f, 50.0f },
//		1.5f,
//		valueColor
//	);
//
//	m_damageValue = CreateText(
//		"-0",
//		{ 20.0f, -50.0f },
//		1.5f,
//		valueColor
//	);
//
//	m_finalValue = CreateText(
//		"0",
//		{ 20.0f, -160.0f },
//		2.0f,
//		{ 1.0f, 0.85f, 0.4f, 1.0f }
//	);
//
//	//==========================================================
//	// 右側：ランク
//	//==========================================================
//	m_rankLabel = CreateText(
//		"RANK",
//		{ 310.0f, 100.0f },
//		1.5f,
//		labelColor
//	);
//
//	m_rankValue = CreateText(
//		"",
//		{ 310.0f, -30.0f },
//		5.0f,
//		{ 1.0f, 0.8f, 0.25f, 1.0f }
//	);
//
//	// 最初はランクを隠す
//	m_rankValue->SetVisible(false);
//
//	// 最初は背景・文字を隠す
//	m_mainBackground->SetVisible(false);
//	m_rankBackground->SetVisible(false);
//
//	m_titleText->SetVisible(false);
//
//	m_baseLabel->SetVisible(false);
//	m_timeLabel->SetVisible(false);
//	m_damageLabel->SetVisible(false);
//	m_finalLabel->SetVisible(false);
//
//	m_baseValue->SetVisible(false);
//	m_timeValue->SetVisible(false);
//	m_damageValue->SetVisible(false);
//	m_finalValue->SetVisible(false);
//
//	m_rankLabel->SetVisible(false);
//}
//
////==============================================================
//// 文字生成
////==============================================================
//std::shared_ptr<FontText> ResultUI::CreateText(
//	const std::string& text,
//	const Math::Vector2& pos,
//	float scale,
//	const Math::Color& color)
//{
//	auto font = std::make_shared<FontText>();
//
//	font->InitMessage(
//		text,
//		pos,
//		scale
//	);
//
//	font->SetColor(color);
//
//	return font;
//}
//
////==============================================================
//// スコア情報設定
////==============================================================
//void ResultUI::SetScoreData(
//	int baseScore,
//	int timePenalty,
//	int damagePenalty,
//	int finalScore,
//	const std::string& rank)
//{
//	m_baseScore = baseScore;
//	m_timePenalty = timePenalty;
//	m_damagePenalty = damagePenalty;
//	m_finalScore = finalScore;
//	m_rank = rank;
//}
//
////==============================================================
//// リザルト演出開始
////==============================================================
//void ResultUI::Start()
//{
//	m_started = true;
//	m_finished = false;
//
//	m_currentStep = 0;
//	m_animationTimer = 0.0f;
//
//	// 背景・タイトル表示
//	m_mainBackground->SetVisible(true);
//	m_rankBackground->SetVisible(true);
//	m_titleText->SetVisible(true);
//
//	// 項目名表示
//	m_baseLabel->SetVisible(true);
//	m_timeLabel->SetVisible(true);
//	m_damageLabel->SetVisible(true);
//	m_finalLabel->SetVisible(true);
//
//	// 数値表示
//	m_baseValue->SetVisible(true);
//	m_timeValue->SetVisible(true);
//	m_damageValue->SetVisible(true);
//	m_finalValue->SetVisible(true);
//
//	// ランク項目名だけ表示
//	m_rankLabel->SetVisible(true);
//	m_rankValue->SetVisible(false);
//
//	UpdateScoreText();
//}
//
////==============================================================
//// 数値をカンマ区切りにする
////==============================================================
//std::string ResultUI::FormatNumber(int value) const
//{
//	std::string number = std::to_string(value);
//
//	for (int i = static_cast<int>(number.size()) - 3;
//		i > 0;
//		i -= 3)
//	{
//		number.insert(i, ",");
//	}
//
//	return number;
//}
//
////==============================================================
//// 数値表示更新
////==============================================================
//void ResultUI::UpdateScoreText()
//{
//	float t = std::clamp(
//		m_animationTimer / m_countDuration,
//		0.0f,
//		1.0f
//	);
//
//	int value = 0;
//
//	switch (m_currentStep)
//	{
//	case 0:
//		value = static_cast<int>(m_baseScore * t);
//		m_baseValue->SetMessageText(
//			FormatNumber(value)
//		);
//		break;
//
//	case 1:
//		value = static_cast<int>(m_timePenalty * t);
//		m_timeValue->SetMessageText(
//			"-" + FormatNumber(value)
//		);
//		break;
//
//	case 2:
//		value = static_cast<int>(m_damagePenalty * t);
//		m_damageValue->SetMessageText(
//			"-" + FormatNumber(value)
//		);
//		break;
//
//	case 3:
//		value = static_cast<int>(m_finalScore * t);
//		m_finalValue->SetMessageText(
//			FormatNumber(value)
//		);
//		break;
//	}
//}
//
////==============================================================
//// 更新
////==============================================================
//void ResultUI::Update()
//{
//	if (!m_started || m_finished)
//	{
//		return;
//	}
//
//	float dt =
//		Application::Instance().GetDeltaTime();
//
//	m_animationTimer += dt;
//
//	//==========================================================
//	// スコアのカウントアップ
//	//==========================================================
//	if (m_currentStep < 4)
//	{
//		UpdateScoreText();
//
//		if (m_animationTimer >= m_countDuration)
//		{
//			m_animationTimer = -m_stepInterval;
//			m_currentStep++;
//		}
//
//		return;
//	}
//
//	//==========================================================
//	// ランク表示
//	//==========================================================
//	if (m_currentStep == 4)
//	{
//		if (m_animationTimer >= 0.3f)
//		{
//			m_rankValue->SetMessageText(m_rank);
//			m_rankValue->SetVisible(true);
//
//			m_currentStep++;
//			m_animationTimer = 0.0f;
//		}
//
//		return;
//	}
//
//	//==========================================================
//	// 全演出終了
//	//==========================================================
//	m_finished = true;
//}
//
////==============================================================
//// 描画
////==============================================================
//void ResultUI::DrawSprite()
//{
//	if (!m_started)
//	{
//		return;
//	}
//
//	// 背景
//	m_mainBackground->DrawSprite();
//	m_rankBackground->DrawSprite();
//
//	// タイトル
//	m_titleText->DrawSprite();
//
//	// 項目名
//	m_baseLabel->DrawSprite();
//	m_timeLabel->DrawSprite();
//	m_damageLabel->DrawSprite();
//	m_finalLabel->DrawSprite();
//
//	// 数値
//	m_baseValue->DrawSprite();
//	m_timeValue->DrawSprite();
//	m_damageValue->DrawSprite();
//	m_finalValue->DrawSprite();
//
//	// ランク
//	m_rankLabel->DrawSprite();
//	m_rankValue->DrawSprite();
//}

#include "ResultUI.h"

#include <Application/GameObject/UI/FontText/FontText.h>
#include <Application/GameObject/UI/PlaeyrUI/UIBackground/UIBackground.h>
#include <Application/main.h>

//==============================================================
// 初期化
//==============================================================
void ResultUI::Init()
{
	m_started = false;
	m_finished = false;

	//==========================================================
	// 背景パネル
	//==========================================================
	m_mainBackground =
		std::make_shared<UIBackground>();

	m_mainBackground->InitTutorial(
		{ -250.0f, 0.0f },
		700.0f,
		450.0f
	);

	m_rankBackground =
		std::make_shared<UIBackground>();

	m_rankBackground->InitTutorial(
		{ 310.0f, 0.0f },
		280.0f,
		450.0f
	);

	//==========================================================
	// タイトル
	//==========================================================
	m_titleText = CreateText(
		"リザルト",
		{ 0.0f, 280.0f },
		2.5f,
		{ 1.0f, 1.0f, 1.0f, 1.0f }
	);

	//==========================================================
	// 左側：項目名
	//==========================================================
	const Math::Color labelColor =
	{
		0.75f, 0.8f, 0.9f, 1.0f
	};

	m_timeLabel = CreateText(
		"タイムスコア",
		{ -430.0f, 100.0f },
		2.0f,
		labelColor
	);

	m_damageLabel = CreateText(
		"被弾回避スコア",
		{ -430.0f, 0.0f },
		2.0f,
		labelColor
	);

	m_finalLabel = CreateText(
		"最終スコア",
		{ -430.0f, -150.0f },
		2.5f,
		{ 1.0f, 0.85f, 0.4f, 1.0f }
	);

	//==========================================================
	// 左側：数値
	//==========================================================
	const Math::Color valueColor =
	{
		1.0f, 1.0f, 1.0f, 1.0f
	};

	m_timeValue = CreateText(
		"0",
		{ 10.0f, 100.0f },
		2.0f,
		valueColor
	);

	m_damageValue = CreateText(
		"0",
		{ 10.0f, 0.0f },
		2.0f,
		valueColor
	);

	m_finalValue = CreateText(
		"0",
		{ 10.0f, -150.0f },
		2.5f,
		{ 1.0f, 0.85f, 0.4f, 1.0f }
	);

	//==========================================================
	// 右側：ランク
	//==========================================================
	m_rankLabel = CreateText(
		"ランク",
		{ 330.0f, 150.0f },
		2.0f,
		labelColor
	);

	m_rankValue = CreateText(
		"",
		{ 385.0f, -30.0f },
		10.0f,
		{ 1.0f, 0.8f, 0.25f, 1.0f }
	);

	// 最初はランクを隠す
	m_rankValue->SetVisible(false);

	// 最初は背景・文字を隠す
	m_mainBackground->SetVisible(false);
	m_rankBackground->SetVisible(false);

	m_titleText->SetVisible(false);

	m_timeLabel->SetVisible(false);
	m_damageLabel->SetVisible(false);
	m_finalLabel->SetVisible(false);

	m_timeValue->SetVisible(false);
	m_damageValue->SetVisible(false);
	m_finalValue->SetVisible(false);

	m_rankLabel->SetVisible(false);
}

//==============================================================
// 文字生成
//==============================================================
std::shared_ptr<FontText> ResultUI::CreateText(
	const std::string& text,
	const Math::Vector2& pos,
	float scale,
	const Math::Color& color)
{
	auto font = std::make_shared<FontText>();

	font->InitMessage(
		text,
		pos,
		scale
	);

	font->SetColor(color);

	return font;
}

//==============================================================
// スコア情報設定
//==============================================================
void ResultUI::SetScoreData(
	int timeScore,
	int damageScore,
	int finalScore,
	const std::string& rank)
{
	m_timeScore = timeScore;
	m_damageScore = damageScore;
	m_finalScore = finalScore;
	m_rank = rank;
}

//==============================================================
// リザルト演出開始
//==============================================================
void ResultUI::Start()
{
	m_started = true;
	m_finished = false;

	m_currentStep = 0;
	m_animationTimer = 0.0f;

	// 背景・タイトル表示
	m_mainBackground->SetVisible(true);
	m_rankBackground->SetVisible(true);
	m_titleText->SetVisible(true);

	// 項目名表示
	m_timeLabel->SetVisible(true);
	m_damageLabel->SetVisible(true);
	m_finalLabel->SetVisible(true);

	// 数値表示
	m_timeValue->SetVisible(true);
	m_damageValue->SetVisible(true);
	m_finalValue->SetVisible(true);

	// ランク項目名だけ表示
	m_rankLabel->SetVisible(true);
	m_rankValue->SetVisible(false);

	UpdateScoreText();
}

//==============================================================
// 数値をカンマ区切りにする
//==============================================================
std::string ResultUI::FormatNumber(int value) const
{
	std::string number = std::to_string(value);

	for (int i = static_cast<int>(number.size()) - 3;
		i > 0;
		i -= 3)
	{
		number.insert(i, ",");
	}

	return number;
}

//==============================================================
// 数値表示更新
//==============================================================
void ResultUI::UpdateScoreText()
{
	float t = std::clamp(
		m_animationTimer / m_countDuration,
		0.0f,
		1.0f
	);

	int value = 0;

	switch (m_currentStep)
	{
	case 0:
		value = static_cast<int>(m_timeScore * t);
		m_timeValue->SetMessageText(
			FormatNumber(value)
		);
		break;

	case 1:
		value = static_cast<int>(m_damageScore * t);
		m_damageValue->SetMessageText(
			FormatNumber(value)
		);
		break;

	case 2:
		value = static_cast<int>(m_finalScore * t);
		m_finalValue->SetMessageText(
			FormatNumber(value)
		);
		break;
	}
}

//==============================================================
// 更新
//==============================================================
void ResultUI::Update()
{
	if (!m_started || m_finished)
	{
		return;
	}

	float dt =
		Application::Instance().GetDeltaTime();

	m_animationTimer += dt;

	//==========================================================
	// スコアのカウントアップ
	//==========================================================
	if (m_currentStep < 3)
	{
		UpdateScoreText();

		if (m_animationTimer >= m_countDuration)
		{
			m_animationTimer = -m_stepInterval;
			m_currentStep++;
		}

		return;
	}

	//==========================================================
	// ランク表示
	//==========================================================
	if (m_currentStep == 3)
	{
		if (m_animationTimer >= 0.3f)
		{
			m_rankValue->SetMessageText(m_rank);
			m_rankValue->SetVisible(true);

			m_currentStep++;
			m_animationTimer = 0.0f;
		}

		return;
	}

	//==========================================================
	// 全演出終了
	//==========================================================
	m_finished = true;
}

//==============================================================
// 描画
//==============================================================
void ResultUI::DrawSprite()
{
	if (!m_started)
	{
		return;
	}

	// 背景
	m_mainBackground->DrawSprite();
	m_rankBackground->DrawSprite();

	// タイトル
	m_titleText->DrawSprite();

	// 項目名
	m_timeLabel->DrawSprite();
	m_damageLabel->DrawSprite();
	m_finalLabel->DrawSprite();

	// 数値
	m_timeValue->DrawSprite();
	m_damageValue->DrawSprite();
	m_finalValue->DrawSprite();

	// ランク
	m_rankLabel->DrawSprite();
	m_rankValue->DrawSprite();
}