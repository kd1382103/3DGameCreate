#include "Player.h"
#include<Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include<Application/Scene/SceneManager.h>
void Player::Init()
{
	if (!m_model)
	{
		m_model = std::make_shared<KdModelWork>();
		m_model->SetModelData("Asset/Models/player/manModel/Player.gltf");


		//3Dアニメーションの描画
		auto anim = m_model->GetAnimation(0);
		m_animator.SetAnimation(anim, true); // ループ再生

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	}
}

void Player::Update()
{
	//================================================================================
	// カメラの回転行列を取得
	//================================================================================
	Math::Matrix camRotMat;

	//存在しているかどうか
	if (m_wpCamera.expired() == false)
	{
		camRotMat = m_wpCamera.lock()->GetRotationYMatrix();
	}

	float moveSpeed = 0.1f;
	float runSpeed = 0.2f;
	bool isMoving = false;

	//================================================================================
	// 移動処理
	//================================================================================

	m_dir = Math::Vector3::Zero;

	bool isRunning = false;

	if (!m_isLanding && !m_isAttacking)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			isRunning = true;
			moveSpeed = runSpeed;
		}

		if (GetAsyncKeyState('W') & 0x8000) { m_dir += {0, 0, 1}; }
		if (GetAsyncKeyState('S') & 0x8000) { m_dir += {0, 0, -1}; }
		if (GetAsyncKeyState('A') & 0x8000) { m_dir += {-1, 0, 0}; }
		if (GetAsyncKeyState('D') & 0x8000) { m_dir += {1, 0, 0}; }
	}

	// ★ 実際に移動方向があるかどうかで判定する
	isMoving = (m_dir.LengthSquared() > 0.0001f);
	if (isMoving)
	{
		m_dir = Math::Vector3::TransformNormal(m_dir, camRotMat);
	}
	m_dir.Normalize();

	// ★ 着地アニメ中は移動方向を完全にゼロにする
	if (m_isLanding && !m_isAttacking)
	{
		m_dir = { 0,0,0 };
		isMoving = false;
	}


	//================================================================================
	// ジャンプ処理
	//================================================================================

	if (!m_isJumping && (GetAsyncKeyState(VK_SPACE) & 0x8000))
	{
		m_isJumping = true;			// ジャンプ開始
		m_gravity = -0.2f;			// 重力反転
		m_nowAnimIndex = 16;		// Jump_Start
		m_animator.SetAnimation(m_model->GetAnimation(16), false);
	}

	m_gravity += 0.005f;         // 重力加算
	m_nowPos.y -= m_gravity;     // y位置更新（放物線）

	// --- アニメーション切り替え ---
	if (m_isJumping)
	{
		// 上昇中
		if (m_isJumping && m_gravity < 0.0f)
		{
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
				m_animator.SetAnimation(m_model->GetAnimation(15), false);
			}
		}
	}
	else
	{
		m_isJumping = false;

		// 着地の強さを判定
		bool isHardLanding = (m_gravity >= 0.4f);

		m_gravity = 0.0f;

		if (isHardLanding)
		{
			// ★ 高所落下 → 着地硬直あり
			m_nowAnimIndex = 14; // Jump_Land
			m_animator.SetAnimation(m_model->GetAnimation(14), false);

			m_isLanding = true;  // ← 着地硬直フラグ
			return;              // ← Idle に上書きされないように止める
		}
		else
		{
			//アニメーションの都合で、着地硬直なしの時は、着地アニメーションを再生しない（現状）

			m_isLanding = false; // ← 着地硬直なし

			// return しない → そのまま Walk/Run に移行できる
		}
	

		if (m_animator.IsAnimationEnd())
		{
			m_isLanding = false;
		}

		if (!m_isJumping && !m_isLanding && !m_isAttacking)
		{
			int nextAnim = isRunning ? 36 : (isMoving ? 41 : 9);
			if (nextAnim != m_nowAnimIndex)
			{
				m_nowAnimIndex = nextAnim;
				m_animator.SetAnimation(m_model->GetAnimation(nextAnim), true);
			}
		}
	}
	//================================================================================
	//　攻撃処理
	//================================================================================

	//現状、アニメーションのみ当たり判定等は後ほど実装
	if (!m_isAttacking && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		m_isAttacking = true;

		m_nowAnimIndex = 39;
		m_animator.SetAnimation(m_model->GetAnimation(39), false);
	}

	// 攻撃アニメ終了チェック
	if (m_isAttacking && m_animator.IsAnimationEnd())
	{
		m_isAttacking = false;
	}


	//================================================================================
	// アニメーション処理
	//================================================================================
	float animSpeed = isRunning ? 1.5f : 1.0f;
	m_animator.AdvanceTime(m_model->WorkNodes(), animSpeed);

	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}

	//================================================================================
	// キャラ回転（自然な向き変更）
	//================================================================================

	// 移動している時だけ向きを変える
	if (isMoving)
	{
		Math::Vector3 nowDir = m_mWorld.Forward();   // 現在の向き
		Math::Vector3 targetDir = m_dir;              // 移動方向

		// 正規化（安全）
		nowDir.Normalize();
		targetDir.Normalize();

		// 範囲を制限したうえで内積を求める
		float dot = std::clamp(nowDir.Dot(targetDir), -1.0f, 1.0f);

		// 差分角度（ラジアン）
		float angle = acos(dot);

		// 外積で符号判定
		Math::Vector3 cross = nowDir.Cross(targetDir);
		if (cross.y < 0) angle = -angle;


		// ★ 回転速度を制限（自然な向き変更の核心）
		const float rotSpeed = DirectX::XMConvertToRadians(5.0f); // 1フレームの回転速度上限
		angle = std::clamp(angle, -rotSpeed, rotSpeed);

		// ★ 現在角度に加算（これが一番重要）
		m_angleY += angle;
	}

	m_nowPos += m_dir * moveSpeed;

	//======================================
	// ワールド行列の更新
	//======================================
	
	// 回転行列
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(m_angleY);
	
	// 平行移動
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_nowPos);

	// ワールド行列
	m_mWorld = rotMat * transMat;

	//Math::Vector3 forward = m_mWorld.Forward();
	//m_pDebugWire->AddDebugLine(m_nowPos, m_nowPos + forward, { 1,0,0,1 }); // 赤線で前方向

	//===============================================================================
	// LogWindowに表示(発表時は非表示)
	//===============================================================================

	KdDebugGUI::Instance().ClearLog();

	//アニメーションの番号一覧をLogWindowに表示
	//for (int i = 0; ; i++)
	//{
	//	auto anim = m_model->GetAnimation(i);
	//	if (!anim) break; // 取得できなくなったら終了

	//	KdDebugGUI::Instance().AddLog("%d : %s\n", i, anim->m_name.c_str());
	//}

	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.x);
	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.z);
	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.y);

	KdDebugGUI::Instance().AddLog("Gravity : %f\n", m_gravity);
	KdDebugGUI::Instance().AddLog("m_isJumping : %s\n", m_isJumping ? "true" : "false");
	KdDebugGUI::Instance().AddLog("m_isLanding : %s\n", m_isLanding ? "true" : "false");
	KdDebugGUI::Instance().AddLog("m_isAttacking : %s\n", m_isAttacking ? "true" : "false");
	KdDebugGUI::Instance().AddLog("%f\n", m_angleY);


}

void Player::PostUpdate()
{

	//================================================================================
	//	当たり判定
	//================================================================================

	//=======================================================================================================================
	// カプセル判定
	//=======================================================================================================================
	
	//地面
	{
		float maxOverlap = 0.0f;
		Math::Vector3 hitDir = Math::Vector3::Zero;
		bool hit = false;

		KdCollider::CapsuleInfo capsule;
		capsule.m_type = KdCollider::TypeGround;
		capsule.m_radius = 0.3f;

		capsule.m_start = m_nowPos;
		capsule.m_start.y += 0.3f;

		capsule.m_end = m_nowPos;
		capsule.m_end.y += 1.5f;

		m_pDebugWire->AddDebugCapsule(capsule.m_start, capsule.m_end, capsule.m_radius, {1,0,0,1});

		std::list<KdCollider::CollisionResult> retCapsuleList;
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects(capsule, &retCapsuleList);
		}

		for (auto& ret : retCapsuleList)
		{
			if (ret.m_overlapDistance > maxOverlap)
			{
				maxOverlap = ret.m_overlapDistance;
				hitDir = ret.m_hitDir;
				hit = true;
			}
		}

		if (hit)
		{
			float pushUp = hitDir.y * maxOverlap;
			if (pushUp > 0.0f)
			{
				m_nowPos.y += pushUp;
				m_gravity = 0.0f;
				m_isJumping = false;
			}
		}
		else
		{
			if (!m_isJumping)
			{
				m_isJumping = true;
			}
		}
	}

	//壁
	{
		float maxOverlap = 0.0f;
		Math::Vector3 hitDir = Math::Vector3::Zero;
		bool hit = false;

		KdCollider::CapsuleInfo capsule;
		capsule.m_type = KdCollider::TypeBump;
		capsule.m_radius = 0.4f;

		capsule.m_start = m_nowPos;
		capsule.m_start.y += 0.8f;
		capsule.m_end = m_nowPos;
		capsule.m_end.y += 1.2f;


		std::list<KdCollider::CollisionResult> retCapsuleList;
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects(capsule, &retCapsuleList);
		}

		for (auto& ret : retCapsuleList)
		{
			if (ret.m_overlapDistance > maxOverlap)
			{
				maxOverlap = ret.m_overlapDistance;

				hitDir = ret.m_hitDir;
				hitDir.y =0;

				hitDir.Normalize();
				hit = true;
			}
		}

		if (hit)
		{
			// ★ 押し返し量を少し強める（安全係数）
			const float pushStrength = 1.2f;
			m_nowPos += hitDir * (maxOverlap * pushStrength);
		}
	}
}

void Player::DrawLit()
{	if (!m_model) { return; }

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model,m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	if (!m_model) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
