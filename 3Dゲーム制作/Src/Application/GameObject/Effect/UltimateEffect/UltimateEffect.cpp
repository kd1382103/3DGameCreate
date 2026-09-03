#include "UltimateEffect.h"

#include <Application/main.h>
#include <Application/GameObject/Camera/CameraBase.h>

//==============================================================
// 初期化
//==============================================================
void UltimateEffect::Init(
	const Math::Vector3& pos,
	const Math::Vector3& direction)
{
	EffectBase::Init(
		EffectType::Ultimate,
		pos
	);

	//==========================================================
	// 再生時間
	//==========================================================
	m_lifeTime = 1.0f;

	//==========================================================
	// 飛んでいく方向
	//==========================================================
	m_direction = direction;

	m_direction.y = 0.0f;

	if (m_direction.LengthSquared() > 0.0001f)
	{
		m_direction.Normalize();
	}
	else
	{
		m_direction = { 0, 0, 1 };
	}

	//==========================================================
	// ポリゴン生成
	//==========================================================
	m_polygon =
		std::make_shared<UltimateEffectPolygon>();

	m_polygon->SetVertices(
		m_width,
		m_height
	);
}

//==============================================================
// 更新
//==============================================================
void UltimateEffect::Update()
{
	//==========================================================
	// 基底更新
	//==========================================================
	EffectBase::Update();

	if (m_isFinished)
	{
		return;
	}

	//==========================================================
	// 移動
	//==========================================================
	const float dt =
		Application::Instance().GetDeltaTime();

	Math::Vector3 pos = GetPos();

	pos +=
		m_direction *
		m_moveSpeed *
		dt;

	SetPos(pos);
}

//==============================================================
// 描画
//==============================================================
void UltimateEffect::DrawEffect()
{
	if (!m_polygon) return;

	float angle = std::atan2f(
		m_direction.x,
		m_direction.z
	);

	// 板ポリの向きを90度回転
	angle += DirectX::XM_PIDIV2;

	Math::Matrix mat =
		Math::Matrix::CreateRotationY(angle);

	mat.Translation(GetPos());

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(
		*m_polygon,
		mat,
		Math::Color(
			0.3f,
			0.9f,
			1.0f,
			1.0f
		)
	);
}


//==============================================================
// 頂点設定
//==============================================================
void UltimateEffectPolygon::SetVertices(
	float width,
	float height)
{
	m_vertices.clear();

	const float halfWidth =
		width * 0.5f;

	const float halfHeight =
		height * 0.5f;

	//==========================================================
	// 縦長の斬撃形状
	//
	//        v0
	//         ▲
	//        / \
	//       /   \
	//   v1 ◀     ▶ v2
	//       \   /
	//        \ /
	//         ▼
	//        v3
	//
	//==========================================================

	Vertex v0;
	v0.pos = {
		0.0f,
		halfHeight,
		0.0f
	};
	v0.UV = { 0.5f, 0.0f };


	Vertex v1;
	v1.pos = {
		-halfWidth,
		halfHeight * 0.15f,
		0.0f
	};
	v1.UV = { 0.0f, 0.35f };


	Vertex v2;
	v2.pos = {
		halfWidth,
		-halfHeight * 0.15f,
		0.0f
	};
	v2.UV = { 1.0f, 0.65f };


	Vertex v3;
	v3.pos = {
		0.0f,
		-halfHeight,
		0.0f
	};
	v3.UV = { 0.5f, 1.0f };


	//==========================================================
	// 三角形2枚
	//==========================================================

	m_vertices.push_back(v0);
	m_vertices.push_back(v1);
	m_vertices.push_back(v2);

	m_vertices.push_back(v1);
	m_vertices.push_back(v3);
	m_vertices.push_back(v2);
}