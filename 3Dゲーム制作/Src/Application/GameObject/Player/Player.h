#pragma once
#include<Application/GameObject/BaseObject/BaseObject.h>

class SkillGauge;
class HPGauge;
class CameraBase;

class Player :public BaseObject
{

public:

	enum class UIType
	{
		SkillGauge,
		HPGauge,
		// 必要なら追加
	};

	std::unordered_map<UIType, std::shared_ptr<BaseObject>> m_uiMap;

	enum class PlayerState {
		Idle,
		Run,
		//Jump,
		Attack1,
		Attack2,
		Attack3,
		Skill,
		Landing,
		Fall,
		Dodge,
	};

	PlayerState m_state = PlayerState::Idle;
	float m_stateTimer = 0.0f;

	Player() {}
	~Player() override { Release(); }

	void Init()							override;
	void Update()						override;
	void PostUpdate()					override;
	void DrawLit()						override;
	void GenerateDepthMapFromLight()	override;

	void Damage(float dmg)override;

	void SetCamera(std::shared_ptr<CameraBase> camera) { m_wpCamera = camera; }
	void RegisterUI(UIType type, const std::shared_ptr<BaseObject>& ui) {m_uiMap[type] = ui;}
	void SetPos(Math::Vector3 pos) { m_nowPos = pos; };

	//void SetRotation(const Math::Vector3& rot) { m_rotation = rot; }
	//void SetRotationY(float y) { m_rotation.y = y; }
	//const Math::Vector3& GetRotation() const { return m_rotation; }



	template <class T>
	std::shared_ptr<T> GetUI(UIType type)
	{
		auto it = m_uiMap.find(type);
		if (it == m_uiMap.end()) return nullptr;

		return std::dynamic_pointer_cast<T>(it->second);
	}


private:
	std::shared_ptr<KdModelWork>m_model;
	KdAnimator m_animator;
	std::weak_ptr<CameraBase>m_wpCamera;

	//方向
	Math::Vector3 m_dir = Math::Vector3::Zero;
	Math::Vector3 m_rotation = Math::Vector3::Zero;
	Math::Vector3 m_nowPos;


	//移動・走り
	float m_angleY = DirectX::XMConvertToRadians(180.0f);

	bool m_keepRunning = false;

	//アニメーション管理
	int m_nowAnimIndex = -1;
	float m_attackAnimeTime;

	//ジャンプ
	bool m_isLanding = false;
	Math::Vector3 m_jumpDir = Math::Vector3::Zero;


	Math::Vector3 m_move;
	Math::Vector3 m_fall;

	//重力
	float m_gravity = 0.0f;

	//ステートマシン
	void UpdateIdle();		//待機
	void UpdateRun();		//歩き・走り
	//void UpdateJump();		//ジャンプ
	void UpdateAttack1();	//攻撃一段目
	void UpdateAttack2();	//攻撃二段目
	void UpdateAttack3();	//攻撃三段目
	void UpdateSkill();		//スキル
	void UpdateDodge();		//回避
	void UpdateLanding();	//高所着地処理
	void UpdateFall();		//落下処理
	//void UpdateDodge();
	//void UpdateHit();
	//void UpdateDown();

	void ChangeState(PlayerState next);

	//その状態かどうかフラグ
	bool m_moving	 = false;
	bool m_running	 = false;
	//bool m_jumping	 = false;
	//bool m_jumpOnce = false;
	bool m_attacking = false;
	bool m_attackOnce = false;
	bool m_skillOnce = false;
	bool m_dodgeing = false;

	//キー管理関数
	bool IsKeyPressedOnce(int vk);
	std::unordered_map<int, bool> m_prevKeyState;
	//　↑　C++標準ライブラリの連想コンテナにある
	//		「int をキーにして bool を保存するハッシュテーブル」

	//キャラクターの位置フレームにおける回転速度上限
	float m_rotationSpeedDeg = 20.0f;

	//攻撃予約
	bool m_canNextAttack = false;
	bool m_skillReserved = false;

	//スキルコスト&ゲージ
	int m_skillGaugeMax = 100;   // 最大値
	float m_skillGauge = m_skillGaugeMax;        // 最大100
	const int m_skillCost = 50;    // スキル発動に必要なコスト
	float m_skillRegen = 0.25f;  // 毎フレームの自動回復量（超微量）

	int m_hpGaugeMax = 1000;
	float m_hpGauge = m_hpGaugeMax;

	void DoAttackHitCheck(float range);
};