#include "EffectManager.h"

#include <Application/GameObject/Effect/HitEffect/HitEffect.h>
#include <Application/GameObject/Effect/SparkEffect/SparkEffect.h>

#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/Scene/SceneManager.h>

//==============================================================
// 通常エフェクト再生
//==============================================================
void EffectManager::Play(
	EffectType type,
	const Math::Vector3& pos)
{
	switch (type)
	{
	case EffectType::Hit:
	{
		auto effect =
			std::make_shared<HitEffect>();

		effect->Init(pos);

		//======================================================
		// カメラ設定
		//======================================================
		auto camera =
			m_wpCamera.lock();

		if (camera)
		{
			effect->SetCamera(camera);
		}

		//======================================================
		// シーンへ追加
		//======================================================
		SceneManager::Instance().AddObject(effect);

		break;
	}

	default:
		break;
	}
}


//==============================================================
// 方向付きエフェクト再生
//==============================================================
void EffectManager::Play(
	EffectType type,
	const Math::Vector3& pos,
	const Math::Vector3& direction)
{
	switch (type)
	{
	case EffectType::Spark:
	{
		auto effect =
			std::make_shared<SparkEffect>();

		effect->Init(
			pos,
			direction
		);

		//======================================================
		// カメラ設定
		//======================================================
		auto camera =
			m_wpCamera.lock();

		if (camera)
		{
			effect->SetCamera(camera);
		}

		//======================================================
		// シーンへ追加
		//======================================================
		SceneManager::Instance().AddObject(effect);

		break;
	}

	default:
		break;
	}
}