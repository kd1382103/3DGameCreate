#pragma once
#include<Application/GameObject/BaseObject/BaseObject.h>

class Stage :public BaseObject
{
public:
	Stage() {}
	~Stage() override { Release(); }

	void Init()			override;
	void DrawUnLit()	override;

private:

	std::shared_ptr<KdModelData>m_model;
};