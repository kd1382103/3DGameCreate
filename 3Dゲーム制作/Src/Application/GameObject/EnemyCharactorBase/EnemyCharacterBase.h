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
	// Player
	//==============================================================

	void SetTarget(const std::shared_ptr<Player>& target)
	{
		m_wpPlayer = target;
	}

	//==============================================================
	// Camera
	//==============================================================

	void SetCamera(std::shared_ptr<CameraBase> cam)
	{
		m_wpCamera = cam;
	}

	//==============================================================
	// 方向
	//==============================================================

	Math::Vector3 GetForward() const
	{
		return GetForwardFromAngle(m_angleY);
	}

	Math::Vector3 GetRight() const
	{
		return GetRightFromAngle(m_angleY);
	}

	//==============================================================
	// アニメーション
	//==============================================================
	void PlayAnimationAuto(const std::string& animName, bool loop = true);
	void SetHPGaugeVisible(bool visible);
	bool IsAttackAnimationEnd() const
	{
		return m_animator.IsAnimationEnd();
	}

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

	//==============================================================
	// GameScene
	//==============================================================

	void SetGameScene(GameScene* scene)
	{
		m_pGameScene = scene;
	}

	void SetGameEnd(bool end)
	{
		m_isGameEnd = end;
	}

	//==============================================================
	// Position
	//==============================================================

	Math::Vector3 GetPos() const override
	{
		return m_nowPos;
	}
	
	//==============================================================
	// 深度マップ描画
	//==============================================================
	void GenerateDepthMapFromLightCommon(bool isDying);

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

	// ゲーム終了
	bool m_isGameEnd = false;

	// スロー
	bool m_isSlow = false;
	float m_slowTimer = 0.0f;
	float m_slowRate = 0.2f;
};