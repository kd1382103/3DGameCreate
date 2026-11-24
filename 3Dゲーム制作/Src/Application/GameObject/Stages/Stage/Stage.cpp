#include "Stage.h"
#include <algorithm>

void Stage::Init()
{
	//=======================================
	// ステージモデル
	//=======================================
	if (!m_model)
	{
		m_model = std::make_shared<KdModelData>();

		m_model->Load(
			"Asset/Models/stages/Stage2/StageMap/stageMap.gltf"
		);

		// ステージの当たり判定
		m_pCollider = std::make_unique<KdCollider>();

		m_pCollider->RegisterCollisionShape(
			"Stage",
			m_model,
			KdCollider::TypeGround | KdCollider::TypeBump
		);
	}

	//=======================================
	// 発光部分モデル
	//=======================================
	if (!m_lightModel)
	{
		m_lightModel = std::make_shared<KdModelData>();

		m_lightModel->Load(
			"Asset/Models/stages/Light/Light.gltf"
		);
	}
}

void Stage::SetLightPositions(
	const std::vector<LightData>& lights)
{
	//=======================================
	// 発光モデルの位置・角度を保存
	//=======================================
	m_lightPositions = lights;
	m_allLights = lights;

	// 初回は原点付近（プレイヤー初期位置）で更新
	UpdateConeLights(Math::Vector3::Zero);
}

void Stage::UpdateConeLights(const Math::Vector3& playerPos)
{
	if (m_allLights.empty())
	{
		return;
	}

	//=======================================
	// プレイヤーに近いライトを抽出
	//=======================================
	std::vector<std::pair<float, size_t>> distList;
	distList.reserve(m_allLights.size());

	// プレイヤーから何m以内のライトを有効対象にするか
	const float cullRadius = 70.0f;
	const float maxCullDistSq = cullRadius * cullRadius;

	for (size_t i = 0; i < m_allLights.size(); ++i)
	{
		float distSq = (m_allLights[i].pos - playerPos).LengthSquared();

		if (distSq <= maxCullDistSq)
		{
			distList.emplace_back(distSq, i);
		}
	}

	// 近い順にソート
	std::sort(distList.begin(), distList.end());

	// 同時に有効にする最大ライト数
	const size_t maxActiveLights = 20;
	const size_t count = std::min(distList.size(), maxActiveLights);

	std::vector<ConeLight> coneLights;
	coneLights.reserve(count);

	for (size_t i = 0; i < count; ++i)
	{
		const auto& data = m_allLights[distList[i].second];

		ConeLight light{};
		light.enable = 1;
		light.pos = data.pos;
		light.dir = { 0.0f, -1.0f, 0.0f };
		light.angle = DirectX::XMConvertToRadians(50.0f);

		// ボスエリアの高いライト(y=20)だけ照射距離を広くする
		light.range = (data.pos.y > 10.0f) ? 45.0f : 25.0f;

		light.color = { 1.0f, 1.0f, 1.0f };

		coneLights.push_back(light);
	}

	// コーンライトの登録
	KdShaderManager::Instance().WriteCBConeLights(
		coneLights
	);
}

void Stage::UpdateAllConeLights()
{
	if (m_allLights.empty())
	{
		return;
	}

	std::vector<ConeLight> coneLights;
	coneLights.reserve(m_allLights.size());

	for (const auto& data : m_allLights)
	{
		ConeLight light{};
		light.enable = 1;
		light.pos = data.pos;
		light.dir = { 0.0f, -1.0f, 0.0f };
		light.angle = DirectX::XMConvertToRadians(50.0f);
		light.range = (data.pos.y > 10.0f) ? 45.0f : 25.0f;
		light.color = { 1.0f, 1.0f, 1.0f };

		coneLights.push_back(light);
	}

	// 全コーンライトをシェーダーに登録
	KdShaderManager::Instance().WriteCBConeLights(
		coneLights
	);
}

void Stage::Update()
{}

void Stage::DrawLit()
{
	//=======================================
	// ステージ描画
	//=======================================
	if (m_model)
	{
		KdShaderManager::Instance()
			.m_StandardShader
			.DrawModel(*m_model);
	}
}

void Stage::DrawBright()
{
	if (!m_lightModel)
	{
		return;
	}

	//=======================================
	// 発光モデル描画
	//=======================================
	for (const auto& data : m_lightPositions)
	{
		// 拡大縮小
		Math::Matrix scaleMat =
			Math::Matrix::CreateScale(m_lightScale);

		// X、Y、Z軸の回転
		Math::Matrix rotMat =
			Math::Matrix::CreateRotationX(
				DirectX::XMConvertToRadians(data.rot.x))
			* Math::Matrix::CreateRotationY(
				DirectX::XMConvertToRadians(data.rot.y))
			* Math::Matrix::CreateRotationZ(
				DirectX::XMConvertToRadians(data.rot.z));

		// 座標
		Math::Matrix transMat =
			Math::Matrix::CreateTranslation(data.pos);

		// ワールド行列
		Math::Matrix worldMat =
			scaleMat * rotMat * transMat;

		// ブルーム用の発光描画
		KdShaderManager::Instance()
			.m_StandardShader
			.DrawModel(
				*m_lightModel,
				worldMat,
				Math::Color(1, 1, 1, 1),
				Math::Vector3(5, 5, 5)
			);
	}
}