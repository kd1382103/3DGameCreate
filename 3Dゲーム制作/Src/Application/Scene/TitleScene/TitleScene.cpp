#include "TitleScene.h"
#include "../SceneManager.h"

#include <Application/GameObject/UI/FontText/FontText.h>
void TitleScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Game
		);
	}
}

void TitleScene::Init()
{
	auto titleText = std::make_shared<FontText>();
	titleText->InitMessage(
		"ゲーム名\nタイトル画面",
		{ 0.0f, 0.0f },
		2.0f
	);
	AddObject(titleText);
}
