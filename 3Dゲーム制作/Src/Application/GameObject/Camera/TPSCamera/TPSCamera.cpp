#include "TPSCamera.h"

#include <Application/GameObject/Player/Player/Player.h>
#include <Application/GameObject/Enemy/EnemyBase.h>
#include <Application/GameObject/Boss/BossBase.h>
#include <Application/main.h>

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	m_mLocalPos =
		Math::Matrix::CreateTranslation(
			0, 1.5f, -4.0f
		);

	m_nowFov = 60.0f;
	m_targetFov = 60.0f;

	m_spCamera->SetProjectionMatrix(m_nowFov);

	SetCursorPos(
		m_FixMousePos.x,
		m_FixMousePos.y
	);
}

void TPSCamera::PostUpdate()
{
	///////////////////////////////////////////////////////////////
	// 入力
	auto& input = KdInputManager::Instance();

	// マウスカーソル自由化の切り替え
	if (input.IsPress("ToggleKey"))
	{
		m_mouseFree = !m_mouseFree;
	}

	//=============================================================
	// カメラの回転
	//=============================================================
	if (!m_mouseFree &&
		!m_ultimateCamera &&
		m_ultimateCameraMove == UltimateCameraMove::None)
	{
		UpdateRotateByMouse();
	}

	m_nowFov +=
		(m_targetFov - m_nowFov);

	m_spCamera->SetProjectionMatrix(
		m_nowFov
	);

	//////////////////////////////////////////////////////////////

	//=============================================================
	// ターゲットの行列
	//=============================================================
	Math::Matrix _targetMat =
		Math::Matrix::Identity;

	std::shared_ptr<KdGameObject> _spTarget =
		m_wpTarget.lock();

	if (_spTarget)
	{
		_targetMat =
			Math::Matrix::CreateTranslation(
				_spTarget->GetPos()
			);
	}

	auto player =
		std::dynamic_pointer_cast<Player>(
			_spTarget
		);

	//=============================================================
	// ロックオン中のカメラ
	//=============================================================
	if (!m_ultimateCamera &&
		m_ultimateCameraMove == UltimateCameraMove::None &&
		player &&
		player->m_lookOn &&
		player->m_lockOnTarget)
	{
		Math::Vector3 camPos =
			GetPos();

		Math::Vector3 targetPos;

		// 通常敵の場合
		if (auto enemy =
			dynamic_cast<EnemyBase*>(
				player->m_lockOnTarget))
		{
			targetPos =
				enemy->GetHitCenter();
		}
		// ボスの場合
		else if (auto boss =
			dynamic_cast<BossBase*>(
				player->m_lockOnTarget))
		{
			targetPos =
				boss->GetHitCenter();
		}
		else
		{
			targetPos =
				player->m_lockOnTarget->GetPos();
		}

		Math::Vector3 dir =
			targetPos - camPos;

		if (dir.LengthSquared() > 0.0001f)
		{
			dir.Normalize();

			// yaw（左右）
			float yaw =
				atan2f(
					dir.x,
					dir.z
				);

			m_DegAng.y =
				DirectX::XMConvertToDegrees(yaw);
		}
	}

	//=============================================================
	// 必殺技カメラ移動
	//=============================================================
	if (m_ultimateCameraMove !=
		UltimateCameraMove::None)
	{
		//=========================================================
		// 移動時間
		//=========================================================
		m_ultimateMoveTime += Application::Instance().GetDeltaTime();

		float t =
			m_ultimateMoveTime /
			m_ultimateMoveDuration;

		if (t >= 1.0f)
		{
			t = 1.0f;
		}

		//=========================================================
		// 滑らかな移動
		//=========================================================
		float smoothT =
			t * t * (3.0f - 2.0f * t);

		Math::Vector3 cameraPos =
			Math::Vector3::Lerp(
				m_ultimateMoveStartPos,
				m_ultimateMoveTargetPos,
				smoothT
			);

		//=========================================================
		// プレイヤーを見る
		//=========================================================
		if (player)
		{
			Math::Vector3 targetPos =
				player->GetPos();

			targetPos.y += 1.5f;

			Math::Vector3 lookDir =
				targetPos - cameraPos;

			if (lookDir.LengthSquared() > 0.0001f)
			{
				lookDir.Normalize();

				// 左右
				float yaw =
					atan2f(
						lookDir.x,
						lookDir.z
					);

				m_DegAng.y =
					DirectX::XMConvertToDegrees(yaw);

				// 上下
				float pitch =
					asinf(-lookDir.y);

				m_DegAng.x =
					DirectX::XMConvertToDegrees(pitch);
			}
		}

		//=========================================================
		// カメラの回転
		//=========================================================
		m_mRotation =
			GetRotationMatrix();

		//=========================================================
		// カメラ位置
		//=========================================================
		m_mWorld =
			m_mRotation;

		m_mWorld.Translation(
			cameraPos
		);

		//=========================================================
		// 移動終了
		//=========================================================
		if (t >= 1.0f)
		{
			// 開始移動完了
			if (m_ultimateCameraMove ==
				UltimateCameraMove::Start)
			{
				// 必殺技中の固定カメラへ
				m_ultimateCameraMove =
					UltimateCameraMove::None;
			}
			// 終了移動完了
			else if (m_ultimateCameraMove ==
				UltimateCameraMove::End)
			{
				// 必殺技カメラ終了
				m_ultimateCameraMove =
					UltimateCameraMove::None;

				m_ultimateCamera = false;
			}
		}
	}
	else
	{
		//=============================================================
		// カメラの回転行列
		//=============================================================
		m_mRotation =
			GetRotationMatrix();

		//=============================================================
		// 必殺技中
		//=============================================================
		if (m_ultimateCamera && player)
		{
			//=========================================================
			// プレイヤーの向いている方向
			//=========================================================
			Math::Vector3 forward =
				player->GetForward();

			forward.y = 0.0f;

			if (forward.LengthSquared() > 0.0001f)
			{
				forward.Normalize();
			}

			//=========================================================
			// プレイヤーの右方向
			//=========================================================
			Math::Vector3 right =
				player->GetRight();

			right.y = 0.0f;

			if (right.LengthSquared() > 0.0001f)
			{
				right.Normalize();
			}

			//=========================================================
			// プレイヤー基準の右後ろ30°
			//
			// 真後ろ = 180°
			// 右へ30° = 150°
			//=========================================================
			float angle =
				DirectX::XMConvertToRadians(
					m_ultimateCameraAngle
				);

			Math::Vector3 cameraDir =
				forward * cosf(angle)
				+
				right * sinf(angle);

			cameraDir.y = 0.0f;

			if (cameraDir.LengthSquared() > 0.0001f)
			{
				cameraDir.Normalize();
			}

			//=========================================================
			// カメラ位置
			//=========================================================
			Math::Vector3 cameraPos =
				player->GetPos();

			cameraPos +=
				cameraDir *
				m_ultimateCameraDistance;

			cameraPos.y = GetPos().y;

			//=========================================================
			// プレイヤーを見る
			//=========================================================
			Math::Vector3 targetPos =
				player->GetPos();

			targetPos.y += 1.5f;

			Math::Vector3 lookDir =
				targetPos - cameraPos;

			if (lookDir.LengthSquared() > 0.0001f)
			{
				lookDir.Normalize();

				float yaw =
					atan2f(lookDir.x, lookDir.z);

				float pitch =
					asinf(-lookDir.y);

				m_DegAng.y =
					DirectX::XMConvertToDegrees(yaw);

				m_DegAng.x =
					DirectX::XMConvertToDegrees(pitch);
			}

			//=========================================================
			// カメラの回転
			//=========================================================
			m_mRotation =
				GetRotationMatrix();

			//=========================================================
			// カメラ位置を直接設定
			//=========================================================
			m_mWorld =
				m_mRotation;

			m_mWorld.Translation(
				cameraPos
			);
		}
		else
		{
			//=========================================================
			// 通常時
			//=========================================================
			m_mWorld =
				m_mLocalPos *
				m_mRotation *
				_targetMat;
		}
	}

	//==============================================================
	// カメラめり込み防止
	//==============================================================
	if (_spTarget)
	{
		// プレイヤー位置
		Math::Vector3 targetPos =
			_spTarget->GetPos();

		// カメラの高さ付近からレイを飛ばす
		targetPos.y += 1.5f;

		// 本来のカメラ位置
		Math::Vector3 cameraPos =
			GetPos();

		// プレイヤー → カメラ
		Math::Vector3 dir =
			cameraPos - targetPos;

		// プレイヤーからカメラまでの距離
		float distance =
			dir.Length();

		if (distance > 0.001f)
		{
			dir.Normalize();

			// レイ情報
			KdCollider::RayInfo rayInfo;

			rayInfo.m_pos =
				targetPos;

			rayInfo.m_dir =
				dir;

			rayInfo.m_range =
				distance;

			rayInfo.m_type =
				KdCollider::TypeGround;

			float maxOverLap = 0.0f;

			Math::Vector3 hitPos =
				Math::Vector3::Zero;

			bool hit = false;

			//==========================================================
			// HIT判定対象オブジェクトに総当たり
			//==========================================================
			for (std::weak_ptr<KdGameObject> wpGameObj :
				m_wpHitObjectList)
			{
				std::shared_ptr<KdGameObject> spGameObj =
					wpGameObj.lock();

				if (!spGameObj)
				{
					continue;
				}

				std::list<KdCollider::CollisionResult>
					retRayList;

				spGameObj->Intersects(
					rayInfo,
					&retRayList
				);

				//======================================================
				// レイの結果を確認
				//======================================================
				for (auto& ret : retRayList)
				{
					// 一番手前の障害物を取得
					if (maxOverLap <
						ret.m_overlapDistance)
					{
						maxOverLap =
							ret.m_overlapDistance;

						hitPos =
							ret.m_hitPos;

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
					hitPos -
					dir * cameraOffset;

				// カメラの位置だけ変更
				m_mWorld.Translation(
					newCameraPos
				);
			}
		}
	}
}
//=============================================================
// 必殺技カメラ開始
//=============================================================
void TPSCamera::StartUltimateCamera()
{
	if (m_ultimateCamera)
	{
		return;
	}

	auto player =
		std::dynamic_pointer_cast<Player>(
			m_wpTarget.lock()
		);

	if (!player)
	{
		return;
	}

	//=========================================================
	// 必殺技カメラ開始
	//=========================================================
	m_ultimateCamera = true;

	// 現在のカメラ位置
	m_ultimateMoveStartPos =
		GetPos();

	//=========================================================
	// プレイヤー基準の右後ろ30°
	//=========================================================
	Math::Vector3 forward =
		player->GetForward();

	forward.y = 0.0f;

	if (forward.LengthSquared() > 0.0001f)
	{
		forward.Normalize();
	}

	Math::Vector3 right =
		player->GetRight();

	right.y = 0.0f;

	if (right.LengthSquared() > 0.0001f)
	{
		right.Normalize();
	}

	float angle =
		DirectX::XMConvertToRadians(
			m_ultimateCameraAngle
		);

	Math::Vector3 cameraDir =
		forward * cosf(angle)
		+
		right * sinf(angle);

	cameraDir.y = 0.0f;
	cameraDir.Normalize();

	//=========================================================
	// 目的地
	//=========================================================
	m_ultimateMoveTargetPos =
		player->GetPos();


	m_ultimateMoveTargetPos +=
		cameraDir * m_ultimateCameraDistance;

	m_ultimateMoveTargetPos.y =
		m_ultimateMoveStartPos.y;

	//=========================================================
	// 移動開始
	//=========================================================
	m_ultimateMoveTime = 0.0f;

	m_ultimateCameraMove =
		UltimateCameraMove::Start;
}
//=============================================================
// 必殺技カメラ終了
//=============================================================
void TPSCamera::EndUltimateCamera()
{
	if (!m_ultimateCamera)
	{
		return;
	}

	auto player =
		std::dynamic_pointer_cast<Player>(
			m_wpTarget.lock()
		);

	if (!player)
	{
		m_ultimateCamera = false;
		return;
	}

	//=========================================================
	// 現在位置を開始地点にする
	//=========================================================
	m_ultimateMoveStartPos =
		GetPos();

	//=========================================================
	// プレイヤーの真後ろ
	//=========================================================
	Math::Vector3 backward =
		-player->GetForward();

	backward.y = 0.0f;

	if (backward.LengthSquared() > 0.0001f)
	{
		backward.Normalize();
	}

	m_ultimateMoveTargetPos =
		player->GetPos();

	m_ultimateMoveTargetPos +=
		backward * m_ultimateCameraDistance;

	m_ultimateMoveTargetPos.y =
		m_ultimateMoveStartPos.y;

	//=========================================================
	// 移動開始
	//=========================================================
	m_ultimateMoveTime = 0.0f;

	m_ultimateCameraMove =
		UltimateCameraMove::End;
}