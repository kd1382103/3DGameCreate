#include "Sky.h"

void Sky::Init()
{
	if (!m_model)
	{
		m_model = std::make_shared<KdModelData>();
		m_model->Load("Asset/Models/stages/Sky/Sky.gltf");
	}
}

void Sky::Update()
{
	//---------------------------------------
	// 空をゆっくり回転
	//---------------------------------------
	m_rotation += m_rotationSpeed;

	// 360度を超えたら戻す
	if (m_rotation >= DirectX::XM_2PI)
	{
		m_rotation -= DirectX::XM_2PI;
	}

	Math::Matrix Trans = Math::Matrix::CreateTranslation(m_pos);

	Math::Matrix Rot = Math::Matrix::CreateRotationY(m_rotation);

	m_mWorld = Rot * Trans;
}

void Sky::DrawUnLit()
{
	if (!m_model) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model,m_mWorld);
}