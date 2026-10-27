#pragma once

#include <Application/GameObject/Effect/EffectType/EffectType.h>

class CameraBase;
class Player;

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

	//==========================================================
	// エフェクト再生
	//==========================================================

	// 通常エフェクト
	void Play(
		EffectType type,
		const Math::Vector3& pos);

	// 方向付きエフェクト
	void Play(
		EffectType type,
		const Math::Vector3& pos,
		const Math::Vector3& direction);

	// 必殺技エフェクト
	void Play(
		EffectType type,
		const Math::Vector3& pos,
		const Math::Vector3& direction,
		float hitInterval,
		int maxHitCount,
		float travelDistance);

	void PlayHeal(
		const std::shared_ptr<Player>& player,
		const Math::Vector3& pos);

private:

	EffectManager() = default;
	~EffectManager() = default;

	std::weak_ptr<CameraBase> m_wpCamera;
};