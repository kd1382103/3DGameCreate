#include "GameScene.h"
#include"../SceneManager.h"

#include<Application/GameObject/Player/Player/Player.h>
#include<Application/GameObject/Enemy/Enemy1/Enemy1.h>

#include<Application/GameObject/Stages/Floor/Stage.h>

#include<Application/GameObject/UI/PlaeyrUI/SkillGauge/SkillGauge.h>
#include<Application/GameObject/UI/HPGauge/HPGauge.h>


#include<Application/GameObject/Camera/TPSCamera/TPSCamera.h>
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
}

void GameScene::Init()
{
	BaseScene::Init();

	//カメラ
	tpsCamera = std::make_shared<TPSCamera>();
	tpsCamera->Init();
	tpsCamera->SetActive(true);
	AddObject(tpsCamera);

	m_camera = tpsCamera;

	//ステージ
	std::shared_ptr<Stage>stage;
	stage = std::make_shared<Stage>();
	stage->Init();
	AddObject(stage);

	//プレイヤー
	player = std::make_shared<Player>();
	player->Init();
	player->SetPos(Math::Vector3{ 0,5,0 });
	AddObject(player);

	//敵系
	enemy1 = std::make_shared<Enemy1>();
	enemy1->Init();
	enemy1->SetPos({ 5, 0, 5 });
	enemy1->SetTarget(player);
	enemy1->SetCamera(m_camera);
	AddObject(enemy1);

	//UI系列

	//スキルゲージ
	skillGauge = std::make_shared<SkillGauge>();
	skillGauge->Init();
	skillGauge->SetGauge(100, 100);

	//体力ゲージ
	hpGauge = std::make_shared<HPGauge>();
	hpGauge->Init();
	hpGauge->SetGauge(100, 100);

	//各オブジェクトに必要な情報を格納
	tpsCamera->SetTarget(player);
	//fpsCamera->SetTarget(player);
	player->SetCamera(m_camera);
	player->RegisterUI(Player::UIType::SkillGauge, skillGauge);
	player->RegisterUI(Player::UIType::HPGauge, hpGauge);

}