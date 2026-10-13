
#include "BattleArea.h"

#include <algorithm>

void BattleArea::Init()
{
	m_areaCenter = Math::Vector3::Zero;

	m_halfWidth = 0;
	m_halfDepth = 0;
	m_barrierHeight = 12.0f;

	m_barrierActive = false;

	// バリアポリゴン
	m_barrierPoly = std::make_shared<KdSquarePolygon>();

	m_barrierPoly->SetMaterial(
		"Asset/Textures/Effect/BattleBarrier.png"
	);

	m_barrierPoly->Set2DObject(false);
	m_barrierPoly->SetScale(1.0f);
}

void BattleArea::Update()
{
	// 必要に応じて、後からバリアのアニメーションを追加する
}

void BattleArea::DrawLit()
{
	if (!m_barrierActive || !m_barrierPoly)
	{
		return;
	}

	auto& shader =
		KdShaderManager::Instance().m_StandardShader;

	// エリアの寸法
	const float width = m_halfWidth * 2.0f;
	const float depth = m_halfDepth * 2.0f;

	// 壁の中心の高さ
	const float centerY =
		m_areaCenter.y + m_barrierHeight * 0.5f;

	// テクスチャの色をそのまま使う
	const Math::Color color(
		1.0f,
		1.0f,
		1.0f,
		0.75f
	);

	//==========================================================
	// 各壁のワールド行列を作成して描画
	//==========================================================
	auto DrawWall =
		[&](const Math::Vector3& position,
			float wallWidth,
			float rotationY)
		{
			const Math::Matrix scaleMat =
				Math::Matrix::CreateScale(
					wallWidth,
					m_barrierHeight,
					1.0f
				);

			const Math::Matrix rotationMat =
				Math::Matrix::CreateRotationY(
					rotationY
				);

			const Math::Matrix translationMat =
				Math::Matrix::CreateTranslation(
					position
				);

			const Math::Matrix worldMat =
				scaleMat *
				rotationMat *
				translationMat;

			shader.DrawPolygon(
				*m_barrierPoly,
				worldMat,
				color
			);
		};

	//==========================================================
	// 前側（-Z）
	//==========================================================
	DrawWall(
		Math::Vector3(
			m_areaCenter.x,
			centerY,
			m_areaCenter.z - m_halfDepth
		),
		width,
		0.0f
	);

	//==========================================================
	// 後側（+Z）
	//==========================================================
	DrawWall(
		Math::Vector3(
			m_areaCenter.x,
			centerY,
			m_areaCenter.z + m_halfDepth
		),
		width,
		0.0f
	);

	//==========================================================
	// 左側（-X）
	//==========================================================
	DrawWall(
		Math::Vector3(
			m_areaCenter.x - m_halfWidth,
			centerY,
			m_areaCenter.z
		),
		depth,
		DirectX::XMConvertToRadians(90.0f)
	);

	//==========================================================
	// 右側（+X）
	//==========================================================
	DrawWall(
		Math::Vector3(
			m_areaCenter.x + m_halfWidth,
			centerY,
			m_areaCenter.z
		),
		depth,
		DirectX::XMConvertToRadians(90.0f)
	);
}

void BattleArea::SetArea(
	const Math::Vector3& center,
	float halfWidth,
	float halfDepth
)
{
	m_areaCenter = center;

	m_halfWidth = std::max(0.1f, halfWidth);
	m_halfDepth = std::max(0.1f, halfDepth);
}

void BattleArea::SetBarrierActive(bool active)
{
	m_barrierActive = active;
}

bool BattleArea::IsInsideArea(
	const Math::Vector3& pos
) const
{
	if (!m_barrierActive)
	{
		return true;
	}

	const float minX = m_areaCenter.x - m_halfWidth;
	const float maxX = m_areaCenter.x + m_halfWidth;

	const float minZ = m_areaCenter.z - m_halfDepth;
	const float maxZ = m_areaCenter.z + m_halfDepth;

	return
		pos.x >= minX &&
		pos.x <= maxX &&
		pos.z >= minZ &&
		pos.z <= maxZ;
}

Math::Vector3 BattleArea::ClampPosition(
	const Math::Vector3& pos
) const
{
	if (!m_barrierActive)
	{
		return pos;
	}

	Math::Vector3 result = pos;

	// プレイヤーがバリアの外に出ないように位置を補正
	result.x = std::clamp(
		result.x,
		m_areaCenter.x - m_halfWidth,
		m_areaCenter.x + m_halfWidth
	);

	result.z = std::clamp(
		result.z,
		m_areaCenter.z - m_halfDepth,
		m_areaCenter.z + m_halfDepth
	);

	return result;
}
