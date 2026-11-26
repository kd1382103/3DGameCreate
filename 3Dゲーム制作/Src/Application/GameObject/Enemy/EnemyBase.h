#pragma once
#include <Application/GameObject/EnemyCharactorBase/EnemyCharacterBase.h>

class Player;
class CameraBase;
class HPGauge;
class GameScene;

template <class T>
class StateMachine;

class EnemyBase : public EnemyCharacterBase
{
public:
	EnemyBase() = default;
	virtual ~EnemyBase() = default;

	void Init() override;
	virtual void Init(int battleNo);
	void Update() override;
	void PostUpdate() override;
	void DrawLit() override;
	void DrawSprite() override;
	void GenerateDepthMapFromLight() override;

	//==============================================================
	// Hit Center
	//==============================================================
	virtual Math::Vector3 GetHitCenter() const
	{
		return m_nowPos + Math::Vector3(0, 1.0f, 0); // 敵ごとに調整可能
	}

	//==============================================================
	// ダメージ
	//==============================================================
	void Damage(
		float dmg,
		bool isUltimate,
		bool finalHit,
		float knockBackRate
	) override;

	//==============================================================
	// 攻撃判定（ステート側が呼ぶ）
	//==============================================================
	void DoAttackHitCheck(float range);

	//==============================================================
	// アニメ再生（ステート側が呼ぶ）
	//==============================================================
	void PlayAnimationAuto(const std::string& animName, bool loop = true);

	//==============================================================
	// 生存
	//==============================================================
	bool IsAlive() const
	{
		return !m_isExpired && !m_isDying;
	}

	//==============================================================
	// Playerとの接触判定用
	//==============================================================
	float GetCollisionRadius() const
	{
		return m_collisionRadius;
	}

	float GetCollisionHeight() const
	{
		return m_collisionHeight;
	}

	//==============================================================
	// Position
	//==============================================================
	void SetPos(const Math::Vector3& pos)
	{
		m_nowPos = pos;
	}

	void SetHPGaugeVisible(bool visible);

	//==============================================================
	// チュートリアルの敵専用関数
	//==============================================================

	// 強制的に削除状態にする
	void SetExpired()
	{
		m_isExpired = true;
	}

	// チュートリアル用攻撃
	void SetCanAttack(bool enable)
	{
		m_canAttack = enable;
	}

	bool GetCanAttack() const
	{
		return m_canAttack;
	}

	void StartTutorialAttack();
	void StopTutorialAttack();

	bool IsTutorialAttackFinished() const
	{
		return m_tutorialAttackFinished;
	}

	void ResetTutorialAttackFinished()
	{
		m_tutorialAttackFinished = false;
	}

	//==============================================================
	// ノックバック
	//==============================================================
	void SetCanKnockBack(bool enable)
	{
		m_canKnockBack = enable;
	}

	//==============================================================
	// 攻撃関連
	//==============================================================
	void StopAttackSound();
	void LookAtPlayer();

	bool IsAttacking() const
	{
		return m_isAttacking;
	}

	//==============================================================
	// Resolve（出現演出）
	//==============================================================
	bool IsResolving() const
	{
		return m_isResolving;
	}

	void StartResolve();

protected:
	void InitModelAndCollider(
		const std::string& modelPath,
		const std::string& colliderName
);private:

	//==============================================================
	// 初期化
	//==============================================================
	void InitHPGauge();
	void InitStateMachine();

	//==============================================================
	// 更新
	//==============================================================
	void UpdateAnimation(float dt);
	void UpdateDebug();
	void UpdateResolve(float frameScale);

	void StartDeath();
	void UpdateDeath(float frameScale);

	//==============================================================
	// 後更新
	//==============================================================
	void UpdateGroundCollision();

	//==============================================================
	// 死亡時の状態リセット
	//==============================================================
	void ResetBattleState();
   
public:

	//==============================================================
	// 移動
	//==============================================================

	float m_moveSpeed;

	//==============================================================
	// 攻撃
	//==============================================================

	float m_attackDist;
	float m_attackDamage;

	//==============================================================
	// 索敵距離
	//==============================================================

	float detectRange;

	//==============================================================
	// HP
	//==============================================================

	float m_hp;
	int m_hpMax;

	//==============================================================
	// ステートマシン
	//==============================================================

	std::shared_ptr<StateMachine<EnemyBase>> stateMachine;

	//==============================================================
	// 地面判定
	//==============================================================

	bool m_isGround = false;

	//==============================================================
	// アニメーション名
	//==============================================================

	// アニメ名・番号は Base に持たない
	// 各敵が Init() で設定する

	std::string animIdleName;
	std::string animWalkName;
	std::string animDashName;
	std::string animAttackName;

	//==============================================================
	// 攻撃サウンド
	//==============================================================

	std::shared_ptr<KdSoundInstance> m_attackSound;

	//==============================================================
	// ノックバック可能か
	//==============================================================

	bool m_canKnockBack = true;

	//==============================================================
	// チュートリアル専用
	//==============================================================

	bool m_canAttack = false;

	// チュートリアル攻撃中か
	bool m_isTutorialAttack = false;

	// チュートリアル攻撃を回避成功で終了したか
	bool m_tutorialAttackFinished = false;

	//==============================================================
	// 攻撃中
	//==============================================================

	bool m_isAttacking = false;

	//==============================================================
	// 死亡
	//==============================================================

	bool m_isDying = false;

private:

	//==============================================================
	// Playerとの接触判定用
	//==============================================================

	float m_collisionRadius = 0.5f;
	float m_collisionHeight = 1.0f;

	//==============================================================
	// Resolve（敵出現演出）
	//==============================================================

	bool m_isResolving = false;
	float m_resolveTimer = 0.0f;

	// 60FPS基準で45フレーム = 0.75秒
	static constexpr float m_resolveDuration = 45.0f;

	// ディゾルブ値
	float m_resolveDissolve = 1.0f;

	// ディゾルブ境界の幅
	float m_resolveEdgeRange = 0.08f;

	// ディゾルブ境界の発光色
	Math::Vector3 m_resolveEmissive = { 0.1f, 0.8f, 1.0f };

	//==============================================================
	// 死亡演出
	//==============================================================

	// 死亡アニメーションが終了したか
	bool m_isDeathAnimationEnd = false;
};