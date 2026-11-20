#include "TutorialEnemy.h"

void TutorialEnemy::Init(int battleNo)
{
	InitModelAndCollider(
		"Asset/Models/Enemy/EnemyRobot/Enemy1/Enemy.gltf",
		"TutorialEnemy"
	);

	// チュートリアル用設定
	m_hpMax = 999999;
	m_hp = m_hpMax;
	m_moveSpeed = 0.0f;
	m_attackDist = 1.5f;
	m_attackDamage = 0.0f;
	detectRange = 0.0f;
	SetCanKnockBack(false);

	switch (battleNo)
	{
	case 0:
		SetPos({ 0, 0, 5 });
		break;

	default:
		break;
	}
}

bool TutorialEnemy::ShouldSpawn(int battleNo)
{
	return battleNo == 0;
}