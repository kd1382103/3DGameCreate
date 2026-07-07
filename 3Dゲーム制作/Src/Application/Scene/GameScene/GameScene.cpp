#include "GameScene.h"
#include"../SceneManager.h"

#include<Application/GameObject/Player/Player.h>
#include<Application/GameObject/Stages/Floor/Stage.h>
#include<Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include<Application/GameObject/Camera/FPSCamera/FPSCamera.h>
#include<Application/GameObject/Camera/CameraBase.h>

void GameScene::Event()
{
	// シーン切り替え（Tキー）
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}

	auto& input = KdInputManager::Instance();
	
	// 視点の切り替え
	/*if (input.IsPress("ChangeKey"))
	{
		if (m_camera == tpsCamera)
		{
			m_camera = fpsCamera;
			tpsCamera->SetActive(false);
			fpsCamera->SetActive(true);
			fpsCamera->m_mouseFree = false;

		}
		else
		{
			m_camera = tpsCamera;
			tpsCamera->SetActive(true);
			fpsCamera->SetActive(false);
			tpsCamera->m_mouseFree = false;

		}
	}*/

	//player->SetCamera(m_camera);
}

void GameScene::Init()
{
	BaseScene::Init();

	//カメラ
	tpsCamera = std::make_shared<TPSCamera>();
	tpsCamera->Init();
	tpsCamera->SetActive(true);
	AddObject(tpsCamera);

	/*fpsCamera = std::make_shared<FPSCamera>();
	fpsCamera->Init();
	fpsCamera->SetActive(false);

	AddObject(fpsCamera);*/

	m_camera = tpsCamera;

	//ステージ
	std::shared_ptr<Stage>stage;
	stage = std::make_shared<Stage>();
	stage->Init();
	AddObject(stage);

	//プレイヤー
	player = std::make_shared<Player>();
	player->Init();
	player->SetPos(Math::Vector3{ 0,0,0 });
	AddObject(player);

	//各オブジェクトに必要な情報を格納
	tpsCamera->SetTarget(player);
	//fpsCamera->SetTarget(player);
	player->SetCamera(m_camera);
}