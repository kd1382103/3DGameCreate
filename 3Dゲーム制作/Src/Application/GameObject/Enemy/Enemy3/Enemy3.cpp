#include "Enemy3.h"

void Enemy3::Init(int battleNo)
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Enemy/manModel/Enemy.gltf");

	EnemyBase::Init();

	// 個別設定
	m_hpMax = 250;
	m_hp = m_hpMax;
	m_moveSpeed = 0.075f;
	m_attackDist = 1.2f;
	m_attackDamage = 20.0f;
	detectRange = 10.0f;

	//アニメ番号
	animIdleIndex = 9;
	animWalkIndex = 36;
	animDashIndex = 36;
	animAttackIndex = 39;
	animSkillIndex = 11;   // 予備動作

	//コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Enemy3", m_model, KdCollider::TypeDamage);

	// 戦闘フェイズごとの出現位置
	switch (battleNo)
	{
	case 0:
		// 出現しない
		break;

	case 1:
		SetPos({ -5, 0, 80 });
		break;

	case 2:
		SetPos({ -80, 0, 80 });
		break;

	case 3:
		SetPos({ -75, 0, 125 });
		break;

	case 4:
		// 出現しない
		break;

	default:
		break;
	}
}

bool Enemy3::ShouldSpawn(int battleNo)
{
	switch (battleNo)
	{
	case 1:
	case 2:
	case 3:
		return true;

	default:
		return false;
	}
}
