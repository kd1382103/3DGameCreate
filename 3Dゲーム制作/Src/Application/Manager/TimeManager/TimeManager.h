#pragma once

class TimeManager
{
public:

	// 制限時間：10分
	static constexpr float kTimeLimit = 600.0f;

	// 初期化
	void Init();

	// タイマー開始
	void Start();

	// タイマー停止
	void Stop();

	// 更新
	void Update(float deltaTime);

	// 残り時間
	float GetRemainingTime() const
	{
		return m_remainingTime;
	}

	// 経過時間
	float GetElapsedTime() const
	{
		return kTimeLimit - m_remainingTime;
	}

	// 時間切れ判定
	bool IsTimeUp() const
	{
		return m_isTimeUp;
	}

	// 動作中か
	bool IsRunning() const
	{
		return m_isRunning;
	}

private:

	float m_remainingTime = kTimeLimit;

	bool m_isRunning = false;
	bool m_isTimeUp = false;
};