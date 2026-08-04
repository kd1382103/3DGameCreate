#include "Enemy2.h"

void Enemy2::Init()
{

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Enemy/manModel/Enemy.gltf");

	EnemyBase::Init();

	// 個別設定
	m_hp = 100;
	m_hpMax = 100;
	m_moveSpeed = 0.04f;
	m_attackDist = 1.2f;
	m_attackDamage = 20.0f;
	detectRange = 6.0f;

	//アニメ番号
	animIdleIndex = 9;
	animWalkIndex = 36;
	animDashIndex = 36;
	animAttack1Index = 39;
	animSkillIndex = 11;   // 予備動作

	//コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Enemy2", m_model, KdCollider::TypeDamage);
}
