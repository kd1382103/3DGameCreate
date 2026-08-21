#include "GameScene.h"
#include"../SceneManager.h"

#include<Application/GameObject/Player/Player/Player.h>
#include<Application/GameObject/Enemy/Enemy1/Enemy1.h>
#include<Application/GameObject/Enemy/Enemy2/Enemy2.h>
#include <Application/GameObject/Enemy/TutorialEnemy/TutorialEnemy.h>
#include<Application/GameObject/Boss/Boss/Boss.h>

#include<Application/GameObject/Stages/Floor/Stage.h>

#include<Application/GameObject/UI/PlaeyrUI/SkillGauge/SkillGauge.h>
#include<Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/GameObject/UI/FontText/FontText.h>
#include <Application/GameObject/UI/GameClearBotton/GameClearButton.h>

#include<Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include<Application/GameObject/Camera/CameraBase.h>

#include<Application/main.h>

void GameScene::Event()
{

	//==============================
	// チュートリアル
	//==============================
	if (m_gamePhase == GamePhase::Tutorial)
	{
		// チュートリアル表示更新
		UpdateTutorialText();

		switch (m_tutorialStep)
		{
			//---------------------------------------
			// 移動
			//---------------------------------------
		case TutorialStep::Move:
		{
			if (player && player->IsMoving())
			{
				m_tutorialStep = TutorialStep::Dash;
			}

			break;
		}

		//---------------------------------------
		// ダッシュ
		//---------------------------------------
		case TutorialStep::Dash:
		{
			if (player && player->IsRunning())
			{
				m_tutorialStep = TutorialStep::Attack;
			}


			break;
		}

		//---------------------------------------
		// 通常攻撃(３段コンボ)
		//---------------------------------------
		case TutorialStep::Attack:
		{
			if (player && player->IsComboFinished())
			{
				player->ResetComboFinished();
				m_tutorialStep = TutorialStep::Skill;
			}

			break;
		}

		//---------------------------------------
		// スキル
		//---------------------------------------
		case TutorialStep::Skill:
		{
			if (player && player->IsSkillOnce())
			{
				player->SetUltimateEnergyMax();
				m_tutorialStep = TutorialStep::Ultimate;
			}

			break;
		}

		//---------------------------------------
		// 必殺技
		//---------------------------------------
		case TutorialStep::Ultimate:
		{
			if (player && player->IsUltimateOnce())
			{
				player->ResetUltimateActivated();		//発動フラグをリセット
				m_tutorialStep = TutorialStep::Dodge;

				//---------------------------------------
				// チュートリアル敵の攻撃を許可
				//---------------------------------------
				if (tutorialEnemy)
				{
					tutorialEnemy->StartTutorialAttack();
				}
			}

			break;
		}

		//---------------------------------------
		// 回避
		//---------------------------------------
		case TutorialStep::Dodge:
		{

			if (tutorialEnemy &&
				tutorialEnemy->IsTutorialAttackFinished())
			{
				tutorialEnemy->ResetTutorialAttackFinished();

				player->ResetJustDodgeSuccess();

				m_tutorialStep = TutorialStep::LockOn;
			}

			break;
		}

		//---------------------------------------
		// ロックオン
		//---------------------------------------
		case TutorialStep::LockOn:
		{
			if (player && player->IsLockOn())
			{
				m_tutorialStep = TutorialStep::Finish;
			}

			break;
		}

		//---------------------------------------
		// チュートリアル終了
		//---------------------------------------
		case TutorialStep::Finish:
		{
			//---------------------------------------
			// チュートリアル敵を削除
			//---------------------------------------
			if (tutorialEnemy)
			{
				tutorialEnemy->SetExpired();
				tutorialEnemy = nullptr;
			}

			m_gamePhase = GamePhase::TutorialComplete;
			m_phaseTimer = 0.0f;

			break;
		}
		}
	}
	//==============================
	// チュートリアル完了
	//==============================
	if (m_gamePhase == GamePhase::TutorialComplete)
	{
		m_phaseTimer += Application::Instance().GetDeltaTime();

		// 2秒経過したら準備フェーズへ
		if (m_phaseTimer >= m_tutorialFinishTime)
		{
			m_gamePhase = GamePhase::Prepare;
			m_phaseTimer = 0.0f;
		}
	}

	//==============================
	// 準備フェーズ
	//==============================
	if (m_gamePhase == GamePhase::Prepare)
	{
		if (!player)
		{
			return;
		}

		// プレイヤーと戦闘開始地点との距離
		float distance = (player->GetPos() - m_battleStartPos).Length();

		// 戦闘開始地点に近づいたら
		if (distance < 2.0f)
		{
			m_gamePhase = GamePhase::Battle;
		}
	}

	//==============================
	// Battle
	//==============================
	if (m_gamePhase == GamePhase::Battle)
	{
		if (!m_battleStarted)
		{
			m_battleStarted = true;

			//---------------------------------------
			// Enemy1生成
			//---------------------------------------
			enemy1 = std::make_shared<Enemy1>();
			enemy1->Init();

			enemy1->SetPos({ -3, 0, 15 });

			enemy1->SetTarget(player);
			enemy1->SetCamera(m_camera);
			enemy1->SetGameScene(this);

			AddObject(enemy1);


			//---------------------------------------
			// Enemy2生成
			//---------------------------------------
			enemy2 = std::make_shared<Enemy2>();
			enemy2->Init();

			enemy2->SetPos({ 3, 0, 15 });

			enemy2->SetTarget(player);
			enemy2->SetCamera(m_camera);
			enemy2->SetGameScene(this);

			AddObject(enemy2);
		}
	}
	//---------------------------------------
	// Boss
	//---------------------------------------
	if (m_gamePhase == GamePhase::Battle && m_battleStarted && !m_isBossSpawned)
	{
		bool enemy1Dead = !enemy1 || !enemy1->IsAlive();

		bool enemy2Dead = !enemy2 || !enemy2->IsAlive();

		// Enemy1とEnemy2が両方死亡
		if (enemy1Dead && enemy2Dead)
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
	if (m_gamePhase == GamePhase::Clear || m_gamePhase == GamePhase::GameOver)
	{
		if (m_gameClearButton && m_gameClearButton->IsClicked())
		{
			if (m_gameBGM)
			{
				m_gameBGM->Stop();
				m_gameBGM = nullptr;
			}
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);
		}
	}
}

void GameScene::Init()
{
	BaseScene::Init();

	//=======================================
	// BGM開始
	//=======================================
	m_gameBGM = KdAudioManager::Instance().Play("Asset/Sounds/BGM/BattleBGM.wav", true);

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
	stage = std::make_shared<Stage>();
	stage->Init();
	AddObject(stage);

	//=======================================
	// プレイヤー
	//=======================================
	player = std::make_shared<Player>();
	player->Init();
	player->SetPos(Math::Vector3{ 0, 0, 0 });
	AddObject(player);

	//---------------------------------------
	// チュートリアル敵
	//---------------------------------------
	tutorialEnemy =
		std::make_shared<TutorialEnemy>();

	tutorialEnemy->Init();

	tutorialEnemy->SetPos({ 0, 0, 5 });

	tutorialEnemy->SetTarget(player);
	tutorialEnemy->SetCamera(m_camera);
	tutorialEnemy->SetGameScene(this);

	AddObject(tutorialEnemy);

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

void GameScene::UpdateTutorialText()
{
	// チュートリアル段階が変わっていなければ何もしない
	if (m_prevTutorialStep == m_tutorialStep)
	{
		return;
	}

	//---------------------------------------
	// 前の文字を消す
	//---------------------------------------
	if (m_tutorialText)
	{
		m_tutorialText->SetExpired();
		m_tutorialText = nullptr;
	}

	//---------------------------------------
	// 新しい文字を作る
	//---------------------------------------
	m_tutorialText = std::make_shared<FontText>();

	switch (m_tutorialStep)
	{
	case TutorialStep::Move:
		m_tutorialText->InitMessage(
			"WASD : MOVE",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::Dash:
		m_tutorialText->InitMessage(
			"RIGHT CLICK : DASH",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::Attack:
		m_tutorialText->InitMessage(
			"LEFT CLICK : ATTACK",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::Skill:
		m_tutorialText->InitMessage(
			"E : SKILL",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::Ultimate:
		m_tutorialText->InitMessage(
			"Q : ULTIMATE",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::Dodge:
		m_tutorialText->InitMessage(
			"RIGHT CLICK : DODGE",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::LockOn:
		m_tutorialText->InitMessage(
			"MOUSE WHEEL : LOCK ON",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;



	//case TutorialStep::Finish:
	//	m_tutorialText->InitMessage(
	//		"TUTORIAL COMPLETE",
	//		{ 0.0f, 300.0f },
	//		1.2f
	//	);
	//	break;

	}

	AddObject(m_tutorialText);

	// 今回の段階を保存
	m_prevTutorialStep = m_tutorialStep;
}