#include "TitleScene.h"
#include "../SceneManager.h"

#include <Application/GameObject/UI/FontText/FontText.h>
#include <Application/GameObject/UI/TitleImage/TitleImage.h>
#include <Application/GameObject/UI/SettingUI/SettingUI.h>


void TitleScene::Event()
{
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
			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::Game
			);

			break;
		}
	}
}

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

	//auto titleText = std::make_shared<FontText>();
	//titleText->InitMessage(
	//	"ゲーム名\nタイトル画面",
	//	{ 0.0f, 0.0f },
	//	2.0f
	//);
	//AddObject(titleText);
}
