#include "UltimateEffect.h"

#include <Application/main.h>
#include <Application/GameObject/Camera/CameraBase.h>

//==============================================================
// 初期化
//==============================================================
void UltimateEffect::Init(
	const Math::Vector3& pos,
	const Math::Vector3& direction,
	float hitInterval,
	int maxHitCount,
	float travelDistance)
{
	EffectBase::Init(
		EffectType::Ultimate,
		pos
	);

	//==========================================================
	// 必殺技の総移動時間
	//
	// 5フレーム × 5回 = 25フレーム
	// 60FPS基準なので秒に変換
	//==========================================================
	m_effectDuration =
		(hitInterval * maxHitCount) / 60.0f;

	//==========================================================
	// 再生時間
	//==========================================================
	m_lifeTime = m_effectDuration;

	//==========================================================
	// 移動速度
	//
	// 総移動距離 ÷ 総移動時間
	//==========================================================
	if (m_effectDuration > 0.0f)
	{
		m_moveSpeed =
			travelDistance / m_effectDuration;
	}
	else
	{
		m_moveSpeed = 0.0f;
	}

	//==========================================================
	// 飛んでいく方向
	//==========================================================
	m_direction = direction;

	m_direction.y = 0.0f;

	if (m_direction.LengthSquared() > 0.0001f)
	{
		m_direction.Normalize();
	}
	else
	{
		m_direction = { 0, 0, 1 };
	}

	//==========================================================
	// ポリゴン生成
	//==========================================================
	m_polygon =
		std::make_shared<KdSquarePolygon>();

	m_polygon->SetMaterial(
		"Asset/Textures/Effect/UltimateSlash.png"
	);

	m_polygon->Set2DObject(false);
	m_polygon->SetScale(1.0f);
}

//==============================================================
// 更新
//==============================================================
void UltimateEffect::Update()
{
	//==========================================================
	// 基底更新
	//==========================================================
	EffectBase::Update();

	if (m_isFinished)
	{
		return;
	}

	//==========================================================
	// 移動
	//==========================================================
	const float dt =
		Application::Instance().GetDeltaTime();

	Math::Vector3 pos = GetPos();

	pos +=
		m_direction *
		m_moveSpeed *
		dt;

	SetPos(pos);
}

//==============================================================
// 描画
//==============================================================
void UltimateEffect::DrawEffect()
{
	if (!m_polygon) return;

	float angle = std::atan2f(
		m_direction.x,
		m_direction.z
	);

	// 板ポリの向きを90度回転
	angle -= DirectX::XM_PIDIV2;

	//==========================================================
	// 拡大
	//==========================================================
	Math::Matrix mat =
		Math::Matrix::CreateScale(2.0f);

	mat *= Math::Matrix::CreateRotationY(angle);

	//==========================================================
	// 位置
	//
	// プレイヤーの足元ではなく、
	// 少し上に浮かせて表示
	//==========================================================
	Math::Vector3 pos = GetPos();

	pos.y += 1.0f;

	mat.Translation(pos);

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(
		*m_polygon,
		mat,
		Math::Color(
			1.0f,
			1.0f,
			1.0f,
			1.0f
		)
	);
}