//#include "ScoreManager.h"
//
//void ScoreManager::Init()
//{
//	m_totalDamage = 0.0f;
//
//	m_timePenalty = 0;
//	m_damagePenalty = 0;
//
//	m_finalScore = kBaseScore;
//
//	m_rank = Rank::S;
//
//	m_isFinalized = false;
//}
//
//void ScoreManager::AddDamage(float damage)
//{
//	if (m_isFinalized || damage <= 0.0f)
//	{
//		return;
//	}
//
//	m_totalDamage += damage;
//}
//
//void ScoreManager::Finalize(float elapsedTime)
//{
//	if (m_isFinalized)
//	{
//		return;
//	}
//
//	//=======================================
//	// 時間減点
//	//=======================================
//
//	float timeRate =
//		std::clamp(elapsedTime / kTimeLimit, 0.0f, 1.0f);
//
//	m_timePenalty = static_cast<int>(
//		std::lround(kMaxTimePenalty * timeRate)
//		);
//
//	//=======================================
//	// 被ダメージ減点
//	//=======================================
//
//	float damageRate =
//		std::clamp(
//			m_totalDamage / kDamageForMaxPenalty,
//			0.0f,
//			1.0f
//		);
//
//	m_damagePenalty = static_cast<int>(
//		std::lround(kMaxDamagePenalty * damageRate)
//		);
//
//	//=======================================
//	// 最終スコア
//	//=======================================
//
//	m_finalScore =
//		kBaseScore
//		- m_timePenalty
//		- m_damagePenalty;
//
//	m_finalScore = std::clamp(
//		m_finalScore,
//		0,
//		kBaseScore
//	);
//
//	//=======================================
//	// ランク判定
//	//=======================================
//
//	if (m_finalScore >= 9000)
//	{
//		m_rank = Rank::S;
//	}
//	else if (m_finalScore >= 8000)
//	{
//		m_rank = Rank::A;
//	}
//	else if (m_finalScore >= 7000)
//	{
//		m_rank = Rank::B;
//	}
//	else if (m_finalScore >= 5000)
//	{
//		m_rank = Rank::C;
//	}
//	else
//	{
//		m_rank = Rank::D;
//	}
//
//	m_isFinalized = true;
//}
//
//const char* ScoreManager::GetRankText() const
//{
//	switch (m_rank)
//	{
//	case Rank::S: return "S";
//	case Rank::A: return "A";
//	case Rank::B: return "B";
//	case Rank::C: return "C";
//	case Rank::D: return "D";
//	}
//
//	return "D";
//}

#include "ScoreManager.h"

void ScoreManager::Init()
{
	m_totalDamage = 0.0f;

	m_timeScore = 0;
	m_damageScore = 0;

	m_finalScore = 0;

	m_rank = Rank::S;

	m_isFinalized = false;
}

void ScoreManager::AddDamage(float damage)
{
	if (m_isFinalized || damage <= 0.0f)
	{
		return;
	}

	m_totalDamage += damage;
}

void ScoreManager::Finalize(float elapsedTime)
{
	if (m_isFinalized)
	{
		return;
	}

	//=======================================
	// 時間評価点
	//=======================================

	float timeRate =
		std::clamp(
			(elapsedTime - kTimeFullScoreLimit) /
			(kTimeZeroScoreLimit - kTimeFullScoreLimit),
			0.0f,
			1.0f
		);

	m_timeScore = static_cast<int>(
		std::lround(kMaxTimeScore * (1.0f - timeRate))
		);

	//=======================================
	// ダメージ評価点
	//=======================================

	float damageRate =
		std::clamp(
			m_totalDamage / kDamageForZeroScore,
			0.0f,
			1.0f
		);

	m_damageScore = static_cast<int>(
		std::lround(kMaxDamageScore * (1.0f - damageRate))
		);

	//=======================================
	// 合計スコア
	//=======================================

	m_finalScore = m_timeScore + m_damageScore;

	m_finalScore = std::clamp(
		m_finalScore,
		0,
		kMaxScore
	);

	//=======================================
	// ランク判定
	//=======================================

	if (m_finalScore >= 8000)
	{
		m_rank = Rank::S;
	}
	else if (m_finalScore >= 7000)
	{
		m_rank = Rank::A;
	}
	else if (m_finalScore >= 5000)
	{
		m_rank = Rank::B;
	}
	else if (m_finalScore >= 3000)
	{
		m_rank = Rank::C;
	}
	else
	{
		m_rank = Rank::D;
	}

	m_isFinalized = true;
}

const char* ScoreManager::GetRankText() const
{
	switch (m_rank)
	{
	case Rank::S: return "S";
	case Rank::A: return "A";
	case Rank::B: return "B";
	case Rank::C: return "C";
	case Rank::D: return "D";
	}

	return "D";
}