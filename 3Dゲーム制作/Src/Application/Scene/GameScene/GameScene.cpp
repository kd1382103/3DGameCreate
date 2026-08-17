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
	//---------------------------------------
	// チュートリアル
	//---------------------------------------
	if (m_gamePhase == GamePhase::Tutorial)
	{
		// 今は仮でSPACEを押したらチュートリアル終了
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			m_gamePhase = GamePhase::Battle;

			// 通常敵①
			enemy1 = std::make_shared<Enemy1>();
			enemy1->Init();
			enemy1->SetPos({ 5, 0, 5 });
			enemy1->SetTarget(player);
			enemy1->SetCamera(m_camera);
			enemy1->SetGameScene(this);
			AddObject(enemy1);

			// 通常敵②
			enemy2 = std::make_shared<Enemy2>();
			enemy2->Init();
			enemy2->SetPos({ -5, 0, -5 });
			enemy2->SetTarget(player);
			enemy2->SetCamera(m_camera);
			enemy2->SetGameScene(this);
			AddObject(enemy2);
		}

		// チュートリアル中は通常戦闘処理をしない
		return;
	}

	//==============================
	// Battle
	//==============================
	if (m_gamePhase == GamePhase::Battle)
	{
		// 敵撃破 → Boss出現
	}

	//---------------------------------------
	// Boss
	//---------------------------------------
	if (m_gamePhase == GamePhase::Battle && 
		!m_isBossSpawned && 
		m_killCount >= m_bossSpawnKillCount)
	{
		m_isBossSpawned = true;

		m_gamePhase = GamePhase::Boss;

		boss = std::make_shared<Boss>();
		boss->Init();

		boss->SetPos({ 0, 0, 0 });

		boss->SetTarget(player);
		boss->SetCamera(m_camera);
		boss->SetGameScene(this);

		AddObject(boss);
	}

	//---------------------------------------
	// GAME CLEAR
	//---------------------------------------
	
	if (!m_isGameClear &&
		!m_isGameOver &&
		m_gamePhase == GamePhase::Boss &&
		boss &&
		!boss->IsAlive())
	{
		m_isGameClear = true;
		m_gamePhase = GamePhase::Clear;

		//---------------------------------------
		// プレイヤー停止
		//---------------------------------------
		if (player) { player->SetGameEnd(true); }

		//---------------------------------------
		// 敵停止
		//---------------------------------------
		if (enemy1) { enemy1->SetGameEnd(true); }
		if (enemy2) { enemy2->SetGameEnd(true); }

		//---------------------------------------
		// Boss停止
		//---------------------------------------
		if (boss) { boss->SetGameEnd(true); }

		//---------------------------------------
		// カメラ操作停止
		//---------------------------------------
		if (tpsCamera) { tpsCamera->m_mouseFree = true; }

		// マウス解放
		ShowCursor(TRUE);
		ClipCursor(nullptr);

		//---------------------------------------
		// GAME CLEAR表示
		//---------------------------------------
		auto clearText = std::make_shared<FontText>();
		clearText->InitMessage("GAME CLEAR");
		AddObject(clearText);

		// タイトルに戻るボタン表示
		if (m_gameClearButton)
		{
			m_gameClearButton->SetVisible(true);
		}
	}

	//---------------------------------------
	// GAME OVER
	//---------------------------------------
	if (!m_isGameClear &&
		!m_isGameOver &&
		player &&
		!player->IsAlive())
	{
		m_isGameOver = true;
		m_gamePhase = GamePhase::GameOver;

		//---------------------------------------
		// プレイヤー停止
		//---------------------------------------
		if (player) { player->SetGameEnd(true); }

		//---------------------------------------
		// 敵停止
		//---------------------------------------
		if (enemy1) { enemy1->SetGameEnd(true); }
		if (enemy2) { enemy2->SetGameEnd(true); }

		//---------------------------------------
		// Boss停止
		//---------------------------------------
		if (boss) { boss->SetGameEnd(true); }

		//---------------------------------------
		// カメラ操作停止
		//---------------------------------------
		if (tpsCamera) { tpsCamera->m_mouseFree = true; }

		// マウス解放
		ShowCursor(TRUE);
		ClipCursor(nullptr);

		//---------------------------------------
		// GAME OVER表示
		//---------------------------------------
		auto overText = std::make_shared<FontText>();
		overText->InitMessage("GAME OVER");
		AddObject(overText);

		// タイトルに戻るボタン表示
		if (m_gameClearButton)
		{
			m_gameClearButton->SetVisible(true);
		}
	}

	//---------------------------------------
	// 終了処理
	//---------------------------------------
	if (m_gamePhase == GamePhase::Clear ||
		m_gamePhase == GamePhase::GameOver)
	{
		if (m_gameClearButton &&
			m_gameClearButton->IsClicked())
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

	//=======================================
	// カメラ
	//=======================================
	tpsCamera = std::make_shared<TPSCamera>();
	tpsCamera->Init();
	tpsCamera->SetActive(true);
	AddObject(tpsCamera);

	m_camera = tpsCamera;

	//=======================================
	// ステージ
	//=======================================
	stage = std::shared_ptr<Stage>();

	stage = std::make_shared<Stage>();
	stage->Init();
	AddObject(stage);

	//=======================================
	// プレイヤー
	//=======================================
	player = std::make_shared<Player>();

	player->Init();
	player->SetPos(Math::Vector3{ 0, 5, 0 });
	AddObject(player);


	//=======================================
	// UI
	//=======================================

	// スキルゲージ
	skillGauge = std::make_shared<SkillGauge>();

	skillGauge->Init();
	skillGauge->SetGauge(100, 100);


	// HPゲージ
	hpGauge = std::make_shared<HPGauge>();

	hpGauge->Init();
	hpGauge->SetGauge(100, 100);


	//=======================================
	// GAME CLEAR / GAME OVER ボタン
	//=======================================
	m_gameClearButton =
		std::make_shared<GameClearButton>();

	m_gameClearButton->Init();

	m_gameClearButton->SetVisible(false);

	AddObject(m_gameClearButton);


	//=======================================
	// 各オブジェクトに必要な情報を設定
	//=======================================
	tpsCamera->SetTarget(player);

	player->SetCamera(m_camera);

	player->RegisterUI(
		Player::UIType::SkillGauge,
		skillGauge
	);

	player->RegisterUI(
		Player::UIType::HPGauge,
		hpGauge
	);
}