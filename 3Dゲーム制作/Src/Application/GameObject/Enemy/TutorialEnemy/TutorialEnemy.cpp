#include "TutorialEnemy.h"

void TutorialEnemy::Init()
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Enemy/manModel/Enemy.gltf");

	EnemyBase::Init();

	//---------------------------------------
	// チュートリアル用設定
	//---------------------------------------

	m_hpMax = 999999;
	m_hp = m_hpMax;

	// 動かない
	m_moveSpeed = 0.0f;

	m_attackDist = 1.5f;

	// 普段はダメージを与えない
	m_attackDamage = 0.0f;

	// 索敵しない
	detectRange = 0.0f;

	//---------------------------------------
	// アニメーション
	//---------------------------------------

	animIdleIndex = 9;
	animAttackIndex = 39;

	//---------------------------------------
	// ノックバックしない
	//---------------------------------------

	SetCanKnockBack(false);

	//---------------------------------------
	// コライダー
	//---------------------------------------

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape(
		"TutorialEnemy",
		m_model,
		KdCollider::TypeDamage
	);
}