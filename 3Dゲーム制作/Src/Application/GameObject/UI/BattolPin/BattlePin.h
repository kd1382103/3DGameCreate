#pragma once

class CameraBase;
class FontText;

//==============================================================
// バトル開始地点ピン
//==============================================================
class BattlePin : public KdGameObject
{
public:

	BattlePin() {}
	~BattlePin() override {}

	//==========================================================
	// 初期化・更新・描画
	//==========================================================
	void Init() override;
	void Update() override;
	void DrawSprite() override;

	//==========================================================
	// 画面端の方向
	//==========================================================
	enum class Edge
	{
		Left,
		Right,
		Top,
		Bottom
	};

	//==========================================================
	// 位置
	//==========================================================
	void SetPos(const Math::Vector3& pos)
	{
		m_pos = pos;
	}

	Math::Vector3 GetPos() const
	{
		return m_pos;
	}

	//==========================================================
	// バトル範囲
	//==========================================================
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

	//==========================================================
	// カメラ
	//==========================================================
	void SetCamera(
		const std::shared_ptr<CameraBase>& camera)
	{
		m_camera = camera;
	}

	//==========================================================
	// 表示・非表示
	//==========================================================
	void SetVisible(bool visible)
	{
		m_visible = visible;
	}

	bool IsVisible() const
	{
		return m_visible;
	}

	//==========================================================
	// プレイヤー位置
	//==========================================================
	void SetPlayerPos(const Math::Vector3& pos)
	{
		m_playerPos = pos;
	}

private:

	//==========================================================
	// 画面端の描画
	//==========================================================
	void DrawEdgeGlow(Edge edge);

	void DrawEdgePin(
		Edge edge,
		const Math::Vector2& screen);

	void DrawDistance(
		Edge edge,
		const Math::Vector2& screen,
		float distance);

	//==========================================================
	// バトル開始地点
	//==========================================================
	Math::Vector3 m_pos = Math::Vector3::Zero;

	//==========================================================
	// プレイヤー位置
	//==========================================================
	Math::Vector3 m_playerPos = Math::Vector3::Zero;

	//==========================================================
	// バトル範囲
	//==========================================================
	float m_battleRange = 1.0f;

	//==========================================================
	// カメラ
	//==========================================================
	std::shared_ptr<CameraBase> m_camera;

	//==========================================================
	// ピン画像
	//==========================================================
	std::shared_ptr<KdSquarePolygon> m_pinPoly;
	float m_scale = 0.6f;

	//==========================================================
	// 距離表示
	//==========================================================
	std::shared_ptr<FontText> m_distanceText;
	float m_distanceTextScale = 1.0f;

	//==========================================================
	// 画面端表示設定
	//==========================================================
	float m_glowSize = 100.0f;
	int m_glowSteps = 10;
	float m_edgePinSize = 20.0f;

	//==========================================================
	// 表示状態
	//==========================================================
	bool m_visible = true;
};