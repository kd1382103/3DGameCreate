#include "BattlePin.h"

#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/GameObject/UI/FontText/FontText.h>
#include <Application/main.h>

void BattlePin::Init()
{
	m_pinPoly =
		std::make_shared<KdSquarePolygon>();

	m_pinPoly->SetMaterial(
		"Asset/Textures/UI/Pin/Pin.png"
	);

	m_pinPoly->Set2DObject(false);
	m_pinPoly->SetScale(1.0f);

	//---------------------------------------
	// 距離表示
	//---------------------------------------
	m_distanceText =
		std::make_shared<FontText>();

	m_distanceText->InitMessage(
		"0m",
		{ 0.0f, 0.0f },
		m_distanceTextScale
	);
}

void BattlePin::Update()
{}

//============================================================
// 画面端の発光
//============================================================
void BattlePin::DrawEdgeGlow(Edge edge)
{
	auto& sprite =
		KdShaderManager::Instance()
		.m_spriteShader;

	auto whiteTex =
		KdDirect3D::Instance()
		.GetWhiteTex();

	if (!whiteTex)
	{
		return;
	}

	//---------------------------------------
	// UI座標
	// 中央原点：1280 × 720
	//---------------------------------------
	const float screenWidth = 1280.0f;
	const float screenHeight = 720.0f;

	const float halfWidth = screenWidth * 0.5f;   // 640
	const float halfHeight = screenHeight * 0.5f; // 360

	const float glowSize = m_glowSize;
	const int steps = m_glowSteps;

	if (steps <= 0 || glowSize <= 0.0f)
	{
		return;
	}

	const float stepSize =
		glowSize / static_cast<float>(steps);

	//---------------------------------------
	// 発光を何層か重ねる
	//---------------------------------------
	for (int i = 0; i < steps; ++i)
	{
		const float distance =
			stepSize * static_cast<float>(i);

		//---------------------------------------
		// 外側ほど薄くする
		//---------------------------------------
		const float t =
			1.0f -
			(static_cast<float>(i) /
				static_cast<float>(steps));

		const float alpha =
			0.32f * t * t;

		Math::Color color =
		{
			0.0f,  // R
			0.4f,  // G
			1.0f,  // B
			alpha  // A
		};

		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;

		switch (edge)
		{
			//-----------------------------------
			// 左
			//-----------------------------------
		case Edge::Left:
		{
			width = stepSize;
			height = screenHeight;

			x =
				-halfWidth +
				distance +
				stepSize * 0.5f;

			y = 0.0f;

			break;
		}

		//-----------------------------------
		// 右
		//-----------------------------------
		case Edge::Right:
		{
			width = stepSize;
			height = screenHeight;

			x =
				halfWidth -
				distance -
				stepSize * 0.5f;

			y = 0.0f;

			break;
		}

		//-----------------------------------
		// 上
		//-----------------------------------
		case Edge::Top:
		{
			width = screenWidth;
			height = stepSize;

			x = 0.0f;

			y =
				halfHeight -
				distance -
				stepSize * 0.5f;

			break;
		}

		//-----------------------------------
		// 下
		//-----------------------------------
		case Edge::Bottom:
		{
			width = screenWidth;
			height = stepSize;

			x = 0.0f;

			y =
				-halfHeight +
				distance +
				stepSize * 0.5f;

			break;
		}
		}

		//---------------------------------------
		// 発光描画
		//---------------------------------------
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

//============================================================
// 画面端の小ピン描画
//============================================================
void BattlePin::DrawEdgePin(
	Edge edge,
	const Math::Vector2& screen)
{
	auto& sprite =
		KdShaderManager::Instance()
		.m_spriteShader;

	auto whiteTex =
		KdDirect3D::Instance()
		.GetWhiteTex();

	if (!whiteTex)
	{
		return;
	}

	const float halfWidth = 640.0f;
	const float halfHeight = 360.0f;

	const float size = m_edgePinSize;

	//---------------------------------------
	// 画面端からの距離
	//---------------------------------------
	const float edgeOffset = 25.0f;

	float x = 0.0f;
	float y = 0.0f;

	switch (edge)
	{
	case Edge::Left:
	{
		//-----------------------------------
		// 左端から少し内側
		//-----------------------------------
		x = -halfWidth + edgeOffset;

		y = std::clamp(
			screen.y,
			-halfHeight + size * 0.5f,
			halfHeight - size * 0.5f);

		break;
	}

	case Edge::Right:
	{
		//-----------------------------------
		// 右端から少し内側
		//-----------------------------------
		x = halfWidth - edgeOffset;

		y = std::clamp(
			screen.y,
			-halfHeight + size * 0.5f,
			halfHeight - size * 0.5f);

		break;
	}

	case Edge::Top:
	{
		//-----------------------------------
		// 上端から少し内側
		//-----------------------------------
		x = std::clamp(
			screen.x,
			-halfWidth + size * 0.5f,
			halfWidth - size * 0.5f);

		y = halfHeight - edgeOffset;

		break;
	}

	case Edge::Bottom:
	{
		//-----------------------------------
		// 下端から少し内側
		//-----------------------------------
		x = std::clamp(
			screen.x,
			-halfWidth + size * 0.5f,
			halfWidth - size * 0.5f);

		y = -halfHeight + edgeOffset;

		break;
	}
	}

	//---------------------------------------
	// 小ピン
	//---------------------------------------
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

	// 小数点以下を表示しない
	const int distanceInt =
		static_cast<int>(distance);

	const std::string text =
		std::to_string(distanceInt) + "m";

	//---------------------------------------
	// 距離文字の表示位置
	//---------------------------------------
	float x = screen.x;
	float y = screen.y;

	const float offset = 45.0f;

	switch (edge)
	{
	case Edge::Left:
	case Edge::Right:
	{
		// 上半分 → ピンの下
		if (screen.y >= 0.0f)
		{
			y = screen.y - offset;
		}
		// 下半分 → ピンの上
		else
		{
			y = screen.y + offset;
		}

		break;
	}

	case Edge::Top:
	{
		// 上端 → 常にピンの下
		y = halfHeight - offset;

		break;
	}

	case Edge::Bottom:
	{
		// 下端 → 常にピンの上
		y = -halfHeight + offset;

		break;
	}
	}

	//---------------------------------------
	// 画面中央から大きく外れないようにする
	//---------------------------------------
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

	//---------------------------------------
	// 距離テキスト更新
	//---------------------------------------
	m_distanceText->InitMessage(
		text,
		{ x, y },
		m_distanceTextScale
	);

	m_distanceText->DrawSprite();
}

//============================================================
// 描画
//============================================================
void BattlePin::DrawSprite()
{
	if (!m_pinPoly)
	{
		return;
	}

	if (!m_camera)
	{
		return;
	}

	if (!m_visible)
	{
		return;
	}

	//========================================
	// カメラ情報
	//========================================
	const Math::Vector3 camPos =
		m_camera->GetCameraPos();

	const Math::Vector3 camForward =
		m_camera->GetCameraDir();

	Math::Vector3 toPin =
		m_pos - camPos;

	if (toPin.LengthSquared() < 0.00001f)
	{
		return;
	}

	toPin.Normalize();

	//========================================
	// ピンがカメラの後ろ
	// → 下端を発光
	//========================================
	const float dot =
		camForward.Dot(toPin);

	if (dot < 0.0f)
	{
		DrawEdgeGlow(Edge::Bottom);

		//---------------------------------------
		// カメラの正面方向
		//---------------------------------------
		Math::Vector3 camForward =
			m_camera->GetCameraDir();

		camForward.y = 0.0f;

		if (camForward.LengthSquared() > 0.00001f)
		{
			camForward.Normalize();
		}

		//---------------------------------------
		// カメラの右方向を計算
		//---------------------------------------
		Math::Vector3 camRight =
		{
			camForward.z,
			0.0f,
			-camForward.x
		};

		//---------------------------------------
		// ターゲット方向
		//---------------------------------------
		Math::Vector3 toTarget =
			m_pos - camPos;

		toTarget.y = 0.0f;

		if (toTarget.LengthSquared() > 0.00001f)
		{
			toTarget.Normalize();
		}

		//---------------------------------------
		// ターゲットが画面の左右どちらにいるか
		//---------------------------------------
		const float side =
			camRight.Dot(toTarget);

		//---------------------------------------
		// 画面下端の位置
		//---------------------------------------
		const float maxX =
			640.0f - m_edgePinSize * 0.5f;

		Math::Vector2 backScreen;

		backScreen.x =
			std::clamp(
				side * maxX,
				-maxX,
				maxX
			);

		backScreen.y = -360.0f;

		//---------------------------------------
		// 小ピン
		//---------------------------------------
		DrawEdgePin(
			Edge::Bottom,
			backScreen
		);

		//---------------------------------------
		// 距離
		//---------------------------------------
		const Math::Vector3 diff =
			m_pos - m_playerPos;

		Math::Vector3 groundDiff = diff;
		groundDiff.y = 0.0f;

		const float distance =
			groundDiff.Length();

		DrawDistance(
			Edge::Bottom,
			backScreen,
			distance
		);

		return;
	}

	//========================================
	// ピンのワールド座標
	//========================================
	Math::Vector3 pinWorldPos =
		m_pos +
		Math::Vector3(0.0f, 1.5f, 0.0f);

	//========================================
	// 1280×720基準
	//
	// 中央 = (0, 0)
	// 左   = -640
	// 右   = +640
	// 上   = +360
	// 下   = -360
	//========================================
	Math::Vector2 screen =
		m_camera->WorldToScreen(pinWorldPos);

	//========================================
	// 距離計算
	//========================================
	Math::Vector3 diff =
		m_pos - m_playerPos;

	diff.y = 0.0f;

	const float distance =
		diff.Length();

	//========================================
	// 画面内判定
	//========================================
	const float left = -640.0f;
	const float right = 640.0f;
	const float top = 360.0f;
	const float bottom = -360.0f;

	const bool insideScreen =
		screen.x >= left &&
		screen.x <= right &&
		screen.y >= bottom &&
		screen.y <= top;

	//========================================
	// 画面内
	//========================================
	if (insideScreen)
	{
		const float width =
			128.0f * m_scale;

		const float height =
			128.0f * m_scale;

		auto& sprite =
			KdShaderManager::Instance()
			.m_spriteShader;

		Math::Color color =
		{
			1.0f,
			1.0f,
			1.0f,
			1.0f
		};

		sprite.DrawTex(
			m_pinPoly
			->GetMaterial()
			->m_baseColorTex.get(),
			static_cast<int>(screen.x),
			static_cast<int>(screen.y),
			static_cast<int>(width),
			static_cast<int>(height),
			nullptr,
			&color,
			{ 0.5f, 0.5f }
		);

		//---------------------------------------
		// 距離表示
		//---------------------------------------
		Math::Vector2 distancePos = screen;

		// ピンの上に表示
		distancePos.y += 75.0f;

		m_distanceText->InitMessage(
			std::to_string(
				static_cast<int>(distance)
			) + "m",
			distancePos,
			m_distanceTextScale
		);

		m_distanceText->DrawSprite();

		return;
	}

	//---------------------------------------
	// 画面外の発光方向
	//---------------------------------------
	const float absX = std::abs(screen.x);
	const float absY = std::abs(screen.y);

	Edge nearestEdge;

	if (absX > absY)
	{
		// 横方向が強い
		if (screen.x > 0.0f)
		{
			nearestEdge = Edge::Right;
		}
		else
		{
			nearestEdge = Edge::Left;
		}
	}
	else
	{
		// 縦方向が強い
		if (screen.y > 0.0f)
		{
			nearestEdge = Edge::Top;
		}
		else
		{
			nearestEdge = Edge::Bottom;
		}
	}

	DrawEdgeGlow(nearestEdge);
	DrawEdgePin(nearestEdge, screen);
	DrawDistance(nearestEdge, screen, distance);
}