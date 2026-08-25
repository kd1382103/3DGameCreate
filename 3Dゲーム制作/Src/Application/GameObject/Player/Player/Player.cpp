#include "Player.h"

#include <Application/GameObject/Camera/TPSCamera/TPSCamera.h>
#include <Application/GameObject/Enemy/EnemyBase.h>
#include <Application/GameObject/Boss/BossBase.h>
#include <Application/GameObject/UI/PlaeyrUI/SkillGauge/SkillGauge.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/GameObject/UI/FontText/FontText.h>
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
	if (m_isGameEnd) return;

	//========================================
	// 60FPS基準のフレーム倍率
	//========================================
	const float frameScale =
		Application::Instance().GetFPSController().GetFrameScale();

	//========================================
	// ゲーム全体の速度
	//========================================
	const float timeScale =
		SceneManager::Instance().GetTimeScale();

	const float scaledFrameScale = frameScale * timeScale;

	//========================================
	// 入力
	//========================================
	UpdateInput();

	//========================================
	// 移動
	//========================================
	UpdateMovementInput();

	//========================================
	// アニメーション
	//========================================
	UpdateAnimation(scaledFrameScale);

	//========================================
	// 重力
	//========================================
	UpdateGravity(scaledFrameScale);

	//========================================
	// スキルゲージ
	//========================================
	UpdateSkillGauge(scaledFrameScale);

	//========================================
	// 遅延ダメージ
	//========================================
	UpdatePendingDamage();

	//========================================
	// 回避スロー
	//========================================
	UpdateDodgeSlow(scaledFrameScale);

	//========================================
	// 必殺技ポイント加算
	//========================================
	if (m_canGainUltimate && m_attackContact)
	{
		AddUltimateEnergy(1.0f);
	}

	//========================================
	// ヒットストップ
	//========================================
	if (m_hitStopTimer > 0.0f)
	{
		m_hitStopTimer -= scaledFrameScale;

		if (m_hitStopTimer > 0.0f)
		{
			return;
		}

		m_hitStopTimer = 0.0f;
	}

	//========================================
	// ステート更新
	//========================================
	stateMachine->Update(*this);

	//========================================
	// デバッグ
	//========================================
	UpdateDebug();
}

void Player::UpdateInput()
{
	m_dir = Math::Vector3::Zero;

	if (!m_inputLock)
	{
		//========================================
		// 移動入力
		//========================================
		if (GetAsyncKeyState('W') & 0x8000)
			m_dir += { 0, 0, 1 };

		if (GetAsyncKeyState('S') & 0x8000)
			m_dir += { 0, 0, -1 };

		if (GetAsyncKeyState('A') & 0x8000)
			m_dir += { -1, 0, 0 };

		if (GetAsyncKeyState('D') & 0x8000)
			m_dir += { 1, 0, 0 };

		//========================================
		// 移動中か
		//========================================
		m_moving =
			(m_dir.LengthSquared() > 0.0001f);

		//========================================
		// 回避 / 走行切り替え
		//========================================
		bool right = IsKeyPressedOnce(VK_RBUTTON);

		m_dodgeing = false;

		if (right)
		{
			if (m_canDodge)
			{
				m_dodgeing = true;
				m_justDodgeSuccess = true;

				AddUltimateEnergy(10.0f);

				//========================================
				// 回避スロー
				//========================================
				m_slowTimer = 10.0f;
				SceneManager::Instance().SetTimeScale(0.2f);

				//========================================
				// 回避カメラ演出
				//========================================
				if (auto cam =
					std::dynamic_pointer_cast<TPSCamera>(
						m_wpCamera.lock()))
				{
					cam->StartDodgeCamera();
				}

				//========================================
				// 攻撃中の敵をスロー
				//========================================
				for (auto& obj : SceneManager::Instance().GetObjList())
				{
					auto enemy =
						dynamic_cast<EnemyBase*>(obj.get());

					if (!enemy) continue;
					if (!enemy->IsAlive()) continue;
					if (!enemy->IsAttacking()) continue;

					enemy->StopAttackSound();
					enemy->m_attackSEPlayed = true;
					enemy->StartSlow(2.0f);
				}
			}
			else
			{
				m_running = !m_running;
			}
		}

		//========================================
		// 攻撃・スキル・必殺技
		//========================================
		m_attackOnce = IsKeyPressedOnce(VK_LBUTTON);
		m_skillOnce = IsKeyPressedOnce('E');
		m_ultimateOnce = IsKeyPressedOnce('Q');

		//========================================
		// ロックオン
		//========================================
		if (IsKeyPressedOnce(VK_MBUTTON))
		{
			m_lookOn = !m_lookOn;

			if (m_lookOn)
			{
				KdGameObject* nearest = nullptr;
				float nearestDist = FLT_MAX;

				for (auto& obj : SceneManager::Instance().GetObjList())
				{
					EnemyBase* enemy =
						dynamic_cast<EnemyBase*>(obj.get());

					BossBase* boss =
						dynamic_cast<BossBase*>(obj.get());

					if (!enemy && !boss) continue;

					if (enemy && !enemy->IsAlive())
						continue;

					if (boss && !boss->IsAlive())
						continue;

					Math::Vector3 targetPos;

					if (enemy)
					{
						targetPos = enemy->GetHitCenter();
					}
					else
					{
						targetPos = boss->GetHitCenter();
					}

					float dist =
						(targetPos - m_nowPos).Length();

					if (dist < nearestDist)
					{
						nearestDist = dist;
						nearest = obj.get();
					}
				}

				m_lockOnTarget = nearest;

				// ロックオン表示ON
				if (auto enemy =
					dynamic_cast<EnemyBase*>(m_lockOnTarget))
				{
					enemy->m_lockOnActive = true;
				}
				else if (auto boss =
					dynamic_cast<BossBase*>(m_lockOnTarget))
				{
					boss->m_lockOnActive = true;
				}
			}
			else
			{
				// ロックオン表示OFF
				if (auto enemy =
					dynamic_cast<EnemyBase*>(m_lockOnTarget))
				{
					enemy->m_lockOnActive = false;
				}
				else if (auto boss =
					dynamic_cast<BossBase*>(m_lockOnTarget))
				{
					boss->m_lockOnActive = false;
				}

				m_lockOnTarget = nullptr;
			}
		}

		//========================================
		// ロックオン対象の生存確認
		//========================================
		if (m_lookOn)
		{
			bool targetAlive = false;

			if (auto enemy =
				dynamic_cast<EnemyBase*>(m_lockOnTarget))
			{
				targetAlive = enemy->IsAlive();
			}
			else if (auto boss =
				dynamic_cast<BossBase*>(m_lockOnTarget))
			{
				targetAlive = boss->IsAlive();
			}

			if (!targetAlive)
			{
				if (auto enemy =
					dynamic_cast<EnemyBase*>(m_lockOnTarget))
				{
					enemy->m_lockOnActive = false;
				}
				else if (auto boss =
					dynamic_cast<BossBase*>(m_lockOnTarget))
				{
					boss->m_lockOnActive = false;
				}

				m_lookOn = false;
				m_lockOnTarget = nullptr;
			}
		}
	}
	else
	{
		//========================================
		// 入力ロック中
		//========================================
		m_moving = false;
		m_attackOnce = false;
		m_skillOnce = false;
		m_ultimateOnce = false;
		m_dodgeing = false;

		// 現在のキー状態を保存
		m_prevKeyState[VK_LBUTTON] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
		m_prevKeyState[VK_RBUTTON] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
		m_prevKeyState[VK_MBUTTON] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
		m_prevKeyState['E'] = (GetAsyncKeyState('E') & 0x8000) != 0;
		m_prevKeyState['Q'] = (GetAsyncKeyState('Q') & 0x8000) != 0;
	}
}

void Player::UpdateMovementInput()
{
	if (!m_moving) return;
	if (m_wpCamera.expired()) return;

	Math::Matrix camRotMat =
		m_wpCamera.lock()->GetRotationYMatrix();

	m_dir =
		Math::Vector3::TransformNormal(
			m_dir,
			camRotMat
		);

	m_dir.Normalize();
}

void Player::UpdateAnimation(float dt)
{
	m_animator.AdvanceTime(
		m_model->WorkNodes(),
		dt
	);

	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}
}

void Player::UpdateGravity(float dt)
{
	m_gravity += 0.005f * dt;

	m_nowPos.y -= m_gravity * dt;
}

void Player::UpdateSkillGauge(float dt)
{
	m_skillGauge += m_skillRegen * dt;

	if (m_skillGauge > m_skillGaugeMax)
	{
		m_skillGauge = m_skillGaugeMax;
	}

	//========================================
	// UI更新
	//========================================
	if (auto ui =
		GetUI<SkillGauge>(UIType::SkillGauge))
	{
		ui->SetGauge(
			m_skillGauge,
			m_skillGaugeMax
		);
	}

	if (auto ui =
		GetUI<HPGauge>(UIType::HPGauge))
	{
		ui->SetGauge(
			m_nowHp,
			m_hpGaugeMax
		);
	}
}

void Player::UpdatePendingDamage()
{
	if (m_pendingDelay <= 0)
	{
		return;
	}

	m_pendingDelay--;

	if (m_pendingDelay != 0)
	{
		return;
	}

	float before = m_pendingBeforeHP;

	float after =
		before - m_pendingDamage;

	if (after < 0)
	{
		after = 0;
	}

	//========================================
	// HP減少
	//========================================
	m_nowHp = after;

	//========================================
	// HPゲージ更新
	//========================================
	if (auto hpUI =
		GetUI<HPGauge>(UIType::HPGauge))
	{
		hpUI->OnDamage(before, after);
		hpUI->SetGauge(after, m_hpGaugeMax);
	}

	//========================================
	// リセット
	//========================================
	m_pendingBeforeHP = -1.0f;
	m_pendingAfterHP = -1.0f;
	m_pendingDamage = 0.0f;
}

void Player::UpdateDodgeSlow(float frameScale)
{
	if (m_slowTimer <= 0.0f)
	{
		return;
	}

	m_slowTimer -= frameScale;

	if (m_slowTimer > 0.0f)
	{
		return;
	}

	m_slowTimer = 0.0f;

	SceneManager::Instance().SetTimeScale(1.0f);

	if (auto cam =
		std::dynamic_pointer_cast<TPSCamera>(
			m_wpCamera.lock()))
	{
		cam->EndDodgeCamera();
	}
}

void Player::UpdateDebug()
{
	//KdDebugGUI::Instance().ClearLog();

	////========================================
	//// アニメーション一覧
	////========================================
	//for (int i = 0; ; i++)
	//{
	//	auto anim = m_model->GetAnimation(i);

	//	if (!anim)
	//	{
	//		break;
	//	}

	//	KdDebugGUI::Instance().AddLog(
	//		"%d : %s\n",
	//		i,
	//		anim->m_name.c_str()
	//	);
	//}

	/*
	KdDebugGUI::Instance().AddLog(
		"%f\n",
		m_nowPos.x
	);

	KdDebugGUI::Instance().AddLog(
		"%f\n",
		m_nowPos.z
	);

	KdDebugGUI::Instance().AddLog(
		"%f\n",
		m_nowPos.y
	);

	KdDebugGUI::Instance().AddLog(
		"Gravity : %f\n",
		m_gravity
	);

	KdDebugGUI::Instance().AddLog(
		"m_isLanding : %s\n",
		m_isLanding ? "true" : "false"
	);

	KdDebugGUI::Instance().AddLog(
		"m_isAttacking : %s\n",
		m_attacking ? "true" : "false"
	);

	KdDebugGUI::Instance().AddLog(
		"m_skillGauge : %f\n",
		m_skillGauge
	);
	*/

	//========================================
	// デバッグキー
	//========================================

	/*
	if (GetAsyncKeyState('1') & 0x8000)
	{
		m_hpGauge--;
	}

	if (GetAsyncKeyState('2') & 0x8000)
	{
		m_ultimateEnergy =
			m_ultimateEnergyMax;
	}

	if (GetAsyncKeyState('3') & 0x8000)
	{
		m_nowHp = 1;
	}
	*/
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

	// 敵との接触判定
	ResolveContact();
	
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
	auto fly = std::make_shared<FontText>();
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

void Player::ResolveContact()
{
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		// 通常敵
		EnemyBase* enemy = dynamic_cast<EnemyBase*>(obj.get());

		// ボス
		BossBase* boss = dynamic_cast<BossBase*>(obj.get());

		// どちらでもなければ無視
		if (!enemy && !boss) continue;

		// 死んでいたら無視
		if (enemy && !enemy->IsAlive()) continue;
		if (boss && !boss->IsAlive()) continue;

		//==============================
		// Playerの当たり判定中心
		//==============================
		Math::Vector3 playerCenter = m_nowPos + Math::Vector3(0, m_collisionHeight, 0);

		//==============================
		// Enemy / Boss の中心座標
		//==============================
		Math::Vector3 targetCenter;

		float targetRadius = 0.0f;

		if (enemy)
		{
			targetCenter =
				enemy->GetPos() + Math::Vector3(0, enemy->GetCollisionHeight(), 0);

			targetRadius = enemy->GetCollisionRadius();
		}
		else if (boss)
		{
			targetCenter =
				boss->GetPos() + Math::Vector3(0, boss->GetCollisionHeight(), 0);

			targetRadius = boss->GetCollisionRadius();
		}

		//==============================
		// Enemy / Boss → Player の方向
		//==============================
		Math::Vector3 diff =
			playerCenter - targetCenter;

		// Y方向は無視
		diff.y = 0.0f;

		float distance = diff.Length();

		// 半径の合計
		float radius = m_collisionRadius + targetRadius;

		//==============================
		// 接触判定
		//==============================
		if (distance < radius)
		{
			float penetration =
				radius - distance;

			if (distance <= 0.0001f)
			{
				diff = Math::Vector3(1, 0, 0);
			}
			else
			{
				diff.Normalize();
			}

			// PlayerをEnemy/Bossから押し出す
			m_nowPos += diff * penetration;
		}
	}
}