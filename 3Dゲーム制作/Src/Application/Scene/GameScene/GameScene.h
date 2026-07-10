#pragma once

#include"../BaseScene/BaseScene.h"

class CameraBase;
class TPSCamera;
class FPSCamera;
class Player;
class Enemy1;
class SkillGauge;
class HPGauge;

class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

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
	
	//UI
	std::shared_ptr<SkillGauge>skillGauge;
	std::shared_ptr<HPGauge>hpGauge;
};
