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

//攻撃(攻撃1〜3の共通処理)
class BossStateAttackBase : public StateBase<BossBase>
{
public:
	void Enter(BossBase& owner) override;
	void Update(BossBase& owner) override;

protected:
	// animName : 再生する攻撃アニメーション名
	explicit BossStateAttackBase(const char* animName)
		: m_animName(animName)
	{}

	void GoNextAttack(BossBase& owner);

private:
	std::string m_animName;
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
};

// 攻撃1
class BossStateAttack1 : public BossStateAttackBase
{
public:
	BossStateAttack1() : BossStateAttackBase("Attack1") {}
};

// 攻撃2
class BossStateAttack2 : public BossStateAttackBase
{
public:
	BossStateAttack2() : BossStateAttackBase("Attack2") {}
};

// 攻撃3
class BossStateAttack3 : public BossStateAttackBase
{
public:
	BossStateAttack3() : BossStateAttackBase("Attack3") {}
};

//==============================================================
// 被弾
//==============================================================
class BossStateHit : public StateBase<BossBase>
{
public:
	void Enter(BossBase& owner) override;
	void Update(BossBase& owner) override;
};
