#pragma once

#include"../BaseScene/BaseScene.h"

class CameraBase;
class TPSCamera;
class FPSCamera;
class Player;
class Enemy1;
class Enemy2;
class SkillGauge;
class HPGauge;
class GameClearButton;
class Boss;
class Stage;

class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

	void AddKillCount() { m_killCount++; }
	int GetKillCount() const { return m_killCount; }

	enum class GamePhase
	{
		Tutorial,	// 訓練場
		Battle,		// 通常戦闘
		Boss,		// ボス戦
		Clear,
		GameOver
	};

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
	std::shared_ptr<Boss>boss;

	//ステージ
	std::shared_ptr<Stage>stage;

	//UI
	std::shared_ptr<SkillGauge>skillGauge;
	std::shared_ptr<HPGauge>hpGauge;
	std::shared_ptr<GameClearButton> m_gameClearButton;

	//========================================
	// ゲーム進行
	//========================================
	GamePhase m_gamePhase = GamePhase::Tutorial;

	//========================================
	// チュートリアル進行
	//========================================
	TutorialStep m_tutorialStep = TutorialStep::Move;

	//キルカウント
	int m_killCount = 0;
	static const int m_bossSpawnKillCount = 2;

	//ボス出現
	bool m_isBossSpawned = false;

	//ゲームクリア or ゲームオーバー
	bool m_isGameClear = false;
	bool m_isGameOver = false;


};
