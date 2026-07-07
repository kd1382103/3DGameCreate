#pragma once

#include"../BaseScene/BaseScene.h"

class CameraBase;
class TPSCamera;
class FPSCamera;
class Player;
class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

private:

	void Event() override;
	void Init()  override;

	std::shared_ptr<CameraBase>m_camera;
	std::shared_ptr<TPSCamera>tpsCamera;
	std::shared_ptr<FPSCamera>fpsCamera;
	std::shared_ptr<Player>player;

};
