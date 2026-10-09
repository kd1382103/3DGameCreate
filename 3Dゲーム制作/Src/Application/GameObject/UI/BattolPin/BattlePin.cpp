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
	m_pinPoly = std::make_shared<KdSquarePolygon>();

	m_pinPoly->SetMaterial(
		"Asset/Textures/UI/Pin/Pin.png"
	);

	m_pinPoly->Set2DObject(false);
	m_pinPoly->SetScale(1.0f);

	//==========================================================
	// 距離表示
	//==========================================================
	m_distanceText = std::make_shared<FontText>();

	m_distanceText->InitMessage(
		"0",
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
// 画面端の発光
//==============================================================
void BattlePin::DrawEdgeGlow(Edge edge)
{
	auto& sprite =
		KdShaderManager::Instance().m_spriteShader;

	auto whiteTex =
		KdDirect3D::Instance().GetWhiteTex();

	if (!whiteTex)
	{
		return;
	}

	const float screenWidth = 1280.0f;
	const float screenHeight = 720.0f;

	const float halfWidth = screenWidth * 0.5f;
	const float halfHeight = screenHeight * 0.5f;

	const float glowSize = m_glowSize;
	const int steps = m_glowSteps;

	if (steps <= 0 || glowSize <= 0.0f)
	{
		return;
	}

	const float stepSize =
		glowSize / static_cast<float>(steps);

	//==========================================================
	// 発光を複数回重ねる
	//==========================================================
	for (int i = 0; i < steps; ++i)
	{
		const float distance =
			stepSize * static_cast<float>(i);

		const float t =
			1.0f -
			(static_cast<float>(i) /
				static_cast<float>(steps));

		const float alpha = 0.32f * t * t;

		Math::Color color =
		{
			0.0f,
			0.4f,
			1.0f,
			alpha
		};

		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;

		switch (edge)
		{
		case Edge::Left:
			width = stepSize;
			height = screenHeight;

			x = -halfWidth + distance + stepSize * 0.5f;
			y = 0.0f;
			break;

		case Edge::Right:
			width = stepSize;
			height = screenHeight;

			x = halfWidth - distance - stepSize * 0.5f;
			y = 0.0f;
			break;

		case Edge::Top:
			width = screenWidth;
			height = stepSize;

			x = 0.0f;
			y = halfHeight - distance - stepSize * 0.5f;
			break;

		case Edge::Bottom:
			width = screenWidth;
			height = stepSize;

			x = 0.0f;
			y = -halfHeight + distance + stepSize * 0.5f;
			break;
		}

		sprite.DrawTex(
			whiteTex.get(),
			x,
			y,
			width,
			height,
			nullptr,
			&color,
			{ 0.5f, 0.5f }
		);
	}
}

//==============================================================
// 画面端の小ピン描画
//==============================================================
void BattlePin::DrawEdgePin(
	Edge edge,
	const Math::Vector2& screen)
{
	auto& sprite =
		KdShaderManager::Instance().m_spriteShader;

	auto whiteTex =
		KdDirect3D::Instance().GetWhiteTex();

	if (!whiteTex)
	{
		return;
	}

	const float halfWidth = 640.0f;
	const float halfHeight = 360.0f;

	const float size = m_edgePinSize;
	const float edgeOffset = 25.0f;

	float x = 0.0f;
	float y = 0.0f;

	switch (edge)
	{
	case Edge::Left:
		x = -halfWidth + edgeOffset;

		y = std::clamp(
			screen.y,
			-halfHeight + size * 0.5f,
			halfHeight - size * 0.5f
		);
		break;

	case Edge::Right:
		x = halfWidth - edgeOffset;

		y = std::clamp(
			screen.y,
			-halfHeight + size * 0.5f,
			halfHeight - size * 0.5f
		);
		break;

	case Edge::Top:
		x = std::clamp(
			screen.x,
			-halfWidth + size * 0.5f,
			halfWidth - size * 0.5f
		);

		y = halfHeight - edgeOffset;
		break;

	case Edge::Bottom:
		x = std::clamp(
			screen.x,
			-halfWidth + size * 0.5f,
			halfWidth - size * 0.5f
		);

		y = -halfHeight + edgeOffset;
		break;
	}

	//==========================================================
	// 黄色い小ピン
	//==========================================================
	Math::Color color =
	{
		1.0f,
		0.85f,
		0.1f,
		1.0f
	};

	sprite.DrawTex(
		whiteTex.get(),
		x,
		y,
		size,
		size,
		nullptr,
		&color,
		{ 0.5f, 0.5f }
	);
}

//==============================================================
// 距離表示
//==============================================================
void BattlePin::DrawDistance(
	Edge edge,
	const Math::Vector2& screen,
	float distance)
{
	if (!m_distanceText)
	{
		return;
	}

	const float halfWidth = 640.0f;
	const float halfHeight = 360.0f;

	const int distanceInt =
		static_cast<int>(distance);

	const std::string text =
		std::to_string(distanceInt) + " m";

	float x = screen.x;
	float y = screen.y;

	const float offset = 45.0f;

	switch (edge)
	{
	case Edge::Left:
	case Edge::Right:
		if (screen.y >= 0.0f)
		{
			y = screen.y - offset;
		}
		else
		{
			y = screen.y + offset;
		}
		break;

	case Edge::Top:
		y = halfHeight - offset;
		break;

	case Edge::Bottom:
		y = -halfHeight + offset;
		break;
	}

	x = std::clamp(
		x,
		-halfWidth + 50.0f,
		halfWidth - 50.0f
	);

	y = std::clamp(
		y,
		-halfHeight + 30.0f,
		halfHeight - 30.0f
	);

	m_distanceText->InitMessage(
		text,
		{ x, y },
		m_distanceTextScale
	);

	m_distanceText->DrawSprite();
}

//==============================================================
// 描画
//==============================================================
void BattlePin::DrawSprite()
{
	if (!m_visible || !m_pinPoly || !m_camera)
	{
		return;
	}

	//==========================================================
	// カメラ情報
	//==========================================================
	const Math::Vector3 camPos =
		m_camera->GetCameraPos();

	const Math::Vector3 camForward =
		m_camera->GetCameraDir();

	Math::Vector3 toPin = m_pos - camPos;

	if (toPin.LengthSquared() < 0.00001f)
	{
		return;
	}

	toPin.Normalize();

	const float dot = camForward.Dot(toPin);

	//==========================================================
	// プレイヤーとバトル地点の距離
	//==========================================================
	Math::Vector3 diff = m_pos - m_playerPos;
	diff.y = 0.0f;

	const float distance = diff.Length();

	//==========================================================
	// カメラの後ろにある場合
	// 画面下端に方向ピンを表示
	//==========================================================
	if (dot < 0.0f)
	{
		Math::Vector3 flatForward = camForward;
		flatForward.y = 0.0f;

		if (flatForward.LengthSquared() > 0.00001f)
		{
			flatForward.Normalize();
		}

		const Math::Vector3 camRight =
		{
			flatForward.z,
			0.0f,
			-flatForward.x
		};

		Math::Vector3 toTarget = m_pos - camPos;
		toTarget.y = 0.0f;

		if (toTarget.LengthSquared() > 0.00001f)
		{
			toTarget.Normalize();
		}

		const float side = camRight.Dot(toTarget);

		const float maxX =
			640.0f - m_edgePinSize * 0.5f;

		Math::Vector2 backScreen;

		backScreen.x = std::clamp(
			side * maxX,
			-maxX,
			maxX
		);

		backScreen.y = -360.0f;

		DrawEdgeGlow(Edge::Bottom);
		DrawEdgePin(Edge::Bottom, backScreen);
		DrawDistance(Edge::Bottom, backScreen, distance);

		return;
	}

	//==========================================================
	// ワールド座標 → スクリーン座標
	//==========================================================
	Math::Vector2 screen =
		m_camera->WorldToScreen(m_pos);

	const float halfWidth = 640.0f;
	const float halfHeight = 360.0f;

	//==========================================================
	// 画面内判定
	//==========================================================
	const bool insideScreen =
		screen.x >= -halfWidth &&
		screen.x <= halfWidth &&
		screen.y >= -halfHeight &&
		screen.y <= halfHeight;

	//==========================================================
	// 画面内：通常のピンと距離表示
	//==========================================================
	if (insideScreen)
	{
		const float size = 128.0f * m_scale;

		auto& sprite =
			KdShaderManager::Instance().m_spriteShader;

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

		if (m_distanceText)
		{
			Math::Vector2 distancePos = screen;

			distancePos.y += size * 0.5f + 20.0f;

			m_distanceText->InitMessage(
				std::to_string(
					static_cast<int>(distance)
				) + " m",
				distancePos,
				m_distanceTextScale
			);

			m_distanceText->DrawSprite();
		}

		return;
	}

	//==========================================================
	// 画面外：最寄りの画面端を求める
	//==========================================================
	const float absX = std::abs(screen.x);
	const float absY = std::abs(screen.y);

	Edge nearestEdge;

	if (absX > absY)
	{
		nearestEdge =
			(screen.x > 0.0f)
			? Edge::Right
			: Edge::Left;
	}
	else
	{
		nearestEdge =
			(screen.y > 0.0f)
			? Edge::Top
			: Edge::Bottom;
	}

	//==========================================================
	// 画面端の発光・小ピン・距離表示
	//==========================================================
	DrawEdgeGlow(nearestEdge);
	DrawEdgePin(nearestEdge, screen);
	DrawDistance(nearestEdge, screen, distance);
}
