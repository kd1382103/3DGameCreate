#include "BattlePin.h"

#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/GameObject/UI/FontText/FontText.h>
#include <Application/main.h>

//==============================================================
// 初期化
//==============================================================
void BattlePin::Init()
{
	//==========================================================
	// ピン画像
	//==========================================================
	m_pinPoly =
		std::make_shared<KdSquarePolygon>();

	m_pinPoly->SetMaterial(
		"Asset/Textures/UI/Pin/Pin.png"
	);

	m_pinPoly->Set2DObject(false);
	m_pinPoly->SetScale(1.0f);

	//==========================================================
	// 距離表示
	//==========================================================
	m_distanceText =
		std::make_shared<FontText>();

	m_distanceText->InitMessage(
		"0m",
		{ 0.0f, 0.0f },
		m_distanceTextScale
	);
}

//==============================================================
// 更新
//==============================================================
void BattlePin::Update()
{}

//==============================================================
// 描画
// HPGaugeのWorld表示と同じ方式
//==============================================================
void BattlePin::DrawSprite()
{
	if (!m_visible)
	{
		return;
	}

	if (!m_pinPoly)
	{
		return;
	}

	//==========================================================
	// カメラ取得
	//==========================================================
	auto cam = m_camera;

	if (!cam)
	{
		return;
	}

	//==========================================================
	// 背面チェック
	// HPGaugeのWorld表示と同じ
	//==========================================================
	Math::Vector3 camForward =
		cam->GetCameraDir();

	Math::Vector3 toPin =
		m_pos - cam->GetCameraPos();

	if (toPin.LengthSquared() < 0.00001f)
	{
		return;
	}

	toPin.Normalize();

	float dot =
		camForward.Dot(toPin);

	// 背面なら描画しない
	if (dot < 0.0f)
	{
		return;
	}

	//==========================================================
	// ワールド座標 → スクリーン座標
	// HPGaugeと同じ
	//==========================================================
	Math::Vector2 screen =
		cam->WorldToScreen(m_pos);

	//==========================================================
	// ピンサイズ
	//==========================================================
	const float size =
		128.0f * m_scale;

	//==========================================================
	// 画面外なら描画しない
	//==========================================================
	const float halfWidth = 640.0f;
	const float halfHeight = 360.0f;

	if (screen.x < -halfWidth ||
		screen.x >  halfWidth ||
		screen.y < -halfHeight ||
		screen.y >  halfHeight)
	{
		return;
	}

	//==========================================================
	// 距離計算
	//==========================================================
	Math::Vector3 diff =
		m_pos - m_playerPos;

	diff.y = 0.0f;

	float distance =
		diff.Length();

	//==========================================================
	// スプライト
	//==========================================================
	auto& sprite =
		KdShaderManager::Instance().m_spriteShader;

	//==========================================================
	// ピン描画
	//
	// HPGaugeと同じく
	// WorldToScreen → DrawTex
	//==========================================================
	Math::Color pinColor =
	{
		1.0f,
		1.0f,
		1.0f,
		1.0f
	};

	sprite.DrawTex(
		m_pinPoly->GetMaterial()->m_baseColorTex.get(),
		static_cast<int>(screen.x),
		static_cast<int>(screen.y),
		static_cast<int>(size),
		static_cast<int>(size),
		nullptr,
		&pinColor,
		{ 0.5f, 0.5f }
	);

	//==========================================================
	// 距離表示
	//==========================================================
	if (m_distanceText)
	{
		Math::Vector2 distancePos =
			screen;

		// ピンの下に表示
		distancePos.y +=
			size * 0.5f + 20.0f;

		m_distanceText->InitMessage(
			std::to_string(
				static_cast<int>(distance)
			) + "m",
			distancePos,
			m_distanceTextScale
		);

		m_distanceText->DrawSprite();
	}
}