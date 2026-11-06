#include "Enemy2.h"

void Enemy2::Init(int battleNo)
{
	InitModelAndCollider(
		"Asset/Models/Enemy/EnemyRobot/Enemy2/Enemy.gltf",
		"Enemy2"
	);

	// 個別設定
	m_hpMax = 100;
	m_hp = m_hpMax;
	m_moveSpeed = 0.04f;
	m_attackDist = 1.2f;
	m_attackDamage = 75.0f;
	detectRange = 6.0f;

	// 戦闘番号ごとの初期位置
	switch (battleNo)
	{
	case 0:
		SetPos({ 3, 0, 30 });
		break;

	case 1:
		SetPos({ -4, 0, 75 });
		break;

	case 2:
		SetPos({ -80, 0, 85 });
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
	case 1:
	case 2:
		return true;

	default:
		return false;
	}
}