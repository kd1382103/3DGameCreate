#include "TitleScene.h"

#include "../SceneManager.h"

#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Stages/Stage/Stage.h>
#include <Application/GameObject/Stages/Sky/Sky.h>

#include <Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include <Application/GameObject/Camera/CameraBase.h>

#include <Application/GameObject/UI/TitleImage/TitleImage.h>
#include <Application/GameObject/UI/SettingUI/SettingUI.h>

#include <Application/GameObject/Effect/EffectManager.h>

#include <Application/main.h>

//============================================================
// イベント
//============================================================
void TitleScene::Event()
{
	//========================================
	// 音量更新
	//========================================
	UpdateAudioVolume();

	//========================================
	// 設定画面 開閉
	// TABキー
	//========================================
	if (GetAsyncKeyState(VK_TAB) & 0x0001)
	{
		if (m_settingUI)
		{
			if (m_settingUI->IsVisible())
			{
				m_settingUI->Close();
			}
			else
			{
				m_settingUI->Open();
			}
		}

		return;
	}

	//========================================
	// 設定画面を開いている間
	//========================================
	if (m_settingUI && m_settingUI->IsVisible())
	{
		// 設定中はAny Keyを無効にする
		return;
	}

	//========================================
	// Jump中
	//========================================
	if (m_startJump)
	{
		if (m_player->m_animator.IsAnimationEnd())
		{
			//=======================================
			// タイトルBGM停止
			//=======================================
			if (m_titleBGM)
			{
				m_titleBGM->Stop();
				m_titleBGM = nullptr;
			}

			//=======================================
			// ゲームシーンへ
			//=======================================
			SceneManager::Instance().SetNextScene(
				SceneManager::SceneType::Game
			);
		}

		return;
	}

	//---------------------------------------
	// Any Key
	//---------------------------------------
	for (int key = 0x08; key <= 0xFE; key++)
	{
		if (GetAsyncKeyState(key) & 0x0001)
		{
			//=======================================
			// Jumpアニメーション開始
			//=======================================
			m_player->PlayAnimationAuto("Jump", false);

			m_startJump = true;

			break;
		}
	}
}

//============================================================
// 初期化
//============================================================
void TitleScene::Init()
{
	BaseScene::Init();

	CURSORINFO ci = { sizeof(CURSORINFO) };
	GetCursorInfo(&ci);

	//カーソルが表示されているかどうか
	if (ci.flags & CURSOR_SHOWING)
	{
		ShowCursor(FALSE);
	}

	//カーソルの移動範囲制限を解除
	ClipCursor(nullptr);
	
	//==========================================================
	// タイトル画面 3D
	//==========================================================

	InitStage();
	InitSky();
	InitCamera();
	InitPlayer();

	//---------------------------------------
	// タイトル背景
	//---------------------------------------
	auto titleImage = std::make_shared<TitleImage>();
	titleImage->Init();
	AddObject(titleImage);

	//---------------------------------------
	// 設定UI
	//---------------------------------------
	m_settingUI =
		std::make_shared<SettingUI>();

	m_settingUI->Init();

	AddObject(m_settingUI);

	//---------------------------------------
	// 音声
	//---------------------------------------
	InitAudio();

	//=======================================
	// 環境光
	//=======================================
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 1,1,1,0.3f });

	//=======================================
	// ポイントライト
	//=======================================
	KdShaderManager::Instance().WorkAmbientController().AddPointLight(
		{ 1, 1, 1 },
		10.0f,
		{ -60.0f, 0.0f, 55.0f }
	);

}

//============================================================
// タイトル用ステージ
//============================================================
void TitleScene::InitStage()
{
	//=======================================
	// ステージ
	//=======================================

	m_stage =
		std::make_shared<Stage>();

	m_stage->Init();

	AddObject(m_stage);
}


//============================================================
// タイトル用Sky
//============================================================
void TitleScene::InitSky()
{
	//=======================================
	// 空
	//=======================================

	m_sky =
		std::make_shared<Sky>();

	m_sky->Init();

	m_sky->SetPos({ 0, 0, 0 });

	AddObject(m_sky);
}


//============================================================
// タイトル用カメラ
//============================================================
void TitleScene::InitCamera()
{
	//=======================================
	// カメラ
	//=======================================

	m_tpsCamera =
		std::make_shared<TPSCamera>();

	m_tpsCamera->Init();

	m_tpsCamera->SetActive(true);

	m_tpsCamera->SetLocalPos({ 0.0f, 1.5f, -3.0f });
	m_tpsCamera->SetAngleY(125.0f);
	m_tpsCamera->SetAngleX(-20.0f);

	m_tpsCamera->m_mouseFree = true;

	//=======================================
	// ステージをカメラの
	// めり込み判定対象にする
	//=======================================

	m_tpsCamera->RegistHitObject(m_stage);

	AddObject(m_tpsCamera);

	//=======================================
	// CameraBaseとして保持
	//=======================================

	m_camera = m_tpsCamera;

	//=======================================
	// エフェクトマネージャー
	//=======================================

	EffectManager::Instance().SetCamera(m_camera);
}


//============================================================
// タイトル用Player
//============================================================
void TitleScene::InitPlayer()
{
	m_player = std::make_shared<Player>();
	m_player->Init();

	m_player->SetPos(Math::Vector3{ -55.0f, 0.0f, 50.0f });
	m_player->SetAngleY(125.0f);

	// タイトル画面では操作させない
	m_player->SetTitleMode(true);

	// ゲーム用UIを表示しない
	m_player->SetUltimatePointVisible(false);

	AddObject(m_player);

	//==========================================================
	// Playerをカメラのターゲットにする
	//==========================================================
	m_tpsCamera->SetTarget(m_player);

	//==========================================================
	// Playerからカメラを参照できるようにする
	//==========================================================
	m_player->SetCamera(m_camera);
}

//============================================================
// 音声初期化
//============================================================
void TitleScene::InitAudio()
{
	//---------------------------------------
	// 保存されている音量を取得
	//---------------------------------------
	float bgmVolume = m_settingUI->GetBGMVolume();
	float seVolume = m_settingUI->GetSEVolume();

	//---------------------------------------
	// BGM・SE音量を設定
	//---------------------------------------
	KdAudioManager::Instance().SetBGMVolume(bgmVolume);
	KdAudioManager::Instance().SetSEVolume(seVolume);

	//---------------------------------------
	// タイトルBGM開始
	//---------------------------------------
	m_titleBGM = KdAudioManager::Instance().Play(
		"Asset/Sounds/BGM/TitleBGM.wav",
		SoundType::BGM,
		true
	);
}

//============================================================
// 音量更新
//============================================================
void TitleScene::UpdateAudioVolume()
{
	if (!m_settingUI)
	{
		return;
	}

	//---------------------------------------
	// BGM音量
	//---------------------------------------
	float bgmVolume = m_settingUI->GetBGMVolume();

	if (m_titleBGM)
	{
		m_titleBGM->SetVolume(bgmVolume);
	}

	//---------------------------------------
	// SE音量
	//---------------------------------------
	float seVolume = m_settingUI->GetSEVolume();

	KdAudioManager::Instance().SetSEVolume(seVolume);
}