#pragma once
#include<Application/GameObject/BaseObject/BaseObject.h>

class BaseUI : public BaseObject
{
public:
	BaseUI()
	{
		m_drawType = eDrawTypeUI;
	}

	virtual ~BaseUI() {}
	virtual void DrawSprite() override {}
};
