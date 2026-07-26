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
	//if (!m_active) { return; }
	if (!m_spCamera) { return; }

	m_spCamera->SetCameraMatrix(m_mWorld);
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
