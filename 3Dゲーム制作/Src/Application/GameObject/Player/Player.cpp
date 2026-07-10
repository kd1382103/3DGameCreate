#include "Player.h"

#include<Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include<Application/GameObject/UI/PlaeyrUI/SkillGauge/SkillGauge.h>
#include<Application/GameObject/UI/PlaeyrUI/HPGauge/HPGauge.h>
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

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Player", m_model, KdCollider::TypeDamage);
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
	if (m_state != PlayerState::Fall && m_state != PlayerState::Landing)
	{
		m_running = (GetAsyncKeyState(VK_SHIFT) & 0x8000);	//ダッシュ
	}

	//攻撃入力
	m_attackOnce = IsKeyPressedOnce(VK_LBUTTON);
	m_skillOnce = IsKeyPressedOnce('E');

	//  落下中は攻撃・スキルを無効化
	if (m_state == PlayerState::Fall)
	{
		m_attackOnce = false;
		m_skillOnce = false;
	}

	//m_jumpOnce = IsKeyPressedOnce(VK_SPACE);

	//回避入力
	m_dodgeing = IsKeyPressedOnce(VK_RBUTTON);


	//================================================================================
	// 	カメラ方向へ変換
	//================================================================================
	if (m_moving && !m_wpCamera.expired())
	{
		m_dir = Math::Vector3::TransformNormal(m_dir, camRotMat);
		m_dir.Normalize();
	}
	//================================================================================
	// ★ スキルは最優先で割り込む	
	//================================================================================

	if (m_skillOnce && m_skillGauge >= m_skillCost)
	{
		if (m_state != PlayerState::Fall &&
			m_state != PlayerState::Landing)
		{
			ChangeState(PlayerState::Skill);
			return;
		}
	}

	//================================================================================
	//	ステート呼び出し
	//================================================================================

	switch (m_state)
	{
	case PlayerState::Idle:    UpdateIdle();    break;
	case PlayerState::Run:     UpdateRun();     break;
	//case PlayerState::Jump:     UpdateJump();     break;
	case PlayerState::Attack1:  UpdateAttack1();  break;
	case PlayerState::Attack2:  UpdateAttack2();  break;
	case PlayerState::Attack3:  UpdateAttack3();  break;
	case PlayerState::Skill:	UpdateSkill();	break;
	case PlayerState::Landing:  UpdateLanding();  break;
	case PlayerState::Fall:  UpdateFall();  break;
	case PlayerState::Dodge:  UpdateDodge();  break;
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

	// ★ スキルゲージ自動回復（超微量）
	m_skillGauge += m_skillRegen;
	if (m_skillGauge > m_skillGaugeMax) m_skillGauge = m_skillGaugeMax;


	if (m_gravity > 0.01f)
	{
		if (m_state != PlayerState::Fall)
		{
			ChangeState(PlayerState::Fall);
		}
	}

	float t = m_animator.GetAnimeCurrentTime();
	m_attackAnimeTime = t;   // ← 一時保存

	// SkillGauge 更新
	if (auto ui = GetUI<SkillGauge>(UIType::SkillGauge))
	{
		ui->SetGauge(m_skillGauge, m_skillGaugeMax);
	}

	// HPGauge 更新（作ったら）
	if (auto ui = GetUI<HPGauge>(UIType::HPGauge))
	{
		ui->SetGauge(m_hpGauge, m_hpGaugeMax);
	}

	//===============================================================================
	//	デバック関係
	//===============================================================================

	//===============================================================================
	// LogWindowに表示(発表時は非表示)
	//===============================================================================

	//デバック（プレイヤーの向いている方向）
	//Math::Vector3 forward = m_mWorld.Forward();
	//m_pDebugWire->AddDebugLine(m_nowPos, m_nowPos + forward, { 1,0,0,1 }); // 赤線で前方向


	//KdDebugGUI::Instance().ClearLog();

	//アニメーションの番号一覧をLogWindowに表示
	//for (int i = 0; ; i++)
	//{
	//	auto anim = m_model->GetAnimation(i);
	//	if (!anim) break; // 取得できなくなったら終了

	//	KdDebugGUI::Instance().AddLog("%d : %s\n", i, anim->m_name.c_str());
	//}

	/*KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.x);
	KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.z);
	KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.y);

	KdDebugGUI::Instance().AddLog("Gravity : %f\n", m_gravity);
	KdDebugGUI::Instance().AddLog("m_isLanding : %s\n", m_isLanding ? "true" : "false");
	KdDebugGUI::Instance().AddLog("m_isAttacking : %s\n", m_attacking ? "true" : "false");
	KdDebugGUI::Instance().AddLog("m_skillGauge : %f\n", m_skillGauge);*/

	//===============================================================================
	//	デバックキー一覧
	//===============================================================================

	//if (GetAsyncKeyState('P') & 0x8000) { m_hpGauge--; }	//体力ゲージの減少確認

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
		capsule.m_ownerWorld = m_mWorld;

		//m_pDebugWire->AddDebugCapsule(capsule.m_start, capsule.m_end, capsule.m_radius, { 1,1,1,1 });

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

	//if (m_isAttacking)
	//{
	//	float t = m_attackAnimeTime;

	//	// 例：攻撃アニメの 20〜35 フレームだけ踏み込む
	//	if (t > 20.0f && t < 35.0f)
	//	{
	//		Math::Vector3 forward = m_mWorld.Forward();
	//		forward.Normalize();
	//		m_nowPos += forward * 0.05f;
	//	}
	//}

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
	if (m_isLanding) return;

	// 移動入力があれば Run へ
	if (m_moving)
	{
		ChangeState(PlayerState::Run);
		return;
	}

	// 攻撃
	if (m_attackOnce)
	{
		ChangeState(PlayerState::Attack1);
		return;
	}

	//回避
	if (m_dodgeing)
	{
		ChangeState(PlayerState::Dodge);
	}

	//ジャンプ
	//if (m_jumpOnce)
	//{
	//	ChangeState(PlayerState::Jump);
	//	return;
	//}

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
	if (m_isLanding) return;

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
	if (m_attackOnce)
	{
		ChangeState(PlayerState::Attack1);
		return;
	}

	//===============================
	// 回避
	//===============================
	if (m_dodgeing)
	{
		ChangeState(PlayerState::Dodge);
		return;
	}

	//===============================
	//ジャンプ
	//===============================
	/*if (m_jumpOnce)
	{
		ChangeState(PlayerState::Jump);
		return;
	}*/

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

		const float rotSpeed = DirectX::XMConvertToRadians(m_rotationSpeedDeg);
		angle = std::clamp(angle, -rotSpeed, rotSpeed);

		m_angleY += angle;
	}

	//===============================
	// 実際の移動
	//===============================
	float moveSpeed = m_running ? 0.15f : 0.05f;
	m_nowPos += m_dir * moveSpeed;
}

//void Player::UpdateJump()
//{
//	// ジャンプ開始時のアニメ
//	if (m_nowAnimIndex != 12)
//	{
//		m_nowAnimIndex = 12;
//		m_animator.SetAnimation(m_model->GetAnimation(12), false);
//
//		m_gravity = -0.25f;   // 上方向へ初速
//		m_isJumping = true;
//	}
//
//	// ★ 空中移動（Fall と同じ）
//	float jumpMoveSpeed = m_running ? 0.12f : 0.05f;
//	m_nowPos += m_dir * jumpMoveSpeed;
//
//	// ★ 空中でキャラ回転（Fall と同じ）
//	if (m_moving)
//	{
//		Math::Vector3 nowDir = m_mWorld.Forward();
//		Math::Vector3 targetDir = m_dir;
//
//		nowDir.Normalize();
//		targetDir.Normalize();
//
//		float dot = std::clamp(nowDir.Dot(targetDir), -1.0f, 1.0f);
//		float angle = acos(dot);
//
//		Math::Vector3 cross = nowDir.Cross(targetDir);
//		if (cross.y < 0) angle = -angle;
//
//		const float rotSpeed = DirectX::XMConvertToRadians(m_rotationSpeedDeg);
//		angle = std::clamp(angle, -rotSpeed, rotSpeed);
//
//		m_angleY += angle;
//	}
//
//	// ★ 上昇 → 落下へ切り替え
//	if (m_gravity > 0.0f)
//	{
//		ChangeState(PlayerState::Fall);
//		return;
//	}
//
//	// ★ 空中スキル
//	if (m_skillOnce)
//	{
//		ChangeState(PlayerState::Skill);
//		return;
//	}
//
//	// ★ 空中攻撃
//	if (m_attackOnce)
//	{
//		ChangeState(PlayerState::Attack1);
//		return;
//	}
//}

void Player::UpdateAttack1()
{
	m_moving = false;
	m_dir = Math::Vector3::Zero;
	float t = m_animator.GetAnimeCurrentTime();

	//KdDebugGUI::Instance().AddLog("Attack1 time: %f\n", t);
	if (m_nowAnimIndex != 39)
	{
		m_nowAnimIndex = 39;
		m_animator.SetAnimation(m_model->GetAnimation(39), false);
	}

	// ★ 踏み込み（開始〜0.2秒）
	if (t >20.0f && t< 30.0f)
	{
		Math::Vector3 forward = m_mWorld.Forward();
		forward.Normalize();
		m_nowPos += forward * 0.05f;
	}


	// ★ 攻撃判定（0.15〜0.25秒）
	/*if (t > 0.15f && t < 0.25f)
	{
		DoAttackHitCheck(0.8f);
	}*/

	// ★ Attack2 の受付（予約制）
	if (t > 30.0f && t < 85.0f)
	{
		if (m_attackOnce)
		{
			m_canNextAttack = true; 
		}
	}

	// ★ アニメ終了後に予約があれば次へ
	if (m_animator.IsAnimationEnd())
	{
		if (m_skillOnce && m_skillGauge >= m_skillCost)
		{
			m_skillReserved = true;
		}

		if (m_canNextAttack)
		{
			ChangeState(PlayerState::Attack2);
			return;
		}

		ChangeState(PlayerState::Idle);
		return;
	}
}


void Player::UpdateAttack2()
{	
	m_moving = false; 
	m_dir = Math::Vector3::Zero;
	float t = m_animator.GetAnimeCurrentTime();

	if (m_nowAnimIndex != 40)
	{
		m_nowAnimIndex = 40;
		m_animator.SetAnimation(m_model->GetAnimation(40), false);
	}

	// ★ 踏み込み（Attack1より強い）
	if (t > 20.0f && t < 32.0f)
	{
		Math::Vector3 forward = m_mWorld.Forward();
		forward.Normalize();
		m_nowPos += forward * 0.08f;
	}

	// ★ 攻撃判定（Attack1より広い）
	//if (t > 0.18f && t < 0.30f)
	//{
	//	DoAttackHitCheck(1.2f);
	//}

	// ★ Attack3 の受付（予約）
	if (t > 25.0f && t < 80.0f)
	{
		if (m_attackOnce)
		{
			m_canNextAttack = true;
		}
	}

	// ★ アニメ終了後に予約があれば次へ
	if (m_animator.IsAnimationEnd())
	{
		if (m_skillOnce && m_skillGauge >= m_skillCost)
		{
			m_skillReserved = true;
		}

		if (m_canNextAttack)
		{
			ChangeState(PlayerState::Attack3);
			return;
		}

		ChangeState(PlayerState::Idle);
		return;
	}
}


void Player::UpdateAttack3()
{
	m_moving = false;
	m_dir = Math::Vector3::Zero;
	float t = m_animator.GetAnimeCurrentTime();

	if (m_nowAnimIndex != 41)
	{
		m_nowAnimIndex = 41;
		m_animator.SetAnimation(m_model->GetAnimation(41), false);
	}

	// ★ フィニッシュの強い踏み込み
	if (t > 20.0f && t < 35.0f)
	{
		Math::Vector3 forward = m_mWorld.Forward();
		forward.Normalize();
		m_nowPos += forward * 0.12f;
	}


	// ★ 最大攻撃判定
	/*if (t > 0.20f && t < 0.35f)
	{
		DoAttackHitCheck(1.6f);
	}*/

	if (m_animator.IsAnimationEnd())
	{
		if (m_skillOnce && m_skillGauge >= m_skillCost)
		{
			ChangeState(PlayerState::Skill);
			return;
		}

		ChangeState(PlayerState::Idle);
		return;
	}
}

void Player::UpdateSkill()
{
	m_moving = false;
	m_dir = Math::Vector3::Zero;

	float t = m_animator.GetAnimeCurrentTime();

	if (m_nowAnimIndex != 20)   // スキルアニメ番号
	{
		m_nowAnimIndex = 20;
		m_animator.SetAnimation(m_model->GetAnimation(20), false);

		// ★ スキル開始時に通常攻撃予約はリセット
		m_canNextAttack = false;

		m_skillGauge -= m_skillCost;
		if (m_skillGauge < 0)m_skillGauge = 0;
	}

	// ★ スキルの踏み込み（任意）
	if (t > 10 && t < 25)
	{
		Math::Vector3 forward = m_mWorld.Forward();
		forward.Normalize();
		m_nowPos += forward * 0.10f;
	}

	// ★ スキルの攻撃判定（任意）
	/*if (t > 20 && t < 35)
	{
		DoAttackHitCheck(1.5f);
	}*/

	// ★ スキル終了時の遷移
	if (m_animator.IsAnimationEnd())
	{
		// ★ スキル連打ならスキル継続
		if (m_skillOnce)
		{
			ChangeState(PlayerState::Skill);
			return;
		}

		// ★ スキル終了時に通常攻撃が押されていれば Attack1 へ
		if (m_attackOnce)
		{
			ChangeState(PlayerState::Attack1);
			return;
		}

		// ★ 何も押されていなければ Idle
		ChangeState(PlayerState::Idle);
		return;
	}
}



void Player::UpdateLanding()
{
	// 着地アニメ（14）
	if (m_nowAnimIndex != 14)
	{
		m_nowAnimIndex = 14;
		m_animator.SetAnimation(m_model->GetAnimation(14), false);
	}

	// アニメ終了で着地完了 → Idle へ
	if (m_animator.IsAnimationEnd())
	{
		m_isLanding = false;
		ChangeState(PlayerState::Idle);
		return;
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
	if (m_moving)
	{
		Math::Vector3 nowDir = m_mWorld.Forward();
		Math::Vector3 targetDir = m_dir;

		nowDir.Normalize();
		targetDir.Normalize();

		float dot = std::clamp(nowDir.Dot(targetDir), -1.0f, 1.0f);
		float angle = acos(dot);

		Math::Vector3 cross = nowDir.Cross(targetDir);
		if (cross.y < 0) angle = -angle;

		const float rotSpeed = DirectX::XMConvertToRadians(m_rotationSpeedDeg);
		angle = std::clamp(angle, -rotSpeed, rotSpeed);

		m_angleY += angle;
	}

	//===============================
	// 着地したら Idle へ
	//===============================
	if (!m_isLanding)
	{
		ChangeState(PlayerState::Idle);
		return;
	}
}

void Player::UpdateDodge()
{
	//===============================
	// アニメ再生（Dodge = 20 と仮定）
	//===============================
	if (m_nowAnimIndex != 20)
	{
		m_nowAnimIndex = 20;
		m_animator.SetAnimation(m_model->GetAnimation(20), false);
	}

	//===============================
	// 無敵時間（最初の10フレーム）
	//===============================
	const int iFrame = 10;
	bool isInvincible = (m_stateTimer < iFrame);

	//===============================
	// ドッジ移動（高速）
	//===============================
	float dodgeSpeed = 0.25f;  // 走りより速い
	Math::Vector3 forward = m_mWorld.Forward();
	forward.Normalize();

	m_nowPos += forward * dodgeSpeed;

	//===============================
	// ドッジ終了判定
	//===============================
	if (m_animator.IsAnimationEnd())
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

bool Player::IsKeyPressedOnce(int vk)
{
	bool now = (GetAsyncKeyState(vk) & 0x8000);
	bool prev = m_prevKeyState[vk];

	m_prevKeyState[vk] = now;   // 次のフレームのために保存

	return (now && !prev);      // 押された瞬間だけ true
}

