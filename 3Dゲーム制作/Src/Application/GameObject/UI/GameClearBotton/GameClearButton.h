#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class GameClearButton : public BaseObject
{
public:

	void Init() override;
	void Update() override;
	void DrawSprite() override;

	// クリックされたか
	bool IsClicked() const
	{
		return m_clicked;
	}

	// 表示・非表示
	void SetVisible(bool visible)
	{
		m_visible = visible;
	}

private:

	// マウスがボタンの上にあるか
	bool IsMouseOver() const;

private:

	std::shared_ptr<KdTexture> m_normalTex = nullptr;
	std::shared_ptr<KdTexture> m_hoverTex = nullptr;

	// ボタンの位置
	float m_posX = -225.0f;
	float m_posY = -230.0f;

	// ボタンサイズ
	float m_width = 450.0f;
	float m_height = 100.0f;

	bool m_visible = false;
	bool m_hover = false;
	bool m_clicked = false;
};