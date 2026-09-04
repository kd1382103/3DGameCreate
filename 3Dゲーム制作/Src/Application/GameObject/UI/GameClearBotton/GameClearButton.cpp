#include "GameClearButton.h"

#include <Windows.h>

void GameClearButton::Init()
{
	// 通常状態
	m_normalTex = std::make_shared<KdTexture>();
	m_normalTex->Load(
		"Asset/Textures/UI/GameClear/ReturnTitle.png"
	);

	// マウスを乗せた状態
	m_hoverTex = std::make_shared<KdTexture>();
	m_hoverTex->Load(
		"Asset/Textures/UI/GameClear/ReturnTitleHover.png"
	);
}


//==============================================================
// Update
//==============================================================
void GameClearButton::Update()
{
	if (!m_visible)
	{
		return;
	}

	// 毎フレームクリック状態をリセット
	m_clicked = false;

	// マウスがボタン上にあるか
	m_hover = IsMouseOver();

	// 左クリック
	if (m_hover && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		m_clicked = true;
	}
}


//==============================================================
// マウスがボタンの上にあるか
//==============================================================
bool GameClearButton::IsMouseOver() const
{
	POINT mousePos;

	if (!GetCursorPos(&mousePos))
	{
		return false;
	}

	// ウィンドウハンドルを取得
	HWND hWnd = GetActiveWindow();

	if (!hWnd)
	{
		return false;
	}

	// スクリーン座標 → ウィンドウ座標
	ScreenToClient(hWnd, &mousePos);

	RECT rect;
	GetClientRect(hWnd, &rect);

	float screenWidth =
		static_cast<float>(rect.right - rect.left);

	float screenHeight =
		static_cast<float>(rect.bottom - rect.top);

	//---------------------------------------
	// 実際の画面座標 → 1280×720座標へ変換
	//---------------------------------------
	const float logicalWidth = 1280.0f;
	const float logicalHeight = 720.0f;

	float logicalMouseX =
		static_cast<float>(mousePos.x)
		* logicalWidth
		/ screenWidth;

	float logicalMouseY =
		static_cast<float>(mousePos.y)
		* logicalHeight
		/ screenHeight;

	//---------------------------------------
	// 画面中央を (0, 0) にする
	//---------------------------------------
	float mouseX =
		logicalMouseX - logicalWidth * 0.5f;

	float mouseY =
		logicalHeight * 0.5f - logicalMouseY;

	//---------------------------------------
	// ボタンの矩形
	//---------------------------------------
	float left = m_posX;
	float right = m_posX + m_width;

	float top = m_posY + m_height;
	float bottom = m_posY;

	return
		mouseX >= left &&
		mouseX <= right &&
		mouseY >= bottom &&
		mouseY <= top;
}

//==============================================================
// DrawSprite
//==============================================================
void GameClearButton::DrawSprite()
{
	if (!m_visible)
	{
		return;
	}

	auto& sprite =
		KdShaderManager::Instance().m_spriteShader;

	KdTexture* tex = nullptr;

	if (m_hover)
	{
		tex = m_hoverTex.get();
	}
	else
	{
		tex = m_normalTex.get();
	}

	if (!tex)
	{
		return;
	}

	sprite.DrawTex(
		tex,
		m_posX,
		m_posY,
		m_width,
		m_height,
		nullptr,
		nullptr,
		{ 0, 0 }
	);
}