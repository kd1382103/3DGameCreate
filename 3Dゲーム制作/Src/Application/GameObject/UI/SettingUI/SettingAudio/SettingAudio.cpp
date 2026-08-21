#include "SettingAudio.h"

#include "BGMVolume/BGMVolume.h"
#include "SEVolume/SEVolume.h"

void SettingAudio::Init()
{
	//---------------------------------------
	// BGM
	//---------------------------------------
	m_bgmVolume = std::make_shared<BGMVolume>();
	m_bgmVolume->Init();

	//---------------------------------------
	// SE
	//---------------------------------------
	m_seVolume = std::make_shared<SEVolume>();
	m_seVolume->Init();
}

void SettingAudio::Update()
{
	if (m_bgmVolume)
	{
		m_bgmVolume->Update();
	}

	if (m_seVolume)
	{
		m_seVolume->Update();
	}
}

void SettingAudio::DrawSprite()
{
	if (m_bgmVolume)
	{
		m_bgmVolume->DrawSprite();
	}

	if (m_seVolume)
	{
		m_seVolume->DrawSprite();
	}
}