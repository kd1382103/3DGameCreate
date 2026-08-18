#pragma once
#include <Application/StateMachine/StateMachine.h>

class EnemyBase;

// 待機
class EnemyBaseStateIdle : public StateBase<EnemyBase>
{
public:
	void Enter(EnemyBase& owner) override;
	void Update(EnemyBase& owner) override;
};

// 移動
class EnemyBaseStateMove : public StateBase<EnemyBase>
{
public:
	void Update(EnemyBase& owner) override;
};

// 歩き
class EnemyBaseStateWalk : public EnemyBaseStateMove
{
public:
	void Enter(EnemyBase& owner) override;
	void Update(EnemyBase& owner) override;
};

// 走り
class EnemyBaseStateDash : public EnemyBaseStateMove
{
public:
	void Enter(EnemyBase& owner) override;
	void Update(EnemyBase& owner) override;
};

// 攻撃1
class EnemyBaseStateAttack1 : public StateBase<EnemyBase>
{
public:
	void Enter(EnemyBase& owner) override;
	void Update(EnemyBase& owner) override;
};

// 攻撃2
class EnemyBaseStateAttack2 : public StateBase<EnemyBase>
{
public:
	void Enter(EnemyBase& owner) override;
	void Update(EnemyBase& owner) override;
};

// 攻撃3
class EnemyBaseStateAttack3 : public StateBase<EnemyBase>
{
public:
	void Enter(EnemyBase& owner) override;
	void Update(EnemyBase& owner) override;
};