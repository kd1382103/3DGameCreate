#pragma once

class CameraBase;

class BattlePin : public KdGameObject
{
public:

	BattlePin() {}
	~BattlePin() override {}

	void Init() override;
	void Update() override;
	void DrawSprite() override;

	//---------------------------------------
	// 座標
	//---------------------------------------
	void SetPos(const Math::Vector3& pos)
	{
		m_pos = pos;
	}

	Math::Vector3 GetPos() const
	{
		return m_pos;
	}

	//---------------------------------------
	// 戦闘開始範囲
	//---------------------------------------
	void SetBattleRange(float range)
	{
		m_battleRange = range;
	}

	float GetBattleRange() const
	{
		return m_battleRange;
	}

	// プレイヤーが戦闘範囲内にいるか
	bool IsInsideRange(const Math::Vector3& pos) const
	{
		Math::Vector3 diff = pos - m_pos;

		// 高さ(Y)は判定しない
		diff.y = 0.0f;

		return diff.LengthSquared() <=
			m_battleRange * m_battleRange;
	}

	//---------------------------------------
	// カメラ
	//---------------------------------------
	void SetCamera(const std::shared_ptr<CameraBase>& camera)
	{
		m_camera = camera;
	}

	void SetVisible(bool visible)
	{
		m_visible = visible;
	}

	bool IsVisible() const
	{
		return m_visible;
	}

private:

	Math::Vector3 m_pos = Math::Vector3::Zero;

	// 戦闘開始範囲
	float m_battleRange = 1.0f;

	std::shared_ptr<CameraBase> m_camera;

	std::shared_ptr<KdSquarePolygon> m_pinPoly;

	float m_scale = 0.6f;

	bool m_visible = true;
};