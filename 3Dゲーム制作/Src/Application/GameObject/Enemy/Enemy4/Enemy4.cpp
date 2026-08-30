#include "Enemy4.h"

void Enemy4::Init(int battleNo)
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Enemy/manModel/Enemy.gltf");

	EnemyBase::Init();

	// 個別設定
	m_hpMax = 300;
	m_hp = m_hpMax;
	m_moveSpeed = 0.02f;
	m_attackDist = 1.2f;
	m_attackDamage = 60.0f;
	detectRange = 6.0f;

	//アニメ番号
	animIdleIndex = 9;
	animWalkIndex = 36;
	animDashIndex = 36;
	animAttackIndex = 39;
	animSkillIndex = 11;   // 予備動作

	//コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Enemy4", m_model, KdCollider::TypeDamage);

	// 戦闘フェイズごとの出現位置
	switch (battleNo)
	{
	case 2:
		SetPos({ -80, 0, 75 });
		break;

	case 3:
		SetPos({ -85, 0, 125 });
		break;

	case 4:
		SetPos({ -82, 0, 20 });
		break;

	default:
		break;
	}

}

bool Enemy4::ShouldSpawn(int battleNo)
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
