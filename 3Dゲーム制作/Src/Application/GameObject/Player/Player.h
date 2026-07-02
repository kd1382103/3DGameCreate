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

	//void SetCamera(const std::shared_ptr<CameraBase>& camera) { m_wpCamera = camera; }

private:
	std::shared_ptr<KdModelWork>m_model;
	KdAnimator m_animator;

	//カメラ情報
	//std::weak_ptr<CameraBase> m_wpCamera;

	//方向
	Math::Vector3 m_dir;

	//移動・走り
	bool isMoving = false;
	bool isRunning = false;
	float m_angleY = 0.0f;

	//アニメーション管理
	int m_nowAnimIndex = -1;

	//ジャンプ
	bool m_isJumping = false;
	bool m_isLanding = false;

	//攻撃
	bool m_isAttacking = false;

	//重力
	float m_gravity = 0.0f;

};