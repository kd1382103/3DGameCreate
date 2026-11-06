#pragma once
#include <Application/GameObject/Enemy/EnemyBase.h>

class Enemy1 : public EnemyBase
{
public:
	void Init(int battleNo)override;
	static bool ShouldSpawn(int battleNo);
};