#include "Player.h"

void Player::Init()
{
	m_model = std::make_shared<KdModelData>();
	m_model->Load("Asset/Models/player/player.gltf");
}

void Player::Update()
{
	float moveSpeed = 0.5f;
	if (GetAsyncKeyState('W') & 0x8000) { m_nowPos.z += moveSpeed; }
	if (GetAsyncKeyState('S') & 0x8000) { m_nowPos.z -= moveSpeed; }
	if (GetAsyncKeyState('A') & 0x8000) { m_nowPos.x -= moveSpeed; }
	if (GetAsyncKeyState('D') & 0x8000) { m_nowPos.x += moveSpeed; }

	//KdDebugGUI::Instance().ClearLog();
	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.x);
	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.z);


}

void Player::PostUpdate()
{
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_nowPos);
	m_mWorld = transMat;
}

void Player::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model,m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
