#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class SettingAudio;
class SettingDisplay;
class SettingControl;

class SettingUI : public BaseObject
{
public:

	void Init() override;
	void Update() override;
	void DrawSprite() override;

	//---------------------------------------
	// 表示
	//---------------------------------------
	void SetVisible(bool visible)
	{
		m_visible = visible;
	}

	bool IsVisible() const
	{
		return m_visible;
	}

	//---------------------------------------
	// 設定画面を開く
	//---------------------------------------
	void Open();

	//---------------------------------------
	// 設定画面を閉じる
	//---------------------------------------
	void Close();

private:

	bool m_visible = false;

	//---------------------------------------
	// 各設定
	//---------------------------------------
	std::shared_ptr<SettingAudio>   m_audio;
	std::shared_ptr<SettingDisplay> m_display;
	std::shared_ptr<SettingControl> m_control;
};