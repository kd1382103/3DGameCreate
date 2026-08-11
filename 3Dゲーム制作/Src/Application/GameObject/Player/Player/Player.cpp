#include "Player.h"

#include <Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include <Application/GameObject/Enemy/EnemyBase.h>
#include <Application/GameObject/Boss/BossBase.h>
#include <Application/GameObject/UI/PlaeyrUI/SkillGauge/SkillGauge.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/GameObject/UI/FlyText/FlyText.h>
#include <Application/main.h>
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

		m_hpGauge = std::make_shared<HPGauge>();
		m_hpGauge->Init();
		m_hpGauge->SetMode(HPGauge::GaugeMode::Screen);
	}

	// ステートマシン初期化
	stateMachine = std::make_shared<StateMachine<Player>>();
	stateMachine->ChangeStateImmediate(std::make_unique<PlayerStateIdle>(), *this);
}

void Player::Update()
{
	if (m_isGameClear)
	{
		return;
	}

	//ゲーム全体の速度を取得
	float dt = SceneManager::Instance().GetTimeScale();

	//================================================================================
	// カメラの回転行列
	//================================================================================
	Math::Matrix camRotMat;
	if (!m_wpCamera.expired())
	{
		camRotMat = m_wpCamera.lock()->GetRotationYMatrix();
	}

	//=====================================
	// 必殺技ポイント加算
	//=====================================
	if (m_canGainUltimate && m_attackContact)
	{
		AddUltimateEnergy(1.0f);
	}

	//============================
	// ヒットストップ解除
	//============================
	if (m_hitStopTimer > 0.0f)
	{
		m_hitStopTimer -= 0.016f;  // 1フレーム分

		return;  // Update停止
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

	bool right = IsKeyPressedOnce(VK_RBUTTON);
	m_dodgeing = false;

	if (right)
	{
		if (m_canDodge)
		{
			m_dodgeing = true;
			m_justDodgeSuccess = true;
		}
		else
		{
			m_running = !m_running;
		}
	}

	m_attackOnce = IsKeyPressedOnce(VK_LBUTTON);
	m_skillOnce = IsKeyPressedOnce('E');
	m_ultimateOnce = IsKeyPressedOnce('Q');

	// ① ロックオン切り替え
	if (IsKeyPressedOnce(VK_MBUTTON))
	{
		m_lookOn = !m_lookOn;

		if (m_lookOn)
		{
			EnemyBase* nearest = nullptr;
			float nearestDist = FLT_MAX;

			for (auto& obj : SceneManager::Instance().GetObjList())
			{
				EnemyBase* enemy = dynamic_cast<EnemyBase*>(obj.get());
				if (!enemy) continue;
				if (!enemy->IsAlive()) continue;

				float dist = (enemy->GetHitCenter() - m_nowPos).Length();
				if (dist < nearestDist)
				{
					nearestDist = dist;
					nearest = enemy;
				}
			}

			m_lockOnTarget = nearest;

			if (m_lockOnTarget)
			{
				m_lockOnTarget->m_lockOnActive = true; 
			}
		}
		else
		{
			if (m_lockOnTarget)
			{
				m_lockOnTarget->m_lockOnActive = false;
			}

			m_lockOnTarget = nullptr;
		}
	}

	// ② 死んだ敵を参照しない
	if (m_lookOn)
	{
		if (!m_lockOnTarget || !m_lockOnTarget->IsAlive())
		{
			if (m_lockOnTarget)
			{
				m_lockOnTarget->m_lockOnActive = false;
			}
			m_lookOn = false;
			m_lockOnTarget = nullptr;
		}
	}


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
	m_animator.AdvanceTime(m_model->WorkNodes(), dt);

	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}

	//================================================================================
	// 重力
	//================================================================================
	m_gravity += 0.005f * dt;
	m_nowPos.y -= m_gravity * dt;

	//================================================================================
	// スキルゲージ回復
	//================================================================================
	m_skillGauge += m_skillRegen * dt;
	if (m_skillGauge > m_skillGaugeMax) m_skillGauge = m_skillGaugeMax;

	// UI 更新
	if (auto ui = GetUI<SkillGauge>(UIType::SkillGauge))
	{
		ui->SetGauge(m_skillGauge, m_skillGaugeMax);
	}
	if (auto ui = GetUI<HPGauge>(UIType::HPGauge))
	{
		ui->SetGauge(m_nowHp, m_hpGaugeMax);
	}
	
	//===============================
	// 遅延ダメージ処理
	//===============================
	if (m_pendingDelay > 0)
	{
		m_pendingDelay--;

		if (m_pendingDelay == 0)
		{
			float before = m_pendingBeforeHP;
			float after = before - m_pendingDamage;
			if (after < 0) after = 0;

			// HPを減らす
			m_nowHp = after;

			// UIへ通知
			if (auto hpUI = GetUI<HPGauge>(UIType::HPGauge))
			{
				hpUI->OnDamage(before, after);        // 赤バー幅決定
				hpUI->SetGauge(after, m_hpGaugeMax);  // 緑バー更新
			}

			// リセット
			m_pendingBeforeHP = -1.0f;
			m_pendingAfterHP = -1.0f;
			m_pendingDamage = 0.0f;
		}
	}

	//===============================
	// 回避スロー処理
	//===============================
	if (m_slowTimer > 0)
	{
		m_slowTimer -= Application::Instance().GetDeltaTime();

		if (m_slowTimer <= 0)
		{
			SceneManager::Instance().SetTimeScale(1.0f);
			if (auto cam = std::dynamic_pointer_cast<TPSCamera>(m_wpCamera.lock()))
			{
				cam->EndDodgeCamera();
			}
		}
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
	//
	////アニメーションの番号一覧をLogWindowに表示
	//for (int i = 0; ; i++)
	//{
	//	auto anim = m_model->GetAnimation(i);
	//	if (!anim) break; // 取得できなくなったら終了
	//
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
	
	//if (GetAsyncKeyState('1') & 0x8000) { m_hpGauge--; }	//体力ゲージの減少確認
	if (GetAsyncKeyState('2') & 0x8000) { m_ultimateEnergy = m_ultimateEnergyMax; }	
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

void Player::DrawSprite()
{
	if (auto ui = GetUI<SkillGauge>(UIType::SkillGauge))
		ui->DrawSprite();

	if (auto ui = GetUI<HPGauge>(UIType::HPGauge))
		ui->DrawSprite();

	// 必殺技ポイント表示
	KdSpriteShader::FontParam param;
	param.pos = { -500,180 };
	param.scale = 2.0f;
	param.pivot = { 0,0 };
	param.angle = 0;
	param.spacing = 0;

	if (m_ultimateEnergy >= m_ultimateEnergyMax)
	{
		param.color = { 1.0f,0.85f,0.0f,1.0f }; // 金色
	}
	else
	{
		param.color = { 0,0,0,1 };
	}

	KdShaderManager::Instance().m_spriteShader.DrawFontEx(
		param,
		"%d",
		(int)m_ultimateEnergy
	);
}

void Player::GenerateDepthMapFromLight()
{
	if (!m_model) return;
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void Player::Damage(float dmg, bool isUltimate, bool finalHit)
{
	auto fly = std::make_shared<FlyText>();
	fly->Init(m_nowPos + Math::Vector3(0, 2.0f, 0), (int)dmg);
	fly->SetCamera(m_wpCamera.lock());

	SceneManager::Instance().AddObject(fly);

	// 食らった瞬間のHPを保存
	m_pendingBeforeHP = m_nowHp;

	// 遅れて減らすHPを計算（まだ適用しない）
	float after = m_nowHp - dmg;
	if (after < 0) after = 0;

	m_pendingAfterHP = after;
	m_pendingDamage = dmg;

	// 遅延フレーム（例：10）
	m_pendingDelay = 10;

	//今は敵に必殺技やスキル等はないため、引数を使わないようにする（追加した時は変更）
	(void)isUltimate;
	(void)finalHit;
}

bool Player::IsKeyPressedOnce(int vk)
{
	bool now = (GetAsyncKeyState(vk) & 0x8000);
	bool prev = m_prevKeyState[vk];

	m_prevKeyState[vk] = now;

	return (now && !prev);
}

//単体判定用
//void Player::DoAttackHitCheck(float range, int damage)
//{
//	if (m_attackHitOnce) return;
//	if (m_isInvincible) return;
//
//	Math::Vector3 forward = GetForward();
//	forward.y = 0;
//	forward.Normalize();
//
//	for (auto& obj : SceneManager::Instance().GetObjList())
//	{
//		EnemyBase* enemy = dynamic_cast<EnemyBase*>(obj.get());
//		if (!enemy) continue;
//
//		Math::Vector3 toEnemy = enemy->GetHitCenter() - m_nowPos;
//		toEnemy.y = 0;
//		
//		float dist = toEnemy.Length();
//		if (dist > range) continue;
//		if (dist < 0.0001f) continue;
//
//		// 角度チェック
//		toEnemy.Normalize();
//
//		float dot = std::clamp(forward.Dot(toEnemy), -1.0f, 1.0f);
//		float angle = acos(dot);		
//		if (angle > DirectX::XMConvertToRadians(90.0f)) continue;
//
//		// ヒット
//		enemy->Damage(damage);
//		m_attackHitOnce = true;
//		m_hitStopTimer = 0.45f;   // 0.05秒停止
//		return;
//	}
//}

//複数判定用
void Player::DoAttackHitCheckMulti(float range, float width, int damage)
{
	if (m_attackHitOnce) return;
	if (m_isInvincible) return;

	Math::Vector3 forward = GetForward();
	forward.y = 0;
	forward.Normalize();

	bool hit = false;

	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		//通常敵
		EnemyBase* enemy = dynamic_cast<EnemyBase*>(obj.get());
		
		//Boss
		BossBase* boss = dynamic_cast<BossBase*>(obj.get());

		// どちらでもなければ対象外
		if (!enemy && !boss) continue;

		// 生存チェック
		if (enemy && !enemy->IsAlive()) continue;
		if (boss && !boss->IsAlive()) continue;

		Math::Vector3 targetPos= Math::Vector3::Zero;
		if (enemy)
		{
			targetPos = enemy->GetHitCenter();
		}
		else if (boss)
		{
			targetPos = boss->GetHitCenter();
		}

		Math::Vector3 toTarget = targetPos - m_nowPos;
		toTarget.y = 0;

		float dist = toTarget.Length();
		if (dist > range) continue;
		if (dist < 0.0001f) continue;

		toTarget.Normalize();

		float dot = std::clamp(forward.Dot(toTarget), -1.0f, 1.0f);
		float angle = acos(dot);
		if (angle > DirectX::XMConvertToRadians(width)) continue;

		// ダメージ
		if (enemy)
		{
			enemy->Damage(damage, false, false);
		}
		else if (boss)
		{
			boss->Damage(damage, false, false);
		}

		// 必殺技ポイント加算
		if(m_canGainUltimate)
		{
			m_attackContact = true;
		}
		hit = true;
	}

	// 一度でも当たったら終了
	if (hit)
	{
		m_attackHitOnce = true;
		m_hitStopTimer = 0.45f;
	}

}

void Player::DoUltimateHitCheck(float range, float width, int damage)
{
	if (m_isInvincible) return;

	Math::Vector3 forward = GetForward();
	forward.y = 0;
	forward.Normalize();

	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		// 通常敵
		EnemyBase* enemy = dynamic_cast<EnemyBase*>(obj.get());

		// ボス
		BossBase* boss = dynamic_cast<BossBase*>(obj.get());

		// どちらでもなければ対象外
		if (!enemy && !boss) continue;

		// 生存チェック
		if (enemy && !enemy->IsAlive()) continue;
		if (boss && !boss->IsAlive()) continue;

		// 攻撃対象の座標
		Math::Vector3 targetPos = Math::Vector3::Zero;

		if (enemy)
		{
			targetPos = enemy->GetHitCenter();
		}
		else if (boss)
		{
			targetPos = boss->GetHitCenter();
		}

		Math::Vector3 toTarget = targetPos - m_nowPos;
		toTarget.y = 0;

		float dist = toTarget.Length();
		if (dist > range) continue;
		if (dist < 0.0001f) continue;

		toTarget.Normalize();

		float dot = std::clamp(forward.Dot(toTarget), -1.0f, 1.0f);
		float angle = acos(dot);

		if (angle > DirectX::XMConvertToRadians(width)) continue;

		// 5回目のヒットならフィニッシュ判定
		bool finalHit = (m_ultimateHitCount >= 5);

		// ダメージ
		if (enemy)
		{
			enemy->Damage(damage, true, finalHit);
		}
		else if (boss)
		{
			boss->Damage(damage, true, finalHit);
		}
	}
}