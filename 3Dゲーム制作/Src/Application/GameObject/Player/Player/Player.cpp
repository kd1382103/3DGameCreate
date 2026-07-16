#include "Player.h"

#include <Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include <Application/GameObject/Enemy/EnemyBase.h>
#include <Application/GameObject/UI/PlaeyrUI/SkillGauge/SkillGauge.h>
#include <Application/GameObject/UI/PlaeyrUI/HPGauge/HPGauge.h>

#include <Application/GameObject/Player/PlayerState/PlayerState.h>
#include <Application/Scene/SceneManager.h>

void Player::Init()
{
	if (!m_model)
	{
		m_model = std::make_shared<KdModelWork>();
		m_model->SetModelData("Asset/Models/player/manModel/Player.gltf");

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		m_pCollider = std::make_unique<KdCollider>();
	}

	// ステートマシン初期化
	stateMachine = std::make_shared<StateMachine<Player>>();
	stateMachine->ChangeStateImmediate(std::make_unique<PlayerStateIdle>(), *this);
}

void Player::Update()
{
	//================================================================================
	// カメラの回転行列
	//================================================================================
	Math::Matrix camRotMat;
	if (!m_wpCamera.expired())
	{
		camRotMat = m_wpCamera.lock()->GetRotationYMatrix();
	}

	//================================================================================
	// 入力
	//================================================================================
	m_dir = Math::Vector3::Zero;

	if (GetAsyncKeyState('W') & 0x8000) m_dir += {0, 0, 1};
	if (GetAsyncKeyState('S') & 0x8000) m_dir += {0, 0, -1};
	if (GetAsyncKeyState('A') & 0x8000) m_dir += {-1, 0, 0};
	if (GetAsyncKeyState('D') & 0x8000) m_dir += {1, 0, 0};

	m_moving = (m_dir.LengthSquared() > 0.0001f);
	m_running = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
	m_attackOnce = IsKeyPressedOnce(VK_LBUTTON);
	m_skillOnce = IsKeyPressedOnce('E');
	m_dodgeing = IsKeyPressedOnce(VK_RBUTTON);

	//================================================================================
	// カメラ方向へ変換
	//================================================================================
	if (m_moving && !m_wpCamera.expired())
	{
		m_dir = Math::Vector3::TransformNormal(m_dir, camRotMat);
		m_dir.Normalize();
	}

	//================================================================================
	// アニメ進行（速度固定）
	//================================================================================
	m_animator.AdvanceTime(m_model->WorkNodes(), 1.0f);

	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}

	//================================================================================
	// 重力
	//================================================================================
	m_gravity += 0.005f;
	m_nowPos.y -= m_gravity;

	//================================================================================
	// スキルゲージ回復
	//================================================================================
	m_skillGauge += m_skillRegen;
	if (m_skillGauge > m_skillGaugeMax) m_skillGauge = m_skillGaugeMax;

	// UI 更新
	if (auto ui = GetUI<SkillGauge>(UIType::SkillGauge))
	{
		ui->SetGauge(m_skillGauge, m_skillGaugeMax);
	}
	if (auto ui = GetUI<HPGauge>(UIType::HPGauge))
	{
		ui->SetGauge(m_hpGauge, m_hpGaugeMax);
	}

	//================================================================================
	// ステート更新（最重要）
	//================================================================================
	stateMachine->Update(*this);

	//===============================================================================
	//	デバック関係
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
	// 地面判定（レイ）
	//================================================================================
	{
		KdCollider::RayInfo ray;
		ray.m_type = KdCollider::TypeGround;

		ray.m_pos = m_nowPos;

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
			m_nowPos.y = hitPos.y;
			m_gravity = 0.0f;
		}
	}

	//================================================================================
	// 壁判定（カプセル）
	//================================================================================
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

				if (dir.LengthSquared() > 0.00001f)
				{
					dir.Normalize();
				}
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

		if (hit)
		{
			m_nowPos += bestDir * (maxOverlap * 0.9f);
		}
	}

	//================================================================================
	// ワールド行列更新
	//================================================================================
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(m_angleY);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_nowPos);

	m_mWorld = rotMat * transMat;
}

void Player::DrawLit()
{
	if (!m_model) return;
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	if (!m_model) return;
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void Player::Damage(float dmg)
{
	m_hpGauge -= dmg;
	if (m_hpGauge < 0) m_hpGauge = 0;
}

bool Player::IsKeyPressedOnce(int vk)
{
	bool now = (GetAsyncKeyState(vk) & 0x8000);
	bool prev = m_prevKeyState[vk];

	m_prevKeyState[vk] = now;

	return (now && !prev);
}

void Player::DoAttackHitCheck(float range)
{
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj.get() == this) continue;

		EnemyBase* enemy = dynamic_cast<EnemyBase*>(obj.get());
		if (!enemy) continue;

		float dist = (enemy->GetPos() - m_nowPos).Length();
		if (dist < range)
		{
			enemy->Damage(20);
		}
	}
}
