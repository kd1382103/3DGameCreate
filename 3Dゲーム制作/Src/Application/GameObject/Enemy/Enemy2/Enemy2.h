#pragma once
#include <Application/GameObject/Enemy/EnemyBase.h>

class Enemy2 : public EnemyBase
{
public:
	void Init(int battleNo);
	static bool ShouldSpawn(int battleNo);
};