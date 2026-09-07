#include "BattlePin.h"

#include <Application/GameObject/Camera/CameraBase.h>
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

		//====================================
		// DrawTexは中央基準の座標なので
		// WorldToScreenの座標をそのまま使用
		//
		// pivot = {0.5, 0.5}
		// → ピンの中心をscreenに合わせる
		//====================================
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
}