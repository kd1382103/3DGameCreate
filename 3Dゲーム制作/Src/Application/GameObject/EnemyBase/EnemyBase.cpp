#include "EnemyBase.h"
#include <Application/Scene/SceneManager.h>
#include <Application/GameObject/Player/Player.h>
#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/main.h>

void EnemyBase::Init()
{
	m_animator = std::make_shared<KdAnimator>();

	// ★攻撃予知エフェクト（SquarePolygon）
	m_preAttackEffect = std::make_shared<KdSquarePolygon>();
	m_preAttackEffect->SetMaterial("Asset/Textures/Effect/PreAttack.png");
	m_preAttackEffect->SetPivot(KdSquarePolygon::PivotType::Center_Bottom);
	m_preAttackEffect->Set2DObject(false);

	m_effectAlpha = 0.0f;
}

void EnemyBase::Update()
{
	m_stateTimer += 1.0f;
	m_gravity += 0.005f;
	m_nowPos.y -= m_gravity;

	m_attackCooldown -= 1.0f;

	// プレイヤー距離チェック（共通）
	if (m_state != State::Attack && m_state != State::PreAttack)
	{
		if (auto player = m_wpPlayer.lock())
		{
			float dist = (player->GetPos() - m_nowPos).Length();

			if (dist <= m_attackDist && m_attackCooldown <= 0.0f)
			{
				m_preAttackTimer = 0.5f;   // ★予備動作の総時間
				ChangeState(State::PreAttack);
				return;
			}
			else if (dist <= m_orbitDist)
			{
				ChangeState(State::Orbit);
			}
			else
			{
				ChangeState(State::Move);
			}
		}
	}

	// ステート処理
	switch (m_state)
	{
	case State::Idle:      UpdateIdle();      break;
	case State::Move:      UpdateMove();      break;
	case State::Orbit:     UpdateOrbit();     break;
	case State::PreAttack: UpdatePreAttack(); break;
	case State::Attack:    UpdateAttack();    break;
	case State::Hit:       UpdateHit();       break;
	case State::Dead:      UpdateDead();      break;
	}

	UpdateAnimation();
}

void EnemyBase::PostUpdate()
{
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(m_angleY);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_nowPos);
	m_mWorld = rotMat * transMat;

	CapsuleCollision();
	GroundCheck();
	PlayerCollusion();
}

void EnemyBase::DrawLit()
{
	if (!m_model) return;

	DrawEffect();
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void EnemyBase::DrawEffect()
{
	if (m_state != State::PreAttack) return;

	float size = 0.8f;

	// 顔の近く
	Math::Vector3 headPos = m_nowPos + Math::Vector3(0, 1.6f, 0);

	// カメラ正面を向く（ビルボード）
	auto cam = m_wpCamera.lock();
	Math::Matrix camRot = cam ? cam->GetRotationMatrix() : Math::Matrix::Identity;

	Math::Matrix scale = Math::Matrix::CreateScale(size);
	Math::Matrix trans = Math::Matrix::CreateTranslation(headPos);
	Math::Matrix mWorld = scale * camRot * trans;

	// ★フェード（出現 → 最大 → 消失）
	float total = 0.5f;          // 予備動作の総時間
	float t = m_preAttackTimer;  // 残り時間

	float alpha = 1.0f;

	if (t > total * 0.7f)
	{
		// 出現フェードイン
		float p = (t - total * 0.7f) / (total * 0.3f);
		alpha = 1.0f - p;
	}
	else if (t < total * 0.3f)
	{
		// 消失フェードアウト
		float p = t / (total * 0.3f);
		alpha = p;
	}

	// ★保存して UpdatePreAttack で使う
	m_effectAlpha = alpha;

	m_preAttackEffect->GetMaterial()->m_baseColorRate.w = alpha;

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_preAttackEffect, mWorld);
}

void EnemyBase::UpdateIdle()
{
	m_moveDir = Math::Vector3::Zero;
}

void EnemyBase::UpdateMove()
{
	if (auto player = m_wpPlayer.lock())
	{
		Math::Vector3 dir = player->GetPos() - m_nowPos;
		dir.y = 0;

		if (dir.LengthSquared() > 0.0001f)
		{
			dir.Normalize();
			m_moveDir = dir;

			m_nowPos += dir * m_moveSpeed;
		}
	}

	RotateToMoveDir();
}

void EnemyBase::UpdateOrbit()
{
	auto player = m_wpPlayer.lock();
	if (!player) return;

	Math::Vector3 toPlayer = player->GetPos() - m_nowPos;
	toPlayer.y = 0;

	float dist = toPlayer.Length();

	if (dist > m_orbitDist)
	{
		ChangeState(State::Move);
		return;
	}

	toPlayer.Normalize();

	m_moveDir = toPlayer;
	RotateToMoveDir();

	Math::Vector3 moveDir = { toPlayer.z, 0, -toPlayer.x };
	moveDir.Normalize();

	float orbitSpeed = m_moveSpeed * m_orbitSpeedRate;
	m_nowPos += moveDir * orbitSpeed;
}

void EnemyBase::UpdatePreAttack()
{
	m_moveDir = Math::Vector3::Zero;

	m_preAttackTimer -= Application::Instance().GetDeltaTime();

	// 予備動作終了 → Attack
	if (m_preAttackTimer <= 0.0f)
	{
		EndWarningFlash();
		ChangeState(State::Attack);
		return;
	}
}

void EnemyBase::UpdateAttack()
{
	auto player = m_wpPlayer.lock();
	if (!player)
	{
		ChangeState(State::Idle);
		return;
	}

	float dist = (player->GetPos() - m_nowPos).Length();

	float t = m_animator->GetAnimeCurrentTime();
	if (t > 10 && t < 20)
	{
		DoAttackHitCheck();
	}

	if (m_animator->IsAnimationEnd())
	{
		m_attackCooldown = m_attackInterval;
		ChangeState(State::Orbit);
		return;
	}
}

void EnemyBase::UpdateHit()
{
	if (m_stateTimer > 20)
	{
		ChangeState(State::Move);
	}
}

void EnemyBase::UpdateDead()
{}

void EnemyBase::RotateToMoveDir()
{
	if (m_moveDir.LengthSquared() < 0.0001f) return;

	Math::Vector3 nowDir = m_mWorld.Forward();
	Math::Vector3 targetDir = m_moveDir;

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

void EnemyBase::SetAnimator(const AnimatorInfo& info)
{
	if (!m_animator || !m_model) return;

	auto animData = m_model->GetAnimation(info.animName);
	if (!animData) return;

	m_animator->SetAnimation(animData, info.loop);
	m_animSpeed = info.speed;
}

void EnemyBase::SetAnimation(const std::string& animName, bool loop)
{
	if (!m_animator || !m_model) return;

	auto animData = m_model->GetAnimation(animName);
	if (!animData) return;

	m_animator->SetAnimation(animData, loop);
}

void EnemyBase::SetAnimationSpeed(float speed)
{
	m_animSpeed = speed;
}

void EnemyBase::UpdateAnimation()
{
	if (!m_animator || !m_model) return;

	m_animator->AdvanceTime(m_model->WorkNodes(), m_animSpeed);
	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}
}

void EnemyBase::Damage(float dmg)
{
	m_hp -= dmg;

	if (m_hp <= 0)
	{
		m_hp = 0;
		ChangeState(State::Dead);
		return;
	}

	ChangeState(State::Hit);
}

void EnemyBase::PlayAnimation(const std::string& animName)
{
	AnimatorInfo info;
	info.animName = animName;
	info.speed = m_animSpeed;
	info.loop = true;

	SetAnimator(info);
}

void EnemyBase::CapsuleCollision()
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

void EnemyBase::PlayerCollusion()
{
	KdCollider::CapsuleInfo capsule;
	capsule.m_type = KdCollider::TypeDamage;
	capsule.m_radius = 0.35f;
	capsule.m_start = m_nowPos + Math::Vector3(0, 0.5f, 0);
	capsule.m_end = m_nowPos + Math::Vector3(0, 1.5f, 0);
	capsule.m_ownerWorld = m_mWorld;

	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		std::list<KdCollider::CollisionResult> retList;
		obj->Intersects(capsule, &retList);

		if (retList.empty()) continue;

		Player* player = dynamic_cast<Player*>(obj.get());
		if (!player) continue;

		Math::Vector3 pushDir = m_moveDir;
		pushDir.y = 0;

		if (pushDir.LengthSquared() > 0.0001f)
		{
			pushDir.Normalize();
			pushDir = -pushDir;
		}
		else
		{
			Math::Vector3 n = retList.front().m_hitNDir;
			n.y = 0;
			n.Normalize();
			pushDir = n;
		}

		m_nowPos += pushDir * 0.02f;
		return;
	}
}

void EnemyBase::GroundCheck()
{
	KdCollider::RayInfo ray;
	ray.m_type = KdCollider::TypeGround;
	ray.m_pos = m_nowPos + Math::Vector3(0, 0.5f, 0);
	ray.m_dir = { 0, -1, 0 };
	ray.m_range = 2.0f;

	std::list<KdCollider::CollisionResult> retRayList;

	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		obj->Intersects(ray, &retRayList);
	}

	bool hitGround = false;
	Math::Vector3 hitPos = {};

	for (auto& ret : retRayList)
	{
		hitPos = ret.m_hitPos;
		hitGround = true;
		break;
	}

	if (hitGround)
	{
		float groundY = hitPos.y;

		if (m_nowPos.y <= groundY + 0.05f)
		{
			m_nowPos.y = groundY;
			m_gravity = 0.0f;
			m_isGround = true;
		}
	}
	else
	{
		m_isGround = false;
		m_gravity += 0.005f;
		m_nowPos.y -= m_gravity;
	}
}

void EnemyBase::DoAttackHitCheck()
{
	auto player = m_wpPlayer.lock();
	if (!player) return;

	float dist = (player->GetPos() - m_nowPos).Length();

	if (dist < 1.2f)
	{
		player->Damage(10);
	}
}

void EnemyBase::StartWarningFlash()
{
	auto data = m_model->GetData();
	if (!data) return;

	auto& mats = const_cast<std::vector<KdMaterial>&>(data->GetMaterials());

	for (auto& m : mats)
	{
		m.m_emissiveRate = { 20.0f, 20.0f, 20.0f };
	}
}

void EnemyBase::EndWarningFlash()
{
	auto data = m_model->GetData();
	if (!data) return;

	auto& mats = const_cast<std::vector<KdMaterial>&>(data->GetMaterials());

	for (auto& m : mats)
	{
		m.m_emissiveRate = { 1.0f, 1.0f, 1.0f };
	}

	m_isWarningFlash = false;
}
