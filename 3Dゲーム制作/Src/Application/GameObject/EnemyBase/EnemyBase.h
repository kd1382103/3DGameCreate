#pragma once
#include <Application/GameObject/BaseObject/BaseObject.h>

class Player;

//--------------------------------------
// アニメーション情報まとめ構造体
//--------------------------------------
struct AnimatorInfo
{
	std::string animName;   // アニメーション名
	float speed = 1.0f;     // 再生速度
	bool loop = true;       // ループ再生
};

class EnemyBase : public BaseObject
{
public:
	enum class State
	{
		Idle,
		Move,
		Orbit,
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
	virtual void UpdateOrbit();
	virtual void UpdateHit();
	virtual void UpdateDead();
	virtual void RotateToMoveDir();

	// アニメーション再生
	virtual void PlayAnimation(const std::string& animName);

	// アニメーション管理
	void SetAnimator(const AnimatorInfo& info);
	void SetAnimation(const std::string& animName, bool loop);
	void SetAnimationSpeed(float speed);
	void UpdateAnimation();

	void ChangeState(State next)
	{
		m_state = next;
		m_stateTimer = 0.0f;

		switch (m_state)
		{
		case State::Idle:
			PlayAnimation("Idle");
			break;

		case State::Move:
			PlayAnimation("Run");
			break;

		case State::Orbit:
			PlayAnimation("Strafe");
			break;

		case State::Hit:
			PlayAnimation("Hit");
			break;

		case State::Dead:
			PlayAnimation("Dead");
			break;
		}
	}

	// カプセル当たり判定（プレイヤーと同じ）
	void CapsuleCollision();

	// 地面判定
	void GroundCheck();

protected:
	std::shared_ptr<KdModelWork> m_model;

	Math::Vector3 m_nowPos = Math::Vector3::Zero;   // 現在座標
	Math::Vector3 m_moveDir = Math::Vector3::Zero;  // 移動方向

	float m_angleY = 0.0f;                          // 敵の向き（Y回転）
	float m_rotationSpeedDeg = 5.0f;                // 回転速度（度）

	std::weak_ptr<Player> m_wpPlayer;

	float m_moveSpeed = 0.03f;

	float m_hp = 100.0f;

	State m_state = State::Idle;                    // ステートマシンの初期状態
	float m_stateTimer = 0.0f;

	float m_chaseDist = 4.0f;       // 追跡開始距離
	float m_orbitDist = 2.0f;       // 周回距離
	float m_orbitSpeedRate = 0.5f;  // 移動速度に対する倍率

	float m_gravity = 0.0f;
	bool m_isGround = false;

	// アニメーション
	std::shared_ptr<KdAnimator> m_animator;   // アニメーション管理
	int m_nowAnimIndex = 0;                         // 現在のアニメーション番号
	float m_animSpeed = 1.0f;                 // 再生速度
};
