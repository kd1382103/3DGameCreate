#include "main.h"

#include "Scene/SceneManager.h"

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// エントリーポイント
// アプリケーションはこの関数から進行する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_  HINSTANCE, _In_ LPSTR , _In_ int)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		CoUninitialize();

		return 0;
	}

	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// 実行]
	//===================================================================
	Application::Instance().Execute();

	// COM解放
	CoUninitialize();

	return 0;
}

//============================================================
// 表示モード変更(9/2追加)
//============================================================
void Application::SetDisplayMode(DisplayMode mode)
{
	//============================================================
	// 1280 x 720 ウィンドウ
	//============================================================
	if (mode == DisplayMode::Window1280x720)
	{
		//---------------------------------------
		// すでに1280x720ウィンドウなら何もしない
		//---------------------------------------
		if (m_displayMode == DisplayMode::Window1280x720)
		{
			return;
		}

		//---------------------------------------
		// フルスクリーン解除
		//---------------------------------------
		if (m_displayMode == DisplayMode::Fullscreen)
		{
			KdDirect3D::Instance()
				.SetFullscreenState(FALSE, nullptr);
		}

		//---------------------------------------
		// ウィンドウサイズ変更
		//---------------------------------------
		m_window.SetClientSize(
			1280,
			720
		);

		//---------------------------------------
		// Direct3Dの描画サイズ変更
		//---------------------------------------
		if (!KdDirect3D::Instance().Resize(
			1280,
			720))
		{
			return;
		}

		m_resolutionWidth = 1280;
		m_resolutionHeight = 720;

		m_displayMode =
			DisplayMode::Window1280x720;

		return;
	}

	//============================================================
	// 1920 x 1080 ウィンドウ
	//============================================================
	if (mode == DisplayMode::Window1920x1080)
	{
		//---------------------------------------
		// すでに1920x1080ウィンドウなら何もしない
		//---------------------------------------
		if (m_displayMode == DisplayMode::Window1920x1080)
		{
			return;
		}

		//---------------------------------------
		// フルスクリーン解除
		//---------------------------------------
		if (m_displayMode == DisplayMode::Fullscreen)
		{
			KdDirect3D::Instance()
				.SetFullscreenState(FALSE, nullptr);
		}

		//---------------------------------------
		// ウィンドウサイズ変更
		//---------------------------------------
		m_window.SetClientSize(
			1920,
			1080
		);

		//---------------------------------------
		// Direct3Dの描画サイズ変更
		//---------------------------------------
		if (!KdDirect3D::Instance().Resize(
			1920,
			1080))
		{
			return;
		}

		m_resolutionWidth = 1920;
		m_resolutionHeight = 1080;

		m_displayMode =
			DisplayMode::Window1920x1080;

		return;
	}

	//============================================================
	// フルスクリーン
	//============================================================
	if (mode == DisplayMode::Fullscreen)
	{
		//---------------------------------------
		// すでにフルスクリーンなら何もしない
		//---------------------------------------
		if (m_displayMode == DisplayMode::Fullscreen)
		{
			return;
		}

		//---------------------------------------
		// フルスクリーン化
		//---------------------------------------
		HRESULT hr =
			KdDirect3D::Instance()
			.SetFullscreenState(
				TRUE,
				nullptr
			);

		if (FAILED(hr))
		{
			return;
		}

		//---------------------------------------
		// モニターサイズを取得
		//---------------------------------------
		int width =
			GetSystemMetrics(SM_CXSCREEN);

		int height =
			GetSystemMetrics(SM_CYSCREEN);

		//---------------------------------------
		// Direct3Dの描画サイズ変更
		//---------------------------------------
		if (!KdDirect3D::Instance().Resize(
			width,
			height))
		{
			//---------------------------------------
			// 失敗した場合はフルスクリーン解除
			//---------------------------------------
			KdDirect3D::Instance()
				.SetFullscreenState(
					FALSE,
					nullptr
				);

			return;
		}

		m_resolutionWidth = width;
		m_resolutionHeight = height;

		m_displayMode =
			DisplayMode::Fullscreen;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginUpdate()
{
	// 入力状況の更新
	KdInputManager::Instance().Update();

	// 空間環境の更新
	KdShaderManager::Instance().WorkAmbientController().Update();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostUpdate()
{
	// 3DSoundListnerの行列を更新
	KdAudioManager::Instance().SetListnerMatrix(KdShaderManager::Instance().GetCameraCB().mView.Invert());
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新の前処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PreUpdate()
{
	SceneManager::Instance().PreUpdate();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Update()
{
	SceneManager::Instance().Update();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新の後処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PostUpdate()
{
	SceneManager::Instance().PostUpdate();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginDraw(bool usePostProcess)
{
	KdDirect3D::Instance().ClearBackBuffer();

	KdShaderManager::Instance().WorkAmbientController().Draw();

	if (!usePostProcess) return;
	KdShaderManager::Instance().m_postProcessShader.Draw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostDraw()
{
	// Imguiのレンダリング
	KdDebugGUI::Instance().GuiProcess();

	// BackBuffer -> 画面表示
	KdDirect3D::Instance().WorkSwapChain()->Present(0, 0);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画の前処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PreDraw()
{
	SceneManager::Instance().PreDraw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Draw()
{
	SceneManager::Instance().Draw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画の後処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PostDraw()
{
	// 画面のぼかしや被写界深度処理の実施
	KdShaderManager::Instance().m_postProcessShader.PostEffectProcess();

	// 現在のシーンのデバッグ描画
	SceneManager::Instance().DrawDebug();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 2Dスプライトの描画
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::DrawSprite()
{
	SceneManager::Instance().DrawSprite();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション初期設定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool Application::Init(int w, int h)
{
	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, "3D GameProgramming", "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	bool bFullScreen = false;
//	if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
//		bFullScreen = true;
//	}

	//===================================================================
	// Direct3D初期化
	//===================================================================

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
#ifdef _DEBUG
	deviceDebugMode = true;
#endif

	// Direct3D初期化
	std::string errorMsg;
	if (KdDirect3D::Instance().Init(m_window.GetWndHandle(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン設定
	if (bFullScreen) {
		HRESULT hr;

		hr = KdDirect3D::Instance().SetFullscreenState(TRUE, 0);
		if (FAILED(hr))
		{
			MessageBoxA(m_window.GetWndHandle(), "フルスクリーン設定失敗", "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
			return false;
		}
	}

	//===================================================================
	// imgui初期化
	//===================================================================
	KdDebugGUI::Instance().GuiInit(w, h);

	//===================================================================
	// シェーダー初期化
	//===================================================================
	KdShaderManager::Instance().Init();

	//===================================================================
	// オーディオ初期化
	//===================================================================
	KdAudioManager::Instance().Init();

	//===================================================================
	// フォント初期化
	//===================================================================
	KdFontManager::Instance().Init(GetWindowHandle());

	// フォントの追加
	KdFontManager::Instance().AddFont(
		0,
		"Meiryo",
		32
	);
	//===================================================================
	// ゲーム固有の初期化
	//===================================================================
	// 例えばカーソルを消したい場合
	ShowCursor(false);

	//===================================================================
	//トグルキー登録
	//===================================================================
	auto& input = KdInputManager::Instance();

	// キーボードデバイスを追加（初回のみ）
	KdInputCollector* keyboard = new KdInputCollector();
	input.AddDevice("Keyboard", keyboard);

	// トグルキー登録（初回のみ）
	keyboard->AddButton("ToggleKey", new KdInputButtonForWindows(VK_F2));

	//============================================================
	// 初期表示モード
	//============================================================
	m_displayMode =
		DisplayMode::Window1280x720;

	//============================================================
	// 現在の画面サイズを保存
	//============================================================
	m_resolutionWidth = w;
	m_resolutionHeight = h;

	//↑一番最初に実行したいものはここより上に書く
	return true;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション実行
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Execute()
{
	int width = 1280;
	int height = 720;

	//============================================================
	// 初期化
	//============================================================
	if (Application::Instance().Init(
		width,
		height) == false)
	{
		return;
	}

	//============================================================
	// ゲームループ
	//============================================================

	m_fpsController.Init();

	while (1)
	{
		m_fpsController.UpdateStartTime();

		if (m_endFlag)
		{
			break;
		}

		m_window.ProcessMessage();

		if (m_window.IsCreated() == false)
		{
			break;
		}

		if (GetAsyncKeyState(VK_ESCAPE))
		{
			End();
		}

		KdBeginUpdate();
		{
			PreUpdate();

			Update();

			PostUpdate();
		}

		KdPostUpdate();

		KdBeginDraw();
		{
			PreDraw();

			Draw();

			PostDraw();

			DrawSprite();
		}

		KdPostDraw();

		m_fpsController.Update();

		std::string titleBar =
			"アイアンフロンティア:DEMO FPS : "
			+ std::to_string(
				m_fpsController.m_nowfps
			);

		SetWindowTextA(
			m_window.GetWndHandle(),
			titleBar.c_str()
		);
	}

	Release();
}
// アプリケーション終了
void Application::Release()
{
	KdInputManager::Instance().Release();

	KdShaderManager::Instance().Release();

	KdAudioManager::Instance().Release();

	KdDirect3D::Instance().Release();

	// ウィンドウ削除
	m_window.Release();
}
