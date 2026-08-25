#include "TitleScene.h"
#include "../SceneManager.h"

#include <Application/GameObject/UI/FontText/FontText.h>
#include <Application/GameObject/UI/TitleImage/TitleImage.h>
#include <Application/GameObject/UI/SettingUI/SettingUI.h>

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

	//---------------------------------------
	// Any Key
	//---------------------------------------
	for (int key = 0x08; key <= 0xFE; key++)
	{
		if (GetAsyncKeyState(key) & 0x0001)
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

			break;
		}
	}
}

//============================================================
// 初期化
//============================================================
void TitleScene::Init()
{
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