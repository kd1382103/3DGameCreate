#include "TitleImage.h"

void TitleImage::Init()
{
	//---------------------------------------
	// タイトル背景
	//---------------------------------------
	m_titleTex = std::make_shared<KdTexture>();

	m_titleTex->Load(
		"Asset/Textures/TitleScene/Background.png"
	);

	//---------------------------------------
	// PRESS ANY BUTTON
	//---------------------------------------
	m_pressTex = std::make_shared<KdTexture>();

	m_pressTex->Load(
		"Asset/Textures/TitleScene/Button.png"
	);
}

void TitleImage::DrawSprite()
{
	//---------------------------------------
	// タイトル背景
	//---------------------------------------
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_titleTex.get(),
		-640.0f,
		-360.0f,
		1280.0f,
		720.0f,
		nullptr,
		nullptr,
		{ 0, 0 }
	);

	//---------------------------------------
	// PRESS ANY BUTTON
	//---------------------------------------
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_titleTex.get(),
		-640.0f,
		-360.0f,
		1280.0f,
		720.0f,
		nullptr,
		nullptr,
		{ 0, 0 }
	);
}