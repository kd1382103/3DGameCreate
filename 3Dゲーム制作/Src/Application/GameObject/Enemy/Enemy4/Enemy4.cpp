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
	case 0:
		// Battle 0では出現させない
		break;

	case 1:
		break;

	case 2:
		//SetPos({ 10, 0, 100 });
		break;

	case 3:
		//SetPos({ 10, 0, 100 });
		break;

	case 4:
		//SetPos({ 10, 0, 100 });
		break;

	default:
		break;
	}

}

bool Enemy4::ShouldSpawn(int battleNo)
{
	switch (battleNo)
	{
	case 0:
		return false;

	case 1:
		return false;

	case 2:
		return true;

	case 3:
		return true;
	case 4:
		return true;

	default:
		return false;
	}
}
