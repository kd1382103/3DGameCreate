#include "GameScene.h"
#include"../SceneManager.h"

#include<Application/GameObject/Player/Player.h>
#include<Application/GameObject/Stage/Stage.h>
#include<Application/GameObject/Camera/TPSCamera/TPSCamera.h>

void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}
}

void GameScene::Init()
{
	//カメラ
	std::shared_ptr<TPSCamera>camera;
	camera = std::make_shared<TPSCamera>();
	camera->Init();
	AddObject(camera);

	//ステージ
	std::shared_ptr<Stage>stage;
	stage = std::make_shared<Stage>();
	stage->Init();
	AddObject(stage);
	//プレイヤー
	std::shared_ptr<Player>player;
	player = std::make_shared<Player>();
	player->Init();
	player->SetNowPos(Math::Vector3{ 0,0,0 });



	//カメラのターゲットを設定
	camera->SetPlayerTarget(player);
	AddObject(player);
}
