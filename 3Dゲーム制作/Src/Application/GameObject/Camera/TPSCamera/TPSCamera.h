#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void Init()				override;
	void PostUpdate()		override;

	void StartDodgeCamera() { m_targetFov = 48.0f; }
	void EndDodgeCamera() { m_targetFov = 60.0f; }

	bool m_mouseFree = false;

private:
	float m_nowFov = 60.0f;
	float m_targetFov = 60.0f;
};