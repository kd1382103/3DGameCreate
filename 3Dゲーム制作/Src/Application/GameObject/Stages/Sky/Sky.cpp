#include "Sky.h"
#include <Application/main.h>
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
	// フレーム倍率
	// 60FPSを基準にする
	//---------------------------------------
	float frameScale =
		Application::Instance().GetFPSController().GetFrameScale();

	//---------------------------------------
	// 空をゆっくり回転
	//---------------------------------------
	m_rotation += m_rotationSpeed * frameScale;

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