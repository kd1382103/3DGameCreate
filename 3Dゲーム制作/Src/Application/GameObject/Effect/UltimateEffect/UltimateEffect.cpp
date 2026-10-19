
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

	//==========================================================
	// 進行方向を基準にした直交する2枚の斬撃
	//==========================================================
	Math::Vector3 forward = m_direction;
	forward.y = 0.0f;

	if (forward.LengthSquared() > 0.0001f)
	{
		forward.Normalize();
	}
	else
	{
		forward = { 0.0f, 0.0f, 1.0f };
	}

	const Math::Vector3 up =
	{ 0.0f, 1.0f, 0.0f };

	// 進行方向に対する左右方向
	Math::Vector3 right = up.Cross(forward);

	if (right.LengthSquared() > 0.0001f)
	{
		right.Normalize();
	}
	else
	{
		right = { 1.0f, 0.0f, 0.0f };
	}

	//==========================================================
	// 縦斬撃の回転
	// X軸 = 進行方向
	// Y軸 = 上方向
	// Z軸 = 法線方向
	//==========================================================
	Math::Vector3 verticalNormal =
		forward.Cross(up);

	verticalNormal.Normalize();

	Math::Matrix verticalRotation =
		Math::Matrix::Identity;

	verticalRotation.Right(forward);
	verticalRotation.Up(up);
	verticalRotation.Backward(verticalNormal);

	//==========================================================
	// 横斬撃の回転
	// X軸 = 進行方向
	// Y軸 = 左右方向
	// Z軸 = 法線方向
	//==========================================================
	Math::Vector3 horizontalNormal =
		forward.Cross(right);

	horizontalNormal.Normalize();

	Math::Matrix horizontalRotation =
		Math::Matrix::Identity;

	horizontalRotation.Right(forward);
	horizontalRotation.Up(right);
	horizontalRotation.Backward(horizontalNormal);

	//==========================================================
	// 斬撃の描画位置
	//==========================================================
	Math::Vector3 pos = GetPos();
	pos.y += 1.0f;

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

		float scaleX =
			m_width * (0.65f + ratio * 0.2f);

		float scaleY =
			m_height * (0.65f + ratio * 0.2f);

		Math::Vector3 trailPos =
			m_trailPositions[i];

		trailPos.y += 1.0f;

		float alpha =
			0.06f +
			0.12f * (1.0f - ratio);

		// 縦向きの残像
		Math::Matrix trailMat =
			Math::Matrix::CreateScale(
				scaleX,
				scaleY,
				1.0f
			);

		trailMat *= verticalRotation;
		trailMat.Translation(trailPos);

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

		// 横向きの残像
		Math::Matrix horizontalTrailMat =
			Math::Matrix::CreateScale(
				scaleX,
				scaleY,
				1.0f
			);

		horizontalTrailMat *= horizontalRotation;
		horizontalTrailMat.Translation(trailPos);

		KdShaderManager::Instance()
			.m_StandardShader.DrawPolygon(
				*m_polygon,
				horizontalTrailMat,
				Math::Color(
					0.15f,
					0.85f,
					1.0f,
					alpha
				)
			);
	}

	//==========================================================
	// 縦向き：外側の青白い大きな斬撃
	//==========================================================
	Math::Matrix outerMat =
		Math::Matrix::CreateScale(
			m_width * 1.15f,
			m_height * 1.1f,
			1.0f
		);

	outerMat *= verticalRotation;
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
	// 縦向き：内側の明るいシアンの斬撃
	//==========================================================
	Math::Matrix innerMat =
		Math::Matrix::CreateScale(
			m_width * 0.72f,
			m_height * 0.85f,
			1.0f
		);

	innerMat *= verticalRotation;
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
	// 縦向き：元の斬撃本体
	//==========================================================
	Math::Matrix mat =
		Math::Matrix::CreateScale(
			m_width,
			m_height,
			1.0f
		);

	mat *= verticalRotation;
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

	//==========================================================
	// 横向き：外側の青白い大きな斬撃
	//==========================================================
	Math::Matrix horizontalOuterMat =
		Math::Matrix::CreateScale(
			m_width * 1.15f,
			m_height * 1.1f,
			1.0f
		);

	horizontalOuterMat *= horizontalRotation;
	horizontalOuterMat.Translation(pos);

	KdShaderManager::Instance()
		.m_StandardShader.DrawPolygon(
			*m_polygon,
			horizontalOuterMat,
			Math::Color(
				0.1f,
				0.65f,
				1.0f,
				0.45f
			)
		);

	//==========================================================
	// 横向き：内側の明るいシアンの斬撃
	//==========================================================
	Math::Matrix horizontalInnerMat =
		Math::Matrix::CreateScale(
			m_width * 0.72f,
			m_height * 0.85f,
			1.0f
		);

	horizontalInnerMat *= horizontalRotation;
	horizontalInnerMat.Translation(pos);

	KdShaderManager::Instance()
		.m_StandardShader.DrawPolygon(
			*m_polygon,
			horizontalInnerMat,
			Math::Color(
				0.45f,
				1.0f,
				1.0f,
				0.9f
			)
		);

	//==========================================================
	// 横向き：白い斬撃本体
	//==========================================================
	Math::Matrix horizontalMat =
		Math::Matrix::CreateScale(
			m_width,
			m_height,
			1.0f
		);

	horizontalMat *= horizontalRotation;
	horizontalMat.Translation(pos);

	KdShaderManager::Instance()
		.m_StandardShader.DrawPolygon(
			*m_polygon,
			horizontalMat,
			Math::Color(
				1.0f,
				1.0f,
				1.0f,
				1.0f
			)
		);
}
