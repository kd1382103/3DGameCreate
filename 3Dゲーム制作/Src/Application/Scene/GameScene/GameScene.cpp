#include "GameScene.h"
#include"../SceneManager.h"

#include<Application/GameObject/Player/Player/Player.h>
#include<Application/GameObject/Enemy/Enemy1/Enemy1.h>
#include<Application/GameObject/Enemy/Enemy2/Enemy2.h>
#include<Application/GameObject/Boss/Boss/Boss.h>

#include<Application/GameObject/Stages/Floor/Stage.h>

#include<Application/GameObject/UI/PlaeyrUI/SkillGauge/SkillGauge.h>
#include<Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/GameObject/UI/FontText/FontText.h>
#include <Application/GameObject/UI/GameClearBotton/GameClearButton.h>

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

	//---------------------------------------
	// 一定数の敵を倒したらBoss出現
	//---------------------------------------
	if (!m_isBossSpawned &&
		m_killCount >= m_bossSpawnKillCount)
	{
		m_isBossSpawned = true;

		boss = std::make_shared<Boss>();
		boss->Init();

		// Bossの出現位置
		boss->SetPos({ 0, 0, 0 });

		// 必要な情報を設定
		boss->SetTarget(player);
		boss->SetCamera(m_camera);
		boss->SetGameScene(this);

		AddObject(boss);
	}

	//---------------------------------------
	// Boss撃破
	//---------------------------------------
	if (!m_isGameClear && !m_isGameOver && m_isBossSpawned && boss && !boss->IsAlive())
	{
		m_isGameClear = true;

		//プレイヤーの操作停止 & 敵、ボスの行動停止
		player->SetGameEnd(true);
		enemy1->SetGameEnd(true);
		enemy2->SetGameEnd(true);

		if (boss)
		{
			boss->SetGameEnd(true);
		}
		
		//カメラの操作停止
		tpsCamera->m_mouseFree = true;

		//マウス解放
		ShowCursor(TRUE);
		ClipCursor(nullptr);

		// GAME CLEAR表示
		auto clearText = std::make_shared<FontText>();
		clearText->InitMessage("GAME CLEAR");
		AddObject(clearText);

		// タイトルに戻るボタン表示
		m_gameClearButton->SetVisible(true);
	}

	//---------------------------------------
	// Player死亡
	//---------------------------------------
	if (!m_isGameClear && !m_isGameOver && player && !player->IsAlive())
	{
		m_isGameOver = true;

		//プレイヤーの操作停止 & 敵、ボスの行動停止
		player->SetGameEnd(true);
		enemy1->SetGameEnd(true);
		enemy2->SetGameEnd(true);

		if (boss)
		{
			boss->SetGameEnd(true);
		}

		// カメラの操作停止
		tpsCamera->m_mouseFree = true;

		// マウス解放
		ShowCursor(TRUE);
		ClipCursor(nullptr);

		// GAME OVER表示
		auto overText = std::make_shared<FontText>();
		overText->InitMessage("GAME OVER");
		AddObject(overText);

		// タイトルに戻るボタン表示
		m_gameClearButton->SetVisible(true);
	}
	//---------------------------------------
	// クリア・ゲームオーバー後の処理
	//---------------------------------------
	if (m_isGameClear || m_isGameOver)
	{
		// ボタンがクリックされた
		if (m_gameClearButton->IsClicked())
		{
			SceneManager::Instance().SetNextScene(
				SceneManager::SceneType::Title
			);
		}
	}
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
	enemy1->SetGameScene(this);
	AddObject(enemy1);

	enemy2 = std::make_shared<Enemy2>();
	enemy2->Init();
	enemy2->SetPos({ -5, 0, -5 });
	enemy2->SetTarget(player);
	enemy2->SetCamera(m_camera);
	enemy2->SetGameScene(this);
	AddObject(enemy2);

	//UI系列
	//スキルゲージ
	skillGauge = std::make_shared<SkillGauge>();
	skillGauge->Init();
	skillGauge->SetGauge(100, 100);

	//体力ゲージ
	hpGauge = std::make_shared<HPGauge>();
	hpGauge->Init();
	hpGauge->SetGauge(100, 100);

	//ゲームクリアボタン
	m_gameClearButton = std::make_shared<GameClearButton>();
	m_gameClearButton->Init();
	m_gameClearButton->SetVisible(false);
	AddObject(m_gameClearButton);

	//各オブジェクトに必要な情報を格納
	tpsCamera->SetTarget(player);
	//fpsCamera->SetTarget(player);
	player->SetCamera(m_camera);
	player->RegisterUI(Player::UIType::SkillGauge, skillGauge);
	player->RegisterUI(Player::UIType::HPGauge, hpGauge);
}