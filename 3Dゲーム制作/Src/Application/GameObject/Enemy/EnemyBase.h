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
	// 生存
	//==============================================================
	bool IsAlive() const
	{
		return !m_isExpired && !m_isDying;
	}

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


protected:
	void InitModelAndCollider(
		const std::string& modelPath,
		const std::string& colliderName
);

private:
	//==============================================================
	// 更新
	//==============================================================
	void UpdateDebug();

	void StartDeath();
	void UpdateDeath(float frameScale);

	//==============================================================
	// 死亡時の状態リセット
	//==============================================================
	void ResetBattleState();


   
public:

	//==============================================================
	// ステートマシン
	//==============================================================

	std::shared_ptr<StateMachine<EnemyBase>> stateMachine;

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
	// Resolve（敵出現演出）
	//==============================================================
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