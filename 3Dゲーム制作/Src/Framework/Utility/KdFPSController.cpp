#include "KdFPSController.h"

//============================================================
// FPS上限設定
// 0 = 無制限
//============================================================
void KdFPSController::SetMaxFPS(int fps)
{
	// 0未満にはしない
	if (fps < 0)
	{
		fps = 0;
	}

	m_maxFps = fps;
}

float KdFPSController::GetFrameScale() const
{
	//============================================================
	// 60FPSを基準とする
	//============================================================
	constexpr float kBaseFPS = 60.0f;

	return kBaseFPS * m_DeltaTime;
}

//============================================================
// FPSの制御コントローラー
//============================================================
void KdFPSController::Init()
{
	DWORD now = timeGetTime();
	m_frameStartTime = now;
	m_fpsMonitorBeginTime = now;

	m_DeltaTime = 1.0f / 60.0f;
}

//============================================================
// フレーム開始時間
//============================================================
void KdFPSController::UpdateStartTime()
{
	m_frameStartTime = timeGetTime();
}

//============================================================
// 更新
//============================================================
void KdFPSController::Update()
{
	Control();

	// デルタ時間の計算
	DWORD currentTime = timeGetTime();

	m_DeltaTime =
		(currentTime - m_frameStartTime)
		/ static_cast<float>(kSecond);

	if (m_DeltaTime > 0.1f)
	{
		m_DeltaTime = 1.0f / 60.0f;
	}

	Monitoring();
}

//============================================================
// FPS制御
//============================================================
void KdFPSController::Control()
{
	//========================================================
	// 無制限
	//========================================================
	if (m_maxFps <= 0)
	{
		return;
	}

	//========================================================
	// 処理終了時間Get
	//========================================================
	DWORD frameProcessEndTime = timeGetTime();

	//========================================================
	// 1フレームで経過すべき時間
	//========================================================
	DWORD timePerFrame = kSecond / m_maxFps;

	DWORD elapsedTime =
		frameProcessEndTime - m_frameStartTime;

	//========================================================
	// FPS上限を超えないように待機
	//========================================================
	if (elapsedTime < timePerFrame)
	{
		Sleep(timePerFrame - elapsedTime);
	}
}

//============================================================
// 現在のFPS計測
//============================================================
void KdFPSController::Monitoring()
{
	// 0.5秒おき
	constexpr float kFpsRefreshFrame = 500;

	m_fpsCnt++;

	if (m_frameStartTime - m_fpsMonitorBeginTime >= kFpsRefreshFrame)
	{
		m_nowfps =
			(m_fpsCnt * kSecond)
			/ (m_frameStartTime - m_fpsMonitorBeginTime);

		m_fpsMonitorBeginTime = m_frameStartTime;

		m_fpsCnt = 0;
	}
}