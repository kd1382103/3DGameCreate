#include "Stage.h"

void Stage::Init()
{
	m_model = std::make_shared<KdModelData>();
	m_model->Load("Asset/Models/stage/Stage.gltf");
}

void Stage::DrawUnLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model);
}
