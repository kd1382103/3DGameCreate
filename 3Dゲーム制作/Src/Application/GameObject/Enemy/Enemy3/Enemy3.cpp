#include "Enemy3.h"

void Enemy3::Init(int battleNo)
{
	InitModelAndCollider(
		"Asset/Models/Enemy/EnemyRobot/Enemy3/Enemy.gltf",
		"Enemy3"
	);

	// 個別設定
	m_hpMax = 250;
	m_hp = m_hpMax;
	m_moveSpeed = 0.075f;
	m_attackDist = 1.2f;
	m_attackDamage = 20.0f;
	detectRange = 10.0f;

	// 戦闘フェイズごとの出現位置
	switch (battleNo)
	{
	case 2:
		SetPos({ -7, 0, 80 });
		break;

	case 3:
		SetPos({ -80, 0, 80 });
		break;

	case 4:
		SetPos({ -75, 0, 125 });
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