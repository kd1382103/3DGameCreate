#include "TPSCamera.h"
#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Enemy/EnemyBase.h>
#include <Application/GameObject/Boss/BossBase.h>
void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 1.5f, -4.0f);
	m_nowFov = 60.0f;
	m_targetFov = 60.0f;

	m_spCamera->SetProjectionMatrix(m_nowFov);

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
}

void TPSCamera::PostUpdate()
{
	///////////////////////////////////////////////////////////////
	// 追加・変更
	auto& input = KdInputManager::Instance();

	// マウスカーソル自由化の切り替え
	if (input.IsPress("ToggleKey"))
	{
		m_mouseFree = !m_mouseFree;
	}

	// カメラの回転
	if (!m_mouseFree)
	{
		UpdateRotateByMouse();
	}

	m_nowFov += (m_targetFov - m_nowFov);

	m_spCamera->SetProjectionMatrix(m_nowFov);

	//////////////////////////////////////////////////////////////

	// ターゲットの行列(有効な場合利用する)
	Math::Matrix _targetMat = Math::Matrix::Identity;

	std::shared_ptr<KdGameObject> _spTarget = m_wpTarget.lock();

	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}

	auto player = std::dynamic_pointer_cast<Player>(_spTarget);

	if (player && player->m_lookOn && player->m_lockOnTarget)
	{
		Math::Vector3 camPos = GetPos();
		Math::Vector3 targetPos;

		// 通常敵の場合
		if (auto enemy =
			dynamic_cast<EnemyBase*>(player->m_lockOnTarget))
		{
			targetPos = enemy->GetHitCenter();
		}
		// ボスの場合
		else if (auto boss =
			dynamic_cast<BossBase*>(player->m_lockOnTarget))
		{
			targetPos = boss->GetHitCenter();
		}
		else
		{
			targetPos = player->m_lockOnTarget->GetPos();
		}

		Math::Vector3 dir = targetPos - camPos;

		if (dir.LengthSquared() > 0.0001f)
		{
			dir.Normalize();

			// yaw（左右）
			float yaw = atan2(dir.x, dir.z);

			// カメラ角度を補正
			m_DegAng.y = DirectX::XMConvertToDegrees(yaw);
		}
	}

	// カメラの回転行列
	m_mRotation = GetRotationMatrix();

	// 本来のカメラ位置を計算
	m_mWorld = m_mLocalPos * m_mRotation * _targetMat;


	//==============================================================
	// カメラめり込み防止
	// プレイヤー → 本来のカメラ位置へレイを飛ばす
	//==============================================================

	if (_spTarget)
	{
		// プレイヤー位置
		Math::Vector3 targetPos = _spTarget->GetPos();

		// カメラの高さ付近からレイを飛ばす
		targetPos.y += 1.5f;

		// 本来のカメラ位置
		Math::Vector3 cameraPos = GetPos();

		// プレイヤー → カメラ
		Math::Vector3 dir = cameraPos - targetPos;

		// プレイヤーからカメラまでの距離
		float distance = dir.Length();

		if (distance > 0.001f)
		{
			dir.Normalize();

			// レイ情報
			KdCollider::RayInfo rayInfo;

			// レイの発射位置
			rayInfo.m_pos = targetPos;

			// プレイヤーからカメラ方向
			rayInfo.m_dir = dir;

			// プレイヤーから本来のカメラ位置まで
			rayInfo.m_range = distance;

			// ステージを判定
			rayInfo.m_type = KdCollider::TypeGround;

			// 一番近い障害物
			float maxOverLap = 0.0f;

			Math::Vector3 hitPos = Math::Vector3::Zero;

			bool hit = false;

			//==========================================================
			// HIT判定対象オブジェクトに総当たり
			//==========================================================

			for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
			{
				std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();

				if (!spGameObj)
				{
					continue;
				}

				std::list<KdCollider::CollisionResult> retRayList;

				spGameObj->Intersects(rayInfo, &retRayList);

				//======================================================
				// レイの結果を確認
				//======================================================

				for (auto& ret : retRayList)
				{
					// 一番手前の障害物を取得
					if (maxOverLap < ret.m_overlapDistance)
					{
						maxOverLap = ret.m_overlapDistance;

						hitPos = ret.m_hitPos;

						hit = true;
					}
				}
			}

			//==========================================================
			// 障害物に当たった場合
			//==========================================================

			if (hit)
			{
				// 壁に少しめり込まないための余白
				const float cameraOffset = 0.4f;

				// 壁の位置からプレイヤー側へ戻す
				Math::Vector3 newCameraPos =
					hitPos - dir * cameraOffset;

				// カメラの位置だけ変更
				m_mWorld.Translation(newCameraPos);
			}
		}
	}
}
