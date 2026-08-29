#pragma once
#include <Application/GameObject/Enemy/EnemyBase.h>

class Enemy4 : public EnemyBase
{
public:
	void Init(int battleNo);
	static bool ShouldSpawn(int battleNo);
};