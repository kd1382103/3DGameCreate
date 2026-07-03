#pragma once
#include<Application/GameObject/BaseObject/BaseObject.h>

class Wall :public BaseObject
{
public:
	Wall() {}
	~Wall() override { Release(); }

	void Init()			override;
	void DrawUnLit()	override;

private:

	std::shared_ptr<KdModelData>m_model;
};