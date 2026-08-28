#pragma once

#include "../BaseScene/BaseScene.h"

class CameraBase;
class TPSCamera;

class Player;
class Enemy1;
class Enemy2;
class TutorialEnemy;
class Boss;

class Stage;

class SkillGauge;
class HPGauge;
class GameClearButton;
class FontText;
class SettingUI;
class BattlePin;
//class SwordTrail;

class GameScene : public BaseScene
{
public:

	GameScene() { Init(); }
	~GameScene() {}

	//========================================
	// UI表示
	//========================================
	void SetGameUIVisible(bool visible);

	//========================================
	// フライテキスト表示
	//========================================
	void SetFlyTextVisible(bool visible);

	//========================================
	// 設定画面
	//========================================
	bool IsSettingOpen() const;

	//========================================
	// ゲームフェーズ
	//========================================
	enum class GamePhase
	{
		Tutorial,			// チュートリアル
		TutorialComplete,	// チュートリアル完了演出
		Prepare,			// 戦闘準備
		Battle,				// 通常戦闘
		Boss,				// ボス戦
		Clear,				// ゲームクリア
		GameOver			// ゲームオーバー
	};


	//========================================
	// チュートリアルステップ
	//========================================
	enum class TutorialStep
	{
		Move,		// 移動
		Dash,		// ダッシュ
		Attack,		// 通常攻撃
		Skill,		// スキル
		Ultimate,	// 必殺技
		Dodge,		// 回避
		LockOn,		// ロックオン
		Finish		// 終了
	};


private:

	//========================================
	// BaseScene
	//========================================
	void Event() override;
	void Init() override;

	//========================================
	// 初期化
	//========================================
	void InitSetting();
	void InitAudio();
	void InitCamera();
	void InitStage();
	void InitPlayer();
	void InitTutorialEnemy();
	void InitUI();

	//========================================
	// ゲーム進行
	//========================================
	void UpdateGameFlow();

	//========================================
	// チュートリアル
	//========================================
	void UpdateTutorial();

	void UpdateTutorialComplete();
	void UpdatePrepare();
	void UpdateBattle();
	void UpdateBoss();
	void UpdateGameEnd();

	//========================================
	// ゲーム終了
	//========================================
	void GameClear();
	void GameOver();

	void StopGameObjects();

	//========================================
	// チュートリアル文字
	//========================================
	void UpdateTutorialText();

	//========================================
	// 設定
	//========================================
	void UpdateSetting();
	void OpenSetting();
	void CloseSetting();

	//========================================
	// 音量
	//========================================
	void UpdateAudioVolume();

	//========================================
	// デバッグ
	//========================================
	void UpdateDebug();

	Math::Vector3 GetBattleStartPos(int battleNo)
	{
		switch (battleNo)
		{
		case 0:
			return { 0, 0, 10 };

		case 1:
			return { 0, 0, 60 };

		case 2:
			return { 0, 0, 100 };

		default:
			return { 0, 0, 0 };
		}
	}

private:

	//========================================
	// カメラ
	//========================================
	std::shared_ptr<CameraBase> m_camera;
	std::shared_ptr<TPSCamera> m_tpsCamera;

	//========================================
	// キャラクター
	//========================================
	std::shared_ptr<Player> m_player;

	std::shared_ptr<Enemy1> m_enemy1;
	std::shared_ptr<Enemy2> m_enemy2;

	std::shared_ptr<TutorialEnemy> m_tutorialEnemy;

	std::shared_ptr<Boss> m_boss;

	//========================================
	// ステージ
	//========================================
	std::shared_ptr<Stage> m_stage;

	//========================================
	// バトル開始位置表示ピン
	//========================================
	std::shared_ptr<BattlePin> m_battlePin;

	//========================================
	// UI
	//========================================
	std::shared_ptr<SkillGauge> m_skillGauge;
	std::shared_ptr<HPGauge> m_hpGauge;

	std::shared_ptr<GameClearButton> m_gameClearButton;


	//========================================
	// テキスト
	//========================================
	std::shared_ptr<FontText> m_tutorialText;


	//========================================
	// 設定
	//========================================
	std::shared_ptr<SettingUI> m_settingUI;


	//========================================
	// BGM
	//========================================
	std::shared_ptr<KdSoundInstance> m_gameBGM;


	//========================================
	// ゲーム進行
	//========================================

	// 現在のゲームフェーズ
	GamePhase m_gamePhase = GamePhase::Tutorial;


	//========================================
	// チュートリアル進行
	//========================================

	// 現在のチュートリアル
	TutorialStep m_tutorialStep = TutorialStep::Move;

	// 前回のチュートリアル
	// 文字更新の判定に使用
	TutorialStep m_prevTutorialStep = TutorialStep::Finish;


	//========================================
	// ゲームフェーズ用タイマー
	//========================================

	// TutorialCompleteなどで使用
	float m_phaseTimer = 0.0f;

	// チュートリアル完了からPrepareへ移行する時間
	const float m_tutorialFinishTime = 2.0f;

	//========================================
	// ゲーム終了
	//========================================

	// ゲームクリアしたか
	bool m_isGameClear = false;

	// ゲームオーバーしたか
	bool m_isGameOver = false;


	//========================================
	// 戦闘開始
	//========================================
	
	int m_battleNo = 0;
	 
	// 戦闘開始地点
	Math::Vector3 m_battleStartPos = GetBattleStartPos(m_battleNo);
	void InitBattleStartPin();

	// Enemy1・Enemy2を生成済みか
	bool m_battleStarted = false;

	// Bossをすでに出現させたか
	bool m_isBossSpawned = false;

	//========================================
	// 設定画面用
	//========================================

	// TABキーの前フレーム状態
	bool m_settingKeyPrev = false;

	//========================================
	// エフェクト
	//========================================
	//std::shared_ptr<SwordTrail> m_swordTrail;
};