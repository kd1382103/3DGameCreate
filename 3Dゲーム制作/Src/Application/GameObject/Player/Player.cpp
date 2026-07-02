#include "Player.h"
//#include<Application/GameObject/Camera/CameraBase.h>
#include<Application/Scene/SceneManager.h>
void Player::Init()
{
	if (!m_model)
	{
		m_model = std::make_shared<KdModelWork>();
		m_model->SetModelData("Asset/Models/player/manModel/Player.gltf");

		//3Dアニメーションの描画
		auto anim = m_model->GetAnimation(0);
		m_animator.SetAnimation(anim, true); // ループ再生
	}
}

void Player::Update()
{

	float moveSpeed = 0.1f;
	float runSpeed = 0.2f;
	bool isMoving = false;

	//================================================================================
	// 移動処理
	//================================================================================

	m_dir = Math::Vector3::Zero;

	bool isRunning = false;

	if (!m_isLanding && !m_isAttacking)
	{
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			isRunning = true;
			moveSpeed = runSpeed;
		}

		if (GetAsyncKeyState('W') & 0x8000) { m_dir += {0, 0, 1}; }
		if (GetAsyncKeyState('S') & 0x8000) { m_dir += {0, 0, -1}; }
		if (GetAsyncKeyState('A') & 0x8000) { m_dir += {-1, 0, 0}; }
		if (GetAsyncKeyState('D') & 0x8000) { m_dir += {1, 0, 0}; }
	}
	/*std::shared_ptr<CameraBase> spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		m_dir = m_dir.TransformNormal(m_dir, spCamera->GetRotationYMatrix());
	}*/

	m_dir.Normalize();

	// ★ 実際に移動方向があるかどうかで判定する
	isMoving = (m_dir.LengthSquared() > 0.0001f);
	
	// ★ 着地アニメ中は移動方向を完全にゼロにする
	if (m_isLanding && !m_isAttacking)
	{
		m_dir = { 0,0,0 };
		isMoving = false;
	}

	m_nowPos += m_dir * moveSpeed;

	//================================================================================
	// ジャンプ処理
	//================================================================================

	if (!m_isJumping && (GetAsyncKeyState(VK_SPACE) & 0x8000))
	{
		m_isJumping = true;			// ジャンプ開始
		m_gravity = -0.2f;			// 重力反転
		m_nowAnimIndex = 16;		// Jump_Start
		m_animator.SetAnimation(m_model->GetAnimation(16), false);
	}

	m_gravity += 0.005f;         // 重力加算
	m_nowPos.y -= m_gravity;     // y位置更新（放物線）

	// --- アニメーション切り替え ---
	if (m_nowPos.y > 0.0f)
	{
		// 上昇中
		if (m_isJumping && m_gravity < 0.0f)
		{
			if (m_nowAnimIndex != 16)
			{
				m_nowAnimIndex = 16;
				m_animator.SetAnimation(m_model->GetAnimation(16), false);
			}
		}
		else
		{
			// 下降
			if (m_nowAnimIndex != 15)
			{
				m_nowAnimIndex = 15;
				m_animator.SetAnimation(m_model->GetAnimation(15), false);
			}
		}
	}
	else
	{
		// 着地した瞬間
		if (m_isJumping)
		{
			m_isJumping = false;

			// 着地の強さを判定
			bool isHardLanding = (m_gravity >= 0.4f); 

			m_gravity = 0.0f;

			if (isHardLanding)
			{
				// ★ 高所落下 → 着地硬直あり
				m_nowAnimIndex = 14; // Jump_Land
				m_animator.SetAnimation(m_model->GetAnimation(14), false);

				m_isLanding = true;  // ← 着地硬直フラグ
				return;              // ← Idle に上書きされないように止める
			}
			else
			{
				//アニメーションの都合で、着地硬直なしの時は、着地アニメーションを再生しない（現状）
				
				m_isLanding = false; // ← 着地硬直なし
				
				// return しない → そのまま Walk/Run に移行できる
			}
		}

		if (m_animator.IsAnimationEnd())
		{
			m_isLanding = false;
		}

		if (!m_isLanding && !m_isAttacking)
		{
			int nextAnim = 9; // Idle

			if (isMoving)
			{
				if (isRunning) nextAnim = 36; // Run
				else nextAnim = 41;           // Walk
			}

			if (nextAnim != m_nowAnimIndex)
			{
				m_nowAnimIndex = nextAnim;
				m_animator.SetAnimation(m_model->GetAnimation(nextAnim), true);
			}
		}
	}
	//================================================================================
	//　攻撃処理
	//================================================================================

	//現状、アニメーションのみ当たり判定等は後ほど実装
	if (!m_isAttacking && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		m_isAttacking = true;

		m_nowAnimIndex = 39;
		m_animator.SetAnimation(m_model->GetAnimation(39), false);
	}

	// 攻撃アニメ終了チェック
	if (m_isAttacking && m_animator.IsAnimationEnd())
	{
		m_isAttacking = false;
	}


	//================================================================================
	// アニメーション処理
	//================================================================================
	m_animator.AdvanceTime(m_model->WorkNodes(), 1.0f);

	if (m_model->NeedCalcNodeMatrices())
	{
		m_model->CalcNodeMatrices();
	}

	//================================================================================
	// ワールド行列の更新
	//================================================================================

//================================================================================
// キャラ回転（自然な向き変更）
//================================================================================

// 移動している時だけ向きを変える
	if (isMoving)
	{
		Math::Vector3 nowDir = m_mWorld.Backward();   // 現在の向き
		Math::Vector3 targetDir = m_dir;              // 移動方向

		// 方向が変わった時だけ回転処理
		if (targetDir.LengthSquared() > 0.0001f)
		{
			// 正規化（安全）
			nowDir.Normalize();
			targetDir.Normalize();

			// dot の範囲を安全にクランプ
			float dot = std::clamp(nowDir.Dot(targetDir), -1.0f, 1.0f);

			// 差分角度（ラジアン）
			float angle = acos(dot);

			// 外積で符号判定
			Math::Vector3 cross = nowDir.Cross(targetDir);
			if (cross.y < 0) angle = -angle;


			// ★ 回転速度を制限（自然な向き変更の核心）
			const float rotSpeed = DirectX::XMConvertToRadians(5.0f); // 1フレーム最大5度
			angle = std::clamp(angle, -rotSpeed, rotSpeed);

			// ★ 現在角度に加算（これが一番重要）
			m_angleY += angle;
		}
	}

	// 回転行列
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(-m_angleY + DirectX::XMConvertToRadians(180.0f));

	// 平行移動
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_nowPos);

	// ワールド行列
	m_mWorld = rotMat * transMat;

	//===============================================================================
	// LogWindowに表示(発表時は非表示)
	//===============================================================================

	KdDebugGUI::Instance().ClearLog();

	//アニメーションの番号一覧をLogWindowに表示
	for (int i = 0; ; i++)
	{
		auto anim = m_model->GetAnimation(i);
		if (!anim) break; // 取得できなくなったら終了

		KdDebugGUI::Instance().AddLog("%d : %s\n", i, anim->m_name.c_str());
	}

	auto anim = m_model->GetAnimation(14);
	KdDebugGUI::Instance().AddLog("Jump_Land length: %f\n", anim->m_maxLength);

	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.x);
	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.z);
	//KdDebugGUI::Instance().AddLog("%f\n", m_nowPos.y);

}

void Player::PostUpdate()
{
	//================================================================================
	//	当たり判定
	//================================================================================

	//==================================
	//　カプセルで判定したい場合
	//==================================
	//KdCollider::CapsuleInfo capsule;
	//capsule.m_type = KdCollider::TypeGround;
	//capsule.m_radius = 0.5f;

	//// 下端
	//capsule.m_start = m_nowPos;
	//capsule.m_start.y += 0.0f;   // 足元

	//// 上端
	//capsule.m_end = m_nowPos;
	//capsule.m_end.y += 1.0f;     // 胴体くらいまで

	//当たり判定タイプ設定
	//capsule.m_type = KdCollider::TypeGround;

	//Math::Color col = Math::Color(1, 0, 0, 1); // 赤
	//m_pDebugWire->AddDebugCapsule(capsule.m_start, capsule.m_end, capsule.m_radius, col);

	//カプセルに当たったオブジェクト情報格納
	//std::list<KdCollider::CollisionResult> retCapsuleList;

	//========================================
	
	float maxOverlap = 0;
	Math::Vector3 hitPos = Math::Vector3::Zero;
	bool hit = false;

	KdCollider::RayInfo rayInfo;
	rayInfo.m_pos = m_nowPos;

	// 許容範囲を設定
	static const float enableStepHigh = 0.2f;
	rayInfo.m_pos.y += enableStepHigh;			

	// 方向を設定
	rayInfo.m_dir = { 0.0f, -1.0f, 0.0f };

	// 長さを設定
	rayInfo.m_range = enableStepHigh + m_gravity;

	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	std::list<KdCollider::CollisionResult> retRayList;

	// 当たり判定
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		obj->Intersects(rayInfo, &retRayList);
	}

	// 当たったリストから一番近いオブジェクトを検出

	for (auto& ret : retRayList)
	{
		// レイが当たったオブジェクトの中から
		// 「m_overlapDistance = 貫通した長さ」が一番長いものを探す
		// 「m_overlapDistance が一番長い = 一番近くで当たった」と判定できる
		if (maxOverlap < ret.m_overlapDistance)
		{
			maxOverlap = ret.m_overlapDistance;
			hitPos = ret.m_hitPos;
			hit = true;
		}
	}

	// 当たっていたら
	if (hit)
	{
		m_nowPos = hitPos;	// レイの着弾地点に着地
		m_gravity = 0.0f;
	}
}

void Player::DrawLit()
{	if (!m_model) { return; }

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model,m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	if (!m_model) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
