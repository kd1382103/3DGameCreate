#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void Init()				override;
	void PostUpdate()		override;

	void SetPlayerTarget(std::shared_ptr<KdGameObject>target) { m_wpTarget = target; }

private:

	bool m_mouseFree = false;
};