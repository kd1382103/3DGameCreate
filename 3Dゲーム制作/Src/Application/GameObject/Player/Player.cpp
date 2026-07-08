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
	//================================================================================
	// 移動入力（WASD）
	//================================================================================
	m_dir = Math::Vector3::Zero;

	if (GetAsyncKeyState('W') & 0x8000) m_dir += {0, 0, 1};
	if (GetAsyncKeyState('S') & 0x8000) m_dir += {0, 0, -1};
	if (GetAsyncKeyState('A') & 0x8000) m_dir += {-1, 0, 0};
	if (GetAsyncKeyState('D') & 0x8000) m_dir += {1, 0, 0};

	m_moving = (m_dir.LengthSquared() > 0.0001f);
	if (!m_isJumping)
	{
		m_running = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
	}
	m_attacking = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);

	//================================================================================
	// 	カメラ方向へ変換
	//================================================================================
	if (m_moving && !m_wpCamera.expired())
	{
		m_dir = Math::Vector3::TransformNormal(m_dir, camRotMat);
		m_dir.Normalize();
	}

	//================================================================================
	//	ステート呼び出し
	//================================================================================

	switch (m_state)
	{
	case PlayerState::Idle:    UpdateIdle();    break;
	case PlayerState::Run:     UpdateRun();     break;
	case PlayerState::Attack:  UpdateAttack();  break;
	case PlayerState::Landing:  UpdateLanding();  break;
	case PlayerState::Fall:  UpdateFall();  break;
	}

	//================================================================================
	// アニメーション処理
	//================================================================================
	float animSpeed = m_running ? 1.5f : 1.0f;
	m_animator.AdvanceTime(m_model->WorkNodes(), animSpeed);

	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}

	//================================================================================
	// 重力（下降）
	//================================================================================
	m_gravity += 0.005f;
	m_nowPos.y -= m_gravity;

	//================================================================================
	// 落下判定呼び出し
	//================================================================================
	if (m_gravity > 0.01f)
	{
		m_isJumping = true;

		// ステートを落下へ
		if (m_state != PlayerState::Fall)
		{
			ChangeState(PlayerState::Fall);
		}
	}
	else
	{
		m_isJumping = false;
	}


	//	//デバック（プレイヤーの向いている方向）
	//	//Math::Vector3 forward = m_mWorld.Forward();
	//	//m_pDebugWire->AddDebugLine(m_nowPos, m_nowPos + forward, { 1,0,0,1 }); // 赤線で前方向
	//
	//	float t = m_animator.GetAnimeCurrentTime();
	//	m_attackAnimeTime = t;   // ← 一時保存
	//
	//	//===============================================================================
	//	// LogWindowに表示(発表時は非表示)
	//	//===============================================================================
	//
		KdDebugGUI::Instance().ClearLog();
	//
		////アニメーションの番号一覧をLogWindowに表示
		//for (int i = 0; ; i++)
		//{
		//	auto anim = m_model->GetAnimation(i);
		//	if (!anim) break; // 取得できなくなったら終了
	
		//	KdDebugGUI::Instance().AddLog("%d : %s\n", i, anim->m_name.c_str());
		//}
	//
	//	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.x);
	//	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.z);
	//	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.y);
	//
	//	//KdDebugGUI::Instance().AddLog("Gravity : %f\n", m_gravity);
	//	//KdDebugGUI::Instance().AddLog("m_isJumping : %s\n", m_isJumping ? "true" : "false");
	//	//KdDebugGUI::Instance().AddLog("m_isLanding : %s\n", m_isLanding ? "true" : "false");
	//	//KdDebugGUI::Instance().AddLog("m_isAttacking : %s\n", m_isAttacking ? "true" : "false");
	//
	//}
}

void Player::PostUpdate()
{

	//================================================================================
	//	当たり判定
	//================================================================================

	//========================
	// 地面（レイ判定）
	//========================
	{
		KdCollider::RayInfo ray;
		ray.m_type = KdCollider::TypeGround;

		ray.m_pos = m_nowPos;

		// 段差の許容範囲を設定
		static const float enableStepHigh = 0.2f;
		ray.m_pos.y += enableStepHigh;

		ray.m_dir = { 0, -1, 0 };
		ray.m_range = enableStepHigh + m_gravity;

		std::list<KdCollider::CollisionResult> retRayList;
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects(ray, &retRayList);
		}

		bool hit = false;
		float maxOverLap = 0;
		Math::Vector3 hitPos = {};

		for (auto& ret : retRayList)
		{
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				hitPos = ret.m_hitPos;
				hit = true;
			}
		}

		if (hit)
		{
			float landingSpeed = m_gravity;
			m_nowPos.y = hitPos.y;
			m_gravity = 0.0f;

			// ★ 高所落下の着地硬直判定
			if (landingSpeed > 0.2f)   // 落下速度が大きいなら硬直
			{
				m_isLanding = true;
				m_nowAnimIndex = 14; // 着地アニメ
				m_animator.SetAnimation(m_model->GetAnimation(14), false);
				ChangeState(PlayerState::Landing);
			}
		}
	}

	//========================
	//壁・構造体（カプセル）
	//========================
	{
		float maxOverlap = 0.0f;
		Math::Vector3 bestDir = Math::Vector3::Zero;
		bool hit = false;

		KdCollider::CapsuleInfo capsule;
		capsule.m_type = KdCollider::TypeBump;
		capsule.m_radius = 0.3f;
		capsule.m_start = m_nowPos + Math::Vector3(0, 0.5f, 0);
		capsule.m_end = m_nowPos + Math::Vector3(0, 1.5f, 0);

		m_pDebugWire->AddDebugCapsule(capsule.m_start, capsule.m_end, capsule.m_radius, { 1,1,1,1 });

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
				Math::Vector3 dir = ret.m_hitNDir;
				dir.y = 0;

				if (dir.LengthSquared() > 0.00001f) { dir.Normalize(); }
				else
				{
					Math::Vector3 fallback = ret.m_hitDir;
					fallback.y = 0;
					fallback.Normalize();
					dir = fallback;
				}

				bestDir = dir;
				hit = true;
			}
		}
		if (hit) { m_nowPos += bestDir * (maxOverlap * 0.9f); }
	}

	//======================================
	// ワールド行列の更新
	//======================================

	// 回転行列
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(m_angleY);
	m_rotation.y = DirectX::XMConvertToDegrees(m_angleY);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_nowPos);

	// ワールド行列
	m_mWorld = rotMat * transMat;

	if (m_isAttacking)
	{
		float t = m_attackAnimeTime;

		// 例：攻撃アニメの 20〜35 フレームだけ踏み込む
		if (t > 20.0f && t < 35.0f)
		{
			Math::Vector3 forward = m_mWorld.Forward();
			forward.Normalize();
			m_nowPos += forward * 0.05f;
		}
	}

}

void Player::DrawLit()
{	
	//if (auto cam = m_wpCamera.lock())
	//{
	//	if (cam->IsFPS())
	//	{
	//		return;
	//	}
	//}
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model,m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	/*if (auto cam = m_wpCamera.lock())
	{
		if (cam->IsFPS())
		{
			return;
		}
	}*/
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void Player::UpdateIdle()
{
	if (m_isLanding || m_isAttacking || m_isJumping) return;

	// 移動入力があれば Run へ
	if (m_moving)
	{
		ChangeState(PlayerState::Run);
		return;
	}

	// 攻撃
	if (m_attacking)
	{
		ChangeState(PlayerState::Attack);
		return;
	}

	// Idle アニメ
	if (m_nowAnimIndex != 9)
	{
		m_nowAnimIndex = 9;
		m_animator.SetAnimation(m_model->GetAnimation(9), true);
	}
}

void Player::UpdateRun()
{
	// 着地硬直中・攻撃中は行動できない
	if (m_isLanding || m_isAttacking || m_isJumping) return;

	//===============================
	// 移動入力が消えたら Idle へ
	//===============================
	if (!m_moving)
	{
		ChangeState(PlayerState::Idle);
		return;
	}

	//===============================
	// 攻撃
	//===============================
	if (m_attacking && !m_isAttacking)
	{
		ChangeState(PlayerState::Attack);
		return;
	}

	//===============================
	// 着地中は移動禁止
	//===============================
	if (m_isLanding)
	{
		m_dir = { 0,0,0 };
	}

	//===============================
	// Run / Walk アニメ切替
	//===============================
	int nextAnim = m_running ? 36 : 41;
	if (m_nowAnimIndex != nextAnim)
	{
		m_nowAnimIndex = nextAnim;
		m_animator.SetAnimation(m_model->GetAnimation(nextAnim), true);
	}

	//===============================
	// キャラ回転
	//===============================
	{
		Math::Vector3 nowDir = m_mWorld.Forward();
		Math::Vector3 targetDir = m_dir;

		nowDir.Normalize();
		targetDir.Normalize();

		float dot = std::clamp(nowDir.Dot(targetDir), -1.0f, 1.0f);
		float angle = acos(dot);

		Math::Vector3 cross = nowDir.Cross(targetDir);
		if (cross.y < 0) angle = -angle;

		const float rotSpeed = DirectX::XMConvertToRadians(5.0f);
		angle = std::clamp(angle, -rotSpeed, rotSpeed);

		m_angleY += angle;
	}

	//===============================
	// 実際の移動
	//===============================
	float moveSpeed = m_running ? 0.15f : 0.05f;
	m_nowPos += m_dir * moveSpeed;
}

void Player::UpdateAttack()
{
	if (!m_isAttacking)
	{
		m_isAttacking = true;
		m_nowAnimIndex = 39;
		m_animator.SetAnimation(m_model->GetAnimation(39), false);
	}

	if (m_animator.IsAnimationEnd())
	{
		m_isAttacking = false;
		ChangeState(PlayerState::Idle);
		return;
	}
}

void Player::UpdateLanding()
{
	if (m_nowAnimIndex != 14)
	{
		m_nowAnimIndex = 14;
		m_animator.SetAnimation(m_model->GetAnimation(14), false);
	}

	if (m_animator.IsAnimationEnd())
	{
		m_isLanding = false;
		ChangeState(PlayerState::Idle);
	}
}

void Player::UpdateFall()
{
	//===============================
	// 落下アニメ（Jump_Fall = 15）
	//===============================
	if (m_nowAnimIndex != 15)
	{
		m_nowAnimIndex = 15;
		m_animator.SetAnimation(m_model->GetAnimation(15), false);
	}

	//===============================
	// 落下中の慣性移動
	//===============================
	float fallSpeed = m_running ? 0.12f : 0.04f;
	m_nowPos += m_dir * fallSpeed;

	//===============================
	// キャラ回転（空中でも向きを変える）
	//===============================
	{
		Math::Vector3 nowDir = m_mWorld.Forward();
		Math::Vector3 targetDir = m_dir;

		nowDir.Normalize();
		targetDir.Normalize();

		float dot = std::clamp(nowDir.Dot(targetDir), -1.0f, 1.0f);
		float angle = acos(dot);

		Math::Vector3 cross = nowDir.Cross(targetDir);
		if (cross.y < 0) angle = -angle;

		const float rotSpeed = DirectX::XMConvertToRadians(5.0f);
		angle = std::clamp(angle, -rotSpeed, rotSpeed);

		m_angleY += angle;
	}

	//===============================
	// 着地したら Idle へ
	//===============================
	if (!m_isJumping)   
	{
		ChangeState(PlayerState::Idle);
		return;
	}
}


void Player::ChangeState(PlayerState next)
{
	m_state = next;
	m_stateTimer = 0.0f;
}