#include "EnemyCharacterBase.h"

#include <Application/GameObject/Camera/CameraBase.h>
#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/UI/HPGauge/HPGauge.h>
#include <Application/Scene/SceneManager.h>
#include <Application/main.h>
//==============================================================
// アニメーション
//==============================================================

void EnemyCharacterBase::PlayAnimationAuto(
	const std::string& animName,
	bool loop)
{
	if (!m_model) return;

	auto anim = m_model->GetAnimation(animName);

	if (anim)
	{
		m_animator.SetAnimation(anim, loop);
	}
}

//==============================================================
// UI初期化・表示
//==============================================================
//HPゲージ初期化
void EnemyCharacterBase::InitHPGauge()
{
	m_hpGauge = std::make_shared<HPGauge>();
	m_hpGauge->Init();
	m_hpGauge->SetMode(HPGauge::GaugeMode::World);
}

// ワールドUI初期化
void EnemyCharacterBase::InitWorldUI()
{
	m_preAttackPoly = std::make_shared<KdSquarePolygon>();
	m_lockOnIcon = std::make_shared<KdSquarePolygon>();

	SetupWorldPolygon(
		*m_preAttackPoly,
		"Asset/Textures/Effect/PreAttack.png",
		1.0f
	);

	SetupWorldPolygon(
		*m_lockOnIcon,
		"Asset/Textures/Effect/LookOn.png",
		0.8f
	);
}

// HPゲージ表示
void EnemyCharacterBase::SetHPGaugeVisible(bool visible)
{
	if (m_hpGauge)
	{
		m_hpGauge->SetVisible(visible);
	}
}

//==============================================================
// 深度マップ描画
//==============================================================
void EnemyCharacterBase::GenerateDepthMapFromLightCommon(bool isDying)
{
	if (!m_model) return;

	//==========================================================
	// 視錐台カリング（影）
	//==========================================================
	if (!isDying && !IsInCameraView())
	{
		return;
	}

	KdShaderManager::Instance()
		.m_StandardShader.DrawModel(
			*m_model,
			m_mWorld
		);
}

//==========================================================
// 攻撃予知アイコン位置更新
//==========================================================
void EnemyCharacterBase::UpdatePreAttackPosition()
{
	if (!m_model) return;

	const auto* headNode = m_model->FindNode("head");

	if (headNode)
	{
		Math::Vector3 headPos =
			Math::Vector3::Transform(
				Math::Vector3::Zero,
				headNode->m_worldTransform * m_mWorld);

		headPos.y += 0.2f;

		m_preAttackPos = headPos;
	}
	else
	{
		// Headノードがない場合は従来の位置
		m_preAttackPos =
			m_mWorld.Translation() +
			Math::Vector3(0, 1.8f, 0);
	}
}

//==============================================================
// ワールド行列・UI位置更新
//==============================================================
void EnemyCharacterBase::UpdateWorldMatrixAndUI()
{
	// ワールド行列更新
	Math::Matrix rotMat =
		Math::Matrix::CreateRotationY(m_angleY);

	Math::Matrix transMat =
		Math::Matrix::CreateTranslation(m_nowPos);

	m_mWorld = rotMat * transMat;

	// 攻撃予知アイコン位置更新
	UpdatePreAttackPosition();

	// HPゲージ位置更新
	if (m_hpGauge)
	{
		Math::Vector3 worldPos = m_mWorld.Translation();

		m_hpGauge->SetWorldPos(
			worldPos + Math::Vector3(0, 2.0f, 0)
		);
	}

	// ロックオン位置更新
	m_lockOnPos =
		m_nowPos + Math::Vector3(0, 1.0f, 0);
}

//==========================================================
// 地面判定
//==========================================================
void EnemyCharacterBase::GroundCollisionCheck()
{
	Math::Vector3 groundPos = m_nowPos;
	groundPos.y += 0.2f;

	Math::Vector3 rayDir = Math::Vector3(0, -1, 0);
	float rayRange = 0.2f + m_gravity;

	KdCollider::RayInfo rayInfo;
	rayInfo.m_pos = groundPos;
	rayInfo.m_dir = rayDir;
	rayInfo.m_range = rayRange;
	rayInfo.m_type = KdCollider::TypeGround;

	std::list<KdCollider::CollisionResult> results;

	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		obj->Intersects(rayInfo, &results);
	}

	float maxOverlap = 0.0f;
	Math::Vector3 hitPos;

	for (auto& result : results)
	{
		if (maxOverlap < result.m_overlapDistance)
		{
			maxOverlap = result.m_overlapDistance;
			hitPos = result.m_hitPos;
		}
	}

	if (maxOverlap > 0.0f)
	{
		m_nowPos.y = hitPos.y;
		m_gravity = 0.0f;
	}
}

//==========================================================
// 壁判定
//==========================================================
void EnemyCharacterBase::WallCollisionCheck()
{
	float maxOverlap = 0.0f;
	Math::Vector3 bestDir = Math::Vector3::Zero;
	bool hit = false;

	KdCollider::CapsuleInfo capsule;
	capsule.m_type = KdCollider::TypeBump;
	capsule.m_radius = 0.3f;

	capsule.m_start =
		m_nowPos + Math::Vector3(0, 0.5f, 0);

	capsule.m_end =
		m_nowPos + Math::Vector3(0, 1.5f, 0);

	capsule.m_ownerWorld = m_mWorld;

	std::list<KdCollider::CollisionResult> retCapsuleList;

	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		obj->Intersects(
			capsule,
			&retCapsuleList
		);
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

//==========================================================
// ノックバック移動（障害物に当たったら壁沿いに滑る）
//==========================================================
void EnemyCharacterBase::NockBackCheck()
{
	if (m_hasPendingKnockBack)
	{
		Math::Vector3 move = m_knockBackMove;
		move.y = 0.0f;

		m_knockBackMove = Math::Vector3::Zero;
		m_hasPendingKnockBack = false;

		if (move.LengthSquared() > 0.00001f)
		{
			// 指定位置で壁に当たるか調べる
			auto CheckWall =
				[&](const Math::Vector3& pos,
					Math::Vector3& hitNormal) -> bool
				{
					KdCollider::CapsuleInfo capsule;

					capsule.m_type = KdCollider::TypeBump;
					capsule.m_radius = 0.3f;
					capsule.m_start = pos + Math::Vector3(0, 0.5f, 0);
					capsule.m_end = pos + Math::Vector3(0, 1.5f, 0);

					Math::Matrix rotMat =
						Math::Matrix::CreateRotationY(m_angleY);

					Math::Matrix transMat =
						Math::Matrix::CreateTranslation(pos);

					capsule.m_ownerWorld = rotMat * transMat;

					std::list<KdCollider::CollisionResult> results;

					for (auto& obj : SceneManager::Instance().GetObjList())
					{
						// 自分自身は判定しない
						if (obj.get() == this) continue;

						obj->Intersects(capsule, &results);
					}

					float maxOverlap = 0.0f;
					hitNormal = Math::Vector3::Zero;

					for (auto& ret : results)
					{
						if (ret.m_overlapDistance <= 0.0001f)
						{
							continue;
						}

						Math::Vector3 normal = ret.m_hitNDir;
						normal.y = 0.0f;

						if (normal.LengthSquared() < 0.00001f)
						{
							continue;
						}

						normal.Normalize();

						if (ret.m_overlapDistance > maxOverlap)
						{
							maxOverlap = ret.m_overlapDistance;
							hitNormal = normal;
						}
					}

					return maxOverlap > 0.0001f;
				};

			// ノックバック先を確認
			Math::Vector3 targetPos = m_nowPos + move;
			Math::Vector3 wallNormal = Math::Vector3::Zero;

			if (!CheckWall(targetPos, wallNormal))
			{
				// 障害物がなければ通常どおり移動
				m_nowPos = targetPos;
			}
			else
			{
				//==============================================
				// 壁に垂直な移動成分を除去し、壁沿いに滑らせる
				//==============================================
				Math::Vector3 slideMove =
					move - wallNormal * move.Dot(wallNormal);

				slideMove.y = 0.0f;

				// 壁に正面から当たった場合は横方向を選ぶ
				if (slideMove.LengthSquared() < 0.00001f)
				{
					slideMove = GetRight();

					slideMove.y = 0.0f;

					if (slideMove.LengthSquared() > 0.00001f)
					{
						slideMove.Normalize();

						// 敵の右方向を基準に横へ移動
						slideMove *= move.Length();
					}
				}
				else
				{
					slideMove.Normalize();
					slideMove *= move.Length();
				}

				// 横滑り先も障害物に当たるか確認
				Math::Vector3 slideNormal = Math::Vector3::Zero;

				if (!CheckWall(m_nowPos + slideMove, slideNormal))
				{
					m_nowPos += slideMove;
				}
				else
				{
					// 反対方向も試す
					Math::Vector3 reverseMove = -slideMove;

					if (!CheckWall(m_nowPos + reverseMove, slideNormal))
					{
						m_nowPos += reverseMove;
					}
					// 両方向が塞がれている場合は移動しない
				}
			}
		}
	}
}


//==============================================================
// 視野角カリング
//==============================================================
bool EnemyCharacterBase::IsInCameraView() const
{
	auto cam = m_wpCamera.lock();

	if (!cam)
	{
		return true;
	}

	Math::Vector3 center =
		m_nowPos + Math::Vector3(0, 1.0f, 0);

	return cam->IsInCameraView(center, 2.0f);
}

//==============================================================
// スロー・重力の共通更新
//==============================================================
void EnemyCharacterBase::UpdateSlowAndGravity(float dt)
{
	//========================================
	// スロー更新
	//========================================
	UpdateSlowTimer(
		m_isSlow,
		m_slowTimer,
		Application::Instance().GetDeltaTime()
	);

	//========================================
	// 重力
	//========================================
	ApplyGravity(
		m_gravity,
		m_nowPos,
		dt
	);
}

//==============================================================
// 攻撃判定
//==============================================================
void EnemyCharacterBase::DoAttackHitCheck(float range, bool checkHitStop)
{
	// ヒットストップ中は攻撃判定を行わない
	if (checkHitStop && m_hitStopTimer > 0.0f)
	{
		return;
	}

	if (m_attackHitOnce) return;

	auto player = m_wpPlayer.lock();
	if (!player) return;

	if (player->m_isInvincible) return;

	// Playerへの方向
	Math::Vector3 toPlayer =
		player->GetPos() - m_nowPos;

	toPlayer.y = 0.0f;

	float dist = toPlayer.Length();

	if (dist > range) return;
	if (dist < 0.0001f) return;

	toPlayer.Normalize();

	// 正面ベクトル
	Math::Vector3 forward = GetForward();
	forward.y = 0.0f;

	if (forward.LengthSquared() < 0.0001f) return;

	forward.Normalize();

	// 正面判定
	float dot = std::clamp(
		forward.Dot(toPlayer),
		-1.0f,
		1.0f
	);

	float angle = std::acos(dot);

	// 正面90度以内なら攻撃ヒット
	if (angle > DirectX::XMConvertToRadians(90.0f))
	{
		return;
	}

	player->Damage(m_attackDamage, false, false, 1.0f);

	m_attackHitOnce = true;
}

