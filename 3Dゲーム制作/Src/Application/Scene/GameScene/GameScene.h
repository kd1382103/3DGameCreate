#pragma once

#include"../BaseScene/BaseScene.h"

class CameraBase;
class TPSCamera;
class FPSCamera;
class Player;

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
	
	//UI
	std::shared_ptr<SkillGauge>skillGauge;
	std::shared_ptr<HPGauge>hpGauge;
};
