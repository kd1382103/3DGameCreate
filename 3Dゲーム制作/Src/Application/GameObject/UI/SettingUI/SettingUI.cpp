#include "SettingUI.h"

#include "SettingAudio/SettingAudio.h"
//#include "SettingDisplay/SettingDisplay.h"
//#include "SettingControl/SettingControl.h"

void SettingUI::Init()
{
	//---------------------------------------
	// Audio
	//---------------------------------------
	m_audio = std::make_shared<SettingAudio>();
	m_audio->Init();

	////---------------------------------------
	//// Display
	////---------------------------------------
	//m_display = std::make_shared<SettingDisplay>();
	//m_display->Init();

	////---------------------------------------
	//// Control
	////---------------------------------------
	//m_control = std::make_shared<SettingControl>();
	//m_control->Init();

	//---------------------------------------
	// 初期状態
	//---------------------------------------
	m_visible = false;
}

void SettingUI::Update()
{
	if (!m_visible)
	{
		return;
	}

	//---------------------------------------
	// 各設定UI更新
	//---------------------------------------
	if (m_audio)
	{
		m_audio->Update();
	}

	//if (m_display)
	//{
	//	m_display->Update();
	//}

	//if (m_control)
	//{
	//	m_control->Update();
	//}
}

void SettingUI::DrawSprite()
{
	if (!m_visible)
	{
		return;
	}

	//---------------------------------------
	// 各設定UI描画
	//---------------------------------------
	if (m_audio)
	{
		m_audio->DrawSprite();
	}

	//if (m_display)
	//{
	//	m_display->DrawSprite();
	//}

	//if (m_control)
	//{
	//	m_control->DrawSprite();
	//}
}

void SettingUI::Open()
{
	m_visible = true;
}

void SettingUI::Close()
{
	m_visible = false;
}