#include "Enemy1.h"

void Enemy1::Init()
{
	EnemyBase::Init();

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Enemy/manModel/Enemy.gltf");

	m_hp = 50;
	m_moveSpeed = 0.04f;
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Enemy1", m_model, KdCollider::TypeDamage);


}

void Enemy1::UpdateIdle()
{
	if (m_nowAnimIndex != 9)
	{
		m_nowAnimIndex = 9;
		m_animator->SetAnimation(m_model->GetAnimation(9), true);
	}

	EnemyBase::UpdateIdle();
}

void Enemy1::UpdateMove()
{
	if (m_nowAnimIndex != 36)
	{
		m_nowAnimIndex = 36;
		m_animator->SetAnimation(m_model->GetAnimation(36), true);
	}

	EnemyBase::UpdateMove();
}

void Enemy1::UpdateOrbit()
{
	if (m_nowAnimIndex != 36)
	{
		m_nowAnimIndex = 36;
		m_animator->SetAnimation(m_model->GetAnimation(36), true);
	}

	EnemyBase::UpdateOrbit();
}


