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

	void SetPos(const Math::Vector3& pos)
	{
		m_pos = pos;
	}

	Math::Vector3 GetPos() const
	{
		return m_pos;
	}

	void SetBattleRange(float range)
	{
		m_battleRange = range;
	}

	float GetBattleRange() const
	{
		return m_battleRange;
	}

	bool IsInsideRange(const Math::Vector3& pos) const
	{
		Math::Vector3 diff = pos - m_pos;
		diff.y = 0.0f;

		return diff.LengthSquared()
			<= m_battleRange * m_battleRange;
	}

	void SetCamera(
		const std::shared_ptr<CameraBase>& camera)
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

	enum class Edge
	{
		Left,
		Right,
		Top,
		Bottom
	};

	void DrawEdgeGlow(Edge edge);

private:

	Math::Vector3 m_pos =
		Math::Vector3::Zero;

	float m_battleRange = 1.0f;

	std::shared_ptr<CameraBase> m_camera;

	std::shared_ptr<KdSquarePolygon> m_pinPoly;

	float m_scale = 0.6f;

	bool m_visible = true;

	float m_glowSize = 70.0f;

	int m_glowSteps = 8;
};