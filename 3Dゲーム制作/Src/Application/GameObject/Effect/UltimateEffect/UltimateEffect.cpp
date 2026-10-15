
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

	// 必殺技の総移動時間
	m_effectDuration =
		(hitInterval * maxHitCount) / 60.0f;

	m_lifeTime = m_effectDuration;

	// 移動速度
	if (m_effectDuration > 0.0f)
	{
		m_moveSpeed =
			travelDistance / m_effectDuration;
	}
	else
	{
		m_moveSpeed = 0.0f;
	}

	// 飛んでいく方向
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

	// 斬撃ポリゴン生成
	m_polygon =
		std::make_shared<KdSquarePolygon>();

	m_polygon->SetMaterial(
		"Asset/Textures/Effect/UltimateSlash.png"
	);

	m_polygon->Set2DObject(false);
	m_polygon->SetScale(1.0f);

	// 残像初期化
	m_trailPositions.clear();
	m_trailTimer = 0.0f;
}

//==============================================================
// 更新
//==============================================================
void UltimateEffect::Update()
{
	EffectBase::Update();

	if (m_isFinished)
	{
		return;
	}

	const float dt =
		Application::Instance().GetDeltaTime();

	// 移動
	Math::Vector3 pos = GetPos();

	pos +=
		m_direction *
		m_moveSpeed *
		dt;

	SetPos(pos);

	// 残像位置を記録
	m_trailTimer += dt;

	if (m_trailTimer >= kTrailInterval)
	{
		m_trailTimer = 0.0f;

		m_trailPositions.insert(
			m_trailPositions.begin(),
			GetPos()
		);

		if (m_trailPositions.size() > kTrailCount)
		{
			m_trailPositions.pop_back();
		}
	}
}

//==============================================================
// 描画
//==============================================================
void UltimateEffect::DrawEffect()
{
	if (!m_polygon)
	{
		return;
	}

	// 進行方向に合わせて回転
	float angle = std::atan2f(
		m_direction.x,
		m_direction.z
	);

	angle -= DirectX::XM_PIDIV2;

	//==========================================================
	// 残像描画
	//==========================================================
	for (int i = static_cast<int>(m_trailPositions.size()) - 1;
		i >= 0;
		--i)
	{
		// 古い残像ほど小さく、薄くする
		float ratio =
			static_cast<float>(i) /
			static_cast<float>(kTrailCount);

		float scaleX = m_width * (0.65f + ratio * 0.2f);
		float scaleY = m_height * (0.65f + ratio * 0.2f);

		Math::Matrix trailMat =
			Math::Matrix::CreateScale(
				scaleX,
				scaleY,
				1.0f
			);

		trailMat *=
			Math::Matrix::CreateRotationY(angle);

		Math::Vector3 trailPos =
			m_trailPositions[i];

		trailPos.y += 1.0f;

		trailMat.Translation(trailPos);

		float alpha =
			0.06f +
			0.12f * (1.0f - ratio);

		KdShaderManager::Instance()
			.m_StandardShader.DrawPolygon(
				*m_polygon,
				trailMat,
				Math::Color(
					0.15f,
					0.85f,
					1.0f,
					alpha
				)
			);
	}

	//==========================================================
	// 本体の外側：青白い大きな斬撃
	//==========================================================
	Math::Matrix outerMat =
		Math::Matrix::CreateScale(
			m_width * 1.15f,
			m_height * 1.1f,
			1.0f
		);

	outerMat *=
		Math::Matrix::CreateRotationY(angle);

	Math::Vector3 pos = GetPos();
	pos.y += 1.0f;

	outerMat.Translation(pos);

	KdShaderManager::Instance()
		.m_StandardShader.DrawPolygon(
			*m_polygon,
			outerMat,
			Math::Color(
				0.1f,
				0.65f,
				1.0f,
				0.45f
			)
		);

	//==========================================================
	// 本体の内側：明るいシアンの斬撃
	//==========================================================
	Math::Matrix innerMat =
		Math::Matrix::CreateScale(
			m_width * 0.72f,
			m_height * 0.85f,
			1.0f
		);

	innerMat *=
		Math::Matrix::CreateRotationY(angle);

	innerMat.Translation(pos);

	KdShaderManager::Instance()
		.m_StandardShader.DrawPolygon(
			*m_polygon,
			innerMat,
			Math::Color(
				0.45f,
				1.0f,
				1.0f,
				0.9f
			)
		);

	//==========================================================
	// 本体：元の斬撃
	//==========================================================
	Math::Matrix mat =
		Math::Matrix::CreateScale(
			m_width,
			m_height,
			1.0f
		);

	mat *=
		Math::Matrix::CreateRotationY(angle);

	mat.Translation(pos);

	KdShaderManager::Instance()
		.m_StandardShader.DrawPolygon(
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
