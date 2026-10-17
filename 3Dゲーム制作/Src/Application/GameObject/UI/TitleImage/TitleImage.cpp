#include "TitleImage.h"
#include <Application/main.h>
void TitleImage::Init()
{
	////---------------------------------------
	//// タイトル背景
	////---------------------------------------
	//m_titleTex = std::make_shared<KdTexture>();

	//m_titleTex->Load(
	//	"Asset/Textures/TitleScene/Title.png"
	//);

	//==========================================================
	// NEON BLADE ロゴ
	//==========================================================

	m_logoTex = std::make_shared<KdTexture>();

	m_logoTex->Load(
		"Asset/Textures/TitleScene/NEONBLADE.png"
	);

	//---------------------------------------
	// PRESS ANY BUTTON
	//---------------------------------------
	m_pressTex = std::make_shared<KdTexture>();

	m_pressTex->Load(
		"Asset/Textures/TitleScene/Button.png"
	);

	//---------------------------------------
	// フェード初期値
	//---------------------------------------
	m_pressAlpha = 1.0f;

	m_pressFadeOut = true;
}

//==============================================================
// 更新
//==============================================================
void TitleImage::Update()
{
	//---------------------------------------
	// 60FPS基準のフレーム倍率
	//---------------------------------------
	const float frameScale =
		Application::Instance()
		.GetFPSController()
		.GetFrameScale();

	//---------------------------------------
	// フェード速度
	//---------------------------------------
	const float fadeSpeed = 0.01f;

	//---------------------------------------
	// 消えていく
	//---------------------------------------
	if (m_pressFadeOut)
	{
		m_pressAlpha -= fadeSpeed * frameScale;

		if (m_pressAlpha <= 0.2f)
		{
			m_pressAlpha = 0.2f;

			m_pressFadeOut = false;
		}
	}
	//---------------------------------------
	// 見えていく
	//---------------------------------------
	else
	{
		m_pressAlpha += fadeSpeed * frameScale;

		if (m_pressAlpha >= 1.0f)
		{
			m_pressAlpha = 1.0f;

			m_pressFadeOut = true;
		}
	}
}

void TitleImage::DrawSprite()
{
	//==========================================================
	// NEON BLADE ロゴ
	// 左側に大きく表示
	//==========================================================
	if (m_logoTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_logoTex.get(),

			// 表示位置
			-200.0f,
			180.0f,

			// サイズ（変更前：600 x 240）
			900.0f,
			360.0f,

			nullptr,
			nullptr,
			{ 0.5f, 0.5f }
		);
	}

	//==========================================================
	// PRESS ANY KEY
	// ロゴの下に配置
	//==========================================================
	Math::Color pressColor = {
		1.0f,
		1.0f,
		1.0f,
		m_pressAlpha
	};

	if (m_pressTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_pressTex.get(),

			// 表示位置
			-200.0f,
			-100.0f,

			// サイズ
			900.0f,
			250.0f,

			nullptr,
			&pressColor,
			{ 0.5f, 0.5f }
		);
	}
}