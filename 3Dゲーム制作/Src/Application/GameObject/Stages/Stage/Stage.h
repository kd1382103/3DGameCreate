#pragma once

#include <Application/GameObject/BaseObject/BaseObject.h>

//=======================================
// Light.gltf の配置情報
//=======================================
struct LightData
{
	Math::Vector3 pos = { 0, 0, 0 };

	// X、Y、Z軸の回転角度（度数法）
	Math::Vector3 rot = { 0, 0, 0 };
};

class Stage : public BaseObject
{
public:
	Stage() {}
	~Stage() override { Release(); }

	void Init() override;
	void Update() override;
	void DrawLit() override;
	void DrawBright() override;

	// Light.gltf の位置・回転角度を設定
	void SetLightPositions(
		const std::vector<LightData>& lights
	);

	// プレイヤー周辺のコーンライトだけを更新する（動的カリング）
	void UpdateConeLights(const Math::Vector3& playerPos);

	// ボスエリア専用コーンライト
	void SetBossConeLight(const LightData& light);

private:
	std::shared_ptr<KdModelData> m_model;
	std::shared_ptr<KdModelData> m_lightModel;

	// 全ライトのマスターデータ（動的カリング用）
	std::vector<LightData> m_allLights;

	// 発光モデルごとの位置・角度
	std::vector<LightData> m_lightPositions;

	// 発光モデルの大きさ
	float m_lightScale = 0.2f;
};