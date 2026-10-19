
#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

//==========================================================
// TAB設定アイコン
//==========================================================
class SettingIcon : public BaseObject
{
public:
	SettingIcon() {}
	~SettingIcon() override {}

	void Init() override;
	void DrawSprite() override;

	//---------------------------------------
	// 表示・非表示
	//---------------------------------------
	void SetVisible(bool visible)
	{
		m_visible = visible;
	}

	bool IsVisible() const
	{
		return m_visible;
	}

private:

	//---------------------------------------
	// アイコン画像
	//---------------------------------------
	std::shared_ptr<KdTexture> m_iconTex;

	//---------------------------------------
	// 表示状態
	//---------------------------------------
	bool m_visible = true;

	//---------------------------------------
	// 表示位置・サイズ
	//---------------------------------------
	const float m_posX = 550.0f;
	const float m_posY = 310.0f;
	const float m_size = 64.0f;
};
