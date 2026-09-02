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
	// 音量取得
	//---------------------------------------
	float GetBGMVolume() const;
	float GetSEVolume() const;

	//---------------------------------------
	// 設定画面を開く
	//---------------------------------------
	void Open();

	//---------------------------------------
	// 設定画面を閉じる
	//---------------------------------------
	void Close();

private:

	//============================================================
	// 設定項目
	//============================================================
	enum class SettingTab
	{
		Audio,
		Display,
		Control,

		Count
	};

	//---------------------------------------
	// 左側メニュー更新
	//---------------------------------------
	void UpdateMenu();

	//---------------------------------------
	// 左側メニュー描画
	//---------------------------------------
	void DrawMenu();

	//---------------------------------------
	// 現在の設定内容描画
	//---------------------------------------
	void DrawCurrentSetting();

	//---------------------------------------
	// マウスが範囲内か
	//---------------------------------------
	bool IsMouseOver(
		float x,
		float y,
		float width,
		float height
	) const;

private:

	//============================================================
	// 表示状態
	//============================================================
	bool m_visible = false;

	//---------------------------------------
	// 現在選択中の項目
	//---------------------------------------
	SettingTab m_currentTab = SettingTab::Audio;

	//---------------------------------------
	// メニュークリック状態
	//---------------------------------------
	bool m_menuClick = false;

	//---------------------------------------
	// 背景
	//---------------------------------------
	std::shared_ptr<KdTexture> m_backgroundTex;

	//---------------------------------------
	// 各設定
	//---------------------------------------
	std::shared_ptr<SettingAudio>   m_audio;
	std::shared_ptr<SettingDisplay> m_display;
	std::shared_ptr<SettingControl> m_control;

	//============================================================
	// 左側メニュー
	//============================================================
	float m_menuX = -430.0f;
	float m_menuStartY = 100.0f;
	float m_menuSpacing = 100.0f;

	float m_menuWidth = 300.0f;
	float m_menuHeight = 65.0f;

	//============================================================
	// 右側設定エリア
	//============================================================
	float m_contentX = 120.0f;
	float m_contentY = 150.0f;
};