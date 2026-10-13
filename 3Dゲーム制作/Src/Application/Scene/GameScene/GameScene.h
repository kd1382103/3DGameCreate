#pragma once

#include "../BaseScene/BaseScene.h"
#include <memory>
#include <vector>

// カメラ
class CameraBase;
class TPSCamera;

// キャラクター
class Player;
class EnemyBase;
class Enemy1;
class Enemy2;
class Enemy3;
class Enemy4;
class Enemy5;
class TutorialEnemy;
class Boss;

// ステージ
class Stage;
class Sky;
class BattleArea;

// UI・テキスト・エフェクト
class SkillGauge;
class HPGauge;
class GameClearButton;
class FontText;
class SettingUI;
class BattlePin;
class UIBackground;
class SwordTrail;

//==================================================
// 敵の出現設定
//==================================================
enum class EnemyType
{
	Enemy1,
	Enemy2,
	Enemy3,
	Enemy4,
	Enemy5
};

struct EnemySpawnInfo
{
	EnemyType type;
	int count;

	// 隊列の基準位置
	Math::Vector3 startPos;

	// 敵同士の間隔
	Math::Vector3 spacing;

	// 1列あたりの敵の数
	int columns = 3;

	// 隊列全体の向き（度数法）
	float formationAngleY = 180.0f;
};

class GameScene : public BaseScene
{
public:
	GameScene() { Init(); }
	~GameScene() {}

	//========================================
	// UI表示
	//========================================
	void SetGameUIVisible(bool visible);
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
		Tutorial,
		TutorialComplete,
		Prepare,
		Battle,
		Boss,
		Clear,
		GameOver
	};

	//========================================
	// チュートリアルステップ
	//========================================
	enum class TutorialStep
	{
		Move,
		Attack,
		Skill,
		Ultimate,
		Dodge,
		LockOn,
		Finish
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
	void InitSky();
	void InitBattleArea();
	void InitPlayer();
	void InitTutorialEnemy();
	void InitUI();
	void InitPointLights();

	//========================================
	// ゲーム進行
	//========================================
	void UpdateGameFlow();

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

	//========================================
	// 戦闘開始位置
	//========================================
	Math::Vector3 GetBattleStartPos(int battleNo)
	{
		switch (battleNo)
		{
		case 0:
			return { 0, 0, 20 };

		case 1:
			return { -2, 0, 80 };

		case 2:
			return { -60, 0, 80 };

		case 3:
			return { -80, 0, 120 };

		case 4:
			return { -80, 0, 30 };

		case 5:
			return { -35, 0, 0 };

		default:
			return { 0, 0, 0 };
		}
	}

	//========================================
	// 敵の生成・全滅判定
	//========================================
	void SpawnBattleEnemies();
	bool IsBattleEnemiesDead();
	void ClearBattleEnemies();

	//========================================
	// カメラ
	//========================================
	std::shared_ptr<CameraBase> m_camera;
	std::shared_ptr<TPSCamera> m_tpsCamera;

	//========================================
	// キャラクター
	//========================================
	std::shared_ptr<Player> m_player;
	std::vector<std::shared_ptr<EnemyBase>> m_battleEnemies;

	std::shared_ptr<TutorialEnemy> m_tutorialEnemy;
	std::shared_ptr<Boss> m_boss;

	//========================================
	// ステージ
	//========================================
	std::shared_ptr<Stage> m_stage;
	std::shared_ptr<Sky> m_sky;
	std::shared_ptr<BattleArea> m_battleArea;

	//========================================
	// バトル開始地点表示ピン
	//========================================
	std::shared_ptr<BattlePin> m_battlePin;
	void InitBattleStartPin();

	//========================================
	// UI
	//========================================
	std::shared_ptr<SkillGauge> m_skillGauge;
	std::shared_ptr<HPGauge> m_hpGauge;
	std::shared_ptr<GameClearButton> m_gameClearButton;

	std::shared_ptr<UIBackground> m_playerUIBackground;
	std::shared_ptr<UIBackground> m_tutorialBackground;

	//========================================
	// テキスト
	//========================================
	std::shared_ptr<FontText> m_tutorialText;

	//========================================
	// 設定画面
	//========================================
	std::shared_ptr<SettingUI> m_settingUI;
	bool m_settingKeyPrev = false;

	//========================================
	// BGM
	//========================================
	std::shared_ptr<KdSoundInstance> m_gameBGM;

	//========================================
	// ゲーム進行
	//========================================
	GamePhase m_gamePhase = GamePhase::Tutorial;

	float m_phaseTimer = 0.0f;
	const float m_tutorialFinishTime = 2.0f;

	bool m_isGameClear = false;
	bool m_isGameOver = false;

	//========================================
	// チュートリアル進行
	//========================================
	TutorialStep m_tutorialStep = TutorialStep::Move;
	TutorialStep m_prevTutorialStep = TutorialStep::Finish;

	//========================================
	// 戦闘進行
	//========================================
	int m_battleNo = 0;

	Math::Vector3 m_battleStartPos =
		GetBattleStartPos(m_battleNo);

	bool m_battleStarted = false;
	bool m_isBossSpawned = false;

	//========================================
	// 敵生成
	//========================================
	template<class T>
	void SpawnEnemy(std::shared_ptr<T>& enemy, int battleNo);

	//========================================
	// エフェクト
	//========================================
	std::shared_ptr<SwordTrail> m_swordTrail;

	//========================================
	// ゲーム開始演出
	//========================================
	bool m_startLandingFinished = false;

	//========================================
	// デバッグ
	//========================================
	void DebugSkipToBattle(int battleNo);

	Math::Vector3 GetBattleStartDirection(int battleNo)
	{
		switch (battleNo)
		{
		case 0: return { 0.0f, 0.0f, 1.0f };
		case 1: return { -1.0f, 0.0f, 0.0f };
		case 2: return { -1.0f, 0.0f, 0.0f };
		case 3: return { 0.0f, 0.0f, 1.0f };
		case 4: return { 0.0f, 0.0f, -1.0f };
		case 5: return { 0.0f, 0.0f, 1.0f };
		default: return { 0.0f, 0.0f, 1.0f };
		}
	}
};