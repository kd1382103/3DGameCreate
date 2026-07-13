#pragma once
#include <Application/GameObject/EnemyBase/EnemyBase.h>

class Enemy1 : public EnemyBase
{
public:
	void Init() override;

	void UpdateIdle()		override;
	void UpdateMove()		override;
	void UpdateOrbit()		override;
	void UpdatePreAttack()		override;
	void UpdateAttack()		override;
};