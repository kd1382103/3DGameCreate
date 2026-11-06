#include "Enemy1.h"

void Enemy1::Init(int battleNo)
{
	InitModelAndCollider(
		"Asset/Models/Enemy/EnemyRobot/Enemy1/Enemy.gltf",
		"Enemy1"
	);

	// 個別設定
	m_hpMax = 250;
	m_hp = m_hpMax;
	m_moveSpeed = 0.04f;
	m_attackDist = 1.2f;
	m_attackDamage = 50.0f;
	detectRange = 6.0f;

	// 戦闘フェイズごとの出現位置
	switch (battleNo)
	{
	case 0:
		SetPos({ -3, 0, 30 });
		break;

	case 1:
		SetPos({ -4, 0, 85 });
		break;

	case 4:
		SetPos({ -80, 0, 25 });
		break;

	default:
		break;
	}
}
bool Enemy1::ShouldSpawn(int battleNo)
{
	switch (battleNo)
	{
	case 0:
	case 1:
	case 4:
		return true;

	default:
		return false;
	}
}