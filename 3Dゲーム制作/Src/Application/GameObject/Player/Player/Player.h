#pragma once
#include <Application/GameObject/BaseObject/BaseObject.h>

class CameraBase;
class HPGauge;

class EnemyBase;

// 前方宣言：ステートマシン
template <class T>
class StateMachine;

class Player : public BaseObject
{
public:

	// UI 種類
	enum class UIType
	{
		SkillGauge,
		HPGauge,
	};

	std::unordered_map<UIType, std::shared_ptr<BaseObject>> m_uiMap;

	Player() {}
	~Player() override { Release(); }

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void DrawLit() override;
	void DrawSprite() override;
	void GenerateDepthMapFromLight() override;

	void Damage(float dmg, bool isUltimate = false, bool finalHit = false) override;

	void SetCamera(std::shared_ptr<CameraBase> camera) { m_wpCamera = camera; }
	void RegisterUI(UIType type, const std::shared_ptr<BaseObject>& ui) { m_uiMap[type] = ui; }
	void SetPos(Math::Vector3 pos) { m_nowPos = pos; }

	// アニメ番号版
	void SetAnim(int animIndex, bool loop)
	{
		m_nowAnimIndex = animIndex;
		m_animator.SetAnimation(m_model->GetAnimation(animIndex), loop);
	}

	template <class T>
	std::shared_ptr<T> GetUI(UIType type)
	{
		auto it = m_uiMap.find(type);
		if (it == m_uiMap.end()) return nullptr;
		return std::dynamic_pointer_cast<T>(it->second);
	}

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

	void AddUltimateEnergy(float value)
	{
		m_ultimateEnergy += value;

		if (m_ultimateEnergy > m_ultimateEnergyMax)
			m_ultimateEnergy = m_ultimateEnergyMax;
	}

	// 攻撃判定
	//void DoAttackHitCheck(float range,int damage);	//単体
	void DoAttackHitCheckMulti(float range, float width, int damage);	//範囲

	//必殺技用
	void DoUltimateHitCheck(float range, float width, int damage);

	// ステート方式で使う補助関数
	bool IsMoveInput() const { return m_moving; }
	bool IsAttackInput() const { return m_attackOnce; }
	bool IsSkillInput() const { return m_skillOnce; }
	bool IsDodgeInput() const { return m_dodgeing; }
	bool IsUltimateInput() const { return m_ultimateOnce; }

	//ゲームが終了したかどうか
	void SetGameEnd(bool end) { m_isGameEnd = end; }

	//生存フラグ
	bool IsAlive() const { return m_nowHp > 0; }


	//========================================
	// チュートリアル用
	//========================================

	bool IsMoving()			const { return m_moving; }
	bool IsRunning()		const { return m_running; }
	bool IsAttackOnce()		const { return m_attackOnce; }
	bool IsSkillOnce()		const { return m_skillOnce; }
	bool IsUltimateOnce()	const { return m_ultimateOnce; }
	bool IsLockOn()			const { return m_lookOn; }

	bool IsComboFinished()	const { return m_comboFinished; }
	void ResetComboFinished() { m_comboFinished = false; }

	bool IsJustDodgeSuccess() const { return m_justDodgeSuccess; }
	void ResetJustDodgeSuccess() { m_justDodgeSuccess = false; }

	//=======================================
	// 必殺技ゲージをMAXにする
	//=======================================
	void SetUltimateEnergyMax()
	{
		m_ultimateEnergy = m_ultimateEnergyMax;
	}

	bool IsUltimateActivated() const
	{
		return m_ultimateActivated;
	}

	void ResetUltimateActivated()
	{
		m_ultimateActivated = false;
	}
	//========================================

	//敵・ボスとの当たり判定
	void ResolveContact();

	float GetCollisionRadius() const { return m_collisionRadius; }

	void SetInputLock(bool lock)
	{
		m_inputLock = lock;
	}

	//---------------------------------------
	// 剣の軌跡
	//---------------------------------------
	void StartSwordTrail()
	{
		m_isSwordTrailActive = true;
	}

	void StopSwordTrail()
	{
		m_isSwordTrailActive = false;
	}

	bool IsSwordTrailActive() const
	{
		return m_isSwordTrailActive;
	}

public:
	// モデル・アニメーション
	std::shared_ptr<KdModelWork> m_model;
	KdAnimator m_animator;

	std::shared_ptr<HPGauge> m_hpGauge = nullptr;

	// カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	// 位置・方向
	Math::Vector3 m_dir = Math::Vector3::Zero;
	Math::Vector3 m_nowPos = Math::Vector3::Zero;

	// 回避用
	Math::Vector3 m_dodgeDir = Math::Vector3::Zero;
	bool m_isInvincible = false;

	// 回転
	float	m_angleY = DirectX::XMConvertToRadians(180.0f);
	float	m_rotationSpeedDeg = 20.0f;
	bool	m_canDodge = false;

	// 回避スロー
	float m_slowTimer = 0.0f;
	bool m_justDodgeSuccess = false;

	// アニメ
	int m_nowAnimIndex = -1;

	// 入力状態（ステートが参照）
	bool m_moving = false;
	bool m_running = false;
	bool m_attackOnce = false;
	bool m_skillOnce = false;
	bool m_dodgeing = false;
	bool m_attackHitOnce = false;
	bool m_ultimateOnce = false;

	bool m_attackSEPlayed = false;

	//攻撃
	float m_attackDist = 1.25f;

	//ロックオン
	bool m_lookOn = false;
	KdGameObject* m_lockOnTarget = nullptr;

	//歩行速度
	float m_walkSpeed = 0.05f;
	float m_runSpeed = 0.10f;

	// スキル関連
	int   m_skillGaugeMax = 100;
	float m_skillGauge = 100;
	float m_skillRegen = 0.25f;
	const int m_skillCost = 50;

	// 必殺技関連
	float		m_ultimateEnergyMax		= 500;
	float		m_ultimateEnergy		= 0;
	bool		m_attackContact			= false;	// 攻撃が当たったかどうか
	bool		m_canGainUltimate		= false;	// 必殺技の次の攻撃が可能かどうか
	float		m_ultimateHitTimer		= 0.0f;		// 必殺技の攻撃判定の時間
	const float m_ultimateHitInterval	= 5; 
	int			m_ultimateHitCount		= 0;		// 必殺技の攻撃回数
	bool		m_ultimateActivated		= false;	// 必殺技が実際に発動したか


	// HP
	int   m_hpGaugeMax	= 1000;
	float m_nowHp		= 1000;

	float m_pendingBeforeHP = -1.0f;   // 食らった瞬間のHP
	float m_pendingAfterHP	= -1.0f;   // 遅れて減らすHP
	int   m_pendingDelay	= 0;       // 遅延フレーム
	float m_pendingDamage	= 0.0f;    // ダメージ量

	//ヒットストップ
	float m_hitStopTimer = 0.0f;

	// 重力
	float m_gravity = 0.0f;

	// コンボ用
	bool m_canNextAttack = false;

	// ステートマシン
	std::shared_ptr<StateMachine<Player>> stateMachine;

	//ゲームが終了したかどうか
	bool m_isGameEnd = false;

	// 入力ロック
	bool m_inputLock = false;

	// チュートリアル用
	bool m_comboFinished = false;

private:

	// キー管理
	bool IsKeyPressedOnce(int vk);
	std::unordered_map<int, bool> m_prevKeyState;

	//当たり判定
	float m_collisionRadius = 0.5f;
	float m_collisionHeight = 1.0f;

	// 剣の軌跡
	bool m_isSwordTrailActive = false;
};
