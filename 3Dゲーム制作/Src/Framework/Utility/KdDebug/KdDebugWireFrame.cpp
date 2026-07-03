#include "KdDebugWireFrame.h"

void KdDebugWireFrame::AddDebugLine(const Math::Vector3& start, const Math::Vector3& end, const Math::Color& col)
{
	// デバッグラインの始点
	KdPolygon::Vertex v1;
	v1.color = col.RGBA().v;
	v1.UV = Math::Vector2::Zero;
	v1.pos = start;

	// デバッグラインの終点
	KdPolygon::Vertex v2;
	v2.color = col.RGBA().v;
	v2.UV = Math::Vector2::Zero;
	v2.pos = end;

	m_debugVertices.push_back(v1);
	m_debugVertices.push_back(v2);
}

void KdDebugWireFrame::AddDebugLine(const Math::Vector3& start, const Math::Vector3& dir, float length, const Math::Color& col)
{
	// デバッグラインの始点
	KdPolygon::Vertex v1;
	v1.color = col.RGBA().v;
	v1.UV = Math::Vector2::Zero;
	v1.pos = start;

	// デバッグラインの終点
	KdPolygon::Vertex v2;
	v2.color = col.RGBA().v;
	v2.UV = Math::Vector2::Zero;
	v2.pos = v1.pos + (dir * length);

	m_debugVertices.push_back(v1);
	m_debugVertices.push_back(v2);
}

void KdDebugWireFrame::AddDebugLineFromMatrix(const Math::Matrix& mat, float scale)
{
	Math::Vector3 start = mat.Translation();
	Math::Vector3 vAxis;

	// X軸描画
	vAxis = mat.Right();
	vAxis.Normalize();
	AddDebugLine(start, start + vAxis * scale, kRedColor);

	// Y軸描画
	vAxis = mat.Up();
	vAxis.Normalize();
	AddDebugLine(start, start + vAxis * scale, kGreenColor);

	// Z軸描画
	vAxis = mat.Backward();
	vAxis.Normalize();
	AddDebugLine(start, start + vAxis * scale, kBlueColor);
}

// デバッグスフィアの描画
void KdDebugWireFrame::AddDebugSphere(const Math::Vector3& pos, float radius, const Math::Color& col)
{
	KdPolygon::Vertex v;
	v.UV = Math::Vector2::Zero;
	v.color = col.RGBA().v;

	int detail = 16;
	for (int i = 0; i < detail + 1; ++i)
	{
		// XZ面
		v.pos = pos;
		v.pos.x += cos((float)i * (360 / detail) * KdToRadians) * radius;
		v.pos.z += sin((float)i * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);

		v.pos = pos;
		v.pos.x += cos((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		v.pos.z += sin((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);

		// XY面
		v.pos = pos;
		v.pos.x += cos((float)i * (360 / detail) * KdToRadians) * radius;
		v.pos.y += sin((float)i * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);

		v.pos = pos;
		v.pos.x += cos((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		v.pos.y += sin((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);

		// YZ面
		v.pos = pos;
		v.pos.y += cos((float)i * (360 / detail) * KdToRadians) * radius;
		v.pos.z += sin((float)i * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);

		v.pos = pos;
		v.pos.y += cos((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		v.pos.z += sin((float)(i + 1) * (360 / detail) * KdToRadians) * radius;
		m_debugVertices.push_back(v);
	}
}
// デバッグボックスの描画
void KdDebugWireFrame::AddDebugBox(const Math::Matrix& matrix, const Math::Vector3& size, const Math::Vector3& offset, const bool isOriented, const Math::Color& col)
{
	KdPolygon::Vertex v;
	v.UV = Math::Vector2::Zero;
	v.color = col.RGBA().v;

	Math::Vector3					corners[8];
	DirectX::BoundingBox			aabb;
	DirectX::BoundingOrientedBox	obb;
	// ボックス(AABB)
	if (!isOriented)
	{
		aabb.Center = matrix.Translation() + offset;
		
		// 行列側でサイズが弄られた時の対応
		aabb.Extents.x = size.x * (matrix.Right().Length());
		aabb.Extents.y = size.y * (matrix.Up().Length());
		aabb.Extents.z = size.z * (matrix.Backward().Length());

		aabb.GetCorners(corners);
	}
	// ボックス(OBB)
	else
	{
		Math::Matrix createMat = matrix;
		createMat.Translation(createMat.Translation() + offset);
		obb.Transform(obb, createMat);
		
		// 行列側でサイズが弄られた時の対応
		obb.Extents.x = size.x * (matrix.Right().Length());
		obb.Extents.y = size.y * (matrix.Up().Length());
		obb.Extents.z = size.z * (matrix.Backward().Length());

		obb.GetCorners(corners);
	}	

	// 底辺定義
	{
		//底辺1
		v.pos = corners[0];
		m_debugVertices.push_back(v);

		v.pos = corners[1];
		m_debugVertices.push_back(v);

		//底辺2
		v.pos = corners[0];
		m_debugVertices.push_back(v);

		v.pos = corners[4];
		m_debugVertices.push_back(v);

		//底辺3
		v.pos = corners[5];
		m_debugVertices.push_back(v);

		v.pos = corners[1];
		m_debugVertices.push_back(v);

		//底辺4
		v.pos = corners[5];
		m_debugVertices.push_back(v);

		v.pos = corners[4];
		m_debugVertices.push_back(v);
	}

	// 中辺定義
	{
		//中辺1
		v.pos = corners[0];
		m_debugVertices.push_back(v);

		v.pos = corners[3];
		m_debugVertices.push_back(v);

		//中辺2
		v.pos = corners[1];
		m_debugVertices.push_back(v);

		v.pos = corners[2];
		m_debugVertices.push_back(v);

		//中辺3
		v.pos = corners[4];
		m_debugVertices.push_back(v);

		v.pos = corners[7];
		m_debugVertices.push_back(v);

		//中辺4
		v.pos = corners[5];
		m_debugVertices.push_back(v);

		v.pos = corners[6];
		m_debugVertices.push_back(v);
	}

	// 上辺定義
	{
		//上辺1
		v.pos = corners[2];
		m_debugVertices.push_back(v);

		v.pos = corners[3];
		m_debugVertices.push_back(v);

		//上辺2
		v.pos = corners[2];
		m_debugVertices.push_back(v);

		v.pos = corners[6];
		m_debugVertices.push_back(v);

		//上辺3
		v.pos = corners[7];
		m_debugVertices.push_back(v);

		v.pos = corners[3];
		m_debugVertices.push_back(v);

		//上辺4
		v.pos = corners[7];
		m_debugVertices.push_back(v);

		v.pos = corners[6];
		m_debugVertices.push_back(v);
	}
}

void KdDebugWireFrame::Draw()
{
	if (m_debugVertices.size() < 2) { return; }

	KdShaderManager::Instance().m_StandardShader.DrawVertices(m_debugVertices, Math::Matrix::Identity);

	m_debugVertices.clear();
}

void KdDebugWireFrame::Release()
{
	m_debugVertices.clear();
}

//======================================================================
// 追加
//======================================================================

// デバッグカプセルの描画
void KdDebugWireFrame::AddDebugCapsule(
	const Math::Vector3& start,   // 下半球中心
	const Math::Vector3& end,     // 上半球中心
	float radius,
	const Math::Color& col)
{
	KdPolygon::Vertex v;
	v.UV = Math::Vector2::Zero;
	v.color = col.RGBA().v;

	const int detail = 16;

	Math::Vector3 posTop = end;
	Math::Vector3 posBottom = start;

	std::vector<Math::Vector3> topXYEdge;
	std::vector<Math::Vector3> bottomXYEdge;
	std::vector<Math::Vector3> topYZEdge;
	std::vector<Math::Vector3> bottomYZEdge;

	for (int i = 0; i <= detail; ++i)
	{
		float angle = (i * (360.0f / detail)) * KdToRadians;
		float angleNext = ((i + 1) * (360.0f / detail)) * KdToRadians;

		float halfAngle = (i * (180.0f / detail)) * KdToRadians;
		float halfAngleNext = ((i + 1) * (180.0f / detail)) * KdToRadians;

		float halfAngleDown = halfAngle + KdToRadians * 180.0f;
		float halfAngleDownNext = halfAngleNext + KdToRadians * 180.0f;

		// ============================
		// 上半球
		// ============================

		// XZ（円）
		v.pos = posTop;
		v.pos.x += cos(angle) * radius;
		v.pos.z += sin(angle) * radius;
		m_debugVertices.push_back(v);

		v.pos = posTop;
		v.pos.x += cos(angleNext) * radius;
		v.pos.z += sin(angleNext) * radius;
		m_debugVertices.push_back(v);

		// XY
		v.pos = posTop;
		v.pos.x += cos(halfAngle) * radius;
		v.pos.y += sin(halfAngle) * radius;
		m_debugVertices.push_back(v);

		v.pos = posTop;
		v.pos.x += cos(halfAngleNext) * radius;
		v.pos.y += sin(halfAngleNext) * radius;
		m_debugVertices.push_back(v);

		// XY の端点を保存（縦ライン用）
		topXYEdge.push_back({ posTop.x + cos(angle) * radius, posTop.y + sin(angle) * radius, posTop.z });

		// YZ
		v.pos = posTop;
		v.pos.z += cos(halfAngle) * radius;  // Z軸を中心に
		v.pos.y += sin(halfAngle) * radius;  // Y方向へ半円
		m_debugVertices.push_back(v);

		v.pos = posTop;
		v.pos.z += cos(halfAngleNext) * radius;
		v.pos.y += sin(halfAngleNext) * radius;
		m_debugVertices.push_back(v);

		// YZ の端点を保存（縦ライン用）
		topYZEdge.push_back({ posTop.x, posTop.y + sin(halfAngle) * radius, posTop.z + cos(halfAngle) * radius });

		// ============================
		// 下半球
		// ============================

		// XZ（円）
		v.pos = posBottom;
		v.pos.x += cos(angle) * radius;
		v.pos.z += sin(angle) * radius;
		m_debugVertices.push_back(v);

		v.pos = posBottom;
		v.pos.x += cos(angleNext) * radius;
		v.pos.z += sin(angleNext) * radius;
		m_debugVertices.push_back(v);

		// XY
		v.pos = posBottom;
		v.pos.x += cos(halfAngleDown) * radius;
		v.pos.y += sin(halfAngleDown) * radius;
		m_debugVertices.push_back(v);

		v.pos = posBottom;
		v.pos.x += cos(halfAngleDownNext) * radius;
		v.pos.y += sin(halfAngleDownNext) * radius;
		m_debugVertices.push_back(v);

		bottomXYEdge.push_back({ posBottom.x + cos(angle) * radius, posBottom.y + sin(angle) * radius, posBottom.z });

		// YZ
		v.pos = posBottom;
		v.pos.z += cos(halfAngleDown) * radius;
		v.pos.y += sin(halfAngleDown) * radius;
		m_debugVertices.push_back(v);

		v.pos = posBottom;
		v.pos.z += cos(halfAngleDownNext) * radius;
		v.pos.y += sin(halfAngleDownNext) * radius;
		m_debugVertices.push_back(v);

		bottomYZEdge.push_back({ posBottom.x, posBottom.y + sin(halfAngleDown) * radius, posBottom.z + cos(halfAngleDown) * radius });
	}

	// ============================
	// 上下の端点を縦ラインでつなぐ
	// ============================
	for (int i = 0; i <= detail; ++i)
	{
		float angleDeg = i * (360.0f / detail);

		// XZ面と重なる角度（0°, 90°, 180°, 270°）
		if (fabsf(fmodf(angleDeg, 90.0f)) < 0.001f)
		{
			Math::Vector3 topXZ(
				posTop.x + cos(angleDeg * KdToRadians) * radius,
				posTop.y,
				posTop.z + sin(angleDeg * KdToRadians) * radius);

			Math::Vector3 bottomXZ(
				posBottom.x + cos(angleDeg * KdToRadians) * radius,
				posBottom.y,
				posBottom.z + sin(angleDeg * KdToRadians) * radius);

			AddDebugLine(topXZ, bottomXZ, col);
		}
	}
}