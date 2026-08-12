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

class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

	void AddKillCount() { m_killCount++; }
	int GetKillCount() const { return m_killCount; }

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

	//UI
	std::shared_ptr<SkillGauge>skillGauge;
	std::shared_ptr<HPGauge>hpGauge;
	std::shared_ptr<GameClearButton> m_gameClearButton;

	//キルカウント
	int m_killCount = 0;
	static const int m_bossSpawnKillCount = 2;

	//ボス出現
	bool m_isBossSpawned = false;

	//ゲームクリア or ゲームオーバー
	bool m_isGameClear = false;
	bool m_isGameOver = false;

};
