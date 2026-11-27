#include "EnemyBase.h"
#include <Application/GameObject/Player/Player/Player.h>

#include <Application/Scene/SceneManager.h>
#include <Application/main.h>
#include <Application/GameObject/Enemy/EnemyState/EnemyState.h>

//==============================================================
// Init
//==============================================================

void EnemyBase::Init()
{
	m_animator = KdAnimator();

	// HPゲージ
	InitHPGauge();

	// 3D空間上に出すUI
	InitWorldUI();

	stateMachine = std::make_shared<StateMachine<EnemyBase>>();

	stateMachine->ChangeStateImmediate(
		std::make_unique<EnemyBaseStateIdel>(),
		*this
	);

	StartResolveCommon();
}

//各フェイズごとに出現させる敵の初期化
void EnemyBase::Init(int battleNo)
{
	Init();
}

void EnemyBase::Update()
{
	if (m_isGameEnd) return;

	//========================================
	// 60FPS基準のフレーム倍率
	//========================================
	const float frameScale =
		Application::Instance().GetFPSController().GetFrameScale();

	//==========================================================
	// Resolve中は出現演出だけを進める
	//==========================================================

	if (m_isResolving)
	{
		UpdateResolveCommon(frameScale);
		AdvanceModelAnimation(m_animator, m_model, frameScale);
		return;
	}

	//==========================================================
	// 死亡中
	//==========================================================
	if (m_isDying)
	{
		UpdateDeath(frameScale);
		return;
	}

	//========================================
	// ゲーム全体の速度
	//========================================
	const float timeScale =
		SceneManager::Instance().GetTimeScale();

	//========================================
	// Enemy自身の速度
	//========================================
	const float enemyTimeScale =
		GetTimeScale();

	//========================================
	// 最終的なフレーム倍率
	//========================================
	const float scaledFrameScale =
		frameScale *
		timeScale *
		enemyTimeScale;

	//========================================
	// スロー・重力
	//========================================
	UpdateSlowAndGravity(scaledFrameScale);

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
	// ステート
	//========================================
	if (stateMachine)
	{
		stateMachine->Update(*this);
	}

	//========================================
	// アニメーション
	//========================================
	AdvanceModelAnimation(m_animator, m_model, scaledFrameScale);

	//========================================
	// HPゲージ
	//========================================
	UpdateHPGauge();
	
	//========================================
	// デバッグ
	//========================================
	UpdateDebug();
}

//==============================================================
// PostUpdate（地面判定・壁判定）
//==============================================================
void EnemyBase::PostUpdate()
{
	//========================================
	// 地面
	//========================================
	GroundCollisionCheck();

	//==========================================================
	// ノックバック移動（障害物に当たったら壁沿いに滑る）
	//==========================================================
	NockBackCheck();

	//========================================
	// 壁
	//========================================
	WallCollisionCheck();

	//==========================================================
	// 敵同士の重なり防止
	//==========================================================
	{
		const float minDistance = 1.0f;

		// 自分が出現中・死亡中なら補正しない
		if (!m_isResolving && !m_isDying &&
			!m_isExpired && !m_isGameEnd)
		{
			for (auto& obj : SceneManager::Instance().GetObjList())
			{
				if (obj.get() == this) continue;

				auto other =
					std::dynamic_pointer_cast<EnemyBase>(obj);

				if (!other) continue;

				if (other->m_isResolving ||
					other->m_isDying ||
					other->m_isExpired ||
					other->m_isGameEnd)
				{
					continue;
				}

				Math::Vector3 dir =
					m_nowPos - other->m_nowPos;

				dir.y = 0.0f;

				float distance = dir.Length();

				if (distance >= minDistance) continue;

				// 同じ位置にいる場合は向きを固定して分離
				if (distance < 0.001f)
				{
					dir = std::less<const void*>()(
						static_cast<const void*>(this),
						static_cast<const void*>(other.get()))
						? Math::Vector3(1, 0, 0)
						: Math::Vector3(-1, 0, 0);
				}
				else
				{
					dir /= distance;
				}

				const float pushDistance =
					(minDistance - distance) * 0.5f;

				m_nowPos += dir * pushDistance;
			}
		}
	}

	UpdateWorldMatrixAndUI();
}

//==============================================================
// Draw系
//==============================================================
void EnemyBase::DrawLit()
{
	if (!m_model)
	{
		return;
	}

	//========================================
	// 視錐台カリング
	//========================================
	if (!m_isDying && !IsInCameraView())
	{
		return;
	}

	if (m_isResolving || m_isDying)
	{
		KdShaderManager::Instance().m_StandardShader.SetDissolve(
			m_resolveDissolve,
			&m_resolveEdgeRange,
			&m_resolveEmissive
		);

		KdShaderManager::Instance().m_StandardShader.DrawModel(
			*m_model,
			m_mWorld,
			{ 0.6f, 0.8f, 1.0f, 1.0f }
		);

		return;
	}

	KdShaderManager::Instance().m_StandardShader.DrawModel(
		*m_model,
		m_mWorld,
		{ 0.6f, 0.8f, 1.0f, 1.0f }
	);
}

void EnemyBase::GenerateDepthMapFromLight()
{
	GenerateDepthMapFromLightCommon(m_isDying);
}

void EnemyBase::DrawSprite()
{
	if (m_isResolving || m_isDying)return;
	DrawWorldUI();
}

//==============================================================
// Damage
//==============================================================
void EnemyBase::Damage(float dmg, bool isUltimate, bool finalHit, float knockBackRate)
{
	//==========================================================
	// ダメージ関係（HP更新、フライテキスト、Hitエフェクト描画）
	//==========================================================
	CommonDamage(dmg);

	//==========================================================
	// 死亡処理
	//==========================================================
	if (m_hp <= 0)
	{
		StartDeath();
		return;
	}

	//==========================================================
	// ヒットストップ
	//==========================================================
	m_hitStopTimer = 0.35f;

	//==========================================================
	// 被弾ステート
	//==========================================================
	if (stateMachine)
	{
		stateMachine->ChangeState(
			std::make_unique<EnemyStateHit>()
		);
	}

	//==========================================================
	// プレイヤーのジャスト回避受付も終了
	//==========================================================
	if (auto player = m_wpPlayer.lock())
	{
		player->m_canDodge = false;

		//======================================================
		// ノックバック
		//======================================================
		if (m_canKnockBack)
		{
			Math::Vector3 knockDir =
				m_nowPos - player->GetPos();

			knockDir.y = 0.0f;

			if (knockDir.LengthSquared() > 0.00001f)
			{
				knockDir.Normalize();

				float knockPower =
					StateMachineParameter::KnockBackPower *
					knockBackRate;

				// 必殺技の途中ヒットは弱くする
				if (isUltimate && !finalHit)
				{
					knockPower *= 0.2f;
				}

				// ノックバックの移動量を記録
				// 実際の移動はPostUpdateで障害物を確認してから行う
				m_knockBackMove += knockDir * knockPower;
				m_hasPendingKnockBack = true;
			}
		}
	}
}


void EnemyBase::StartTutorialAttack()
{
	if (!stateMachine) return;

	m_isTutorialAttack = true;
	m_tutorialAttackFinished = false;

	stateMachine->ChangeState(
		std::make_unique<EnemyBaseStateAttack>()
	);
}

void EnemyBase::StopTutorialAttack()
{
	m_isTutorialAttack = false;

	if (stateMachine)
	{
		stateMachine->ChangeState(
			std::make_unique<EnemyBaseStateIdel>()
		);
	}
}

void EnemyBase::LookAtPlayer()
{
	auto player = m_wpPlayer.lock();
	if (!player) return;

	Math::Vector3 dir = player->GetPos() - m_nowPos;
	dir.y = 0.0f;

	if (dir.LengthSquared() < 0.0001f) return;

	dir.Normalize();

	m_angleY = std::atan2f(dir.x, dir.z) + DirectX::XM_PI;
}

void EnemyBase::InitModelAndCollider(
	const std::string& modelPath,
	const std::string& colliderName)
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData(modelPath);

	EnemyBase::Init();

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape(
		colliderName,
		m_model,
		KdCollider::TypeDamage
	);
}

//==============================================================
// 死亡開始
//==============================================================
void EnemyBase::StartDeath()
{
	// すでに死亡中なら何もしない
	if (m_isDying)
	{
		return;
	}

	m_isDying = true;
	m_isDeathAnimationEnd = false;

	//==========================================================
	// 既存のResolve用ディゾルブを再利用
	//==========================================================
	m_resolveTimer = 0.0f;
	m_resolveDissolve = 0.0f;

	//---------------------------------------
	// チュートリアル専用
	//---------------------------------------
	m_isTutorialAttack = false;
	m_tutorialAttackFinished = false;
	m_canAttack = false;

	//==========================================================
	// 死亡処理
	//==========================================================
	ResetDeathCommon();

	//==========================================================
	// 死亡アニメーション
	//==========================================================
	PlayAnimationAuto("Death", false);
}

void EnemyBase::UpdateDebug()
{
	//KdDebugGUI::Instance().ClearLog();

	if (GetAsyncKeyState('3') & 0x8000)
	{
		Damage(m_hpMax, false, false, 1.0f);
	}
}

//==============================================================
// 死亡演出更新
//==============================================================
void EnemyBase::UpdateDeath(float frameScale)
{
	//==========================================================
	// 死亡アニメーション再生中
	//==========================================================
	if (!m_isDeathAnimationEnd)
	{
		AdvanceModelAnimation(m_animator, m_model, frameScale);

		if (m_animator.IsAnimationEnd())
		{
			m_isDeathAnimationEnd = true;

			m_resolveTimer = 0.0f;
			m_resolveDissolve = 0.0f;
		}

		return;
	}

	//==========================================================
	// 死亡アニメーション終了後
	// 最終ポーズを維持したままディゾルブ
	//==========================================================
	m_resolveTimer += frameScale;

	float t = m_resolveTimer / m_resolveDuration;

	t = std::clamp(t, 0.0f, 1.0f);

	// 1.0 → 0.0
	m_resolveDissolve =  t;

	//==========================================================
	// ディゾルブ終了
	//==========================================================
	if (m_resolveTimer >= m_resolveDuration)
	{
		m_resolveTimer = m_resolveDuration;
		m_resolveDissolve = 1.0f;

		// 完全に消えたら削除
		m_isExpired = true;
	}
}