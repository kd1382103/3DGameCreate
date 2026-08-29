#include "Enemy2.h"

void Enemy2::Init(int battleNo)
{

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Enemy/manModel/Enemy.gltf");

	EnemyBase::Init();

	// 個別設定
	m_hpMax = 100;
	m_hp = m_hpMax;
	m_moveSpeed = 0.04f;
	m_attackDist = 1.2f;
	m_attackDamage = 75.0f;
	detectRange = 6.0f;

	//アニメ番号
	animIdleIndex = 9;
	animWalkIndex = 36;
	animDashIndex = 36;
	animAttackIndex = 39;
	animSkillIndex = 11;   // 予備動作

	//コライダー
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Enemy2", m_model, KdCollider::TypeDamage);

	//---------------------------------------
	// 戦闘番号ごとの初期位置
	//---------------------------------------
	switch (battleNo)
	{
	case 0:
		SetPos({ 3, 0, 30 });
		break;

	case 1:
		SetPos({ -4, 0, 75 });
		break;

	case 2:
		//SetPos({ -4, 0, 75 });
		break;

	case 3:
		break;

	case 4:
		break;

	default:
		break;
	}
}

bool Enemy2::ShouldSpawn(int battleNo)
{
	switch (battleNo)
	{
	case 0:
		return true;

	case 1:
		return true;

	case 2:
		return true;

	case 3:
		return false;

	case 4:
		return false;

	default:
		return false;
	}
}
