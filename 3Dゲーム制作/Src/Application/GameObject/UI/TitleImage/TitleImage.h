#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class TitleImage : public BaseObject
{
public:

	void Init() override;
	void DrawSprite() override;

private:

	std::shared_ptr<KdTexture> m_titleTex;
	std::shared_ptr<KdTexture> m_pressTex;
};