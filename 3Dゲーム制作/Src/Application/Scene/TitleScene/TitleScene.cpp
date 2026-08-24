#include "TitleScene.h"
#include "../SceneManager.h"

#include <Application/GameObject/UI/FontText/FontText.h>
#include <Application/GameObject/UI/TitleImage/TitleImage.h>

void TitleScene::Event()
{
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

	//auto titleText = std::make_shared<FontText>();
	//titleText->InitMessage(
	//	"ゲーム名\nタイトル画面",
	//	{ 0.0f, 0.0f },
	//	2.0f
	//);
	//AddObject(titleText);
}
