#pragma once
#include<Application/GameObject/BaseObject/BaseObject.h>

class CameraBase;
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

	void SetCamera(std::shared_ptr<CameraBase> camera) { m_wpCamera = camera; }
	void SetPos(Math::Vector3 pos) { m_nowPos = pos; };

	//void SetRotation(const Math::Vector3& rot) { m_rotation = rot; }
	//void SetRotationY(float y) { m_rotation.y = y; }
	//const Math::Vector3& GetRotation() const { return m_rotation; }

private:
	std::shared_ptr<KdModelWork>m_model;
	KdAnimator m_animator;

	std::weak_ptr<CameraBase>m_wpCamera;

	//方向
	Math::Vector3 m_dir = Math::Vector3::Zero;
	Math::Vector3 m_rotation = Math::Vector3::Zero;

	//移動・走り
	float m_angleY = DirectX::XMConvertToRadians(180.0f); 

	bool m_keepRunning = false;

	//アニメーション管理m
	int m_nowAnimIndex = -1;
	float m_attackAnimeTime;

	//ジャンプ
	bool m_isJumping = false;
	bool m_isLanding = false;

	//攻撃
	bool m_isAttacking = false;


	Math::Vector3 m_move;
	Math::Vector3 m_fall;

	//重力
	float m_gravity = 0.0f;


};