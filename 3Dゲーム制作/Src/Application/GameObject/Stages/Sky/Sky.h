#pragma once
#include<Application/GameObject/BaseObject/BaseObject.h>

class Sky :public BaseObject
{
public:
	Sky() {}
	~Sky() override { Release(); }

	void Init()			override;
	void Update()		override;
	void DrawUnLit()	override;
	
	//---------------------------------------
	// 位置設定
	//---------------------------------------
	void SetPos(const Math::Vector3& pos)
	{
		m_pos = pos;
	}

private:

	std::shared_ptr<KdModelData>m_model;
	Math::Vector3 m_pos = Math::Vector3::Zero;
	// 空の回転角度
	float m_rotation = 0.0f;

	// 回転速度
	float m_rotationSpeed = 0.001f;
};