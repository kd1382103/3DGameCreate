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

	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_nowPos);
	m_mWorld = transMat;
}

void Player::PostUpdate()
{}

void Player::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model,m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
