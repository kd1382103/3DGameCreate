
#include "UIBackground.h"

#include <Application/main.h>

//==================================================
// 初期化
//==================================================
void UIBackground::Init()
{
	m_visible = true;
	m_isTutorialBackground = false;

	// 外枠：白
	m_borderColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 内側：明るめの灰色
	m_panelColor = { 0.75f, 0.75f, 0.75f, 0.3f };
}

//==================================================
// プレイヤーHUD用
//==================================================
void UIBackground::InitPlayerUI()
{
	Init();

	m_isTutorialBackground = false;
}

//==================================================
// チュートリアル用
//==================================================
void UIBackground::InitTutorial(
	const Math::Vector2& pos,
	float width,
	float height)
{
	Init();

	m_isTutorialBackground = true;

	m_pos = pos;
	m_width = width;
	m_height = height;
}

//==================================================
// 四角形パネル
// DrawBoxのx,yは中心座標
// halfWidth,halfHeightは半分のサイズ
//==================================================
void UIBackground::DrawPanel(
	int x,
	int y,
	int halfWidth,
	int halfHeight)
{
	auto& shaderManager = KdShaderManager::Instance();
	auto& shader = shaderManager.m_spriteShader;

	// 半透明描画を有効にする
	shaderManager.ChangeBlendState(KdBlendState::Alpha);

	// 内側：灰色の半透明パネル
	Math::Color panelColor = {
		0.75f, 0.75f, 0.75f, 0.25f
	};

	shader.DrawBox(
		x,
		y,
		halfWidth,
		halfHeight,
		&panelColor,
		true
	);

	// 描画前のブレンド状態に戻す
	shaderManager.UndoBlendState();
}

//==================================================
// 描画
//==================================================
void UIBackground::DrawSprite()
{
	if (!m_visible)
	{
		return;
	}

	//---------------------------------------
	// チュートリアル背景
	//---------------------------------------
	if (m_isTutorialBackground)
	{
		DrawPanel(
			static_cast<int>(m_pos.x),
			static_cast<int>(m_pos.y),
			static_cast<int>(m_width * 0.5f),
			static_cast<int>(m_height * 0.5f)
		);

		return;
	}

	//---------------------------------------
	// プレイヤーHUD背景
	//---------------------------------------
	// HP、MP、必殺技ポイントを
	// ひとつの四角形パネルにまとめる
	DrawPanel(
		-485,
		-300,
		150,
		60
	);
}