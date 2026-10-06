#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class TitleImage : public BaseObject
{
public:

	void Init() override;
	void Update() override;
	void DrawSprite() override;

private:

	//std::shared_ptr<KdTexture> m_titleTex;
	
	//---------------------------------------
	// NEON BLADE ロゴ
	//---------------------------------------
	std::shared_ptr<KdTexture> m_logoTex;

	//---------------------------------------
	// PRESS ANY BUTTON
	//---------------------------------------
	std::shared_ptr<KdTexture> m_pressTex;

	//---------------------------------------
	// PRESS ANY BUTTON フェード
	//---------------------------------------
	float m_pressAlpha = 1.0f;

	// true  = 消えていく
	// false = 見えていく
	bool m_pressFadeOut = false;
};