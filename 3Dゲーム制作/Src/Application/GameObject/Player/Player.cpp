#include "Player.h"

void Player::Init()
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/player/manModel/Player.gltf");

	//3Dアニメーションの描画
	auto anim = m_model->GetAnimation(0);
	m_animator.SetAnimation(anim, true); // ループ再生

}

void Player::Update()
{
	float moveSpeed = 0.1f;
	float runSpeed = 1.0f;
	bool isMoving = false;

	// --- 移動入力 ---
	bool isRunning = false;

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		isRunning = true;
		moveSpeed = runSpeed;
	}

	if (GetAsyncKeyState('W') & 0x8000) { m_nowPos.z += moveSpeed; isMoving = true; }
	if (GetAsyncKeyState('S') & 0x8000) { m_nowPos.z -= moveSpeed; isMoving = true; }
	if (GetAsyncKeyState('A') & 0x8000) { m_nowPos.x -= moveSpeed; isMoving = true; }
	if (GetAsyncKeyState('D') & 0x8000) { m_nowPos.x += moveSpeed; isMoving = true; }

	// --- ジャンプ処理 ---
	if (!m_isJumping && (GetAsyncKeyState(VK_SPACE) & 0x8000))
	{
		m_isJumping = true;      // ★ ジャンプ開始
		m_gravity = -0.2f;
		m_nowAnimIndex = 16;     // Jump_Start
		m_animator.SetAnimation(m_model->GetAnimation(16), false);
	}

	m_gravity += 0.005f;         // 重力加算
	m_nowPos.y -= m_gravity;     // y位置更新（放物線）

	// --- アニメーション切り替え ---
	if (m_nowPos.y > 0.0f)
	{
		// ★ ジャンプ中
		if (m_gravity < 0.0f)
		{
			// 上昇
			if (m_nowAnimIndex != 16)
			{
				m_nowAnimIndex = 16;
				m_animator.SetAnimation(m_model->GetAnimation(16), false);
			}
		}
		else
		{
			// 下降
			if (m_nowAnimIndex != 15)
			{
				m_nowAnimIndex = 15;
				m_animator.SetAnimation(m_model->GetAnimation(15), true);
			}
		}
	}
	else
	{
		// ★ 着地
		m_nowPos.y = 0.0f;
		m_gravity = 0.0f;
		m_isJumping = false;     // ★ 着地したのでジャンプ可能に戻す

		int nextAnim = 9; // Idle

		if (isMoving)
		{
			if (isRunning) nextAnim = 36; // Run
			else nextAnim = 41;           // Walk
		}

		if (nextAnim != m_nowAnimIndex)
		{
			m_nowAnimIndex = nextAnim;
			m_animator.SetAnimation(m_model->GetAnimation(nextAnim), true);
		}
	}
	// --- アニメーション更新 ---
	m_animator.AdvanceTime(m_model->WorkNodes(), 1.0f);

	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}

	
	//===============================================================================
	// 
	// LogWindowに表示する
	// 
	//===============================================================================
	
	KdDebugGUI::Instance().ClearLog();

	////アニメーションの番号一覧をLogWindowに表示
	//for (int i = 0; ; i++)
	//{
	//	auto anim = m_model->GetAnimation(i);
	//	if (!anim) break; // 取得できなくなったら終了

	//	KdDebugGUI::Instance().AddLog("%d : %s\n", i, anim->m_name.c_str());
	//}

	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.x);
	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.z);
	KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.y);

}

void Player::PostUpdate()
{
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(180));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_nowPos);
	m_mWorld = rotMat * transMat;
}

void Player::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model,m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
