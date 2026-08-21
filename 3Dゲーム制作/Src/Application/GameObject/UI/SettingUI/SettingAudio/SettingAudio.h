#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

class BGMVolume;
class SEVolume;

class SettingAudio : public BaseObject
{
public:

	void Init() override;
	void Update() override;
	void DrawSprite() override;

	//---------------------------------------
	// BGM音量取得
	//---------------------------------------
	float GetBGMVolume() const;

private:

	std::shared_ptr<BGMVolume> m_bgmVolume;
	std::shared_ptr<SEVolume>  m_seVolume;
};