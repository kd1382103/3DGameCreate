#include "HitEffect.h"
#include <Application/main.h>
#include <Application/GameObject/Camera/CameraBase.h>
//==============================================================
// 初期化
//==============================================================
void HitEffect::Init(const Math::Vector3& pos)
{
	EffectBase::Init(
		EffectType::Hit,
		pos
	);

	//==========================================================
	// 再生時間
	//==========================================================
	m_lifeTime = 0.35f;

	//==========================================================
	// ポリゴン生成
	//==========================================================
	m_polygon =
		std::make_shared<HitEffectPolygon>();

	m_polygon->SetVertices(1.0f);

	//==========================================================
	// 粒子生成
	//==========================================================
	CreateParticles();
}

//==============================================================
// 粒子生成
//==============================================================
void HitEffect::CreateParticles()
{
	m_particles.clear();

	const int particleCount = 10;

	for (int i = 0; i < particleCount; ++i)
	{
		HitParticle particle;

		particle.pos = GetPos();

		//======================================================
		// ランダム方向
		//======================================================
		float x =
			(static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;

		float y =
			(static_cast<float>(rand()) / RAND_MAX) * 1.5f + 0.2f;

		float z =
			(static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;

		Math::Vector3 dir(x, y, z);

		if (dir.LengthSquared() > 0.0001f)
		{
			dir.Normalize();
		}

		particle.velocity = dir * 3.0f;

		particle.size = 0.08f;

		m_particles.push_back(particle);
	}
}

//==============================================================
// 更新
//==============================================================
void HitEffect::Update()
{
	EffectBase::Update();

	const float dt =
		Application::Instance().GetDeltaTime();

	for (auto& particle : m_particles)
	{
		//======================================================
		// 移動
		//======================================================
		particle.pos +=
			particle.velocity * dt;

		//======================================================
		// 重力
		//======================================================
		particle.velocity.y -=
			8.0f * dt;
	}
}

//==============================================================
// エフェクト描画
//==============================================================
void HitEffect::DrawEffect()
{
	if (!m_polygon) { return; }
	auto camera = m_wpCamera.lock();

	if (!camera)
	{
		return;
	}

	//==========================================================
	// 血しぶき描画
	//==========================================================
	for (const auto& particle : m_particles)
	{
		Math::Matrix mat = Math::Matrix::CreateScale(particle.size);
		mat *= camera->GetBillboardMatrix();
		mat.Translation(particle.pos);

		KdShaderManager::Instance()
			.m_StandardShader
			.DrawPolygon(
				*m_polygon,
				mat,
				Math::Color(
					1.0f,
					0.0f,
					0.0f,
					1.0f
				)
			);
	}
}

//==============================================================
// 頂点設定
//==============================================================
void HitEffectPolygon::SetVertices(float size)
{
	m_vertices.clear();

	float half =
		size * 0.5f;

	Vertex v0;
	v0.pos = { -half,  half, 0.0f };
	v0.UV = { 0.0f, 0.0f };

	Vertex v1;
	v1.pos = { half,  half, 0.0f };
	v1.UV = { 1.0f, 0.0f };

	Vertex v2;
	v2.pos = { -half, -half, 0.0f };
	v2.UV = { 0.0f, 1.0f };

	Vertex v3;
	v3.pos = { half, -half, 0.0f };
	v3.UV = { 1.0f, 1.0f };

	m_vertices.push_back(v0);
	m_vertices.push_back(v1);
	m_vertices.push_back(v2);

	m_vertices.push_back(v2);
	m_vertices.push_back(v1);
	m_vertices.push_back(v3);
}