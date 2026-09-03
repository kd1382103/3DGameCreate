#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera() {}
	~TPSCamera() override {}

	void Init() override;
	void PostUpdate() override;

	void StartDodgeCamera()
	{
		m_targetFov = 48.0f;
	}

	void EndDodgeCamera()
	{
		m_targetFov = 60.0f;
	}

	//=========================================================
	// 必殺技カメラ
	//=========================================================
	void StartUltimateCamera();
	void EndUltimateCamera();

	bool m_mouseFree = false;

private:

	float m_nowFov = 60.0f;
	float m_targetFov = 60.0f;

	//=========================================================
	// 必殺技カメラ
	//=========================================================
	bool m_ultimateCamera = false;

	// 必殺技カメラの移動状態
	enum class UltimateCameraMove
	{
		None,
		Start,
		End
	};

	UltimateCameraMove m_ultimateCameraMove =
		UltimateCameraMove::None;

	// 移動開始位置
	Math::Vector3 m_ultimateMoveStartPos;

	// 移動先
	Math::Vector3 m_ultimateMoveTargetPos;

	// 移動時間
	float m_ultimateMoveTime = 0.0f;

	// 何秒かけて移動するか
	float m_ultimateMoveDuration = 0.3f;

	//=========================================================
	// 必殺技カメラの位置
	//=========================================================

	// プレイヤーからの距離
	float m_ultimateCameraDistance = 4.0f;

	// プレイヤーに対する角度(正面からの距離)
	float m_ultimateCameraAngle = -150.0f;

};