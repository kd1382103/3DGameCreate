#include "SettingUI.h"

#include "SettingAudio/SettingAudio.h"
//#include "SettingDisplay/SettingDisplay.h"
//#include "SettingControl/SettingControl.h"

void SettingUI::Init()
{

	//---------------------------------------
	// 背景画像
	//---------------------------------------
	m_backgroundTex =
		std::make_shared<KdTexture>();

	m_backgroundTex->Load(
		"Asset/Textures/UI/Setting/SettingBackground.png"
	);

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
	// 背景
	//---------------------------------------
	if (m_backgroundTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_backgroundTex,
				0.0f,
				0.0f,
				1280.0f,
				720.0f
			);
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

float SettingUI::GetBGMVolume() const
{
	if (m_audio)
	{
		return m_audio->GetBGMVolume();
	}

	return 1.0f;
}

float SettingUI::GetSEVolume() const
{
	if (m_audio)
	{
		return m_audio->GetSEVolume();
	}

	return 1.0f;
}

void SettingUI::Open()
{
	m_visible = true;
}

void SettingUI::Close()
{
	m_visible = false;
}