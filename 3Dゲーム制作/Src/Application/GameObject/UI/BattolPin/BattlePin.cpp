#include "BattlePin.h"

#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/main.h>

void BattlePin::Init()
{
	//---------------------------------------
	// ピン画像
	//---------------------------------------
	m_pinPoly = std::make_shared<KdSquarePolygon>();

	m_pinPoly->SetMaterial(
		"Asset/Textures/UI/Pin/Pin.png"
	);

	// 3D空間上のオブジェクトとして扱う
	m_pinPoly->Set2DObject(false);

	// ポリゴン自体の大きさ
	m_pinPoly->SetScale(1.0f);
}

void BattlePin::Update()
{
	// 特に処理なし
}

void BattlePin::DrawSprite()
{
	if (!m_pinPoly) { return; }

	auto cam = m_camera;
	if (!cam)		{ return; }

	if (!m_visible) { return; }

	//========================================
	// カメラの向いている方向
	//========================================
	Math::Vector3 camForward =
		cam->GetCameraDir();

	//========================================
	// カメラ → ピン方向
	//========================================
	Math::Vector3 toPin =
		m_pos - cam->GetCameraPos();

	if (toPin.LengthSquared() < 0.00001f)
	{
		return;
	}

	toPin.Normalize();

	//========================================
	// 背面なら描画しない
	//========================================
	float dot =
		camForward.Dot(toPin);

	if (dot < 0.0f)
	{
		return;
	}

	//========================================
	// ワールド座標 → スクリーン座標
	//========================================
	Math::Vector3 pinPos =
		m_pos + Math::Vector3(0, 1.5f, 0);

	Math::Vector2 screen =
		cam->WorldToScreen(pinPos);

	//========================================
	// ピンの表示サイズ
	//========================================
	float width =
		128.0f * m_scale;

	float height =
		128.0f * m_scale;

	//========================================
	// 中央揃え
	//========================================
	float x =
		screen.x - width * 0.5f;

	float y =
		screen.y - height * 0.5f;

	//========================================
	// 描画
	//========================================
	auto& sprite =
		KdShaderManager::Instance().m_spriteShader;

	Math::Color color =
	{
		1.0f,
		1.0f,
		1.0f,
		1.0f
	};

	sprite.DrawTex(
		m_pinPoly->GetMaterial()->m_baseColorTex.get(),
		x,
		y,
		width,
		height,
		nullptr,
		&color,
		{ 0, 0 }
	);
}