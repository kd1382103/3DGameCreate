#include "Boss.h"
void Boss::Init()
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/boss/Boss.gltf");

	BossBase::Init();

	animIdleIndex = 9;
	animWalkIndex = 41;
	animDashIndex = 36;
	animAttack1Index = 39;
	animAttack2Index = 40;
	animAttack3Index = 41;
}