
#include "SettingIcon.h"

#include <Application/main.h>

//==========================================================
// 初期化
//==========================================================
void SettingIcon::Init()
{
	//---------------------------------------
	// 設定アイコン画像読み込み
	//---------------------------------------
	m_iconTex = std::make_shared<KdTexture>();

	m_iconTex->Load(
		"Asset/Textures/UI/Setting/SettingIcon.png"
	);

	//---------------------------------------
	// 初期表示
	//---------------------------------------
	m_visible = true;
}

//==========================================================
// スプライト描画
//==========================================================
void SettingIcon::DrawSprite()
{
	//---------------------------------------
	// 非表示なら描画しない
	//---------------------------------------
	if (!m_visible || !m_iconTex)
	{
		return;
	}

	auto& shader =
		KdShaderManager::Instance().m_spriteShader;

	//---------------------------------------
	// 設定アイコン
	//---------------------------------------
	shader.DrawTex(
		m_iconTex.get(),
		m_posX,
		m_posY,
		m_size,
		m_size
	);

	//---------------------------------------
	// 設定画面を開く説明
	//---------------------------------------
	KdSpriteShader::FontParam param;

	param.pos =
	{
		m_posX + 8.0f,
		m_posY - m_size * 0.5f - 8.0f
	};

	param.scale = 0.8f;

	param.pivot =
	{
		0.5f,
		0.5f
	};

	param.color =
	{
		0.0f,
		0.85f,
		1.0f,
		1.0f
	};

	shader.DrawFontEx(
		param,
		"設定を開く"
	);
}
