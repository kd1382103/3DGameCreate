#include "TPSCamera.h"
#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Enemy/EnemyBase.h>
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
	//追加・変更
	auto& input = KdInputManager::Instance();

	// マウスカーソル自由化の切り替え
	if (input.IsPress("ToggleKey"))
	{
		m_mouseFree = !m_mouseFree;
	}

	// カメラの回転
	if (!m_mouseFree) { UpdateRotateByMouse(); }

	m_nowFov += (m_targetFov - m_nowFov) * 0.15f;

	m_spCamera->SetProjectionMatrix(m_nowFov);

	//////////////////////////////////////////////////////////////
	
	// ターゲットの行列(有効な場合利用する)
	Math::Matrix								_targetMat = Math::Matrix::Identity;
	 std::shared_ptr<KdGameObject>	_spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}

	auto player = std::dynamic_pointer_cast<Player>(_spTarget);
	if (player && player->m_lookOn && player->m_lockOnTarget)
	{
		Math::Vector3 camPos = GetPos();
		Math::Vector3 targetPos = player->m_lockOnTarget->GetHitCenter();

		Math::Vector3 dir = targetPos - camPos;
		dir.Normalize();

		// yaw（左右）
		float yaw = atan2(dir.x, dir.z);

		// pitch（上下）
		//float pitch = atan2(dir.y, sqrt(dir.x * dir.x + dir.z * dir.z));

		// カメラ角度を補正（プレイヤーの向きは変えない）
		m_DegAng.y = DirectX::XMConvertToDegrees(yaw);
		//m_DegAng.x = DirectX::XMConvertToDegrees(pitch);
	}

	m_mRotation = GetRotationMatrix();
	m_mWorld = m_mLocalPos * m_mRotation * _targetMat;

	// ↓めり込み防止の為の座標補正計算↓
	// ①当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;
	// レイの長さを設定
	rayInfo.m_range = 1000.f;
	if (_spTarget)
	{
		Math::Vector3 _targetPos = _spTarget->GetPos();
		_targetPos.y += 0.1f;
		rayInfo.m_dir = _targetPos - GetPos();
		rayInfo.m_range = rayInfo.m_dir.Length();
		rayInfo.m_dir.Normalize();
	}

	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	// ②HIT判定対象オブジェクトに総当たり
	for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (spGameObj)
		{
			std::list<KdCollider::CollisionResult> retRayList;
			spGameObj->Intersects(rayInfo, &retRayList);

			// ③ 結果を使って座標を補完する
			// レイに当たったリストから一番近いオブジェクトを検出
			float maxOverLap = 0;
			Math::Vector3 hitPos = {};
			bool hit = false;
			for (auto& ret : retRayList)
			{
				// レイを遮断しオーバーした長さが
				// 一番長いものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}
			if (hit)
			{
				// 何かしらの障害物に当たっている
				Math::Vector3 _hitPos = hitPos;
				_hitPos += rayInfo.m_dir * 0.4f;
				SetPos(_hitPos);
			}
		}
	}
}
