#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>
#include <Application/GameObject/Effect/EffectType/EffectType.h>

class CameraBase;

//==============================================================
// エフェクト基底クラス
//==============================================================
class EffectBase : public BaseObject
{
public:

	EffectBase() = default;
	virtual ~EffectBase() = default;

	virtual void Init(EffectType type, const Math::Vector3& pos);
	void Update() override;
	void DrawLit() override {}

	//==========================================================
	// カメラ設定
	//==========================================================
	void SetCamera(
		const std::shared_ptr<CameraBase>& camera)
	{
		m_wpCamera = camera;
	}

protected:
	//==========================================================
	// ビルボード行列取得
	//==========================================================
	Math::Matrix GetBillboardMatrix() const;

	//==========================================================
	// エフェクト種類
	//==========================================================
	EffectType m_type = EffectType::Hit;

	//==========================================================
	// 再生時間
	//==========================================================
	float m_lifeTime = 1.0f;
	float m_nowTime = 0.0f;

	//==========================================================
	// 終了フラグ
	//==========================================================
	bool m_isFinished = false;

	//==========================================================
	// カメラ
	//==========================================================
	std::weak_ptr<CameraBase> m_wpCamera;
};