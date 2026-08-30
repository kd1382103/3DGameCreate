#include "Enemy5.h"

void Enemy5::Init(int battleNo)
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Enemy/manModel/Enemy.gltf");

	EnemyBase::Init();

	// 個別設定
	m_hpMax = 350;
	m_hp = m_hpMax;
	m_moveSpeed = 0.04f;
	m_attackDist = 1.2f;
	m_attackDamage = 20.0f;
	detectRange = 6.0f;

	//アニメ番号
	animIdleIndex = 9;
	animWalkIndex = 36;
	animDashIndex = 36;
	animAttackIndex = 39;
	animSkillIndex = 11;   // 予備動作

	//コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Enemy5", m_model, KdCollider::TypeDamage);

	// 戦闘フェイズごとの出現位置
	switch (battleNo)
	{
	case 2:
		SetPos({ -85, 0, 80 });
		break;

	case 3:
		SetPos({ -80, 0, 130 });
		break;

	case 4:
		SetPos({ -78, 0, 20 });
		break;

	default:
		break;
	}
}

bool Enemy5::ShouldSpawn(int battleNo)
{
	switch (battleNo)
	{
	case 2:
	case 3:
	case 4:
		return true;

	default:
		return false;
	}
}
