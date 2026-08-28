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
#include <Application/GameObject/UI/BattolPin/BattlePin.h>
#include <Application/GameObject/UI/GameClearBotton/GameClearButton.h>
#include <Application/GameObject/UI/SettingUI/SettingUI.h>

//#include <Application/GameObject/Effect/SwordTrail/SwordTrail.h>

#include<Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include<Application/GameObject/Camera/CameraBase.h>

#include<Application/main.h>

void GameScene::Event()
{
	//=======================================
	// デバッグ
	//=======================================
	UpdateDebug();

	//=======================================
	// ゲーム進行
	//=======================================
	UpdateGameFlow();

	//=======================================
	// 音量
	//=======================================
	UpdateAudioVolume();

	//=======================================
	// 設定画面
	//=======================================
	UpdateSetting();
}

void GameScene::Init()
{
	BaseScene::Init();

	InitSetting();
	InitAudio();
	InitCamera();
	InitStage();
	InitPlayer();
	InitTutorialEnemy();
	InitUI();
	InitBattleStartPin();

	//=======================================
	// 剣の軌跡
	//=======================================
	//m_swordTrail = std::make_shared<SwordTrail>();
	//m_swordTrail->Init();
	//m_swordTrail->SetPlayer(player);
	//AddObject(m_swordTrail);

}

void GameScene::InitSetting()
{
	//=======================================
	// 設定UI
	//=======================================
	m_settingUI = std::make_shared<SettingUI>();
	m_settingUI->Init();
	AddObject(m_settingUI);
}

void GameScene::InitAudio()
{
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
}

void GameScene::InitCamera()
{
	//=======================================
	// カメラ
	//=======================================
	m_tpsCamera = std::make_shared<TPSCamera>();
	m_tpsCamera->Init();
	m_tpsCamera->SetActive(true);

	AddObject(m_tpsCamera);

	m_camera = m_tpsCamera;
}

void GameScene::InitStage()
{
	//=======================================
	// ステージ
	//=======================================
	m_stage = std::make_shared<Stage>();
	m_stage->Init();

	AddObject(m_stage);
}

void GameScene::InitPlayer()
{
	//=======================================
	// プレイヤー
	//=======================================
	m_player = std::make_shared<Player>();
	m_player->Init();
	m_player->SetPos(Math::Vector3{ 0, 0, 0 });

	AddObject(m_player);
}

void GameScene::InitTutorialEnemy()
{
	//=======================================
	// チュートリアル敵
	//=======================================
	m_tutorialEnemy = std::make_shared<TutorialEnemy>();

	m_tutorialEnemy->Init();

	m_tutorialEnemy->SetPos({ 0, 0, 5 });

	m_tutorialEnemy->SetTarget(m_player);
	m_tutorialEnemy->SetCamera(m_camera);
	m_tutorialEnemy->SetGameScene(this);

	AddObject(m_tutorialEnemy);
}

void GameScene::InitUI()
{
	//=======================================
	// スキルゲージ
	//=======================================
	m_skillGauge = std::make_shared<SkillGauge>();
	m_skillGauge->Init();
	m_skillGauge->SetGauge(100, 100);

	//=======================================
	// HPゲージ
	//=======================================
	m_hpGauge = std::make_shared<HPGauge>();
	m_hpGauge->Init();
	m_hpGauge->SetGauge(100, 100);

	//=======================================
	// GAME CLEAR / GAME OVER ボタン
	//=======================================
	m_gameClearButton = std::make_shared<GameClearButton>();
	m_gameClearButton->Init();
	m_gameClearButton->SetVisible(false);

	AddObject(m_gameClearButton);

	//=======================================
	// カメラ設定
	//=======================================
	m_tpsCamera->SetTarget(m_player);

	//=======================================
	// プレイヤー設定
	//=======================================
	m_player->SetCamera(m_camera);

	m_player->RegisterUI(
		Player::UIType::SkillGauge,
		m_skillGauge
	);

	m_player->RegisterUI(
		Player::UIType::HPGauge,
		m_hpGauge
	);
}

void GameScene::UpdateGameFlow()
{
	switch (m_gamePhase)
	{
	case GamePhase::Tutorial:
		UpdateTutorial();
		break;

	case GamePhase::TutorialComplete:
		UpdateTutorialComplete();
		break;

	case GamePhase::Prepare:
		UpdatePrepare();
		break;

	case GamePhase::Battle:
		UpdateBattle();
		break;

	case GamePhase::Boss:
		UpdateBoss();
		UpdateGameEnd();
		break;

	case GamePhase::Clear:
	case GamePhase::GameOver:
		UpdateGameEnd();
		break;
	}
}
void GameScene::UpdateTutorial()
{
	if (m_gamePhase != GamePhase::Tutorial) { return; }

	// チュートリアル表示更新
	UpdateTutorialText();

	switch (m_tutorialStep)
	{
	//---------------------------------------
	// 移動
	//---------------------------------------
	case TutorialStep::Move:
	{
		if (m_player && m_player->IsMoving())
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
		if (m_player && m_player->IsRunning())
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
		if (m_player && m_player->IsComboFinished())
		{
			m_player->ResetComboFinished();
			m_tutorialStep = TutorialStep::Skill;
		}

		break;
	}

	//---------------------------------------
	// スキル
	//---------------------------------------
	case TutorialStep::Skill:
	{
		if (m_player && m_player->IsSkillOnce())
		{
			m_player->SetUltimateEnergyMax();
			m_tutorialStep = TutorialStep::Ultimate;
		}

		break;
	}

	//---------------------------------------
	// 必殺技
	//---------------------------------------
	case TutorialStep::Ultimate:
	{
		if (m_player && m_player->IsUltimateOnce())
		{
			m_player->ResetUltimateActivated();		//発動フラグをリセット
			m_tutorialStep = TutorialStep::Dodge;

			//---------------------------------------
			// チュートリアル敵の攻撃を許可
			//---------------------------------------
			if (m_tutorialEnemy)
			{
				m_tutorialEnemy->StartTutorialAttack();
			}
		}

		break;
	}

	//---------------------------------------
	// 回避
	//---------------------------------------
	case TutorialStep::Dodge:
	{

		if (m_tutorialEnemy && m_tutorialEnemy->IsTutorialAttackFinished())
		{
			//m_tutorialEnemy->ResetTutorialAttackFinished();

			m_player->ResetJustDodgeSuccess();

			m_tutorialStep = TutorialStep::LockOn;
		}

		break;
	}

	//---------------------------------------
	// ロックオン
	//---------------------------------------
	case TutorialStep::LockOn:
	{
		if (m_player && m_player->IsLockOn())
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
		if (m_tutorialEnemy)
		{
			m_tutorialEnemy->SetExpired();
			m_tutorialEnemy = nullptr;
		}

		m_gamePhase = GamePhase::TutorialComplete;
		m_phaseTimer = 0.0f;

		break;
	}
	}
}

void GameScene::UpdateTutorialComplete()
{
	if (m_gamePhase != GamePhase::TutorialComplete)
	{
		return;
	}

	m_phaseTimer += Application::Instance().GetDeltaTime();

	// 2秒経過したら準備フェーズへ
	if (m_phaseTimer >= m_tutorialFinishTime)
	{
		m_gamePhase = GamePhase::Prepare;
		m_phaseTimer = 0.0f;

		//---------------------------------------
		// 戦闘準備になったらPinを表示
		//---------------------------------------
		if (m_battlePin)
		{
			m_battlePin->SetVisible(true);
		}
	}
}

void GameScene::UpdatePrepare()
{
	if (m_gamePhase != GamePhase::Prepare) { return; }
	if (!m_player) { return; }
	if (!m_battlePin) { return; }

	//---------------------------------------
	// 戦闘エリア内に入ったか
	//---------------------------------------
	if (!m_battlePin->IsInsideRange(m_player->GetPos())) { return; }

	//---------------------------------------
	// 次の戦闘開始
	//---------------------------------------
	m_battleStarted = false;
	
	//=======================================
	// 戦闘エリアに入ったのでピンを消す
	//=======================================
	m_battlePin->SetVisible(false);

	//---------------------------------------
	// ボス戦
	//---------------------------------------
	if (m_battleNo >= 2)
	{
		m_gamePhase = GamePhase::Boss;
		m_isBossSpawned = false;
		return;
	}

	//---------------------------------------
	// 通常戦闘
	//---------------------------------------
	m_gamePhase = GamePhase::Battle;
}

void GameScene::UpdateBattle()
{
	if (m_gamePhase != GamePhase::Battle) { return; }

	//---------------------------------------
	// Battle開始
	//---------------------------------------
	if (!m_battleStarted)
	{
		m_battleStarted = true;

		//---------------------------------------
		// Enemy1生成
		//---------------------------------------
		m_enemy1 = std::make_shared<Enemy1>();
		m_enemy1->Init();

		if (m_battleNo == 0)
		{
			m_enemy1->SetPos({ -3, 0, 30 });
		}
		else if (m_battleNo == 1)
		{
			m_enemy1->SetPos({ -3, 0, 60 });
		}

		m_enemy1->SetTarget(m_player);
		m_enemy1->SetCamera(m_camera);
		m_enemy1->SetGameScene(this);

		AddObject(m_enemy1);

		//---------------------------------------
		// Enemy2生成
		//---------------------------------------
		m_enemy2 = std::make_shared<Enemy2>();
		m_enemy2->Init();

		if (m_battleNo == 0)
		{
			m_enemy2->SetPos({ 3, 0, 30 });
		}
		else if (m_battleNo == 1)
		{
			m_enemy2->SetPos({ 3, 0, 60 });
		}

		m_enemy2->SetTarget(m_player);
		m_enemy2->SetCamera(m_camera);
		m_enemy2->SetGameScene(this);

		AddObject(m_enemy2);

		return;
	}

	//---------------------------------------
	// 敵の生存確認
	//---------------------------------------
	bool enemy1Dead =
		!m_enemy1 || !m_enemy1->IsAlive();

	bool enemy2Dead =
		!m_enemy2 || !m_enemy2->IsAlive();

	//---------------------------------------
	// 全滅したら探索へ戻る
	//---------------------------------------
	if (enemy1Dead && enemy2Dead)
	{
		m_battleStarted = false;

		//---------------------------------------
		// 次の戦闘番号へ
		//---------------------------------------
		m_battleNo++;

		//---------------------------------------
		// 次の戦闘地点を取得
		//---------------------------------------
		m_battleStartPos = GetBattleStartPos(m_battleNo);

		//---------------------------------------
		// ピンを次の戦闘地点へ移動
		//---------------------------------------
		if (m_battlePin)
		{
			m_battlePin->SetPos(m_battleStartPos);
			m_battlePin->SetVisible(true);
		}
		//---------------------------------------
		// 準備フェーズへ
		//---------------------------------------
		m_gamePhase = GamePhase::Prepare;
	}
}
void GameScene::UpdateBoss()
{
	if (m_gamePhase != GamePhase::Boss) { return; }
	if (m_isBossSpawned) { return; }

	m_isBossSpawned = true;

	//---------------------------------------
	// Boss生成
	//---------------------------------------
	m_boss = std::make_shared<Boss>();
	m_boss->Init();

	m_boss->SetPos({ -200, 0, 100 });

	m_boss->SetTarget(m_player);
	m_boss->SetCamera(m_camera);
	m_boss->SetGameScene(this);

	AddObject(m_boss);
}

void GameScene::UpdateGameEnd()
{
	//---------------------------------------
	// GAME CLEAR判定
	//---------------------------------------
	if (!m_isGameClear &&
		!m_isGameOver &&
		m_gamePhase == GamePhase::Boss &&
		m_boss &&
		!m_boss->IsAlive())
	{
		GameClear();
	}

	//---------------------------------------
	// GAME OVER判定
	//---------------------------------------
	if (!m_isGameClear &&
		!m_isGameOver &&
		m_player &&
		!m_player->IsAlive())
	{
		GameOver();
	}

	//---------------------------------------
	// 終了後のボタン処理
	//---------------------------------------
	if (m_gamePhase == GamePhase::Clear ||
		m_gamePhase == GamePhase::GameOver)
	{
		if (m_gameClearButton &&
			m_gameClearButton->IsClicked())
		{
			if (m_gameBGM)
			{
				m_gameBGM->Stop();
				m_gameBGM = nullptr;
			}

			SceneManager::Instance().SetNextScene(
				SceneManager::SceneType::Title);
		}
	}
}

void GameScene::GameClear()
{
	m_isGameClear = true;
	m_gamePhase = GamePhase::Clear;

	//---------------------------------------
	// ゲーム停止
	//---------------------------------------
	StopGameObjects();

	//---------------------------------------
	// GAME CLEAR表示
	//---------------------------------------
	auto clearText = std::make_shared<FontText>();
	clearText->InitMessage("GAME CLEAR");
	AddObject(clearText);

	//---------------------------------------
	// タイトルに戻るボタン表示
	//---------------------------------------
	if (m_gameClearButton)
	{
		m_gameClearButton->SetVisible(true);
	}
}

void GameScene::GameOver()
{
	m_isGameOver = true;
	m_gamePhase = GamePhase::GameOver;

	//---------------------------------------
	// ゲーム停止
	//---------------------------------------
	StopGameObjects();

	//---------------------------------------
	// GAME OVER表示
	//---------------------------------------
	auto overText = std::make_shared<FontText>();
	overText->InitMessage("GAME OVER");
	AddObject(overText);

	//---------------------------------------
	// タイトルに戻るボタン表示
	//---------------------------------------
	if (m_gameClearButton)
	{
		m_gameClearButton->SetVisible(true);
	}
}

void GameScene::StopGameObjects()
{
	//---------------------------------------
	// プレイヤー停止
	//---------------------------------------
	if (m_player)
	{
		m_player->SetGameEnd(true);
	}

	//---------------------------------------
	// 敵停止
	//---------------------------------------
	if (m_enemy1)
	{
		m_enemy1->SetGameEnd(true);
	}

	if (m_enemy2)
	{
		m_enemy2->SetGameEnd(true);
	}

	//---------------------------------------
	// Boss停止
	//---------------------------------------
	if (m_boss)
	{
		m_boss->SetGameEnd(true);
	}

	//---------------------------------------
	// カメラ操作停止
	//---------------------------------------
	if (m_tpsCamera)
	{
		m_tpsCamera->m_mouseFree = true;
	}

	//---------------------------------------
	// マウス解放
	//---------------------------------------
	ShowCursor(TRUE);
	ClipCursor(nullptr);
}

void GameScene::UpdateSetting()
{
	bool settingKey =
		(GetAsyncKeyState(VK_TAB) & 0x8000) != 0;

	//---------------------------------------
	// TABキーを押した瞬間だけ処理
	//---------------------------------------
	if (settingKey && !m_settingKeyPrev)
	{
		if (!m_settingUI)
		{
			return;
		}

		if (m_settingUI->IsVisible())
		{
			CloseSetting();
		}
		else
		{
			OpenSetting();
		}
	}

	m_settingKeyPrev = settingKey;
}

void GameScene::OpenSetting()
{
	if (!m_settingUI)
	{
		return;
	}

	//---------------------------------------
	// 設定画面を開く
	//---------------------------------------
	m_settingUI->Open();

	//---------------------------------------
	// ゲームUI非表示
	//---------------------------------------
	SetGameUIVisible(false);

	//---------------------------------------
	// 必殺技ポイント非表示
	//---------------------------------------
	if (m_player)
	{
		m_player->SetUltimatePointVisible(false);
	}

	//---------------------------------------
	// 戦闘エリアピン非表示
	//---------------------------------------
	if (m_battlePin && m_gamePhase == GamePhase::Prepare)
	{
		m_battlePin->SetVisible(false);
	}

	//---------------------------------------
	// フライテキスト非表示
	//---------------------------------------
	SetFlyTextVisible(false);

	//---------------------------------------
	// プレイヤー入力をロック
	//---------------------------------------
	if (m_player)
	{
		m_player->SetInputLock(true);
	}

	//---------------------------------------
	// ゲーム停止
	//---------------------------------------
	SceneManager::Instance().SetTimeScale(0.0f);

	//---------------------------------------
	// マウスを自由にする
	//---------------------------------------
	if (m_tpsCamera)
	{
		m_tpsCamera->m_mouseFree = true;
	}

	ClipCursor(nullptr);
}

void GameScene::CloseSetting()
{
	if (!m_settingUI)
	{
		return;
	}

	//---------------------------------------
	// 設定画面を閉じる
	//---------------------------------------
	m_settingUI->Close();

	//---------------------------------------
	// ゲームUI表示
	//---------------------------------------
	SetGameUIVisible(true);

	//---------------------------------------
	// 必殺技ポイント再表示
	//---------------------------------------
	if (m_player)
	{
		m_player->SetUltimatePointVisible(true);
	}

	//---------------------------------------
	// 戦闘エリアピン再表示
	//---------------------------------------
	if (m_battlePin && m_gamePhase == GamePhase::Prepare)
	{
		m_battlePin->SetVisible(true);
	}

	//---------------------------------------
	// フライテキスト表示
	//---------------------------------------
	SetFlyTextVisible(true);

	//---------------------------------------
	// プレイヤー入力解除
	//---------------------------------------
	if (m_player)
	{
		m_player->SetInputLock(false);
	}

	//---------------------------------------
	// ゲーム再開
	//---------------------------------------
	SceneManager::Instance().SetTimeScale(1.0f);

	//---------------------------------------
	// マウスをゲーム操作に戻す
	//---------------------------------------
	if (m_tpsCamera)
	{
		m_tpsCamera->m_mouseFree = false;
	}

	ClipCursor(nullptr);
}

void GameScene::UpdateAudioVolume()
{
	if (!m_settingUI)
	{
		return;
	}

	//---------------------------------------
	// BGM音量
	//---------------------------------------
	float bgmVolume = m_settingUI->GetBGMVolume();

	if (m_gameBGM)
	{
		m_gameBGM->SetVolume(bgmVolume);
	}

	//---------------------------------------
	// SE音量
	//---------------------------------------
	float seVolume = m_settingUI->GetSEVolume();

	KdAudioManager::Instance().SetSEVolume(seVolume);
}

void GameScene::UpdateDebug()
{
	//========================================
	// F1：チュートリアルスキップ
	//========================================
	static bool prevF1 = false;

	bool nowF1 = (GetAsyncKeyState(VK_F1) & 0x8000) != 0;

	// 押した瞬間だけ
	if (nowF1 && !prevF1)
	{
		// チュートリアル中のみ有効
		if (m_gamePhase == GamePhase::Tutorial ||
			m_gamePhase == GamePhase::TutorialComplete ||
			m_gamePhase == GamePhase::Prepare)
		{
			//========================================
			// チュートリアル敵を削除
			//========================================
			if (m_tutorialEnemy)
			{
				m_tutorialEnemy->SetExpired();
				m_tutorialEnemy = nullptr;
			}

			//========================================
			// チュートリアル文字を削除
			//========================================
			if (m_tutorialText)
			{
				m_tutorialText->SetExpired();
				m_tutorialText = nullptr;
			}

			//========================================
			// チュートリアル完了
			//========================================
			m_tutorialStep = TutorialStep::Finish;

			//========================================
			// 準備フェーズへ
			//========================================
			m_gamePhase = GamePhase::Prepare;

			m_phaseTimer = 0.0f;

			//---------------------------------------
			// Prepareへ移行したのでPinを表示
			//---------------------------------------
			if (m_battlePin)
			{
				m_battlePin->SetVisible(true);
			}

			//========================================
			// デバッグ表示
			//========================================
			KdDebugGUI::Instance().AddLog(
				"Tutorial Skip\n"
			);
		}
	}

	prevF1 = nowF1;
}

void GameScene::InitBattleStartPin()
{
	m_battlePin =
		std::make_shared<BattlePin>();

	m_battlePin->Init();

	m_battlePin->SetPos(m_battleStartPos);
	m_battlePin->SetCamera(m_camera);

	//---------------------------------------
	// 戦闘開始範囲
	//---------------------------------------
	m_battlePin->SetBattleRange(5.0f);

	//---------------------------------------
	// チュートリアル中は非表示
	//---------------------------------------
	m_battlePin->SetVisible(false);

	AddObject(m_battlePin);
}

void GameScene::SetGameUIVisible(bool visible)
{
	if (m_tutorialText)
	{
		m_tutorialText->SetVisible(visible);
	}

	if (m_player)
	{
		auto hp =
			m_player->GetUI<HPGauge>(
				Player::UIType::HPGauge);

		if (hp)
		{
			hp->SetVisible(visible);
		}

		auto skill =
			m_player->GetUI<SkillGauge>(
				Player::UIType::SkillGauge);

		if (skill)
		{
			skill->SetVisible(visible);
		}
	}

	if (m_enemy1)
	{
		m_enemy1->SetHPGaugeVisible(visible);
	}

	if (m_enemy2)
	{
		m_enemy2->SetHPGaugeVisible(visible);
	}

	if (m_tutorialEnemy)
	{
		m_tutorialEnemy->SetHPGaugeVisible(visible);
	}

	if (m_boss)
	{
		m_boss->SetHPGaugeVisible(visible);
	}
}

void GameScene::SetFlyTextVisible(bool visible)
{
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		auto fly = std::dynamic_pointer_cast<FontText>(obj);

		if (fly)
		{
			fly->SetVisible(visible);
		}
	}
}

bool GameScene::IsSettingOpen() const
{
	return m_settingUI && m_settingUI->IsVisible();
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