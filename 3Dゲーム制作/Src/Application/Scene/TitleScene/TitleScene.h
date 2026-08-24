#pragma once

#include"../BaseScene/BaseScene.h"
class SettingUI;
class TitleScene : public BaseScene
{
public :

	TitleScene()  { Init(); }
	~TitleScene() {}

private :

	void Event() override;
	void Init()  override;

	//---------------------------------------
	// 設定UI
	//---------------------------------------
	std::shared_ptr<SettingUI> m_settingUI;
};
