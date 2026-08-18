#pragma once
#include <Application/GameObject/BaseObject/BaseObject.h>
class CameraBase;
class HPGauge : public BaseObject
{
public:

	enum class GaugeMode
	{
		Screen,   // プレイヤーUI
		World     // 敵の頭上
	};

	void Init() override;
	void DrawSprite() override;

	void SetGauge(float hp, float hpMax)
	{
		m_hp = hp;
		m_hpMax = hpMax;
	}

	void OnDamage(float before, float after)
	{
		float beforeRate = before / m_hpMax;
		float afterRate = after / m_hpMax;

		m_damageBarRate =  beforeRate - afterRate;

		m_damageDelayTimer = 0.0f;   // 表示時間リセット
		m_showDamageEffect = true;   // 赤バー表示開始
	}

	// 敵用：ワールド座標をセット
	void SetWorldPos(const Math::Vector3& pos)
	{
		m_worldPos = pos;
		m_mode = GaugeMode::World;
	}

	// カメラをセット（WorldToScreen 用）
	void SetCamera(const std::shared_ptr<CameraBase>& cam)
	{
		m_wpCamera = cam;
	}

	void SetScale(float s) { m_scale = s; }

	void SetMode(GaugeMode mode)
	{
		m_mode = mode;
	}

private:
	std::shared_ptr<KdTexture> m_barTex = nullptr;

	GaugeMode m_mode = GaugeMode::Screen;
	Math::Vector3 m_worldPos = Math::Vector3::Zero;
	std::weak_ptr<CameraBase> m_wpCamera;

	float m_hp = 0.0f;
	float m_hpMax = 1.0f;

	float m_scale = 1.0f;
	bool  m_showDamageEffect = false;

	float m_damageBarRate = 0.0f;		// 赤バーの現在幅
	float m_damageDelay = 1.0f;			// 表示しておく時間
	float m_damageDelayTimer = 0.0f;	// 経過時間
	float m_damageShrinkSpeed = 50.0f;	// 縮む速度
};
