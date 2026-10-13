#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera() {}
	~TPSCamera() override {}

	void Init() override;
	void PostUpdate() override;

	//========================================
	// 回避カメラ
	//========================================
	void StartDodgeCamera()
	{
		m_dodgeCameraActive = true;
	}

	void EndDodgeCamera()
	{
		m_dodgeCameraActive = false;
	}

	//========================================
	// 必殺技カメラ
	//========================================
	void StartUltimateCamera();
	void EndUltimateCamera();

	bool m_mouseFree = false;

private:

	//========================================
	// 視野角
	//========================================
	float m_nowFov = 60.0f;
	float m_targetFov = 60.0f;

	//========================================
	// 回避カメラ設定
	//========================================
	bool m_dodgeCameraActive = false;

	// 現在のカメラ距離
	float m_dodgeCameraDistance = 4.0f;

	//========================================
	// 必殺技カメラ設定
	//========================================
	bool m_ultimateCamera = false;

	enum class UltimateCameraMove
	{
		None,
		Start,
		End
	};

	UltimateCameraMove m_ultimateCameraMove =
		UltimateCameraMove::None;

	Math::Vector3 m_ultimateMoveStartPos;
	Math::Vector3 m_ultimateMoveTargetPos;

	float m_ultimateMoveTime = 0.0f;
	float m_ultimateMoveDuration = 0.3f;

	float m_ultimateCameraDistance = 4.0f;
	float m_ultimateCameraAngle = -150.0f;
};