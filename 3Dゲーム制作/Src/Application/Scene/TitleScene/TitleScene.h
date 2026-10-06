#pragma once

#include "../BaseScene/BaseScene.h"

class SettingUI;

class CameraBase;
class TPSCamera;

class Player;
class Stage;
class Sky;


class TitleScene : public BaseScene
{
public:

	TitleScene() { Init(); }
	~TitleScene() {}

private:

	void Event() override;
	void Init() override;

	//---------------------------------------
	// 設定UI
	//---------------------------------------
	std::shared_ptr<SettingUI> m_settingUI;

	//---------------------------------------
	// 音声
	//---------------------------------------
	std::shared_ptr<KdSoundInstance> m_titleBGM;

	//---------------------------------------
	// 音声初期化
	//---------------------------------------
	void InitAudio();

	//---------------------------------------
	// 音量更新
	//---------------------------------------
	void UpdateAudioVolume();

	//==========================================================
	// タイトル画面 3D
	//==========================================================

	//---------------------------------------
	// カメラ
	//---------------------------------------
	std::shared_ptr<CameraBase> m_camera;
	std::shared_ptr<TPSCamera> m_tpsCamera;

	//---------------------------------------
	// プレイヤー
	//---------------------------------------
	std::shared_ptr<Player> m_player;

	//---------------------------------------
	// ステージ
	//---------------------------------------
	std::shared_ptr<Stage> m_stage;

	//---------------------------------------
	// 空
	//---------------------------------------
	std::shared_ptr<Sky> m_sky;

	//演出用
	bool m_startJump = false;

	//==========================================================
	// タイトル画面 3D 初期化
	//==========================================================

	void InitStage();
	void InitSky();
	void InitCamera();
	void InitPlayer();
};