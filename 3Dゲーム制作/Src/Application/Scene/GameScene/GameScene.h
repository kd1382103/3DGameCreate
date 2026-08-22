#pragma once

#include"../BaseScene/BaseScene.h"

class CameraBase;
class TPSCamera;
class FPSCamera;
class Player;
class Enemy1;
class Enemy2;
class TutorialEnemy;
class SkillGauge;
class HPGauge;
class GameClearButton;
class Boss;
class Stage;
class FontText;
class SettingUI;

class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

	void AddKillCount() { m_killCount++; }
	int GetKillCount() const { return m_killCount; }
	void SetGameUIVisible(bool visible);


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

	enum class TutorialStep
	{
		Move,
		Dash,
		Attack,
		Skill,
		Ultimate,
		Dodge,
		LockOn,
		Finish
	};

private:

	void Event() override;
	void Init()  override;

	//カメラ
	std::shared_ptr<CameraBase>m_camera;
	std::shared_ptr<TPSCamera>tpsCamera;
	std::shared_ptr<FPSCamera>fpsCamera;

	//キャラクター
	std::shared_ptr<Player>player;
	std::shared_ptr<Enemy1>enemy1;
	std::shared_ptr<Enemy2>enemy2;
	std::shared_ptr<TutorialEnemy> tutorialEnemy;
	std::shared_ptr<Boss>boss;

	//ステージ
	std::shared_ptr<Stage>stage;

	//UI
	std::shared_ptr<SkillGauge>skillGauge;
	std::shared_ptr<HPGauge>hpGauge;
	std::shared_ptr<GameClearButton> m_gameClearButton;

	//テキスト
	std::shared_ptr<FontText>m_tutorialText;
	
	//設定
	std::shared_ptr<SettingUI> m_settingUI;
	
	//========================================
	// BGM
	//========================================
	std::shared_ptr<KdSoundInstance> m_gameBGM;

	//========================================
	// ゲーム進行
	//========================================
	GamePhase m_gamePhase = GamePhase::Tutorial;

	//========================================
	// チュートリアル進行
	//========================================
	TutorialStep m_tutorialStep = TutorialStep::Move;
	TutorialStep m_prevTutorialStep = TutorialStep::Finish;

	//========================================
	// チュートリアル完了演出
	//========================================
	float m_tutorialFinishTimer = 0.0f;
	const float m_tutorialFinishTime = 2.0f;

	//キルカウント
	int m_killCount = 0;
	static const int m_bossSpawnKillCount = 2;

	//ボス出現
	bool m_isBossSpawned = false;

	//ゲームクリア or ゲームオーバー
	bool m_isGameClear = false;
	bool m_isGameOver = false;

	float m_phaseTimer = 0.0f;
	Math::Vector3 m_battleStartPos = { 0, 0, 10 };
	bool m_battleStarted = false;

	//設定画面用
	bool m_isSetting = false;
	float m_savedTimeScale = 1.0f;
	bool m_settingKeyPrev = false;

	void UpdateTutorialText();
};
