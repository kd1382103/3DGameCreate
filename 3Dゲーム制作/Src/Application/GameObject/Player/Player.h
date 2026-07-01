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
	std::shared_ptr<KdModelWork>m_model;
	KdAnimator m_animator;

	//方向
	Math::Vector3 m_dir;

	//アニメーション管理
	int m_nowAnimIndex = -1;

	//ジャンプ
	bool m_isJumping = false;
	bool m_isLanding = false;

	//重力
	float m_gravity = 0.0f;
};