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
	////---------------------------------------
	//// タイトル背景
	////---------------------------------------
	//KdShaderManager::Instance().m_spriteShader.DrawTex(
	//	m_titleTex.get(),
	//	-640.0f,
	//	-360.0f,
	//	1280.0f,
	//	720.0f,
	//	nullptr,
	//	nullptr,
	//	{ 0, 0 }
	//);
	
	//==========================================================
	// NEON BLADE
	//==========================================================

	if (m_logoTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_logoTex.get(),

			-350.0f,
			200.0f,

			600.0f,
			240.0f,

			nullptr,
			nullptr,
			{ 0.5f, 0.5f }
		);
	}

	//---------------------------------------
	// PRESS ANY BUTTON
	//---------------------------------------
	Math::Color pressColor = {
		1.0f,
		1.0f,
		1.0f,
		m_pressAlpha
	};

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_pressTex.get(),
		0.0f,
		-300.0f, 
		600.0f, 
		180.0f,
		nullptr,
		&pressColor,
		{ 0.5f, 0.5f }
	);
}