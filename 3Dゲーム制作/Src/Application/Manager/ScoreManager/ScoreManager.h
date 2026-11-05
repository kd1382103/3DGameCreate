//#pragma once
//
//class ScoreManager
//{
//public:
//
//	enum class Rank
//	{
//		S,
//		A,
//		B,
//		C,
//		D
//	};
//
//	//=======================================
//	// スコア設定
//	//=======================================
//
//	static constexpr int kBaseScore = 10000;
//
//	static constexpr int kMaxTimePenalty = 4000;
//	static constexpr int kMaxDamagePenalty = 4000;
//
//	// 600秒経過で時間減点最大
//	static constexpr float kTimeLimit = 600.0f;
//
//	// 累計被ダメージ1000で減点最大
//	static constexpr float kDamageForMaxPenalty = 1000.0f;
//
//
//	//=======================================
//	// 初期化
//	//=======================================
//
//	void Init();
//
//	// 累計被ダメージを加算
//	void AddDamage(float damage);
//
//	// スコア確定
//	void Finalize(float elapsedTime);
//
//
//	//=======================================
//	// スコア取得
//	//=======================================
//
//	int GetBaseScore() const
//	{
//		return kBaseScore;
//	}
//
//	int GetTimePenalty() const
//	{
//		return m_timePenalty;
//	}
//
//	int GetDamagePenalty() const
//	{
//		return m_damagePenalty;
//	}
//
//	int GetFinalScore() const
//	{
//		return m_finalScore;
//	}
//
//	float GetTotalDamage() const
//	{
//		return m_totalDamage;
//	}
//
//	Rank GetRank() const
//	{
//		return m_rank;
//	}
//
//	const char* GetRankText() const;
//
//
//private:
//
//	// 累計被ダメージ
//	float m_totalDamage = 0.0f;
//
//	// 時間減点
//	int m_timePenalty = 0;
//
//	// 被ダメージ減点
//	int m_damagePenalty = 0;
//
//	// 最終スコア
//	int m_finalScore = kBaseScore;
//
//	// ランク
//	Rank m_rank = Rank::S;
//
//	// スコア確定済み
//	bool m_isFinalized = false;
//};

#pragma once

class ScoreManager
{
public:

	enum class Rank
	{
		S,
		A,
		B,
		C,
		D
	};

	//=======================================
	// スコア設定
	//=======================================

	static constexpr int kMaxTimeScore = 5000;
	static constexpr int kMaxDamageScore = 5000;
	static constexpr int kMaxScore = 10000;

	// 3分までは満点
	static constexpr float kTimeFullScoreLimit = 180.0f;

	// 10分で0点
	static constexpr float kTimeZeroScoreLimit = 600.0f;

	// 累計被ダメージ2000でダメージ点0
	static constexpr float kDamageForZeroScore = 2000.0f;


	//=======================================
	// 初期化
	//=======================================

	void Init();

	// 累計被ダメージを加算
	void AddDamage(float damage);

	// スコア確定
	void Finalize(float elapsedTime);


	//=======================================
	// スコア取得
	//=======================================

	int GetTimeScore() const
	{
		return m_timeScore;
	}

	int GetDamageScore() const
	{
		return m_damageScore;
	}

	int GetFinalScore() const
	{
		return m_finalScore;
	}

	float GetTotalDamage() const
	{
		return m_totalDamage;
	}

	Rank GetRank() const
	{
		return m_rank;
	}

	const char* GetRankText() const;


private:

	// 累計被ダメージ
	float m_totalDamage = 0.0f;

	// 時間評価点
	int m_timeScore = 0;

	// ダメージ評価点
	int m_damageScore = 0;

	// 合計スコア
	int m_finalScore = 0;

	// ランク
	Rank m_rank = Rank::S;

	// スコア確定済み
	bool m_isFinalized = false;
};