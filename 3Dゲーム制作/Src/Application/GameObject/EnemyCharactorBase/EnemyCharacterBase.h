#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class Player;
class CameraBase;
class HPGauge;
class GameScene;

// EnemyBaseとBossBaseの共通基底クラス
class EnemyCharacterBase : public BaseObject
{
public:
	EnemyCharacterBase() = default;
	virtual ~EnemyCharacterBase() {}

	//==============================================================
	// 共通UI初期化
	//==============================================================
	void InitHPGauge();
	void InitWorldUI();

	//==============================================================
	// 深度マップ描画
	//==============================================================
	void GenerateDepthMapFromLightCommon(bool isDying);

	//==============================================================
	// 攻撃予知アイコン位置
	//==============================================================
	void UpdatePreAttackPosition();

	//==============================================================
	// ワールド行列・UI位置更新
	//==============================================================
	void UpdateWorldMatrixAndUI();

	//==============================================================
	// 地面・壁判定
	//==============================================================
	void GroundCollisionCheck();
	void WallCollisionCheck();

	//==============================================================
	// Player
	//==============================================================
	void SetTarget(const std::shared_ptr<Player>& target) { m_wpPlayer = target; }

	//==============================================================
	// Camera
	//==============================================================
	void SetCamera(std::shared_ptr<CameraBase> cam) { m_wpCamera = cam; }

	//==============================================================
	// 方向
	//==============================================================
	Math::Vector3 GetForward() const { return GetForwardFromAngle(m_angleY); }
	Math::Vector3 GetRight() const { return GetRightFromAngle(m_angleY); }

	//==============================================================
	// アニメーション
	//==============================================================
	void PlayAnimationAuto(const std::string& animName, bool loop = true);
	void SetHPGaugeVisible(bool visible);
	bool IsAttackAnimationEnd() const { return m_animator.IsAnimationEnd(); }

	//==============================================================
	// スロー
	//==============================================================
	void StartSlow(float time)
	{
		m_isSlow = true;
		m_slowTimer = time;
	}

	float GetTimeScale() const
	{
		if (m_isSlow)
		{
			return m_slowRate;
		}

		return 1.0f;
	}

	void SetGameScene(GameScene* scene) { m_pGameScene = scene; }
	void SetGameEnd(bool end) { m_isGameEnd = end; }

	//==============================================================
	// 現在座標
	//==============================================================
	Math::Vector3 GetPos() const override { return m_nowPos; }
	
	//==============================================================
	// 当たり判定
	//==============================================================
	float GetCollisionRadius() const { return m_collisionRadius; }
	float GetCollisionHeight() const { return m_collisionHeight; }

	//==============================================================
	// 位置取得
	//==============================================================
	void SetPos(const Math::Vector3& pos) override { m_nowPos = pos; }

	//==============================================================
	// 攻撃判定
	//==============================================================
	void DoAttackHitCheck(float range, bool checkHitStop);


	bool IsAttacking() const { return m_isAttacking; }
	bool IsResolving() const { return m_isResolving; }

	virtual Math::Vector3 GetHitCenter() const { return m_nowPos + Math::Vector3(0, 1.0f, 0); }

protected:
	//==============================================================
	// ノックバック判定
	//==============================================================
	void NockBackCheck();

	//==============================================================
	// 視野角カリング
	//==============================================================
	bool IsInCameraView() const;

	//==============================================================
	// スロー・重力の共通更新
	//==============================================================
	void UpdateSlowAndGravity(float dt);

	//==============================================================
	// ダメージ共通部分
	//==============================================================
	void CommonDamage(float dmg);

	//==============================================================
	// 予知・ロックオンUIの描画
	//==============================================================
	void DrawWorldUI();

	//==============================================================
	// HP関係
	//==============================================================
	void UpdateHPGauge();

	//==============================================================
	// リゾルブ処理
	//==============================================================
	void StartResolveCommon();
	void UpdateResolveCommon(float frameScale);

	//==============================================================
	// 死亡時の初期化
	//==============================================================
	void ResetDeathCommon();
public:

	// モデル・アニメーション
	std::shared_ptr<KdModelWork> m_model;
	KdAnimator m_animator;

	// 位置・方向
	Math::Vector3 m_nowPos = Math::Vector3::Zero;
	Math::Vector3 m_moveDir = Math::Vector3::Zero;

	// カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	// GameScene
	GameScene* m_pGameScene = nullptr;

	// Player
	std::weak_ptr<Player> m_wpPlayer;

	// HPゲージ
	std::shared_ptr<HPGauge> m_hpGauge = nullptr;

	// 攻撃サウンド
	std::shared_ptr<KdSoundInstance> m_attackSound;

	// 回転
	float m_angleY = 0.0f;
	float m_rotationSpeedDeg = 5.0f;

	// 重力
	float m_gravity = 0.0f;

	// 攻撃予知
	std::shared_ptr<KdSquarePolygon> m_preAttackPoly;
	float m_preAttackAlpha = 0.0f;
	bool m_preAttackActive = false;
	Math::Vector3 m_preAttackPos = Math::Vector3::Zero;
	float m_preAttackTimer = 0.0f;
	float m_preAttackScale = 1.0f;

	// ヒットストップ
	float m_hitStopTimer = 0.0f;

	// ロックオン
	std::shared_ptr<KdSquarePolygon> m_lockOnIcon;
	bool m_lockOnActive = false;
	Math::Vector3 m_lockOnPos = Math::Vector3::Zero;
	float m_lockOnScale = 1.0f;

	// ノックバック移動の保留
	Math::Vector3 m_knockBackMove = Math::Vector3::Zero;
	bool m_hasPendingKnockBack = false;

	// 攻撃ヒット済み
	bool m_attackHitOnce = false;
	bool m_attackSEPlayed = false;
	bool m_isAttacking = false;

	// 攻撃
	float m_attackDist;
	float m_attackDamage;

	// ゲーム終了
	bool m_isGameEnd = false;

	// スロー
	bool m_isSlow = false;
	float m_slowTimer = 0.0f;
	float m_slowRate = 0.2f;

	// HP
	float m_hp;
	int m_hpMax;

	// 移動
	float m_moveSpeed;

	// 索敵
	float detectRange;

	// Playerとの接触判定
	float m_collisionRadius = 0.5f;
	float m_collisionHeight = 1.0f;
	
	// リゾルブ
	bool m_isResolving = false;
	float m_resolveTimer = 0.0f;
	float m_resolveDissolve = 1.0f;
	float m_resolveDuration = 45.0f;

	// リゾルブ境界の幅
	float m_resolveEdgeRange = 0.08f;

	//リゾルブ境界の発光色
	Math::Vector3 m_resolveEmissive = { 0.1f, 0.8f, 1.0f };
};