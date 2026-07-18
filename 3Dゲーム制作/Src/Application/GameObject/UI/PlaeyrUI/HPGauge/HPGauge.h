#pragma once
#include <Application/GameObject/BaseObject/BaseObject.h>

class HPGauge : public BaseObject
{
public:
	void Init() override;
	void DrawSprite() override;

	void SetGauge(float hp, float hpMax);

	void OnDamage(float before, float after)
	{
		float fullWidth = 200.0f;

		float beforeRate = before / m_hpMax;
		float afterRate = after / m_hpMax;

		float beforeWidth = fullWidth * beforeRate;
		float afterWidth = fullWidth * afterRate;

		m_damageBarWidth = beforeWidth - afterWidth;

		m_damageDelayTimer = 0.0f;   // 表示時間リセット
		m_showDamageEffect = true;   // 赤バー表示開始
	}

private:
	std::shared_ptr<KdTexture> m_barTex;
	std::shared_ptr<KdTexture> m_frameTex;

	float m_hp = 0.0f;
	float m_hpMax = 1.0f;

	float m_damageAmount = 0.0f;
	bool  m_showDamageEffect = false;
	float m_damageTimer = 0.0f;

	float m_damageBarWidth = 0.0;		// 赤バーの現在幅
	float m_damageDelay = 1.0f;			// 表示しておく時間
	float m_damageDelayTimer = 0.0f;	// 経過時間
	float m_damageShrinkSpeed = 50.0f;	// 縮む速度
};
