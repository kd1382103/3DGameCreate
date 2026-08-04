#pragma once
#include <Application/GameObject/BaseObject/BaseObject.h>

class Player;
class CameraBase;
class HPGauge;
template <class T>
class StateMachine;

class EnemyBase : public BaseObject
{
public:
	EnemyBase() {}
	virtual ~EnemyBase() {}

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void DrawLit() override;
	void DrawSprite()override;

	virtual Math::Vector3 GetHitCenter() const
	{
		return m_nowPos + Math::Vector3(0, 1.0f, 0); // 敵ごとに調整可能
	}

	void SetTarget(const std::shared_ptr<Player>& target) { m_wpPlayer = target; }
	void SetPos(const Math::Vector3& pos) { m_nowPos = pos; }
	void SetCamera(std::shared_ptr<CameraBase> cam) { m_wpCamera = cam; }

	virtual void Damage(float dmg);

	// 攻撃判定（ステート側が呼ぶ）
	void DoAttackHitCheck(float range);

	// アニメ再生（ステート側が呼ぶ）
	void PlayAnimationAuto(const std::string& animName, int animIndex, bool loop = true);

	// 方向ベクトル（プレイヤーと同じ）
	Math::Vector3 GetForward() const
	{
		Math::Matrix rot = Math::Matrix::CreateRotationY(m_angleY);
		return rot.Forward();
	}

	Math::Vector3 GetRight() const
	{
		Math::Matrix rot = Math::Matrix::CreateRotationY(m_angleY);
		return rot.Right();
	}

	bool IsAttackAnimationEnd() const
	{
		return m_animator.IsAnimationEnd();
	}

	bool IsAlive() const { return !m_isExpired; }

	void StartSlow(float time)
	{
		m_isSlow = true;
		m_slowTimer = time;
	}

public:

	// モデル・アニメーション
	std::shared_ptr<KdModelWork> m_model;
	KdAnimator m_animator;

	// 位置・方向
	Math::Vector3 m_nowPos = Math::Vector3::Zero;
	Math::Vector3 m_moveDir = Math::Vector3::Zero;

	// カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	// プレイヤー
	std::weak_ptr<Player> m_wpPlayer;

	//HP
	std::shared_ptr<HPGauge> m_hpGauge = nullptr;

	// 回転
	float m_angleY = 0.0f;
	float m_rotationSpeedDeg = 5.0f;  
	
	// 移動
	float m_moveSpeed ;         

	// 攻撃
	float m_attackDist ;      
	float m_attackDamage;   

	//　索敵距離
	float detectRange ;

	// HP
	float m_hp ;
	int   m_hpMax ;

	// ステートマシン
	std::shared_ptr<StateMachine<EnemyBase>> stateMachine;

	// 重力・地面判定・壁判定
	float m_gravity = 0.0f;
	bool  m_isGround = false;

	// ワールド行列
	Math::Matrix m_mWorld;

	// アニメ名・番号は Base に持たない
	//   → 各敵が Init() で設定する
	std::string animIdleName;
	std::string animWalkName;
	std::string animDashName;
	std::string animAttack1Name;
	std::string animSkillName;

	int animIdleIndex = -1;
	int animWalkIndex = -1;
	int animDashIndex = -1;
	int animAttack1Index = -1;
	int animSkillIndex = -1;
	int animHitIndex = -1;

	// 攻撃予知（ビルボード）
	std::shared_ptr<KdSquarePolygon> m_preAttackPoly;
	float m_preAttackAlpha = 0.0f;
	bool  m_preAttackActive = false;

	Math::Vector3 m_preAttackPos = Math::Vector3::Zero;
	float m_preAttackTimer = 0.0f;
	float m_preAttackScale = 1.0f;
	
	float m_hitStopTimer = 0.0f;

	// ロックオンアイコン
	std::shared_ptr<KdSquarePolygon> m_lockOnIcon;
	bool m_lockOnActive = false;
	Math::Vector3 m_lockOnPos = Math::Vector3::Zero;
	float m_lockOnScale = 1.0f;

	//回避成功された後のスロー
	bool m_isSlow = false;
	float m_slowTimer = 0.0f;
	float m_slowRate = 0.2f;

	//ノックバックの強さ
	float m_knockBackPower = 0.75f;

	bool m_attackHitOnce = false;
};
