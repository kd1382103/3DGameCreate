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

	void Damage(float dmg) override;

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


	// 攻撃判定
	void DoAttackHitCheck(float range);

	// ステート方式で使う補助関数
	bool IsMoveInput() const { return m_moving; }
	bool IsAttackInput() const { return m_attackOnce; }
	bool IsSkillInput() const { return m_skillOnce; }
	bool IsDodgeInput() const { return m_dodgeing; }
	bool IsFall()const { return m_falling; }

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
	float m_angleY = DirectX::XMConvertToRadians(180.0f);
	float m_rotationSpeedDeg = 20.0f;

	// アニメ
	int m_nowAnimIndex = -1;

	// 入力状態（ステートが参照）
	bool m_moving = false;
	bool m_running = false;
	bool m_attackOnce = false;
	bool m_skillOnce = false;
	bool m_dodgeing = false;
	bool m_falling = false;

	bool m_attackHitOnce = false;

	//ロックオン
	bool m_lookOn = false;
	EnemyBase* m_lockOnTarget = nullptr;


	// スキル関連
	int   m_skillGaugeMax = 100;
	float m_skillGauge = 100;
	float m_skillRegen = 0.25f;
	const int m_skillCost = 50;

	// HP
	int   m_hpGaugeMax = 1000;
	float m_nowHp = 1000;

	float m_pendingBeforeHP = -1.0f;   // 食らった瞬間のHP
	float m_pendingAfterHP = -1.0f;   // 遅れて減らすHP
	int   m_pendingDelay = 0;       // 遅延フレーム
	float m_pendingDamage = 0.0f;    // ダメージ量

	//ヒットストップ
	float m_hitStopTimer = 0.0f;

	// 回避スロー
	int m_slowTimer = 0;

	// 重力
	float m_gravity = 0.0f;

	// コンボ用
	bool m_canNextAttack = false;

	// ステートマシン
	std::shared_ptr<StateMachine<Player>> stateMachine;

private:

	// キー管理
	bool IsKeyPressedOnce(int vk);
	std::unordered_map<int, bool> m_prevKeyState;
};
