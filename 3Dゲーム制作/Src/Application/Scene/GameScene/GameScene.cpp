#include "GameScene.h"
#include"../SceneManager.h"

#include<Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Player/PlayerState/PlayerState.h>

#include<Application/GameObject/Enemy/Enemy1/Enemy1.h>
#include<Application/GameObject/Enemy/Enemy2/Enemy2.h>
#include<Application/GameObject/Enemy/Enemy3/Enemy3.h>
#include<Application/GameObject/Enemy/Enemy4/Enemy4.h>
#include<Application/GameObject/Enemy/Enemy5/Enemy5.h>
#include <Application/GameObject/Enemy/EnemyBase.h>
#include <Application/GameObject/Enemy/TutorialEnemy/TutorialEnemy.h>
#include<Application/GameObject/Boss/Boss/Boss.h>

#include<Application/GameObject/Stages/Stage/Stage.h>
#include<Application/GameObject/Stages/Sky/Sky.h>
#include <Application/GameObject/Stages/BattleArea/BattleArea.h>

#include <Application/GameObject/UI/PlaeyrUI/SkillGauge/SkillGauge.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/GameObject/UI/PlaeyrUI/UltimateGauge/UltimateGauge.h>

#include <Application/GameObject/UI/FontText/FontText.h>
#include <Application/GameObject/UI/BattolPin/BattlePin.h>
#include <Application/GameObject/UI/GameClearBotton/GameClearButton.h>
#include <Application/GameObject/UI/SettingIcon/SettingIcon.h>
#include <Application/GameObject/UI/SettingUI/SettingUI.h>
#include <Application/GameObject/UI/PlaeyrUI/UIBackground/UIBackground.h>

#include <Application/GameObject/Effect/EffectManager.h>

#include <Application/GameObject/Effect/SwordTrail/SwordTrail.h>

#include<Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include<Application/GameObject/Camera/CameraBase.h>

#include<Application/main.h>

void GameScene::Event()
{
	//=======================================
	// ゲーム開始演出
	//=======================================
	if (!m_startLandingFinished)
	{
		if (m_player &&
			m_player->stateMachine->IsCurrent<PlayerStateLanding>())
		{
			if (m_player->m_animator.IsAnimationEnd())
			{
				m_startLandingFinished = true;

				//=======================================
				// チュートリアル敵生成
				//=======================================
				InitTutorialEnemy();
			}
		}

		return;
	}

	//=======================================
	// BattlePinへプレイヤー位置を渡す
	//=======================================
	if (m_battlePin && m_player)
	{
		m_battlePin->SetPlayerPos(
			m_player->GetPos()
		);
	}

	//=======================================
	// ゲーム終了後
	//=======================================
	if (m_gamePhase == GamePhase::Clear ||
		m_gamePhase == GamePhase::GameOver)
	{
		// タイトルに戻るボタンの処理だけ継続
		UpdateGameEnd();
		return;
	}

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

	//	CURSORINFO ・・・ WindowsAPIの構造体で、カーソルの情報を取得するために使用されます。
	//	CURSORINFO構造体には、カーソルの状態や位置などの情報が含まれています。
	
	//情報を受け取る箱の作成・取得
	CURSORINFO ci = { sizeof(CURSORINFO) };
	GetCursorInfo(&ci);

	//カーソルが表示されているかどうか
	if (ci.flags & CURSOR_SHOWING)
	{
		ShowCursor(FALSE);
	}

	//カーソルの移動範囲制限を解除
	ClipCursor(nullptr);

	InitSetting();
	InitAudio();
	InitStage();
	InitLights();
	InitBattleArea();
	InitSky();
	InitCamera();
	InitPlayer();
	InitUI();
	InitBattleStartPin();

	//---------------------------------------
	// TAB設定アイコン
	//---------------------------------------
	m_settingIcon = std::make_shared<SettingIcon>();

	m_settingIcon->Init();

	AddObject(m_settingIcon);

	//=======================================
	// 剣の軌跡
	//=======================================
	m_swordTrail = std::make_shared<SwordTrail>();
	m_swordTrail->Init();
	m_swordTrail->SetPlayer(m_player);
	AddObject(m_swordTrail);

	//=======================================
	// 環境光
	//=======================================
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({0.5,0.5,0.5,0.5f});

	//=======================================
	// 平行光
	//=======================================
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 1, -1, 1 }, { 0.5f, 0.5f, 0.5f });
}

void GameScene::InitSetting()
{
	//=======================================
	// 設定UI
	//=======================================
	m_settingUI = std::make_shared<SettingUI>();
	m_settingUI->Init();
	AddObject(m_settingUI);

	//=======================================
	// TAB設定アイコン
	//=======================================
	//std::shared_ptr m_settingIconTex = std::make_shared<KdTexture>();
	//m_settingIconTex->Load("Asset/Textures/UI/Setting/SettingIcon.png");
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
		"Asset/Sounds/BGM/BattleBGM2.wav",
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

	//=======================================
	// カメラのめり込み判定対象
	//=======================================
	m_tpsCamera->RegistHitObject(m_stage);

	AddObject(m_tpsCamera);

	m_camera = m_tpsCamera;

	//=======================================
	// エフェクトマネージャーにカメラを設定
	//=======================================
	EffectManager::Instance().SetCamera(m_camera);

	//=======================================
	// Effekseerにカメラを設定
	//=======================================
	KdEffekseerManager::GetInstance().SetCamera(
		m_tpsCamera->WorkCamera()
	);
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

void GameScene::InitSky()
{
	//=======================================
	// 空
	//=======================================
	m_sky = std::make_shared<Sky>();
	m_sky->Init();
	m_sky->SetPos({ 0, 0, 0 });
	AddObject(m_sky);
}

void GameScene::InitBattleArea()
{
	m_battleArea = std::make_shared<BattleArea>();

	m_battleArea->Init();

	// 最初の戦闘エリア
	m_battleArea->SetArea(
		GetBattleStartPos(m_battleNo),
		5,
		5
	);

	// 最初はバリアを出さない
	m_battleArea->SetBarrierActive(false);

	AddObject(m_battleArea);
}

void GameScene::InitPlayer()
{
	//=======================================
	// プレイヤー
	//=======================================
	m_player = std::make_shared<Player>();
	m_player->Init();
	m_player->SetBattleArea(m_battleArea);
	m_player->SetTutorialMode(true);
	m_player->SetPos(Math::Vector3{ 0, 0, 0 });

	//=======================================
	// ゲーム開始演出
	//=======================================
	m_player->stateMachine->ChangeStateImmediate(
		std::make_unique<PlayerStateLanding>(),
		*m_player
	);
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
	// プレイヤーUI背景
	//=======================================
	m_playerUIBackground = std::make_shared<UIBackground>();
	m_playerUIBackground->Init();
	m_playerUIBackground->InitPlayerUI();
	m_playerUIBackground->SetVisible(true);

	AddObject(m_playerUIBackground);

	//=======================================
	// スキルゲージ
	//=======================================
	m_skillGauge = std::make_shared<SkillGauge>();
	m_skillGauge->Init();
	m_skillGauge->SetGauge(0, 500);

	//=======================================
	// HPゲージ
	//=======================================
	m_hpGauge = std::make_shared<HPGauge>();
	m_hpGauge->Init();
	m_hpGauge->SetGauge(100, 100);

	//=======================================
	// 必殺技ゲージ
	//=======================================
	//m_ultimateGauge = std::make_shared<UltimateGauge>();
	//m_ultimateGauge->Init();
	//m_ultimateGauge->SetGauge(0.0f, 500.0f);

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

	//m_player->RegisterUI(
	//	Player::UIType::UltimateGauge,
	//	m_ultimateGauge
	//);
}

void GameScene::InitLights()
{
	//=======================================
	// 街灯・ボス戦エリアの発光位置
	// 座標は変更しない
	//=======================================
	const std::vector<LightData> lights =
	{
		// 街灯(位置、回転角度)
		{{ -5.75,  6.4,  -5.75 },{0,45,0}},
		{{ 20, 6.4,  -5.25 },{0,0,0} },
		{{ 45.75, 6.4, -5.75 },{0,-45,0} },
		{{ 20,  6.4,  5.25 },{0,0,0} },
		{{ 20,  6.4,  14.65 },{0,0,0} },
		{{ 20,  6.4,  25.35 },{0,0,0} },
		{{ 20,  6.4,  34.65 },{0,0,0} },
		{{ 20,  6.4,  45.5 },{0,0,0} },
		{{ 34.7,  6.4,  47.45 },{0,90,0} },
		{{ 45.5,  6.4,  47.45 },{0,90,0} },
		{{ 34,  6.4,  74 },{0,45,0} },
		{{ 45,  6.4,  20.9 },{0,90,0} },		
		{{ 45.85,  6.4,  85.95 },{0,45,0} },	
		
		{{ -5.45,  6.4,  20.9 },{0,90,0} },
		{{ -5.85,  6.4,  47.5 },{0,-45,0} },

		{{ 10,  6.4,  74.75},{0,0,0} },
		{{ -10, 6.4,  74.75 },{0,0,0} },
		{{ -30, 6.4,  74.75 },{0,0,0} },
		{{ -50, 6.4,  74.75 },{0,0,0} },
		{{ -70, 6.4,  74.75 },{0,0,0} },

		{{ 10,  6.4,  85.25 },{0,0,0} },
		{{ -10, 6.4,  85.25 },{0,0,0} },
		{{ -30, 6.4,  85.25 },{0,0,0} },
		{{ -50, 6.4,  85.25},{0,0,0} },
		{{ -70, 6.4,  85.25 },{0,0,0} },
		 
		{{ -74.5, 6.4, 112.5 },{0,90,0} },
		{{ -74.5, 6.4, 128.5 },{0,90,0} },
		{{ -74.25, 6.4, 145.75 },{0,45,0} },
		{{ -85.45, 6.4, 112.5 },{0,90,0} },
		{{ -85.45, 6.4, 128.5 },{0,90,0} },
		{{ -85.75, 6.4, 145.75 },{0,-45,0} },

		{{ -85.5, 6.4,  86.25 },{0,0,0} },

		{{ -74.5, 6.4,  55 },{0,90,0} },
		{{ -74.5, 6.4,  28.15 },{0,90,0} },
		{{ -74.5, 6.4,  1.5 },{0,90,0} },

		{{ -85.5, 6.4,  55 },{0,90,0} },
		{{ -85.5, 6.4,  28.15 },{0,90,0} },
		{{ -85.5, 6.4,   1.5 },{0,90,0} },
		{{ -85.75, 6.4, -15.75 },{0,45,0} },

		{{ -12.25, 6.4,  -4 },{0,45,0} },
		{{ -32.8, 6.4, -15.45 },{0,0,0} },
		{{ -52.85, 6.4, -15.5 },{0,0,0} },
		{{ -69.75, 6.4, -15.5 },{0,0,0} },
		{{ -69.5, 6.4,  -4.45 },{0,0,0} },

		// ボス戦エリアのライト
		{ { -34, 20, 10 },{0,0,0} }
	};

	//=======================================
	// Stage に座標を渡す
	// モデル位置とコーンライト位置は同じ
	//=======================================
	if (m_stage)
	{
		m_stage->SetLightPositions(lights);
	}
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
		UpdateGameEnd();
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
	if (m_gamePhase != GamePhase::Tutorial)
	{
		return;
	}

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
			m_tutorialStep = TutorialStep::Attack;
		}

		break;
	}

	//---------------------------------------
	// 通常攻撃（3段コンボ）
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
	// 強攻撃
	//---------------------------------------
	case TutorialStep::Skill:
	{
		if (m_player && m_player->IsSkillOnce())
		{
			// 回避チュートリアルへ移行
			m_tutorialStep = TutorialStep::Dodge;

			//---------------------------------------
			// 回避チュートリアル開始時に敵の攻撃を許可
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
		if (m_tutorialEnemy &&
			m_tutorialEnemy->IsTutorialAttackFinished())
		{
			if (m_player)
			{
				m_player->ResetJustDodgeSuccess();
			}

			//---------------------------------------
			// 必殺技チュートリアルへ移行
			//---------------------------------------
			m_tutorialStep = TutorialStep::Ultimate;

			//---------------------------------------
			// 必殺技チュートリアル開始時に敵のHPを変更
			//---------------------------------------
			if (m_tutorialEnemy)
			{
				m_tutorialEnemy->m_hpMax = 200.0f;
				m_tutorialEnemy->m_hp = 200.0f;
			}

			// ULTゲージを最大にする
			if (m_player)
			{
				m_player->SetSkillGaugeMax();
			}
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
			// 必殺技発動フラグをリセット
			m_player->ResetUltimateActivated();

			//---------------------------------------
			// 敵撃破チュートリアルへ移行
			//---------------------------------------
			m_tutorialStep = TutorialStep::KillEnemy;
		}

		break;
	}

	//---------------------------------------
	// チュートリアル敵の撃破
	//---------------------------------------
	case TutorialStep::KillEnemy:
	{
		if (m_tutorialEnemy && !m_tutorialEnemy->IsAlive())
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
		// チュートリアル終了時にMPをリセット
		//---------------------------------------
		if (m_player)
		{
			m_player->ResetSkillGauge();
		}

		//---------------------------------------
		// チュートリアル背景を非表示
		//---------------------------------------
		if (m_tutorialBackground)
		{
			m_tutorialBackground->SetVisible(false);
		}

		//---------------------------------------
		// チュートリアル敵は削除しない
		//---------------------------------------

		//---------------------------------------
		// 次のフェーズへ
		//---------------------------------------
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

	// チュートリアル終了
	if (m_player)
	{
		m_player->SetTutorialMode(false);
		m_player->ResetLockOnTarget();
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
	// 戦闘開始
	//---------------------------------------
	m_battleStarted = false;

	//=======================================
	// 戦闘エリアに入ったのでピンを消す
	//=======================================
	m_battlePin->SetVisible(false);

	//// 戦闘エリアを現在の戦闘位置に設定
	//if (m_battleArea)
	//{
	//	m_battleArea->SetArea(
	//		GetBattleStartPos(m_battleNo),
	//		10,
	//		10
	//	);

	//	// バリアを有効化
	//	m_battleArea->SetBarrierActive(true);
	//}

	//---------------------------------------
	// ボス戦
	//---------------------------------------
	if (m_battleNo >= 5)
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
		// 出現する敵だけ生成
		//---------------------------------------
		SpawnBattleEnemies();

		return;
	}

	//---------------------------------------
	// この戦闘で出現する敵の全滅確認
	//---------------------------------------
	if (!IsBattleEnemiesDead())
	{
		return;
	}

	//---------------------------------------
	// 戦闘終了
	//---------------------------------------
	m_battleStarted = false;

	//---------------------------------------
	// 戦闘終了：バリア解除
	//---------------------------------------
	if (m_battleArea)
	{
		m_battleArea->SetBarrierActive(false);
	}

	//---------------------------------------
	// playerのHP回復
	//---------------------------------------
	if (m_player)
	{
		float healAmount =
			m_player->GetHpMax() * 0.25f;

		m_player->Heal(healAmount);
	}

	//---------------------------------------
	// 次の戦闘番号へ
	//---------------------------------------
	m_battleNo++;

	//---------------------------------------
	// 次の戦闘地点を取得
	//---------------------------------------
	m_battleStartPos = GetBattleStartPos(m_battleNo);

	//---------------------------------------
	// 次の戦闘エリアを設定
	//---------------------------------------
	if (m_battleArea)
	{
		m_battleArea->SetArea(
			m_battleStartPos,
			15.0f,
			15.0f
		);

		m_battleArea->SetBarrierActive(false);
	}

	//---------------------------------------
	// ピンを次の戦闘地点へ移動
	//---------------------------------------
	if (m_battlePin)
	{
		m_battlePin->SetPos(m_battleStartPos);
		m_battlePin->SetVisible(true);
	}

	m_gamePhase = GamePhase::Prepare;
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

	m_boss->SetPos({ -35, 0, 20 });

	m_boss->StartResolve();

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
		m_boss->IsHPZero() &&
		m_clearSequence == ClearSequence::None)
	{
		m_boss->SetDeathPending(true);
		StartClearSequence();
	}

	//---------------------------------------
	// クリア演出更新
	//---------------------------------------
	if (m_clearSequence != ClearSequence::None &&
		m_clearSequence != ClearSequence::Finished)
	{
		UpdateClearSequence();
	}

	//---------------------------------------
	// GAME OVER判定
	//---------------------------------------
	if (!m_isGameClear &&
		!m_isGameOver &&
		m_player &&
		!m_player->IsAlive() &&
		m_player->IsGameEnd())
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

	//=======================================
	// 設定画面が開いていたら閉じる
	//=======================================
	if (m_settingUI && m_settingUI->IsVisible())
	{
		m_settingUI->Close();
	}

	//=======================================
	// ゲームUIを通常状態に戻す
	//=======================================
	SetGameUIVisible(true);

	if (m_player)
	{
		m_player->SetUltimatePointVisible(true);
		m_player->SetInputLock(true);
	}

	//=======================================
	// ゲーム停止
	//=======================================
	StopGameObjects();

	//=======================================
	// 時間停止を解除
	//=======================================
	SceneManager::Instance().SetTimeScale(1.0f);

	//=======================================
	// カーソルを表示
	//=======================================
	CURSORINFO ci = { sizeof(CURSORINFO) };
	GetCursorInfo(&ci);

	if (!(ci.flags & CURSOR_SHOWING))
	{
		ShowCursor(TRUE);
	}

	ClipCursor(nullptr);

	//=======================================
	// GAME CLEAR表示
	//=======================================
	auto clearText = std::make_shared<FontText>();
	clearText->InitMessage("GAME CLEAR");
	AddObject(clearText);

	//=======================================
	// タイトルに戻るボタン表示
	//=======================================
	if (m_gameClearButton)
	{
		m_gameClearButton->SetVisible(true);
	}
}
void GameScene::GameOver()
{
	m_isGameOver = true;
	m_gamePhase = GamePhase::GameOver;

	//=======================================
	// 設定画面が開いていたら閉じる
	//=======================================
	if (m_settingUI && m_settingUI->IsVisible())
	{
		m_settingUI->Close();
	}

	//=======================================
	// ゲームUIを通常状態に戻す
	//=======================================
	SetGameUIVisible(true);

	if (m_player)
	{
		m_player->SetUltimatePointVisible(true);
		m_player->SetInputLock(true);
	}

	//=======================================
	// ゲーム停止
	//=======================================
	StopGameObjects();

	//=======================================
	// 時間停止を解除
	//=======================================
	SceneManager::Instance().SetTimeScale(1.0f);

	//=======================================
	// カーソルを表示
	//=======================================
	CURSORINFO ci = { sizeof(CURSORINFO) };
	GetCursorInfo(&ci);

	if (!(ci.flags & CURSOR_SHOWING))
	{
		ShowCursor(TRUE);
	}

	ClipCursor(nullptr);

	//=======================================
	// GAME OVER表示
	//=======================================
	auto overText = std::make_shared<FontText>();
	overText->InitMessage("GAME OVER");
	AddObject(overText);

	//=======================================
	// タイトルに戻るボタン表示
	//=======================================
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
	// 敵・ボス停止
	//---------------------------------------
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj))
		{
			enemy->SetGameEnd(true);
		}

		if (auto boss = std::dynamic_pointer_cast<BossBase>(obj))
		{
			boss->SetGameEnd(true);
		}
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
	CURSORINFO ci = { sizeof(CURSORINFO) };
	GetCursorInfo(&ci);

	if (!(ci.flags & CURSOR_SHOWING))
	{
		ShowCursor(TRUE);
	}

	ClipCursor(nullptr);
}

void GameScene::UpdateSetting()
{
	if (m_gamePhase == GamePhase::GameOver ||
		m_gamePhase == GamePhase::Clear)
	{
		return;
	}

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

	if (m_settingIcon)
	{
		m_settingIcon->SetVisible(false);
	}

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

	if (m_settingIcon)
	{
		m_settingIcon->SetVisible(true);
	}

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
	static bool prevF1 = false;
	static bool prevF2 = false;
	static bool prevF3 = false;
	static bool prevF4 = false;
	static bool prevF5 = false;
	static bool prevF6 = false;


	bool nowF1 =
		(GetAsyncKeyState(VK_F1) & 0x8000) != 0;

	bool nowF2 =
		(GetAsyncKeyState(VK_F2) & 0x8000) != 0;

	bool nowF3 =
		(GetAsyncKeyState(VK_F3) & 0x8000) != 0;

	bool nowF4 =
		(GetAsyncKeyState(VK_F4) & 0x8000) != 0;

	bool nowF5 =
		(GetAsyncKeyState(VK_F5) & 0x8000) != 0;

	bool nowF6 =
		(GetAsyncKeyState(VK_F6) & 0x8000) != 0;


	//============================================================
	// F1 : チュートリアルスキップ → Battle1開始前
	//============================================================
	if (nowF1 && !prevF1)
	{
		DebugSkipToBattle(0);
	}

	//============================================================
	// F2 : Battle1をスキップ → Battle2開始前
	//============================================================
	if (nowF2 && !prevF2)
	{
		DebugSkipToBattle(1);
	}

	//============================================================
	// F3 : Battle1～2をスキップ → Battle3開始前
	//============================================================
	if (nowF3 && !prevF3)
	{
		DebugSkipToBattle(2);
	}

	//============================================================
	// F4 : Battle1～3をスキップ → Battle4開始前
	//============================================================
	if (nowF4 && !prevF4)
	{
		DebugSkipToBattle(3);
	}

	//============================================================
	// F5 : Battle1～4をスキップ → Battle5開始前
	//============================================================
	if (nowF5 && !prevF5)
	{
		DebugSkipToBattle(4);
	}

	//============================================================
	// F6 : Battle1～5をスキップ → Boss開始前
	//============================================================
	if (nowF6 && !prevF6)
	{
		DebugSkipToBattle(5);
	}

	prevF1 = nowF1;
	prevF2 = nowF2;
	prevF3 = nowF3;
	prevF4 = nowF4;
	prevF5 = nowF5;
	prevF6 = nowF6;
}

void GameScene::SpawnBattleEnemies()
{
	//=======================================
	// 今回のフェイズで出現する敵の設定
	//=======================================
	std::vector<EnemySpawnInfo> spawnList;

	switch (m_battleNo)
	{
		//=======================================
		// Battle 1
		//=======================================
	case 0:
		spawnList =
		{
			{ 
				EnemyType::Enemy1,
				3,					//出現数
				{ -3, 0, 30 },		//基準位置
				{ 3, 0, 0 },		//横一列の間隔
				0.0f				//隊列の回転角度	
			},

			{ 
				EnemyType::Enemy2, 
				2,
				{ -1.5, 0, 35 }, 
				{ 3, 0, 0 },
				0.0f
			}
		};
		break;

		//=======================================
		// Battle 2
		//=======================================
	case 1:
		spawnList =
		{
			{ 
				EnemyType::Enemy1,
				2,
				{ -10, 0, 82 }, 
				{ 4, 0, 0 },
				90.0f
			},

			{ 
				EnemyType::Enemy3,
				3,
				{ -8, 0, 83 }, 
				{ 3, 0, 0 } ,
				90.0f
			}
		};
		break;

		//=======================================
		// Battle 3
		//=======================================
	case 2:
		spawnList =
		{
			{ 
				EnemyType::Enemy1,
				1,
				{ -75, 0, 80 },
				{ 0, 0, 0 } ,
				90.0f
			},

			{ 
				EnemyType::Enemy2,
				2,
				{ -79, 0, 81.5 },
				{ 3, 0, 0 } ,
				90.0f
			},

			{ 
				EnemyType::Enemy4, 
				3,
				{ -83, 0, 83 }, 
				{ 3, 0, 0 } ,
				90.0f
			}
		};
		break;

		//=======================================
		// Battle 4
		//=======================================
	case 3:
		spawnList =
		{
			{ 
				EnemyType::Enemy3, 
				3,
				{ -84, 0, 130 }, 
				{ 4, 0, 0 } 
			},

			{ 
				EnemyType::Enemy5, 
				1,
				{ -80, 0, 127 }, 
				{ 0, 0, 0 } 
			}
		};
		break;

		//=======================================
		// Battle 5
		//=======================================
	case 4:
		spawnList =
		{
			{ 
				EnemyType::Enemy1, 
				5,
				{ -76, 0, 20 }, 
				{ 2, 0, 0 } ,
				180.0f
			},

			{ 
				EnemyType::Enemy4, 
				2,
				{ -78, 0, 25 }, 
				{ 4, 0, 0 } ,
				180.0f
			}
		};
		break;

	default:
		break;
	}

	//=======================================
	// 敵を生成
	//=======================================
	m_battleEnemies.clear();

	for (const auto& info : spawnList)
	{
		for (int i = 0; i < info.count; ++i)
		{
			std::shared_ptr<EnemyBase> enemy;

			//---------------------------------------
			// 敵の種類を決める
			//---------------------------------------
			switch (info.type)
			{
			case EnemyType::Enemy1:
			{
				auto e = std::make_shared<Enemy1>();
				e->Init(m_battleNo);
				enemy = e;
				break;
			}

			case EnemyType::Enemy2:
			{
				auto e = std::make_shared<Enemy2>();
				e->Init(m_battleNo);
				enemy = e;
				break;
			}

			case EnemyType::Enemy3:
			{
				auto e = std::make_shared<Enemy3>();
				e->Init(m_battleNo);
				enemy = e;
				break;
			}

			case EnemyType::Enemy4:
			{
				auto e = std::make_shared<Enemy4>();
				e->Init(m_battleNo);
				enemy = e;
				break;
			}

			case EnemyType::Enemy5:
			{
				auto e = std::make_shared<Enemy5>();
				e->Init(m_battleNo);
				enemy = e;
				break;
			}
			}

			if (!enemy)
			{
				continue;
			}

			//=======================================
			// 隊列全体を回転させて配置
			//=======================================
			Math::Vector3 localOffset =
				info.offset * static_cast<float>(i);

			// 度数法からラジアンに変換
			float angleRad =
				DirectX::XMConvertToRadians(info.angleY);

			Math::Matrix rotMat =
				Math::Matrix::CreateRotationY(angleRad);

			Math::Vector3 rotatedOffset =
				Math::Vector3::TransformNormal(
					localOffset,
					rotMat
				);

			Math::Vector3 pos =
				info.startPos + rotatedOffset;

			enemy->SetPos(pos);;

			//---------------------------------------
			// プレイヤー・カメラ・シーン設定
			//---------------------------------------
			enemy->SetTarget(m_player);
			enemy->SetCamera(m_camera);
			enemy->SetGameScene(this);
			enemy->LookAtPlayer();

			//---------------------------------------
			// シーンに登録
			//---------------------------------------
			AddObject(enemy);

			//---------------------------------------
			// 全滅判定用リストに登録
			//---------------------------------------
			m_battleEnemies.push_back(enemy);
		}
	}
}

bool GameScene::IsBattleEnemiesDead()
{
	//=======================================
	// 今回のフェイズで出現した敵を確認
	//=======================================
	for (const auto& enemy : m_battleEnemies)
	{
		// 生存している敵が1体でもいれば戦闘継続
		if (enemy && enemy->IsAlive())
		{
			return false;
		}
	}

	// 全員倒されていれば戦闘終了
	return true;
}

//==============================================================
// クリア演出開始
//==============================================================
void GameScene::StartClearSequence()
{
	if (m_clearSequence != ClearSequence::None)
	{
		return;
	}

	m_clearSequence = ClearSequence::CameraToBoss;
	m_clearSequenceTimer = 0.0f;

	if (m_player)
	{
		m_player->SetInputLock(true);
	}

	if (m_tpsCamera)
	{
		m_tpsCamera->m_mouseFree = true;
	}

	// ボスの死亡開始時にカメラをボスへ近づける
	if (m_boss && m_tpsCamera)
	{
		Math::Vector3 bossPos = m_boss->GetPos();

		Math::Vector3 cameraPos =
			bossPos + Math::Vector3(0.0f, 2.0f, -4.0f);

		Math::Vector3 lookAtPos =
			bossPos + Math::Vector3(0.0f, 1.5f, 0.0f);

		m_tpsCamera->StartClearCameraMove(
			cameraPos,
			lookAtPos,
			1.5f
		);
	}
}

//==============================================================
// クリア演出更新
//==============================================================
void GameScene::UpdateClearSequence()
{
	const float deltaTime =
		Application::Instance().GetDeltaTime();

	m_clearSequenceTimer += deltaTime;

	switch (m_clearSequence)
	{
		//---------------------------------------
		// ボス側へのカメラ移動完了待ち
		//---------------------------------------
	case ClearSequence::CameraToBoss:
	{
		const bool cameraFinished =
			!m_tpsCamera ||
			m_tpsCamera->IsClearCameraMoveFinished();

		if (cameraFinished)
		{
			// カメラがボスに寄り終わってから死亡演出を開始
			if (m_boss)
			{
				m_boss->StartDeath();
			}

			// ボスの死亡演出待ちへ
			m_clearSequence = ClearSequence::BossDeath;
			m_clearSequenceTimer = 0.0f;
		}

		break;
	}

	//---------------------------------------
	// ボスの死亡アニメーション・消滅待ち
	//---------------------------------------
	case ClearSequence::BossDeath:
	{
		// ボスの死亡アニメーションとディゾルブが
		// 完了するまでプレイヤー側へ移動しない
		if (!m_boss || !m_boss->IsAlive())
		{
			// プレイヤー側へのカメラ移動を開始
			if (m_tpsCamera && m_player)
			{
				const Math::Vector3 playerPos =
					m_player->GetPos();

				// ボスがいた方向を基準にカメラを配置する
				Math::Vector3 cameraDirection =
					Math::Vector3(0.0f, 0.0f, 1.0f);

				if (m_boss)
				{
					cameraDirection =
						playerPos - m_boss->GetPos();

					cameraDirection.y = 0.0f;

					if (cameraDirection.LengthSquared() > 0.0001f)
					{
						cameraDirection.Normalize();
					}
					else
					{
						cameraDirection =
							Math::Vector3(0.0f, 0.0f, 1.0f);
					}
				}

				// プレイヤーの正面側から見下ろす位置
				const Math::Vector3 cameraPos =
					playerPos
					- cameraDirection * 4.0f
					+ Math::Vector3(0.0f, 2.0f, 0.0f);

				// プレイヤーの上半身付近を見る
				const Math::Vector3 lookAtPos =
					playerPos
					+ Math::Vector3(0.0f, 1.5f, 0.0f);

				// 1.5秒かけてカメラを移動
				m_tpsCamera->StartClearCameraMove(
					cameraPos,
					lookAtPos,
					1.5f
				);
			}

			// プレイヤー側カメラの移動待ちへ
			m_clearSequence = ClearSequence::CameraToPlayer;
			m_clearSequenceTimer = 0.0f;
		}

		break;
	}

	//---------------------------------------
	// プレイヤー正面へのカメラ移動完了待ち
	//---------------------------------------
	case ClearSequence::CameraToPlayer:
	{
		const bool cameraFinished =
			!m_tpsCamera ||
			m_tpsCamera->IsClearCameraMoveFinished();

		if (cameraFinished)
		{
			if (!m_player)
			{
				FinishClearSequence();
				break;
			}

			// ジャンプ演出開始
			m_clearSequence = ClearSequence::PlayerJump;
			m_clearSequenceTimer = 0.0f;

			// プレイヤーの操作をロック
			m_player->SetInputLock(true);

			// ジャンプアニメーションを再生
			m_player->PlayAnimationAuto("Jump", false);

			// ジャンプ開始位置を保存
			m_clearJumpStartPos = m_player->GetPos();

			// プレイヤー周辺にエフェクトを再生
			EffectManager::Instance().Play(
				EffectType::Ultimate,
				m_player->GetPos()
				+ Math::Vector3(0.0f, 1.0f, 0.0f)
			);
		}

		break;
	}

	//---------------------------------------
	// プレイヤーのジャンプ演出
	//---------------------------------------
	case ClearSequence::PlayerJump:
	{
		if (!m_player)
		{
			FinishClearSequence();
			break;
		}

		// ジャンプアニメーションの終了を待つ
		if (m_player->m_animator.IsAnimationEnd())
		{
			FinishClearSequence();
		}

		break;
	}

	default:
		break;
	}
}

//==============================================================
// クリア演出終了
//==============================================================
void GameScene::FinishClearSequence()
{
	if (m_clearSequence == ClearSequence::Finished)
	{
		return;
	}

	m_clearSequence = ClearSequence::Finished;

	// 演出が終わってから結果画面を表示
	GameClear();
}

void GameScene::InitBattleStartPin()
{
	m_battlePin =
		std::make_shared<BattlePin>();

	m_battlePin->Init();

	m_battlePin->SetPos(m_battleStartPos);
	m_battlePin->SetCamera(m_camera);

	//---------------------------------------
	// プレイヤー位置
	//---------------------------------------
	//m_battlePin->SetPlayerPos(
	//	m_player->GetPos()
	//);

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

void GameScene::DebugSkipToBattle(int nextBattleNo)
{
	//========================================
	// チュートリアル文字を消す
	//========================================
	if (m_tutorialText)
	{
		m_tutorialText->SetExpired();
		m_tutorialText = nullptr;
	}

	//========================================
	// チュートリアル背景を消す
	//========================================
	if (m_tutorialBackground)
	{
		m_tutorialBackground->SetExpired();
		m_tutorialBackground = nullptr;
	}

	//========================================
	// ロックオン解除
	//========================================
	if (m_player)
	{
		m_player->ResetLockOnTarget();
	}

	//========================================
	// チュートリアル終了
	//========================================
	m_tutorialStep = TutorialStep::Finish;

	// チュートリアル敵を消す
	if (m_tutorialEnemy)
	{
		m_tutorialEnemy->SetExpired();
		m_tutorialEnemy = nullptr;
	}

	//========================================
	// 戦闘番号 & バリア解除
	//========================================
	m_battleNo = nextBattleNo;
	m_battleStarted = false;

	// デバッグ移動時はバリアを解除
	if (m_battleArea)
	{
		m_battleArea->SetBarrierActive(false);
	}
	//========================================
	// BattlePinの位置
	//========================================
	m_battleStartPos =
		GetBattleStartPos(
			m_battleNo >= 5 ? 5 : m_battleNo
		);

	if (m_battlePin)
	{
		m_battlePin->SetPos(m_battleStartPos);
		m_battlePin->SetVisible(true);
	}

	//========================================
	// PlayerをBattlePin手前へ
	//========================================
	if (m_player && m_battlePin)
	{
		Math::Vector3 direction =
			GetBattleStartDirection(
				m_battleNo >= 5 ? 5 : m_battleNo
			);

		direction.y = 0.0f;

		if (direction.LengthSquared() > 0.00001f)
		{
			direction.Normalize();

			Math::Vector3 playerPos =
				m_battlePin->GetPos() - direction * 10.0f;

			m_player->SetPos(playerPos);
		}
	}

	//========================================
	// Battle開始前で停止
	//========================================
	m_gamePhase = GamePhase::Prepare;
}

void GameScene::SetGameUIVisible(bool visible)
{
	//---------------------------------------
	// プレイヤーUI背景
	//---------------------------------------
	if (m_playerUIBackground)
	{
		m_playerUIBackground->SetVisible(visible);
	}

	//---------------------------------------
	// プレイヤーUI
	//---------------------------------------
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

		//auto ultimate =
		//	m_player->GetUI<UltimateGauge>(
		//		Player::UIType::UltimateGauge);

		//if (ultimate)
		//{
		//	ultimate->SetVisible(visible);
		//}
	}

	//---------------------------------------
	// 敵・ボスのHPゲージ
	//---------------------------------------
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (auto enemy =
			std::dynamic_pointer_cast<EnemyBase>(obj))
		{
			enemy->SetHPGaugeVisible(visible);
		}

		if (auto boss =
			std::dynamic_pointer_cast<BossBase>(obj))
		{
			boss->SetHPGaugeVisible(visible);
		}
	}

	//---------------------------------------
	// チュートリアル文字
	//---------------------------------------
	if (m_tutorialText)
	{
		m_tutorialText->SetVisible(
			visible &&
			m_gamePhase == GamePhase::Tutorial
		);
	}

	//---------------------------------------
	// チュートリアル背景
	//---------------------------------------
	if (m_tutorialBackground)
	{
		m_tutorialBackground->SetVisible(
			visible &&
			m_gamePhase == GamePhase::Tutorial &&
			m_tutorialStep != TutorialStep::Finish
		);
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
	// チュートリアル背景を初回だけ生成
	//---------------------------------------
	if (!m_tutorialBackground)
	{
		m_tutorialBackground = std::make_shared<UIBackground>();

		m_tutorialBackground->Init();
		m_tutorialBackground->InitTutorial(
			{ 0.0f, -270.0f },
			650.0f,
			150.0f
		);

		m_tutorialBackground->SetVisible(true);

		// 文字より先に登録して背景を描画する
		AddObject(m_tutorialBackground);
	}
	else
	{
		m_tutorialBackground->SetVisible(true);
	}

	//---------------------------------------
	// 新しい文字を作る
	//---------------------------------------
	m_tutorialText = std::make_shared<FontText>();

	switch (m_tutorialStep)
	{
	case TutorialStep::Move:
		m_tutorialText->InitMessage(
			"目標チュートリアルをクリア\nキャラクターの移動\nＷＡＳＤで前後左右に移動",
			{ 0.0f, -230.0f },
			1.0f
		);
		break;

	case TutorialStep::LockOn:
		m_tutorialText->InitMessage(
			"目標チュートリアルをクリア\nＱを押すと一番近い敵をロックオンする",
			{ 0.0f, -250.0f },
			1.0f
		);
		break;

	case TutorialStep::Attack:
		m_tutorialText->InitMessage(
			"目標チュートリアルをクリア\n左クリックで攻撃ができる\nコンボがあり３段まで攻撃できる ",
			{ 0.0f, -230.0f },
			1.0f
		);
		break;

	case TutorialStep::Skill:
		m_tutorialText->InitMessage(
			"目標チュートリアルをクリア\nＥキーで強攻撃の突進斬りを使用する\nまた通常攻撃と強攻撃はＵＬＴゲージを貯ることができる",
			{ 0.0f, -230.0f },
			1.0f
		);
		break;

	case TutorialStep::Dodge:
		m_tutorialText->InitMessage(
			"目標チュートリアルをクリア\n敵は攻撃してきたときに発光する\n発光時に右クリックすると回避を行いＵＬＴゲージを少し貯める",
			{ 0.0f, -235.0f },
			1.0f
		);
		break;

	case TutorialStep::Ultimate:
		m_tutorialText->InitMessage(
			"目標チュートリアルをクリア\nＳＰゲージがＭＡＸになった時\nＳＰＡＳＥを押すと強力な攻撃が可能",
			{ 0.0f, -250.0f },
			1.0f
		);
		break;
	}

	AddObject(m_tutorialText);

	// 今回の段階を保存
	m_prevTutorialStep = m_tutorialStep;
}