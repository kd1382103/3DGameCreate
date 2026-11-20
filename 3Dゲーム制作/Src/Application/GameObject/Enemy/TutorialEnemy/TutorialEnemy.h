#pragma once
#include <Application/GameObject/Enemy/EnemyBase.h>

class TutorialEnemy : public EnemyBase
{
public:
	void Init(int battleNo) override;

	static bool ShouldSpawn(int battleNo);
};