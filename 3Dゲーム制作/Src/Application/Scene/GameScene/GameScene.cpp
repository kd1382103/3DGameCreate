#include "GameScene.h"
#include"../SceneManager.h"

#include<Application/GameObject/Player/Player/Player.h>
#include<Application/GameObject/Enemy/Enemy1/Enemy1.h>
#include<Application/GameObject/Enemy/Enemy2/Enemy2.h>
#include <Application/GameObject/Enemy/TutorialEnemy/TutorialEnemy.h>
#include<Application/GameObject/Boss/Boss/Boss.h>

#include<Application/GameObject/Stages/Floor/Stage.h>

#include <Application/GameObject/UI/PlaeyrUI/SkillGauge/SkillGauge.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/GameObject/UI/FontText/FontText.h>
#include <Application/GameObject/UI/GameClearBotton/GameClearButton.h>
#include <Application/GameObject/UI/SettingUI/SettingUI.h>

#include <Application/GameObject/Effect/SwordTrail/SwordTrail.h>

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

	//=======================================
	// BGM音量反映
	//=======================================
	if (m_settingUI)
	{
		float bgmVolume = m_settingUI->GetBGMVolume();

		if (m_gameBGM)
		{
			m_gameBGM->SetVolume(bgmVolume);
		}
	}

	//=======================================
	// SE音量反映
	//=======================================
	if (m_settingUI)
	{
		float seVolume = m_settingUI->GetSEVolume();

		KdAudioManager::Instance().SetSEVolume(seVolume);
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

	//---------------------------------------
	// 設定画面
	//---------------------------------------
	bool settingKey =
		(GetAsyncKeyState(VK_TAB) & 0x8000) != 0;

	if (settingKey && !m_settingKeyPrev)
	{
		if (m_settingUI)
		{
			if (m_settingUI->IsVisible())
			{
				//---------------------------------------
				// 設定画面を閉じる
				//---------------------------------------
				m_settingUI->Close();

				//---------------------------------------
				// チュートリアル文字や敵・ボスのHPを非表示
				//---------------------------------------
				SetGameUIVisible(true);

				//---------------------------------------
				// プレイヤー入力を解除
				//---------------------------------------
				if (player) { player->SetInputLock(false); }

				//---------------------------------------
				// ゲーム再開
				//---------------------------------------
				SceneManager::Instance().SetTimeScale(1.0f);

				//---------------------------------------
				// マウスをゲーム操作に戻す
				//---------------------------------------
				if (tpsCamera) { tpsCamera->m_mouseFree = false; }

				//---------------------------------------
				// マウスカーソルを非表示
				//---------------------------------------
				ShowCursor(FALSE);
				ClipCursor(nullptr);
			}
			else
			{
				//---------------------------------------
				// 設定画面を開く
				//---------------------------------------
				m_settingUI->Open();

				//---------------------------------------
				// チュートリアル文字や敵・ボスのHPを非表示
				//---------------------------------------
				SetGameUIVisible(false);

				//---------------------------------------
				// プレイヤー入力をロック
				//---------------------------------------
				if (player) { player->SetInputLock(true); }

				//---------------------------------------
				// ゲーム停止
				//---------------------------------------
				SceneManager::Instance().SetTimeScale(0.0f);

				//---------------------------------------
				// マウスを自由にする
				//---------------------------------------
				if (tpsCamera) { tpsCamera->m_mouseFree = true; }

				//---------------------------------------
				// マウスカーソルを表示
				//---------------------------------------
				ShowCursor(TRUE);
				ClipCursor(nullptr);
			}
		}
	}

	m_settingKeyPrev = settingKey;
}

void GameScene::Init()
{
	BaseScene::Init();

	//=======================================
	// 設定UI
	//=======================================
	m_settingUI = std::make_shared<SettingUI>();
	m_settingUI->Init();
	AddObject(m_settingUI);

	//=======================================
	// 保存されている音量を取得
	//=======================================
	float bgmVolume = m_settingUI->GetBGMVolume();
	float seVolume = m_settingUI->GetSEVolume();

	//=======================================
	// BGM・SE音量を先に設定
	//=======================================
	KdAudioManager::Instance().SetBGMVolume(bgmVolume);
	KdAudioManager::Instance().SetSEVolume(seVolume);

	//=======================================
	// BGM開始
	//=======================================
	m_gameBGM = KdAudioManager::Instance().Play(
		"Asset/Sounds/BGM/BattleBGM.wav",
		SoundType::BGM,
		true
	);	
	
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

	//=======================================
	// 剣の軌跡
	//=======================================
	m_swordTrail = std::make_shared<SwordTrail>();
	m_swordTrail->Init();
	m_swordTrail->SetPlayer(player);
	AddObject(m_swordTrail);



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
	m_gameClearButton = std::make_shared<GameClearButton>();
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

void GameScene::SetGameUIVisible(bool visible)
{
	if (m_tutorialText)
	{
		m_tutorialText->SetVisible(visible);
	}

	if (player)
	{
		auto hp =
			player->GetUI<HPGauge>(
				Player::UIType::HPGauge);

		if (hp)
		{
			hp->SetVisible(visible);
		}

		auto skill =
			player->GetUI<SkillGauge>(
				Player::UIType::SkillGauge);

		if (skill)
		{
			skill->SetVisible(visible);
		}
	}

	if (enemy1)
	{
		enemy1->SetHPGaugeVisible(visible);
	}

	if (enemy2)
	{
		enemy2->SetHPGaugeVisible(visible);
	}

	if (tutorialEnemy)
	{
		tutorialEnemy->SetHPGaugeVisible(visible);
	}

	if (boss)
	{
		boss->SetHPGaugeVisible(visible);
	}
}

void GameScene::UpdateTutorialText()
{
	// チュートリアル段階が変わっていなければ何もしない
	if (m_prevTutorialStep == m_tutorialStep) { return; }

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
			"キャラクターの移動\nWASDで前後左右に移動",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::Dash:
		m_tutorialText->InitMessage(
			"歩きから走りへ切り替え\n右クリックで切り替え",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::Attack:
		m_tutorialText->InitMessage(
			"左クリックで攻撃ができる\n攻撃をし続けてコンボを決めよう ",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::Skill:
		m_tutorialText->InitMessage(
			"左の赤紫のスキルゲージの量が一定量の以上\nキーボードのEでスキル攻撃が可能",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::Ultimate:
		m_tutorialText->InitMessage(
			"スキルゲージの下の数字が500なった時\nキーボードのQを押すと強力な攻撃が可能",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::Dodge:
		m_tutorialText->InitMessage(
			"敵は攻撃してきたときに発光する\nその時に右クリックすると回避できるよ",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;

	case TutorialStep::LockOn:
		m_tutorialText->InitMessage(
			"マウスホイールを押し込むと一番近い敵をロックオンできるよ",
			{ 0.0f, 300.0f },
			1.0f
		);
		break;
	}

	AddObject(m_tutorialText);

	// 今回の段階を保存
	m_prevTutorialStep = m_tutorialStep;
}