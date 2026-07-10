#pragma once
#include <Application/GameObject/BaseObject/BaseObject.h>

class Player;

class EnemyBase : public BaseObject
{
public:
	enum class State
	{
		Idle,
		Move,
		Hit,
		Dead,
	};

	EnemyBase() {}
	virtual ~EnemyBase() {}

	virtual void Init() override;
	virtual void Update() override;
	virtual void PostUpdate() override;
	virtual void DrawLit() override;

	void SetTarget(const std::shared_ptr<Player>& target) { m_wpPlayer = target; }
	void SetPos(const Math::Vector3& pos) { m_nowPos = pos; }

	// ダメージ処理
	virtual void Damage(float dmg)
	{
		m_hp -= dmg;
		if (m_hp <= 0) ChangeState(State::Dead);
		else ChangeState(State::Hit);
	}

protected:
	// ステート処理
	virtual void UpdateIdle();
	virtual void UpdateMove();
	virtual void UpdateHit();
	virtual void UpdateDead();

	virtual void RotateToMoveDir();

	void ChangeState(State next)
	{
		m_state = next;
		m_stateTimer = 0.0f;
	}

	// カプセル当たり判定（プレイヤーと同じ）
	void CapsuleCollision();

protected:
	std::shared_ptr<KdModelWork> m_model;

	Math::Vector3 m_nowPos = Math::Vector3::Zero;	 //現在座標

	Math::Vector3 m_moveDir = Math::Vector3::Zero;   // 移動方向
	float m_angleY = 0.0f;                           // 敵の向き（Y回転）
	float m_rotationSpeedDeg = 5.0f;                 // 回転速度（度）


	std::weak_ptr<Player> m_wpPlayer;

	float m_moveSpeed = 0.03f;

	float m_hp = 100.0f;

	State m_state = State::Idle;
	float m_stateTimer = 0.0f;
};
