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
	const int IdleAnim = 9;

	if (m_nowAnimIndex != IdleAnim)
	{
		m_nowAnimIndex = IdleAnim;
		m_animator->SetAnimation(m_model->GetAnimation(9), true);
	}

	EnemyBase::UpdateIdle();
}

void Enemy1::UpdateMove()
{
	const int MoveAnim = 36;

	if (m_nowAnimIndex != MoveAnim)
	{
		m_nowAnimIndex = MoveAnim;
		m_animator->SetAnimation(m_model->GetAnimation(36), true);
	}

	EnemyBase::UpdateMove();
}

void Enemy1::UpdateOrbit()
{
	const int OrbitAnim = 36;

	if (m_nowAnimIndex != OrbitAnim)
	{
		m_nowAnimIndex = OrbitAnim;
		m_animator->SetAnimation(m_model->GetAnimation(OrbitAnim), true);
	}

	EnemyBase::UpdateOrbit();
}

void Enemy1::UpdatePreAttack()
{
	if (m_nowAnimIndex != 11)   // 予知アニメ番号
	{
		m_nowAnimIndex = 11;
		m_animator->SetAnimation(m_model->GetAnimation(11), false);
	}

	EnemyBase::UpdatePreAttack();
}

void Enemy1::UpdateAttack()
{
	// ★攻撃アニメ番号（君の Enemy.gltf に合わせて変更）
	const int attackAnim = 39;

	if (m_nowAnimIndex != attackAnim)
	{
		m_nowAnimIndex = attackAnim;
		m_animator->SetAnimation(m_model->GetAnimation(attackAnim), false);
	}

	EnemyBase::UpdateAttack();
}



