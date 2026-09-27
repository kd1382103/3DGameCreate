#include "SwordTrail.h"

#include <Application/GameObject/Player/Player/Player.h>

void SwordTrail::Init()
{
	//==================================================
	// トレイルポリゴン生成
	//==================================================
	m_tPoly = std::make_shared<KdTrailPolygon>();

	//==================================================
	// トレイル画像
	//==================================================
	m_tPoly->SetMaterial(
		"Asset/Textures/Effect/SwordTrail.png"
	);

	//==================================================
	// 頂点をそのまま繋ぐ
	//==================================================
	m_tPoly->SetPattern(
		KdTrailPolygon::Trail_Pattern::eVertices
	);

	//==================================================
	// 軌跡の長さ
	//==================================================
	m_tPoly->SetLength(15);

	//==================================================
	// 初期状態
	//==================================================
	m_wasTrailActive = false;
}

void SwordTrail::Update()
{
	auto player = m_player.lock();

	if (!player)
	{
		return;
	}

	//==================================================
	// 剣軌跡OFF
	//==================================================
	if (!player->IsSwordTrailActive())
	{
		// 軌跡を完全に消す
		if (m_wasTrailActive)
		{
			m_tPoly->ClearPoints();
		}

		m_wasTrailActive = false;

		return;
	}

	//==================================================
	// 軌跡開始
	//==================================================
	if (!m_wasTrailActive)
	{
		m_tPoly->ClearPoints();

		m_wasTrailActive = true;
	}

	//==================================================
	// 剣の原点・剣先を取得
	//==================================================
	Math::Vector3 basePos =
		player->GetSwordBasePos();

	Math::Vector3 tipPos =
		player->GetSwordTipPos();

	//==================================================
	// 原点を追加
	//==================================================
	Math::Matrix baseMat =
		Math::Matrix::CreateTranslation(basePos);

	//==================================================
	// 剣先を追加
	//==================================================
	Math::Matrix tipMat =
		Math::Matrix::CreateTranslation(tipPos);

	//==================================================
	// KdTrailPolygonに追加
	//==================================================
	m_tPoly->AddPoint(baseMat);
	m_tPoly->AddPoint(tipMat);
}

void SwordTrail::DrawEffect()
{
	if (!m_tPoly)
	{
		return;
	}

	auto player = m_player.lock();

	if (!player)
	{
		return;
	}

	if (!player->IsSwordTrailActive())
	{
		return;
	}

	//==================================================
	// トレイル描画
	//==================================================
	KdShaderManager::Instance()
		.m_StandardShader
		.DrawPolygon(*m_tPoly);
}