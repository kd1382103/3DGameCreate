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

	//---------------------------------------
	// 攻撃音
	//---------------------------------------
	void StopAttackSound()
	{	
		if (m_attackSound)
		{
			m_attackSound->Stop();
			m_attackSound.reset();
		}
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
	void LookAtPlayer();


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
	// 死亡
	//==============================================================

	bool m_isDying = false;

private:
	//==============================================================
	// 死亡演出
	//==============================================================

	// 死亡アニメーションが終了したか
	bool m_isDeathAnimationEnd = false;
};