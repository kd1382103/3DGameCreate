
#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class BattleArea : public BaseObject
{
public:
	BattleArea() = default;
	~BattleArea() override = default;

	void Init() override;
	void Update() override;
	void DrawLit() override;

	// 戦闘エリアの中心と広さを設定
	void SetArea(
		const Math::Vector3& center,
		float halfWidth,
		float halfDepth
	);

	// バリアの有効・無効
	void SetBarrierActive(bool active);

	bool IsBarrierActive() const
	{
		return m_barrierActive;
	}

	// 指定位置がエリア内か調べる
	bool IsInsideArea(const Math::Vector3& pos) const;

	// プレイヤー位置をエリア内に収める
	Math::Vector3 ClampPosition(
		const Math::Vector3& pos
	) const;

	Math::Vector3 GetAreaCenter() const
	{
		return m_areaCenter;
	}

private:
	Math::Vector3 m_areaCenter = Math::Vector3::Zero;

	// 中心から左右・前後への距離
	float m_halfWidth = 15.0f;
	float m_halfDepth = 15.0f;

	// バリアの高さ（描画実装で使用）
	float m_barrierHeight = 6.0f;

	bool m_barrierActive = false;

	// バリア描画用
	std::shared_ptr<KdSquarePolygon> m_barrierPoly;
};
