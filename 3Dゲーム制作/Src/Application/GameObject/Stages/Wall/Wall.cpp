#include "Wall.h"

void Wall::Init()
{
	if (!m_model)
	{
		m_model = std::make_shared<KdModelData>();
		m_model->Load("Asset/Models/Stages/wall.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Wall", m_model, KdCollider::TypeBump);
	}
}

void Wall::DrawUnLit()
{
	if (!m_model) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model);
}
