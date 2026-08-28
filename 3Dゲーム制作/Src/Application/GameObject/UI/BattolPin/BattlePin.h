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

	std::shared_ptr<CameraBase> m_camera;

	std::shared_ptr<KdSquarePolygon> m_pinPoly;

	float m_scale = 0.6f;

	bool m_visible = true;
};