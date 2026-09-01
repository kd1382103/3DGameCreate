#include "Stage.h"

void Stage::Init()
{
	if (!m_model)
	{
		m_model = std::make_shared<KdModelData>();
		m_model->Load("Asset/Models/stages/Stage2/StageMap/stageMap.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Stage", m_model, KdCollider::TypeGround | KdCollider::TypeBump);
	}
}

void Stage::DrawLit()
{
	if (!m_model) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model);
}