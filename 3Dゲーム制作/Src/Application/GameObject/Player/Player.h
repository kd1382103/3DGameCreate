#pragma once
#include<Application/GameObject/BaseObject/BaseObject.h>

class Player :public BaseObject
{
public:
	Player() {}
	~Player() override { Release(); }

	void Init()							override;
	void Update()						override;
	void PostUpdate()					override;
	void DrawLit()						override;
	void GenerateDepthMapFromLight()	override;

private:

	std::shared_ptr<KdModelData>m_model;
};