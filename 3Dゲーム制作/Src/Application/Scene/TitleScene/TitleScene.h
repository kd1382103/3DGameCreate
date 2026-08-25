#pragma once

#include "../BaseScene/BaseScene.h"

class SettingUI;

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
};