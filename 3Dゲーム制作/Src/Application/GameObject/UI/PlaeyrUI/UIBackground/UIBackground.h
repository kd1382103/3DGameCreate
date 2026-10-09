
#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class UIBackground : public BaseObject
{
public:
	UIBackground() {}
	~UIBackground() override {}

	void Init() override;
	void DrawSprite() override;

	// プレイヤーHUD用
	void InitPlayerUI();

	// チュートリアル用
	void InitTutorial(
		const Math::Vector2& pos,
		float width,
		float height
	);

	void SetVisible(bool visible) { m_visible = visible; }
	bool IsVisible() const { return m_visible; }
	void SetExpired() { m_isExpired = true; }

private:
	// 四角形パネルを描画
	void DrawPanel(
		int x,
		int y,
		int halfWidth,
		int halfHeight
	);

	// 用途
	bool m_isTutorialBackground = false;

	// 表示状態
	bool m_visible = true;

	// チュートリアルパネル
	Math::Vector2 m_pos = { 0.0f, 300.0f };
	float m_width = 700.0f;
	float m_height = 100.0f;

	// 外枠の色
	Math::Color m_borderColor =
	{
		0.0f,0.0f,0.0f,0.55f
	};

	// 内側の色
	Math::Color m_panelColor =
	{
		0.15f,0.15f,0.15f,0.35f
	};
};