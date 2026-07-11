#include "EnemyBase.h"
#include <Application/Scene/SceneManager.h>
#include <Application/GameObject/Player/Player.h>

void EnemyBase::Init()
{
	m_animator = std::make_shared<KdAnimator>();
}

void EnemyBase::Update()
{
	m_stateTimer += 1.0f;
	m_gravity += 0.005f;
	m_nowPos.y -= m_gravity;
	// プレイヤー距離チェック（共通）
	if (auto player = m_wpPlayer.lock())
	{
		float dist = (player->GetPos() - m_nowPos).Length();

		if (dist > m_chaseDist)
		{
			ChangeState(State::Move);
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

	// ステート処理
	switch (m_state)
	{
	case State::Idle:  UpdateIdle();  break;
	case State::Move:  UpdateMove();  break;
	case State::Orbit: UpdateOrbit(); break;
	case State::Hit:   UpdateHit();   break;
	case State::Dead:  UpdateDead();  break;
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
}

void EnemyBase::DrawLit()
{
	if (!m_model) return;
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
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
	if (auto player = m_wpPlayer.lock())
	{
		Math::Vector3 toPlayer = player->GetPos() - m_nowPos;
		toPlayer.y = 0;

		if (toPlayer.LengthSquared() < 0.0001f) return;

		toPlayer.Normalize();

		// 向きはプレイヤー方向
		m_moveDir = toPlayer;
		RotateToMoveDir();

		// 横方向に移動
		Math::Vector3 moveDir = { toPlayer.z, 0, -toPlayer.x };
		moveDir.Normalize();

		float orbitSpeed = m_moveSpeed * m_orbitSpeedRate;
		m_nowPos += moveDir * orbitSpeed;

		// 距離補正
		Math::Vector3 raw = m_nowPos - player->GetPos();
		raw.y = 0;

		if (raw.LengthSquared() > 0.0001f)
		{
			raw.Normalize();
			m_nowPos = player->GetPos() + raw * m_orbitDist;
		}
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

//--------------------------------------
// アニメーション関連
//--------------------------------------
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

void EnemyBase::PlayAnimation(const std::string& animName)
{
	AnimatorInfo info;
	info.animName = animName;
	info.speed = m_animSpeed;
	info.loop = true;

	SetAnimator(info);
}

//--------------------------------------
// カプセル当たり判定
//--------------------------------------
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

//--------------------------------------
// 地面判定
//--------------------------------------
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
