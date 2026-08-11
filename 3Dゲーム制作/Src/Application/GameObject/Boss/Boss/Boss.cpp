#include "Boss.h"

void Boss::Init()
{
	BossBase::Init();

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Enemy/manModel/Enemy.gltf");

	animIdleIndex = 9;
	animWalkIndex = 41;
	animDashIndex = 36;
	animAttack1Index = 39;
	animAttack2Index = 40;
	animAttack3Index = 41;

	m_hpMax = 1000;
	m_hp = m_hpMax;
}