#include "EnemyBase.h"
#include<Application/Scene/SceneManager.h>
#include<Application/GameObject/Player/Player.h>

void EnemyBase::Init()
{
	// 派生クラスでモデルを読み込む

}

void EnemyBase::Update()
{
	m_stateTimer += 1.0f;

	switch (m_state)
	{
	case State::Idle: UpdateIdle(); break;
	case State::Move: UpdateMove(); break;
	case State::Hit:  UpdateHit();  break;
	case State::Dead: UpdateDead(); break;
	}

	// ワールド行列更新
	m_mWorld = Math::Matrix::CreateTranslation(m_nowPos);
}

void EnemyBase::PostUpdate()
{
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(m_angleY);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_nowPos);
	m_mWorld = rotMat * transMat;

	CapsuleCollision();
}

void EnemyBase::DrawLit()
{
	if (!m_model) return;
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void EnemyBase::UpdateIdle()
{
	// プレイヤーが近いなら移動へ
	if (auto player = m_wpPlayer.lock())
	{
		float dist = (player->GetPos() - m_nowPos).Length();
		if (dist < 10.0f)
		{
			ChangeState(State::Move);
		}
	}
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
			m_moveDir = dir;   // ★ 移動方向を保存

			m_nowPos += dir * m_moveSpeed;
		}
	}

	// ★ 向きを移動方向に合わせる
	RotateToMoveDir();
}


void EnemyBase::UpdateHit()
{
	// ノックバックなど入れたいならここ
	if (m_stateTimer > 20)
	{
		ChangeState(State::Move);
	}
}

void EnemyBase::UpdateDead()
{
	// 死亡処理（非表示など）
}

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


void EnemyBase::CapsuleCollision()
{

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
}