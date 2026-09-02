#pragma once

//============================================================
// アプリケーションクラス
//	APP.～ でどこからでもアクセス可能
//============================================================
class Application
{
	// メンバ
public:

	//============================================================
	// 表示モード
	//============================================================
	enum class DisplayMode
	{
		Window1280x720,
		Window1920x1080,
		Fullscreen
	};

	// アプリケーション実行
	void Execute();

	// アプリケーション終了
	void End() { m_endFlag = true; }

	HWND GetWindowHandle() const
	{
		return m_window.GetWndHandle();
	}

	int GetMouseWheelValue() const
	{
		return m_window.GetMouseWheelVal();
	}

	int GetNowFPS() const
	{
		return m_fpsController.m_nowfps;
	}

	int GetMaxFPS() const
	{
		return m_fpsController.m_maxFps;
	}

	float GetDeltaTime() const
	{
		return m_fpsController.GetDeltaTime();
	}

	//============================================================
	// FPS
	//============================================================
	KdFPSController& GetFPSController()
	{
		return m_fpsController;
	}

	//============================================================
	// 表示モード
	//============================================================

	void SetDisplayMode(DisplayMode mode);

	DisplayMode GetDisplayMode() const
	{
		return m_displayMode;
	}

	int GetResolutionWidth() const
	{
		return m_resolutionWidth;
	}

	int GetResolutionHeight() const
	{
		return m_resolutionHeight;
	}

private:

	void KdBeginUpdate();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void KdPostUpdate();

	void KdBeginDraw(bool usePostProcess = true);
	void PreDraw();
	void Draw();
	void PostDraw();
	void DrawSprite();
	void KdPostDraw();

	// アプリケーション初期化
	bool Init(int w, int h);

	// アプリケーション解放
	void Release();

	// ゲームウィンドウクラス
	KdWindow m_window;

	// FPSコントローラー
	KdFPSController m_fpsController;

	//============================================================
	// 現在の表示モード
	//============================================================
	DisplayMode m_displayMode =
		DisplayMode::Window1280x720;

	//============================================================
	// 現在の画面サイズ
	//============================================================
	int m_resolutionWidth = 1280;
	int m_resolutionHeight = 720;

	// ゲーム終了フラグ trueで終了する
	bool m_endFlag = false;

	//=====================================================
	// シングルトンパターン
	//=====================================================
private:

	Application() {}

public:

	static Application& Instance()
	{
		static Application Instance;
		return Instance;
	}
};