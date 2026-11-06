#include "Enemy5.h"

void Enemy5::Init(int battleNo)
{
	InitModelAndCollider(
		"Asset/Models/Enemy/EnemyRobot/Enemy5/Enemy.gltf",
		"Enemy5"
	);

	// 個別設定
	m_hpMax = 350;
	m_hp = m_hpMax;
	m_moveSpeed = 0.04f;
	m_attackDist = 1.2f;
	m_attackDamage = 20.0f;
	detectRange = 6.0f;

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