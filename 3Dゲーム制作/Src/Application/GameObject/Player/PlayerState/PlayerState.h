#pragma once

#include <Application/StateMachine/StateMachine.h>

class Player;

// 待機ステート
class PlayerStateIdle : public StateBase<Player>
{
public:
	void Enter(Player& owner)override;
	void Update(Player& owner)override;
};

// 移動ステート
class PlayerStateMove : public StateBase<Player>
{
public:
	void Update(Player& owner)override;
};

// 歩きステート（移動ステートを継承）
class PlayerStateWalk : public PlayerStateMove
{
public:
	void Enter (Player& owner)override;
	void Update(Player& owner)override;
};

// 走りステート（移動ステートを継承）
class PlayerStateDash : public PlayerStateMove
{
public:
	void Enter (Player& owner)override;
	void Update(Player& owner)override;
};

//攻撃系
class PlayerStateAttack1 : public StateBase<Player>
{
public:
	void Enter (Player& owner)override;
	void Update(Player& owner)override;
};
class PlayerStateAttack2 : public StateBase<Player>
{
public:
	void Enter (Player& owner)override;
	void Update(Player& owner)override;
};
class PlayerStateAttack3 : public StateBase<Player>
{
public:
	void Enter (Player& owner)override;
	void Update(Player& owner)override;
};

//スキル
class PlayerStateSkill : public StateBase<Player>
{
public:
	void Enter(Player& owner)override;
	void Update(Player& owner)override;
};

//回避
class PlayerStateDodge : public StateBase<Player>
{
public:
	void Enter(Player& owner)override;
	void Update(Player& owner)override;
};

//必殺技
class PlayerUltimate : public StateBase<Player>
{
public:
	void Enter(Player& owner)override;
	void Update(Player& owner)override;
};