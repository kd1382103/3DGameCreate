#pragma once
#include <Application/StateMachine/StateMachine.h>

class BossBase;

//待機
class BossStateIdle : public StateBase<BossBase>
{
public:
	void Enter(BossBase& owner) override;
	void Update(BossBase& owner) override;
};

// 移動
class BossStateMove : public StateBase<BossBase>
{
public:
	void Update(BossBase& owner) override;
};

//攻撃
class BossStateAttackBase : public StateBase<BossBase>
{
protected:
	void GoNextAttack(BossBase& owner);
};

// 歩き
class BossStateWalk : public BossStateMove
{
public:
	void Enter(BossBase& owner) override;
	void Update(BossBase& owner) override;
};

// 走り
class BossStateDash : public BossStateMove
{
public:
	void Enter(BossBase& owner) override;
	void Update(BossBase& owner) override;
};

// 攻撃予知
class BossStatePreAttack : public StateBase<BossBase>
{
public:
	void Enter(BossBase& owner) override;
	void Update(BossBase& owner) override;

private:
	float m_time = 0.0f;
};

// 攻撃1
class BossStateAttack1 : public BossStateAttackBase
{
public:
	void Enter(BossBase& owner) override;
	void Update(BossBase& owner) override;
};

// 攻撃2
class BossStateAttack2 : public BossStateAttackBase
{
public:
	void Enter(BossBase& owner) override;
	void Update(BossBase& owner) override;
};

// 攻撃3
class BossStateAttack3 : public BossStateAttackBase
{
public:
	void Enter(BossBase& owner) override;
	void Update(BossBase& owner) override;
};
