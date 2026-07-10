#include "Enemy1.h"

void Enemy1::Init()
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Enemy/manModel/Enemy.gltf");

	m_hp = 50;
	m_moveSpeed = 0.04f;
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Enemy1", m_model, KdCollider::TypeDamage);


}
