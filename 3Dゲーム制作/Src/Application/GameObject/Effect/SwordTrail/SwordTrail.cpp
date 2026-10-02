#include "SwordTrail.h"

#include <Application/GameObject/Player/Player/Player.h>
#include <Application/main.h>

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

	//==================================================
	// タイマー初期化
	//==================================================
	m_trailTimer = 0.0f;
}

void SwordTrail::Update()
{
	auto player = m_player.lock();

	if (!player)
	{
		return;
	}

	//==================================================
	// DeltaTime取得
	//==================================================
	float deltaTime = Application::Instance().GetDeltaTime();

	//==================================================
	// 剣軌跡OFF
	//==================================================
	if (!player->IsSwordTrailActive())
	{
		if (m_wasTrailActive)
		{
			m_tPoly->ClearPoints();
		}

		m_wasTrailActive = false;

		m_trailTimer = 0.0f;

		return;
	}

	//==================================================
	// 軌跡開始
	//==================================================
	if (!m_wasTrailActive)
	{
		m_tPoly->ClearPoints();

		m_wasTrailActive = true;

		m_trailTimer = 0.0f;
	}

	//==================================================
	// タイマー更新
	//==================================================
	m_trailTimer += deltaTime;

	//==================================================
	// 一定時間ごとに軌跡ポイント追加
	//==================================================
	if (m_trailTimer >= TrailInterval)
	{
		//==================================================
		// 剣の原点・剣先を取得
		//==================================================
		Math::Vector3 basePos =
			player->GetSwordBasePos();

		Math::Vector3 tipPos =
			player->GetSwordTipPos();

		//==================================================
		// 原点
		//==================================================
		Math::Matrix baseMat =
			Math::Matrix::CreateTranslation(basePos);

		//==================================================
		// 剣先
		//==================================================
		Math::Matrix tipMat =
			Math::Matrix::CreateTranslation(tipPos);

		//==================================================
		// KdTrailPolygonに追加
		//==================================================
		m_tPoly->AddPoint(baseMat);
		m_tPoly->AddPoint(tipMat);

		//==================================================
		// 余った時間を保持
		//==================================================
		m_trailTimer -= TrailInterval;
	}
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