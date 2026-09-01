#pragma once

#include <Application/GameObject/Effect/EffectType/EffectType.h>

class CameraBase;

//==============================================================
// エフェクト管理クラス
//==============================================================
class EffectManager
{
public:

	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

	//==========================================================
	// カメラ設定
	//==========================================================
	void SetCamera(
		const std::shared_ptr<CameraBase>& camera)
	{
		m_wpCamera = camera;
	}

	//==============================================================
	// エフェクト再生
	//==============================================================

	// 通常エフェクト
	void Play(
		EffectType type,
		const Math::Vector3& pos);

	// 方向付きエフェクト
	void Play(
		EffectType type,
		const Math::Vector3& pos,
		const Math::Vector3& direction);

private:

	EffectManager() = default;
	~EffectManager() = default;

	std::weak_ptr<CameraBase> m_wpCamera;
};