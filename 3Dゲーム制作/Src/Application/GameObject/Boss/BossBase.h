#pragma once
#include <Application/GameObject/BaseObject/BaseObject.h>

class Player;
class CameraBase;
class HPGauge;
class GameScene;

template <class T>
class StateMachine;

class BossBase : public BaseObject
{
public:
	BossBase() {}
	virtual ~BossBase() {}

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void DrawLit() override;
	void DrawSprite() override;

	virtual Math::Vector3 GetHitCenter() const
	{
		return m_nowPos + Math::Vector3(0, 1.0f, 0);
	}

	void SetTarget(const std::shared_ptr<Player>& target)
	{
		m_wpPlayer = target;
	}

	void SetCamera(std::shared_ptr<CameraBase> cam)
	{
		m_wpCamera = cam;
	}

	void Damage(float dmg, bool isUltimate, bool finalHit) override;

	// 攻撃判定
	void DoAttackHitCheck(float range);

	// アニメ再生
	void PlayAnimationAuto(
		const std::string& animName,
		int animIndex,
		bool loop = true
	);

	// 方向ベクトル
	Math::Vector3 GetForward() const
	{
		Math::Matrix rot =
			Math::Matrix::CreateRotationY(m_angleY);

		return rot.Forward();
	}

	Math::Vector3 GetRight() const
	{
		Math::Matrix rot =
			Math::Matrix::CreateRotationY(m_angleY);

		return rot.Right();
	}

	bool IsAttackAnimationEnd() const
	{
		return m_animator.IsAnimationEnd();
	}

	bool IsAlive() const
	{
		return !m_isExpired;
	}

	void StartSlow(float time)
	{
		m_isSlow = true;
		m_slowTimer = time;
	}

	void SetGameScene(GameScene* scene)
	{
		m_pGameScene = scene;
	}

	void SetGameEnd(bool end)
	{
		m_isGameEnd = end;
	}

	float GetCollisionRadius() const
	{
		return m_collisionRadius;
	}

	float GetCollisionHeight() const
	{
		return m_collisionHeight;
	}

	void SetPos(const Math::Vector3& pos) override
	{
		m_nowPos = pos;
	}

	Math::Vector3 GetPos() const override
	{
		return m_nowPos;
	}

public:

	//==============================================================
	// モデル・アニメーション
	//==============================================================

	std::shared_ptr<KdModelWork> m_model;
	KdAnimator m_animator;


	//==============================================================
	// 位置・方向
	//==============================================================

	Math::Vector3 m_nowPos = Math::Vector3::Zero;
	Math::Vector3 m_moveDir = Math::Vector3::Zero;


	//==============================================================
	// カメラ
	//==============================================================

	std::weak_ptr<CameraBase> m_wpCamera;


	//==============================================================
	// ゲームシーン
	//==============================================================

	GameScene* m_pGameScene = nullptr;


	//==============================================================
	// プレイヤー
	//==============================================================

	std::weak_ptr<Player> m_wpPlayer;


	//==============================================================
	// HP
	//==============================================================

	std::shared_ptr<HPGauge> m_hpGauge = nullptr;


	//==============================================================
	// 回転
	//==============================================================

	float m_angleY = 0.0f;
	float m_rotationSpeedDeg = 5.0f;


	//==============================================================
	// 移動
	//==============================================================

	float m_moveSpeed = 0.05f;


	//==============================================================
	// 攻撃
	//==============================================================

	float m_attackDist = 2.0f;
	float m_attackDamage = 20.0f;

	enum class BossAttackType
	{
		Attack1,
		Attack2,
		Attack3
	};

	std::vector<BossAttackType> m_attackPattern;

	int m_attackPatternIndex = 0;

	bool m_isComboStarted = false;

	//==============================================================
	// 索敵距離
	//==============================================================

	float detectRange = 10.0f;


	//==============================================================
	// HP
	//==============================================================

	float m_hp = 1000.0f;
	int m_hpMax = 1000;


	//==============================================================
	// Boss専用ステートマシン
	//==============================================================

	std::shared_ptr<StateMachine<BossBase>> stateMachine;


	//==============================================================
	// 重力・地面判定・壁判定
	//==============================================================

	float m_gravity = 0.0f;
	bool m_isGround = false;


	//==============================================================
	// ワールド行列
	//==============================================================

	Math::Matrix m_mWorld;


	//==============================================================
	// アニメーション
	//==============================================================

	std::string animIdleName;
	std::string animWalkName;
	std::string animDashName;
	std::string animAttack1Name;
	std::string animAttack2Name;
	std::string animAttack3Name;
	std::string animSkillName;

	int animIdleIndex = -1;
	int animWalkIndex = -1;
	int animDashIndex = -1;
	int animAttack1Index = -1;
	int animAttack2Index = -1;
	int animAttack3Index = -1;
	int animSkillIndex = -1;
	int animHitIndex = -1;


	//==============================================================
	// 攻撃予知
	//==============================================================

	std::shared_ptr<KdSquarePolygon> m_preAttackPoly;

	float m_preAttackAlpha = 0.0f;
	bool m_preAttackActive = false;

	Math::Vector3 m_preAttackPos = Math::Vector3::Zero;

	float m_preAttackTimer = 0.0f;
	float m_preAttackScale = 1.0f;


	//==============================================================
	// ヒットストップ
	//==============================================================

	float m_hitStopTimer = 0.0f;


	//==============================================================
	// ロックオンアイコン
	//==============================================================

	std::shared_ptr<KdSquarePolygon> m_lockOnIcon;

	bool m_lockOnActive = false;

	Math::Vector3 m_lockOnPos = Math::Vector3::Zero;

	float m_lockOnScale = 1.0f;


	//==============================================================
	// 回避成功後のスロー
	//==============================================================

	bool m_isSlow = false;

	float m_slowTimer = 0.0f;

	float m_slowRate = 0.2f;


	//==============================================================
	// ノックバック
	//==============================================================

	float m_knockBackPower = 0.75f;


	//==============================================================
	// 攻撃ヒット済み
	//==============================================================

	bool m_attackHitOnce = false;

	bool m_isGameEnd = false;

	// Playerとの接触判定用
	float m_collisionRadius = 0.7f;
	float m_collisionHeight = 1.0f;

};