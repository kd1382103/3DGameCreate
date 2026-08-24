#include "SettingUI.h"

#include "SettingAudio/SettingAudio.h"
#include "SettingDisplay/SettingDisplay.h"
//#include "SettingControl/SettingControl.h"

#include <Application/main.h>

//============================================================
// 初期化
//============================================================
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
	m_audio =
		std::make_shared<SettingAudio>();

	m_audio->Init();

	//---------------------------------------
	// Display
	//---------------------------------------
	m_display =
		std::make_shared<SettingDisplay>();

	m_display->Init();

	//---------------------------------------
	// Control
	//---------------------------------------
	//m_control =
	//	std::make_shared<SettingControl>();
	//
	//m_control->Init();

	//---------------------------------------
	// 初期状態
	//---------------------------------------
	m_visible = false;

	m_currentTab = SettingTab::Audio;

	m_menuClick = false;
}

//============================================================
// 更新
//============================================================
void SettingUI::Update()
{
	if (!m_visible)
	{
		return;
	}

	//---------------------------------------
	// 左側メニュー
	//---------------------------------------
	UpdateMenu();

	//---------------------------------------
	// 現在選択されている設定だけ更新
	//---------------------------------------
	switch (m_currentTab)
	{
	case SettingTab::Audio:

		if (m_audio)
		{
			m_audio->Update();
		}

		break;

	case SettingTab::Display:

		if (m_display)
		{
			m_display->Update();
		}

		break;

	//case SettingTab::Control:

	//	if (m_control)
	//	{
	//		m_control->Update();
	//	}

		break;
	}
}

//============================================================
// 描画
//============================================================
void SettingUI::DrawSprite()
{
	if (!m_visible)
	{
		return;
	}

	auto& shader =
		KdShaderManager::Instance().m_spriteShader;

	//---------------------------------------
	// 背景
	//---------------------------------------
	if (m_backgroundTex)
	{
		shader.DrawTex(
			m_backgroundTex,
			0,
			0,
			1280,
			720
		);
	}

	//---------------------------------------
	// 左側メニュー
	//---------------------------------------
	DrawMenu();

	//---------------------------------------
	// 現在の設定
	//---------------------------------------
	DrawCurrentSetting();
}

//============================================================
// 左側メニュー更新
//============================================================
void SettingUI::UpdateMenu()
{
	int menuCount =
		static_cast<int>(SettingTab::Count);

	//---------------------------------------
	// 各項目をチェック
	//---------------------------------------
	for (int i = 0; i < menuCount; i++)
	{
		float y =
			m_menuStartY
			- i * m_menuSpacing;

		if (IsMouseOver(
			m_menuX,
			y,
			m_menuWidth,
			m_menuHeight))
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				if (!m_menuClick)
				{
					m_currentTab =
						static_cast<SettingTab>(i);

					m_menuClick = true;
				}
			}
		}
	}

	//---------------------------------------
	// マウスを離した
	//---------------------------------------
	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		m_menuClick = false;
	}
}

//============================================================
// 左側メニュー描画
//============================================================
void SettingUI::DrawMenu()
{
	auto& shader =
		KdShaderManager::Instance().m_spriteShader;

	const char* menuText[] =
	{
		"音声設定",
		"ディスプレイ設定",
		"操作設定"
	};

	int menuCount =
		static_cast<int>(SettingTab::Count);

	for (int i = 0; i < menuCount; i++)
	{
		float y =
			m_menuStartY
			- i * m_menuSpacing;

		//---------------------------------------
		// 選択中
		//---------------------------------------
		bool selected =
			(static_cast<int>(m_currentTab) == i);

		KdSpriteShader::FontParam param;

		param.pos =
		{
			m_menuX,
			y
		};

		param.scale = 1.0f;

		param.pivot =
		{
			0.5f,
			0.5f
		};

		if (selected)
		{
			// 選択中は少し明るくする
			param.color =
			{
				0.0f,
				0.8f,
				1.0f,
				1.0f
			};

			shader.DrawFontEx(
				param,
				"> %s",
				menuText[i]
			);
		}
		else
		{
			param.color = kWhiteColor;

			shader.DrawFontEx(
				param,
				"  %s",
				menuText[i]
			);
		}
	}
}

//============================================================
// 現在の設定内容
//============================================================
void SettingUI::DrawCurrentSetting()
{
	switch (m_currentTab)
	{
	case SettingTab::Audio:

		if (m_audio)
		{
			m_audio->DrawSprite();
		}

		break;

	case SettingTab::Display:

		if (m_display)
		{
			m_display->DrawSprite();
		}

		break;

	//case SettingTab::Control:

	//	if (m_control)
	//	{
	//		m_control->DrawSprite();
	//	}

	//	break;
	}
}

//============================================================
// マウス判定
//============================================================
bool SettingUI::IsMouseOver(
	float x,
	float y,
	float width,
	float height
) const
{
	POINT mousePos;

	GetCursorPos(&mousePos);

	HWND hWnd =
		Application::Instance().GetWindowHandle();

	ScreenToClient(
		hWnd,
		&mousePos
	);

	float mouseX =
		static_cast<float>(mousePos.x)
		- 640.0f;

	float mouseY =
		360.0f
		- static_cast<float>(mousePos.y);

	return
		mouseX >= x - width * 0.5f &&
		mouseX <= x + width * 0.5f &&
		mouseY >= y - height * 0.5f &&
		mouseY <= y + height * 0.5f;
}

//============================================================
// BGM音量
//============================================================
float SettingUI::GetBGMVolume() const
{
	if (m_audio)
	{
		return m_audio->GetBGMVolume();
	}

	return 1.0f;
}

//============================================================
// SE音量
//============================================================
float SettingUI::GetSEVolume() const
{
	if (m_audio)
	{
		return m_audio->GetSEVolume();
	}

	return 1.0f;
}

//============================================================
// 開く
//============================================================
void SettingUI::Open()
{
	m_visible = true;

	m_currentTab = SettingTab::Audio;

	ShowCursor(TRUE);
}

//============================================================
// 閉じる
//============================================================
void SettingUI::Close()
{
	m_visible = false;

	ShowCursor(FALSE);
}