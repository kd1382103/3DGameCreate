#pragma once
#include <Application/GameObject/EnemyCharactorBase/EnemyCharacterBase.h>

class Player;
class CameraBase;
class HPGauge;
class GameScene;

template <class T>
class StateMachine;

class BossBase : public EnemyCharacterBase
{
public:
public:
	BossBase() 
	{
		// HP
		m_hp = 1000.0f;
		m_hpMax = 1000;

		// 移動
		m_moveSpeed = 0.05f;

		// 攻撃
		m_attackDist = 2.0f;
		m_attackDamage = 20.0f;

		// 索敵
		detectRange = 10.0f;

		// Playerとの接触判定
		m_collisionRadius = 0.7f;
		m_collisionHeight = 1.0f;
	}

	virtual ~BossBase() {}

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void DrawLit() override;
	void DrawSprite() override;
	void GenerateDepthMapFromLight() override;

	virtual Math::Vector3 GetHitCenter() const
	{
		return m_nowPos + Math::Vector3(0, 1.0f, 0);
	}

	//==============================================================
	// Boss専用ダメージ
	//==============================================================
	void DamageBoss(
		float dmg,
		bool isUltimate,
		bool finalHit,
		float knockBackRate = 1.0f,
		bool interruptAttack = false
	);

	//==============================================================
	// 生存
	//==============================================================
	bool IsAlive() const
	{
		return !m_isExpired;
	}

	//==============================================================
	// Resolve（出現演出）
	//==============================================================
	bool IsResolving() const
	{
		return m_isResolving;
	}

	void StartResolve();

	//==============================================================
	// Death（死亡演出）
	//==============================================================
	void StartDeath();
	void UpdateDeath(float frameScale);

	// 死亡演出を開始する前の待機
	void SetDeathPending(bool pending)
	{
		m_isDeathPending = pending;
	}

	bool IsDeathPending() const
	{
		return m_isDeathPending;
	}

	bool IsDying() const
	{
		return m_isDying;
	}

	bool IsHPZero() const
	{
		return m_hp <= 0.0f;
	}

	bool IsDeathSequenceFinished() const
	{
		return m_isExpired;
	}

	// 死亡アニメーションが終了したか
	bool IsDeathAnimationFinished() const
	{
		return m_isDying && m_animator.IsAnimationEnd();
	}

	// 爆発・消滅演出の一時停止
	void SetDeathEffectPaused(bool paused)
	{
		m_isDeathEffectPaused = paused;
	}

public:


	//==============================================================
	// 攻撃
	//==============================================================
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
	// Bossステートマシン
	//==============================================================

	std::shared_ptr<StateMachine<BossBase>> stateMachine;


	//==============================================================
	// アニメーション番号
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
	// ノックバック
	//==============================================================

	float m_knockBackPower = 0.75f;


	//==============================================================
	// Playerとの接触判定
	//==============================================================

	float m_collisionRadius = 0.7f;
	float m_collisionHeight = 1.0f;

private:

	//==============================================================
	// Resolve
	//==============================================================

	void UpdateResolve(float frameScale);

	bool m_isResolving = false;

	float m_resolveTimer = 0.0f;

	static constexpr float m_resolveDuration = 45.0f;

	float m_resolveDissolve = 1.0f;

	float m_resolveEdgeRange = 0.08f;

	Math::Vector3 m_resolveEmissive =
	{
		0.1f,
		0.8f,
		1.0f
	};

	//==============================================================
	// Death（死亡演出）
	//==============================================================

	bool m_isDying = false;

	// Deathアニメーション終了後のディゾルブ中か
	bool m_isDeathDissolving = false;

	// ディゾルブ開始後の経過フレーム
	float m_deathTimer = 0.0f;

	// ディゾルブ値：0.0 = 表示、1.0 = 消滅
	float m_deathDissolve = 0.0f;

	// ディゾルブ時間
	static constexpr float m_deathDuration = 45.0f;

	// ディゾルブの縁の範囲
	float m_deathEdgeRange = 0.08f;

	// 死亡演出を開始する前の待機フラグ
	bool m_isDeathPending = false;

	// ディゾルブの縁の発光色
	Math::Vector3 m_deathEmissive =
	{
		0.1f,
		0.8f,
		1.0f
	};

	bool m_isDeathEffectPlayed = false;

	// 爆発エフェクト再生までの待機フレーム数
	static constexpr float m_deathEffectDelay = 18.0f;

	// 爆発エフェクト再生までの経過フレーム
	float m_deathEffectDelayTimer = 0.0f;

	// 爆発待機中か
	bool m_isDeathEffectWaiting = false;

	// プレイヤー退避中は爆発・消滅演出を停止する
	bool m_isDeathEffectPaused = false;
};