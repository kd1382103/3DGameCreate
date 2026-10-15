#include "Stage.h"

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

	//=======================================
	// コーンライト設定
	// 位置・照射方向は従来どおり変更しない
	//=======================================
	std::vector<ConeLight> coneLights;

	coneLights.reserve(m_lightPositions.size());

	for (const auto& data : m_lightPositions)
	{
		ConeLight light{};

		light.enable = 1;

		// コーンライトの位置は従来の座標を使用
		light.pos = data.pos;

		// 照射方向は真下のまま
		light.dir = { 0.0f, -1.0f, 0.0f };

		light.angle =
			DirectX::XMConvertToRadians(50.0f);

		light.range = 70.0f;

		light.color = { 1.0f, 1.0f, 1.0f };

		coneLights.push_back(light);
	}

	// コーンライトの登録
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