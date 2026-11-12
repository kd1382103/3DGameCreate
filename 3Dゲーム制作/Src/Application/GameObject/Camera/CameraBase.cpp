#include "CameraBase.h"

void CameraBase::Init()
{
	if (!m_spCamera)
	{
		m_spCamera = std::make_shared<KdCamera>();
	}
	// ↓画面中央座標
	m_FixMousePos.x = 640;
	m_FixMousePos.y = 360;
}

void CameraBase::PreDraw()
{
	if (!m_spCamera) { return; }

	m_spCamera->SetCameraMatrix(m_mWorld);

	//TestFrustum();

	m_spCamera->SetToShader();
}

void CameraBase::SetTarget(const std::shared_ptr<KdGameObject>& target)
{
	if (!target) { return; }

	m_wpTarget = target;
}


Math::Vector2 CameraBase::WorldToScreen(const Math::Vector3& worldPos) const
{
	Math::Vector3 result = Math::Vector3::Zero;

	if (m_spCamera)
	{
		m_spCamera->ConvertWorldToScreenDetail(worldPos, result);
	}

	// result.x, result.y がスクリーン座標
	return Math::Vector2(result.x, result.y);
}


void CameraBase::UpdateRotateByMouse()
{
	// マウスでカメラを回転させる処理
	POINT _nowPos;
	GetCursorPos(&_nowPos);

	POINT _mouseMove{};
	_mouseMove.x = _nowPos.x - m_FixMousePos.x;
	_mouseMove.y = _nowPos.y - m_FixMousePos.y;

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);

	//=======================================================================
	// 追加
	//=======================================================================
	
	// 実際にカメラを回転させる処理(sensitivityはただの補正値(感度))
	const float sensitivity = 0.15f;

	//=======================================================================
	m_DegAng.x += _mouseMove.y * sensitivity;
	m_DegAng.y += _mouseMove.x * sensitivity;

	// 回転制御
	m_DegAng.x = std::clamp(m_DegAng.x, -45.f, 45.f);
}

//==============================================================
// 視錐台作成
//==============================================================
bool CameraBase::CreateFrustum(Frustum& frustum) const
{
	if (!m_spCamera)
	{
		return false;
	}

	// 最新のカメラワールド行列からビュー行列を取得
	Math::Matrix view = m_mWorld.Invert();

	// 射影行列
	const Math::Matrix& proj = m_spCamera->GetProjMatrix();

	// ワールド × ビュー × 射影
	Math::Matrix vp = view * proj;

	// 平面の設定
	auto SetPlane =
		[&](FrustumPlane& plane,
			float a, float b, float c, float d)
		{
			Math::Vector3 normal(a, b, c);

			float length = normal.Length();

			if (length <= 0.00000001f)
			{
				return false;
			}

			plane.normal = normal / length;
			plane.distance = d / length;

			return true;
		};

	// 左
	if (!SetPlane(
		frustum.planes[0],
		vp._11 + vp._14,
		vp._21 + vp._24,
		vp._31 + vp._34,
		vp._41 + vp._44))
	{
		return false;
	}

	// 右
	if (!SetPlane(
		frustum.planes[1],
		-vp._11 + vp._14,
		-vp._21 + vp._24,
		-vp._31 + vp._34,
		-vp._41 + vp._44))
	{
		return false;
	}

	// 下
	if (!SetPlane(
		frustum.planes[2],
		vp._12 + vp._14,
		vp._22 + vp._24,
		vp._32 + vp._34,
		vp._42 + vp._44))
	{
		return false;
	}

	// 上
	if (!SetPlane(
		frustum.planes[3],
		-vp._12 + vp._14,
		-vp._22 + vp._24,
		-vp._32 + vp._34,
		-vp._42 + vp._44))
	{
		return false;
	}

	// Near（DirectXはZ >= 0）
	if (!SetPlane(
		frustum.planes[4],
		vp._13,
		vp._23,
		vp._33,
		vp._43))
	{
		return false;
	}

	// Far（Z <= W）
	if (!SetPlane(
		frustum.planes[5],
		-vp._13 + vp._14,
		-vp._23 + vp._24,
		-vp._33 + vp._34,
		-vp._43 + vp._44))
	{
		return false;
	}

	return true;
}

//==============================================================
// 球体が視錐台内にあるか判定
//==============================================================
bool CameraBase::IsInCameraView(
	const Math::Vector3& center,
	float radius
) const
{
	Frustum frustum;

	if (!CreateFrustum(frustum))
	{
		return true;
	}

	for (int i = 0; i < 6; i++)
	{
		const FrustumPlane& plane = frustum.planes[i];

		float distance =
			plane.normal.Dot(center) + plane.distance;

		if (distance < -radius)
		{
			return false;
		}
	}

	return true;
}