#include "KdCollider.h"

// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// KdCollider
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 当たり判定形状の登録関数群
///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, std::unique_ptr<KdCollisionShape> spShape)
{
	if (!spShape) { return; }

	m_collisionShapes.emplace(name.data(), std::move(spShape));
}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, const DirectX::BoundingSphere& sphere, UINT type)
{
	RegisterCollisionShape(name, std::make_unique<KdSphereCollision>(sphere, type));
}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, const DirectX::BoundingBox& box, UINT type)
{
	RegisterCollisionShape(name, std::make_unique<KdBoxCollision>(box, type));
}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, const DirectX::BoundingOrientedBox& box, UINT type)
{
	RegisterCollisionShape(name, std::make_unique<KdBoxCollision>(box, type));
}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, const Math::Vector3& localPos, float radius, UINT type)
{
	RegisterCollisionShape(name, std::make_unique<KdSphereCollision>(localPos, radius, type));
}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, const std::shared_ptr<KdModelData>& model, UINT type)
{
	RegisterCollisionShape(name, std::make_unique<KdModelCollision>(model, type));
}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, KdModelData* model, UINT type)
{
	RegisterCollisionShape(name, std::make_unique<KdModelCollision>(std::shared_ptr<KdModelData>(model), type));
}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, const std::shared_ptr<KdModelWork>& model, UINT type)
{
	RegisterCollisionShape(name, std::make_unique<KdModelCollision>(model, type));
}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, KdModelWork* model, UINT type)
{
	RegisterCollisionShape(name, std::make_unique<KdModelCollision>(std::shared_ptr<KdModelWork>(model), type));
}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, const std::shared_ptr<KdPolygon> polygon, UINT type)
{
	RegisterCollisionShape(name, std::make_unique<KdPolygonCollision>(polygon, type));
}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::RegisterCollisionShape(std::string_view name, KdPolygon* polygon, UINT type)
{
	RegisterCollisionShape(name, std::make_unique<KdPolygonCollision>(std::shared_ptr<KdPolygon>(polygon), type));
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// コライダーvs球に登録された任意の形状の当たり判定
// 球に合わせて何のために当たり判定をするのか type を渡す必要がある
// 第3引数に詳細結果の受け取る機能が付いている
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdCollider::Intersects(const SphereInfo& targetShape, const Math::Matrix& ownerMatrix, std::list<KdCollider::CollisionResult>* pResults) const
{
	// 当たり判定無効のタイプの場合は返る
	if (targetShape.m_type & m_disableType) { return false; }

	bool isHit = false;

	for (auto& collisionShape : m_collisionShapes)
	{
		// 用途が一致していない当たり判定形状はスキップ
		if (!(targetShape.m_type & collisionShape.second->GetType())) { continue; }

		KdCollider::CollisionResult tmpRes;
		KdCollider::CollisionResult* pTmpRes = pResults ? &tmpRes : nullptr;

		if (collisionShape.second->Intersects(targetShape.m_sphere, ownerMatrix, pTmpRes))
		{
			isHit = true;

			// 詳細な衝突結果を必要としない場合は1つでも接触して返す
			if (!pResults) { break; }

			pResults->push_back(tmpRes);
		}
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// コライダーvsBOXに登録された任意の形状の当たり判定
// BOXに合わせて何のために当たり判定をするのか type を渡す必要がある
// 第3引数に詳細結果の受け取る機能が付いている
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdCollider::Intersects(const BoxInfo& targetShape, const Math::Matrix& ownerMatrix, std::list<KdCollider::CollisionResult>* pResults) const
{
	// 当たり判定無効のタイプの場合は返る
	if (targetShape.m_type & m_disableType) { return false; }

	bool isHit = false;

	for (auto& collisionShape : m_collisionShapes)
	{
		// 用途が一致していない当たり判定形状はスキップ
		if (!(targetShape.m_type & collisionShape.second->GetType())) { continue; }

		KdCollider::CollisionResult tmpRes;
		KdCollider::CollisionResult* pTmpRes = pResults ? &tmpRes : nullptr;

		bool isIntersects = (targetShape.CheckBoxType(BoxInfo::BoxType::BoxAABB)) ? collisionShape.second->Intersects(targetShape.m_Abox, ownerMatrix, pTmpRes) :
			collisionShape.second->Intersects(targetShape.m_Obox, ownerMatrix, pTmpRes);
		if (isIntersects)
		{
			isHit = true;

			// 詳細な衝突結果を必要としない場合は1つでも接触して返す
			if (!pResults) { break; }

			pResults->push_back(tmpRes);
		}
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// コライダーvsレイに登録された任意の形状の当たり判定
// レイに合わせて何のために当たり判定をするのか type を渡す必要がある
// 第3引数に詳細結果の受け取る機能が付いている
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdCollider::Intersects(const RayInfo& targetShape, const Math::Matrix& ownerMatrix, std::list<KdCollider::CollisionResult>* pResults) const
{
	// 当たり判定無効のタイプの場合は返る
	if (targetShape.m_type & m_disableType) { return false; }

	// レイの方向ベクトルが存在しない場合は判定不能なのでそのまま返る
	if (!targetShape.m_dir.LengthSquared())
	{
		assert(0 && "KdCollider::Intersects：レイの方向ベクトルが存在していないため、正しく判定できません");

		return false;
	}

	bool isHit = false;

	for (auto& collisionShape : m_collisionShapes)
	{
		// 用途が一致していない当たり判定形状はスキップ
		if (!(targetShape.m_type & collisionShape.second->GetType())) { continue; }

		KdCollider::CollisionResult tmpRes;
		KdCollider::CollisionResult* pTmpRes = pResults ? &tmpRes : nullptr;

		if (collisionShape.second->Intersects(targetShape, ownerMatrix, pTmpRes))
		{
			isHit = true;

			// 詳細な衝突結果を必要としない場合は1つでも接触して返す
			if (!pResults) { break; }

			pResults->push_back(tmpRes);
		}
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// コライダーvsカプセルに登録された任意の形状の当たり判定
// レイに合わせて何のために当たり判定をするのか type を渡す必要がある
// 第3引数に詳細結果の受け取る機能が付いている
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdCollider::Intersects(const CapsuleInfo& targetCapsule,
	const Math::Matrix& ownerMatrix,
	std::list<KdCollider::CollisionResult>* pResults) const
{
	if (targetCapsule.m_type & m_disableType) { return false; }

	bool isHit = false;

	for (auto& [name, shape] : m_collisionShapes)
	{
		if (!(targetCapsule.m_type & shape->GetType())) { continue; }

		KdCollider::CollisionResult tmpRes;
		KdCollider::CollisionResult* pTmpRes = pResults ? &tmpRes : nullptr;
		
		if (shape->Intersects(targetCapsule, ownerMatrix, pTmpRes))
		{
			isHit = true;

			if (!pResults) { break; }

			pResults->push_back(tmpRes);
		}
	}
	return isHit;
}


// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 任意のCollisionShapeを検索して有効/無効を切り替える
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::SetEnable(std::string_view name, bool flag)
{
	auto targetCol = m_collisionShapes.find(name.data());

	if (targetCol != m_collisionShapes.end())
	{
		targetCol->second->SetEnable(flag);
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 特定のタイプの有効/無効を切り替える
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::SetEnable(int type, bool flag)
{
	// 有効にしたい
	if (flag)
	{
		m_disableType &= ~type;
	}
	// 無効にしたい
	else
	{
		m_disableType |= type;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 全てのCollisionShapeの有効/無効を一気に切り替える
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCollider::SetEnableAll(bool flag)
{
	for (auto& col : m_collisionShapes)
	{
		col.second->SetEnable(flag);
	}
}


// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// SphereCollision
// 球形の形状
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 球vs球の当たり判定
// 判定回数は 1 回　計算自体も軽く最も軽量な当たり判定　計算回数も固定なので処理効率は安定
// 片方の球の判定を0にすれば単純な距離判定も作れる
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdSphereCollision::Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes)
{
	if (!m_enable) { return false; }

	DirectX::BoundingSphere myShape;

	m_shape.Transform(myShape, world);

	// 球同士の当たり判定
	bool isHit = myShape.Intersects(target);

	// 詳細リザルトが必要無ければ即結果を返す
	if (!pRes) { return isHit; }

	// 当たった時のみ計算
	if (isHit)
	{
		// お互いに当たらない最小距離
		float needDistance = target.Radius + myShape.Radius;

		// 自身から相手に向かう方向ベクトル
		pRes->m_hitDir = (Math::Vector3(target.Center) - Math::Vector3(myShape.Center));
		float betweenDistance = pRes->m_hitDir.Length();

		// 重なり量 = お互い当たらない最小距離 - お互いの実際距離
		pRes->m_overlapDistance = needDistance - betweenDistance;

		pRes->m_hitDir.Normalize();

		// 当たった座標はお互いの球の衝突の中心点
		pRes->m_hitPos = myShape.Center + pRes->m_hitDir * (myShape.Radius + pRes->m_overlapDistance * 0.5f);
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 球vsBOX(AABB)の当たり判定
// 判定回数は 1 回　計算自体も軽く最も軽量な当たり判定　計算回数も固定なので処理効率は安定
// 片方の球の判定を0にすれば単純な距離判定も作れる
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdSphereCollision::Intersects(const DirectX::BoundingBox& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes)
{
	if (!m_enable) { return false; }

	DirectX::BoundingSphere myShape;

	m_shape.Transform(myShape, world);

	// 球vsBOXの当たり判定
	bool isHit = myShape.Intersects(target);

	// 詳細リザルトが必要無ければ即結果を返す
	if (!pRes) { return isHit; }

	// 当たった時のみ計算
	if (isHit)
	{
		// 点をOBBのローカル座標系へ変換(これでOBBからAABBの判定にできる)
		Math::Vector3 _pointCenter = myShape.Center - Math::Vector3(target.Center);

		// BOXの最近接点を求める
		Math::Vector3 _outPos = { 0,0,0 };
		for (int i = 0; i < 3; i++)
		{
			float dist = (&_pointCenter.x)[i];
			if ((&_pointCenter.x)[i] > (&target.Extents.x)[i])
			{
				dist = (&target.Extents.x)[i];
			}
			else if (dist < -(&target.Extents.x)[i])
			{
				dist = -(&target.Extents.x)[i];
			}
			(&_outPos.x)[i] += dist;
		}
		_outPos += target.Center;

		// 自身から相手に向かう方向ベクトル
		pRes->m_hitDir = (_outPos - Math::Vector3(myShape.Center));
		float betweenDistance = pRes->m_hitDir.Length();

		// 重なり量 = お互い当たらない最小距離 - お互いの実際距離
		pRes->m_overlapDistance = myShape.Radius - betweenDistance;

		pRes->m_hitDir.Normalize();
		pRes->m_hitPos = myShape.Center + pRes->m_hitDir * (myShape.Radius + pRes->m_overlapDistance * 0.5f);
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 球vsBOX(OBB)の当たり判定
// 判定回数は 1 回　計算自体も軽く最も軽量な当たり判定　計算回数も固定なので処理効率は安定
// 片方の球の判定を0にすれば単純な距離判定も作れる
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdSphereCollision::Intersects(const DirectX::BoundingOrientedBox& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes)
{
	if (!m_enable) { return false; }

	DirectX::BoundingSphere myShape;

	m_shape.Transform(myShape, world);

	// 球vsBOXの当たり判定
	bool isHit = myShape.Intersects(target);

	// 詳細リザルトが必要無ければ即結果を返す
	if (!pRes) { return isHit; }

	// 当たった時のみ計算
	if (isHit)
	{
		// OBBの回転(クォータニオン)
		DirectX::XMFLOAT4 obbQuat = target.Orientation;

		// 点をOBBのローカル座標系へ変換(これでOBBからAABBの判定にできる)
		Math::Vector3 _pointCenter = XMVector3InverseRotate(myShape.Center - Math::Vector3(target.Center), Math::Vector4(obbQuat));

		// BOXの最近接点を求める
		Math::Vector3 _outPos = { 0,0,0 };
		for (int i = 0; i < 3; i++)
		{
			float dist = (&_pointCenter.x)[i];
			if ((&_pointCenter.x)[i] > (&target.Extents.x)[i])
			{
				dist = (&target.Extents.x)[i];
			}
			else if (dist < -(&target.Extents.x)[i])
			{
				dist = -(&target.Extents.x)[i];
			}
			(&_outPos.x)[i] += dist;
		}
		// OBBのローカル座標系からワールドへ戻す
		_outPos = XMVector3Rotate(_outPos, Math::Vector4(obbQuat));
		_outPos += target.Center;

		// 自身から相手に向かう方向ベクトル
		pRes->m_hitDir = (_outPos - Math::Vector3(myShape.Center));
		float betweenDistance = pRes->m_hitDir.Length();

		// 重なり量 = お互い当たらない最小距離 - お互いの実際距離
		pRes->m_overlapDistance = myShape.Radius - betweenDistance;

		pRes->m_hitDir.Normalize();
		pRes->m_hitPos = myShape.Center + pRes->m_hitDir * (myShape.Radius + pRes->m_overlapDistance * 0.5f);
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 球vsレイの当たり判定
// 判定回数は 1 回　計算回数が固定なので処理効率は安定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdSphereCollision::Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes)
{
	if (!m_enable) { return false; }

	DirectX::BoundingSphere myShape;

	m_shape.Transform(myShape, world);

	float hitDistance = 0.0f;

	bool isHit = myShape.Intersects(target.m_pos, target.m_dir, hitDistance);

	// 判定限界距離を加味
	isHit &= (target.m_range >= hitDistance);

	// 詳細リザルトが必要無ければ即結果を返す
	if (!pRes) { return isHit; }

	// 当たった時のみ計算
	if (isHit)
	{
		// レイ発射位置 + レイの当たった位置までのベクトル 
		pRes->m_hitPos = target.m_pos + target.m_dir * hitDistance;

		pRes->m_hitDir = target.m_dir * (-1);

		pRes->m_overlapDistance = target.m_range - hitDistance;
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 球vsカプセルの当たり判定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

bool KdSphereCollision::Intersects(const KdCollider::CapsuleInfo& target,const Math::Matrix& world,KdCollider::CollisionResult* pRes)
{
	if (!m_enable) return false;

	// 自分の球をワールド変換
	DirectX::BoundingSphere myShape;
	m_shape.Transform(myShape, world);

	// カプセル線分のワールド変換
	Math::Vector3 s = Math::Vector3::Transform(target.m_start, world);
	Math::Vector3 e = Math::Vector3::Transform(target.m_end, world);

	Math::Vector3 seg = e - s;
	float segLen = seg.Length();
	Math::Vector3 segN = seg / segLen;

	// 球中心から線分への射影
	Math::Vector3 c = Math::Vector3(myShape.Center);
	float t = (c-s).Dot(segN);
	t = std::clamp(t, 0.0f, segLen);

	Math::Vector3 closest = s + segN * t;

	// 距離判定
	float dist = (closest - c).Length();
	float hitDist = myShape.Radius + target.m_radius;

	if (dist > hitDist) return false;

	// 詳細不要なら true だけ返す
	if (!pRes) return true;

	// 押し返し方向
	Math::Vector3 hitDir = c - closest;
	float betweenDistance = hitDir.Length();
	hitDir.Normalize();
	pRes->m_hitDir = hitDir;
	pRes->m_overlapDistance = hitDist - betweenDistance;

	// 衝突位置（球側）
	pRes->m_hitPos = closest + pRes->m_hitDir * (target.m_radius - pRes->m_overlapDistance * 0.5f);

	// 法線方向
	pRes->m_hitNDir = pRes->m_hitDir;

	return true;
}


// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// BOXCollision
// BOXの形状
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// BOXvs球の当たり判定
// 判定回数は 1 回　計算自体も軽く最も軽量な当たり判定　計算回数も固定なので処理効率は安定
// 片方の球の判定を0にすれば単純な距離判定も作れる
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdBoxCollision::Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes)
{
	if (!m_enable) { return false; }

	DirectX::BoundingBox			myAABBShape;
	DirectX::BoundingOrientedBox	myOBBShape;
	m_Abox.Transform(myAABBShape, world);
	m_Obox.Transform(myOBBShape, world);

	DirectX::XMFLOAT4				obbQuat = Math::Vector4::Zero;
	Math::Vector3					myShapeCenter = (!m_IsOriented) ? myAABBShape.Center : myOBBShape.Center;

	// 球vsBOXの当たり判定
	bool isHit = (!m_IsOriented) ? myAABBShape.Intersects(target) : myOBBShape.Intersects(target);

	// 詳細リザルトが必要無ければ即結果を返す
	if (!pRes) { return isHit; }

	// 当たった時のみ計算
	if (isHit)
	{
		// 点をOBBのローカル座標系へ変換(これでOBBからAABBの判定にできる)
		Math::Vector3 _pointCenter = Math::Vector3::Zero;
		if (!m_IsOriented)
		{
			_pointCenter = myShapeCenter - Math::Vector3(target.Center);
		}
		else
		{
			// OBBの回転(クォータニオン)
			obbQuat = myOBBShape.Orientation;
			_pointCenter = XMVector3InverseRotate(myShapeCenter - Math::Vector3(target.Center), Math::Vector4(obbQuat));
		}

		// BOXの最近接点を求める
		Math::Vector3 _outPos = { 0,0,0 };
		Math::Vector3 _myExtents = (!m_IsOriented) ? myAABBShape.Extents : myOBBShape.Extents;
		for (int i = 0; i < 3; i++)
		{
			float dist = (&_pointCenter.x)[i];
			if ((&_pointCenter.x)[i] > (&_myExtents.x)[i])
			{
				dist = (&_myExtents.x)[i];
			}
			else if (dist < -(&_myExtents.x)[i])
			{
				dist = -(&_myExtents.x)[i];
			}
			(&_outPos.x)[i] += dist;
		}
		// OBBのローカル座標系からワールドへ戻す
		if (m_IsOriented)_outPos = XMVector3Rotate(_outPos, Math::Vector4(obbQuat));
		_outPos += target.Center;

		// 自身から相手に向かう方向ベクトル
		pRes->m_hitDir = (_outPos - Math::Vector3(myShapeCenter));
		float betweenDistance = pRes->m_hitDir.Length();

		// 重なり量 = お互い当たらない最小距離 - お互いの実際距離
		pRes->m_overlapDistance = target.Radius - betweenDistance;

		pRes->m_hitDir.Normalize();
		pRes->m_hitPos = myShapeCenter + pRes->m_hitDir * (target.Radius + pRes->m_overlapDistance * 0.5f);
	}

	return isHit;
}
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// BOXvsBOX(AABB)の当たり判定
// 判定回数は 1 回　計算自体も軽く最も軽量な当たり判定　計算回数も固定なので処理効率は安定
// 片方の球の判定を0にすれば単純な距離判定も作れる
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdBoxCollision::Intersects(const DirectX::BoundingBox& target, const Math::Matrix& world, KdCollider::CollisionResult* /*pRes*/)
{
	if (!m_enable) { return false; }

	// BOXvsBOX(AABB)の当たり判定
	DirectX::BoundingBox			myAABBShape;
	DirectX::BoundingOrientedBox	myOBBShape;
	m_Abox.Transform(myAABBShape, world);
	m_Obox.Transform(myOBBShape, world);

	bool isHit = (!m_IsOriented) ? myAABBShape.Intersects(target) : myOBBShape.Intersects(target);

	// 即結果を返す(HITしたかどうかだけが知れる)
	return isHit;
}
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// BOXvsBOX(OBB)の当たり判定
// 判定回数は 1 回　計算自体も軽く最も軽量な当たり判定　計算回数も固定なので処理効率は安定
// 片方の球の判定を0にすれば単純な距離判定も作れる
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdBoxCollision::Intersects(const DirectX::BoundingOrientedBox& target, const Math::Matrix& world, KdCollider::CollisionResult* /*pRes*/)
{
	if (!m_enable) { return false; }

	// BOXvsBOX(OBB)の当たり判定
	DirectX::BoundingBox			myAABBShape;
	DirectX::BoundingOrientedBox	myOBBShape;
	m_Abox.Transform(myAABBShape, world);
	m_Obox.Transform(myOBBShape, world);

	bool isHit = (!m_IsOriented) ? myAABBShape.Intersects(target) : myOBBShape.Intersects(target);

	// 即結果を返す(HITしたかどうかだけが知れる)
	return isHit;
}
bool KdBoxCollision::Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* /*pRes*/)
{
	if (!m_enable) { return false; }

	// AABB vs レイ
	float AABBdist = FLT_MAX;

	// BOXvsBOX(OBB)の当たり判定
	DirectX::BoundingBox			myAABBShape;
	DirectX::BoundingOrientedBox	myOBBShape;
	m_Abox.Transform(myAABBShape, world);
	m_Obox.Transform(myOBBShape, world);
	bool isHit = (!m_IsOriented) ? myAABBShape.Intersects(target.m_pos, target.m_dir, AABBdist) : myOBBShape.Intersects(target.m_pos, target.m_dir, AABBdist);

	// 即結果を返す(HITしたかどうかだけが知れる)
	return isHit;
}

//追加

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// BOXvsカプセルの当たり判定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
//
//bool KdBoxCollision::Intersects(
//	const KdCollider::CapsuleInfo& target,
//	const Math::Matrix& world,
//	KdCollider::CollisionResult* pRes)
//{
//	if (!m_enable) return false;
//
//	// BOX のワールド形状
//	DirectX::BoundingBox aabbWS;
//	DirectX::BoundingOrientedBox obbWS;
//	m_Abox.Transform(aabbWS, world);
//	m_Obox.Transform(obbWS, world);
//
//	// カプセル線分のワールド変換
//	Math::Vector3 s = Math::Vector3::Transform(target.m_start, world);
//	Math::Vector3 e = Math::Vector3::Transform(target.m_end, world);
//
//	Math::Vector3 seg = e - s;
//	float segLen = seg.Length();
//	Math::Vector3 segN = seg / segLen;
//
//	// BOX中心
//	Math::Vector3 boxCenter = (!m_IsOriented)
//		? Math::Vector3(aabbWS.Center)
//		: Math::Vector3(obbWS.Center);
//
//	// BOX中心から線分への射影
//	float t = (boxCenter - s).Dot(segN);
//	t = std::clamp(t, 0.0f, segLen);
//
//	// カプセル線分上の最近接点
//	Math::Vector3 capsulePoint = s + segN * t;
//
//	// カプセルを球に変換
//	DirectX::BoundingSphere sphere;
//	sphere.Center = capsulePoint;
//	sphere.Radius = target.m_radius;
//
//	// まず高速判定
//	bool isHit = (!m_IsOriented)
//		? sphere.Intersects(aabbWS)
//		: sphere.Intersects(obbWS);
//
//	if (!isHit) return false;
//	if (!pRes) return true;
//
//	// BOX の最近接点計算（SphereVSBox と同じ）
//	Math::Vector3 localPoint;
//
//	DirectX::XMFLOAT4 obbQuat = { 0,0,0,1 };
//
//	if (!m_IsOriented)
//	{
//		localPoint = capsulePoint - Math::Vector3(aabbWS.Center);
//	}
//	else
//	{
//		obbQuat = obbWS.Orientation;
//		localPoint = XMVector3InverseRotate(
//			capsulePoint - Math::Vector3(obbWS.Center),
//			Math::Vector4(obbQuat)
//		);
//	}
//
//	Math::Vector3 outPos = { 0,0,0 };
//	Math::Vector3 ext = (!m_IsOriented)
//		? Math::Vector3(aabbWS.Extents)
//		: Math::Vector3(obbWS.Extents);
//
//	for (int i = 0; i < 3; i++)
//	{
//		float dist = (&localPoint.x)[i];
//		if (dist > (&ext.x)[i]) dist = (&ext.x)[i];
//		else if (dist < -(&ext.x)[i]) dist = -(&ext.x)[i];
//		(&outPos.x)[i] = dist;
//	}
//
//	if (m_IsOriented)
//	{
//		outPos = XMVector3Rotate(outPos, Math::Vector4(obbQuat));
//		outPos += Math::Vector3(obbWS.Center);
//	}
//	else
//	{
//		outPos += Math::Vector3(aabbWS.Center);
//	}
//
//	// 押し返し方向
//	Math::Vector3 hitDir = outPos - capsulePoint;
//	float betweenDistance = hitDir.Length();
//	hitDir.Normalize();
//	pRes->m_hitDir = hitDir;
//	pRes->m_overlapDistance = target.m_radius - betweenDistance;
//
//	// 衝突位置
//	pRes->m_hitPos =
//		capsulePoint + pRes->m_hitDir * (target.m_radius + pRes->m_overlapDistance * 0.5f);
//
//	pRes->m_hitNDir = pRes->m_hitDir;
//
//	return true;
//}


// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// ModelCollision
// 3Dメッシュの形状
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// モデルvs球の当たり判定
// 判定回数は メッシュの個数 x 各メッシュのポリゴン数 計算回数がモデルのデータ依存のため処理効率は不安定
// 単純に計算回数が多くなる可能性があるため重くなりがち
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdModelCollision::Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes)
{
	// 当たり判定が無効 or 形状が解放済みなら判定せず返る
	if (!m_enable || !m_shape) { return false; }

	std::shared_ptr<KdModelData> spModelData = m_shape->GetData();

	// データが無ければ判定不能なので返る
	if (!spModelData) { return false; }

	const std::vector<KdModelData::Node>& dataNodes = spModelData->GetOriginalNodes();
	const std::vector<KdModelWork::Node>& workNodes = m_shape->GetNodes();

	// 各メッシュに押される用の球・押される毎に座標を更新する必要がある
	DirectX::BoundingSphere pushedSphere = target;
	// 計算用にFloat3 → Vectorへ変換
	Math::Vector3 pushedSphereCenter = DirectX::XMLoadFloat3(&pushedSphere.Center);

	bool isHit = false;

	Math::Vector3 hitPos;
	Math::Vector3 hitNDir;

	// 当たり判定ノードとのみ当たり判定
	for (int index : spModelData->GetCollisionMeshNodeIndices())
	{
		const KdModelData::Node& dataNode = dataNodes[index];
		const KdModelWork::Node& workNode = workNodes[index];

		// あり得ないはずだが一応チェック
		if (!dataNode.m_spMesh) { continue; }

		CollisionMeshResult tmpResult;
		CollisionMeshResult* pTmpResult = pRes ? &tmpResult : nullptr;

		// メッシュと球形の当たり判定実行
		if (!MeshIntersect(*dataNode.m_spMesh, pushedSphere, workNode.m_worldTransform * world, pTmpResult))
		{
			continue;
		}

		// 詳細リザルトが必要無ければ即結果を返す
		if (!pRes) { return true; }

		isHit = true;

		// 重なった分押し戻す
		pushedSphereCenter = DirectX::XMVectorAdd(pushedSphereCenter, DirectX::XMVectorScale(tmpResult.m_hitDir, tmpResult.m_overlapDistance));

		DirectX::XMStoreFloat3(&pushedSphere.Center, pushedSphereCenter);

		// とりあえず当たった座標で更新
		hitPos = tmpResult.m_hitPos;

		// 最後に当たった面の法線情報を記憶しておく
		hitNDir = tmpResult.m_hitDir;
	}

	if (pRes && isHit)
	{
		// 最後に当たった座標が使用される
		pRes->m_hitPos = hitPos;

		// 複数のメッシュに押された最終的な位置 - 移動前の位置 = 押し出しベクトル
		pRes->m_hitDir = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&pushedSphere.Center), DirectX::XMLoadFloat3(&target.Center));

		pRes->m_overlapDistance = DirectX::XMVector3Length(pRes->m_hitDir).m128_f32[0];

		pRes->m_hitDir = DirectX::XMVector3Normalize(pRes->m_hitDir);

		// 最後に当たった面の法線が使用される
		pRes->m_hitNDir = hitNDir;
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// モデルvsBOX(AABB)の当たり判定
// 判定回数は メッシュの個数 x 各メッシュのポリゴン数 計算回数がモデルのデータ依存のため処理効率は不安定
// 単純に計算回数が多くなる可能性があるため重くなりがち
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdModelCollision::Intersects(const DirectX::BoundingBox& /*target*/, const Math::Matrix& /*world*/, KdCollider::CollisionResult* /*pRes*/)
{
	// TODO: 当たり計算は各自必要に応じて拡張して下さい
	return false;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// モデルvsBOX(OBB)の当たり判定
// 判定回数は メッシュの個数 x 各メッシュのポリゴン数 計算回数がモデルのデータ依存のため処理効率は不安定
// 単純に計算回数が多くなる可能性があるため重くなりがち
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdModelCollision::Intersects(const DirectX::BoundingOrientedBox& /*target*/, const Math::Matrix& /*world*/, KdCollider::CollisionResult* /*pRes*/)
{
	// TODO: 当たり計算は各自必要に応じて拡張して下さい
	return false;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// モデルvsレイの当たり判定
// 判定回数は メッシュの個数 x 各メッシュのポリゴン数 計算回数がモデルのデータ依存のため処理効率は不安定
// 単純に計算回数が多くなる可能性があるため重くなりがち
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdModelCollision::Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes)
{
	// 当たり判定が無効 or 形状が解放済みなら判定せず返る
	if (!m_enable || !m_shape) { return false; }

	std::shared_ptr<KdModelData> spModelData = m_shape->GetData();

	// データが無ければ判定不能なので返る
	if (!spModelData) { return false; }

	CollisionMeshResult nearestResult;

	bool isHit = false;

	const std::vector<KdModelData::Node>& dataNodes = spModelData->GetOriginalNodes();
	const std::vector<KdModelWork::Node>& workNodes = m_shape->GetNodes();

	for (int index : spModelData->GetCollisionMeshNodeIndices())
	{
		const KdModelData::Node& dataNode = dataNodes[index];
		const KdModelWork::Node& workNode = workNodes[index];

		if (!dataNode.m_spMesh) { continue; }

		CollisionMeshResult tmpResult;
		CollisionMeshResult* pTmpResult = pRes ? &tmpResult : nullptr;

		if (!MeshIntersect(*dataNode.m_spMesh, target.m_pos, target.m_dir, target.m_range,
			workNode.m_worldTransform * world, pTmpResult))
		{
			continue;
		}

		// 詳細リザルトが必要無ければ即結果を返す
		if (!pRes) { return true; }

		isHit = true;

		if (tmpResult.m_overlapDistance > nearestResult.m_overlapDistance)
		{
			nearestResult = tmpResult;
		}
	}

	if (pRes && isHit)
	{
		// 最も近くで当たったヒット情報をコピーする
		pRes->m_hitPos = nearestResult.m_hitPos;

		pRes->m_hitDir = nearestResult.m_hitDir;

		pRes->m_overlapDistance = nearestResult.m_overlapDistance;

		// 最も近くで当たった面の法線が使用される
		pRes->m_hitNDir = nearestResult.m_hitNDir;
	}

	return isHit;
}

// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// PolygonCollision
// 多角形ポリゴン(頂点の集合体)の形状
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 多角形ポリゴン(頂点の集合体)vs球の当たり判定
// 判定回数は ポリゴンの個数 計算回数がポリゴンデータ依存のため処理効率は不安定
// 単純に計算回数が多くなる可能性があるため重くなりがち
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdPolygonCollision::Intersects(const DirectX::BoundingSphere& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes)
{
	// 当たり判定が無効 or 形状が解放済みなら判定せず返る
	if (!m_enable || !m_shape) { return false; }

	CollisionMeshResult result;
	CollisionMeshResult* pTmpResult = pRes ? &result : nullptr;

	// メッシュと球形の当たり判定実行
	if (!PolygonsIntersect(*m_shape, target, world, pTmpResult))
	{
		// 当たっていなければ無条件に返る
		return false;
	}

	if (pRes)
	{
		pRes->m_hitPos = result.m_hitPos;

		pRes->m_hitDir = result.m_hitDir;

		pRes->m_overlapDistance = result.m_overlapDistance;

		pRes->m_hitNDir = result.m_hitNDir;
	}

	return true;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 多角形ポリゴン(頂点の集合体)vsBOX(AABB)の当たり判定
// 判定回数は ポリゴンの個数 計算回数がポリゴンデータ依存のため処理効率は不安定
// 単純に計算回数が多くなる可能性があるため重くなりがち
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdPolygonCollision::Intersects(const DirectX::BoundingBox& /*target*/, const Math::Matrix& /*world*/, KdCollider::CollisionResult* /*pRes*/)
{
	// TODO: 当たり計算は各自必要に応じて拡張して下さい
	return false;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 多角形ポリゴン(頂点の集合体)vsBOX(OBB)の当たり判定
// 判定回数は ポリゴンの個数 計算回数がポリゴンデータ依存のため処理効率は不安定
// 単純に計算回数が多くなる可能性があるため重くなりがち
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdPolygonCollision::Intersects(const DirectX::BoundingOrientedBox& /*target*/, const Math::Matrix& /*world*/, KdCollider::CollisionResult* /*pRes*/)
{
	// TODO: 当たり計算は各自必要に応じて拡張して下さい
	return false;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 多角形ポリゴン(頂点の集合体)vsレイの当たり判定
// 判定回数は ポリゴンの個数 計算回数がポリゴンデータ依存のため処理効率は不安定
// 単純に計算回数が多くなる可能性があるため重くなりがち
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool KdPolygonCollision::Intersects(const KdCollider::RayInfo& target, const Math::Matrix& world, KdCollider::CollisionResult* pRes)
{
	// 当たり判定が無効 or 形状が解放済みなら判定せず返る
	if (!m_enable || !m_shape) { return false; }

	CollisionMeshResult result;
	CollisionMeshResult* pTmpResult = pRes ? &result : nullptr;

	if (!PolygonsIntersect(*m_shape, target.m_pos, target.m_dir, target.m_range, world, pTmpResult))
	{
		// 当たっていなければ無条件に返る
		return false;
	}

	if (pRes)
	{
		pRes->m_hitPos = result.m_hitPos;

		pRes->m_hitDir = result.m_hitDir;

		pRes->m_overlapDistance = result.m_overlapDistance;

		pRes->m_hitNDir = result.m_hitNDir;
	}

	return true;
}


//====================================================================================================================
//	追加
//====================================================================================================================

// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// CapsuleCollision
// カプセルの形状
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

//=======================================================
// カプセル vs 球の当たり判定
//=======================================================

bool KdCapsuleCollision::Intersects(const DirectX::BoundingSphere& target,const Math::Matrix& world,KdCollider::CollisionResult* pRes)
{
	Math::Vector3 ws = Math::Vector3::Transform(m_start, world);
	Math::Vector3 we = Math::Vector3::Transform(m_end, world);

	Math::Vector3 seg = we - ws;
	float segLen = seg.Length();
	Math::Vector3 segN = seg / segLen;

	Math::Vector3 v = Math::Vector3(target.Center) - ws;
	float t = v.Dot(segN);        // ★ 修正ポイント
	t = std::clamp(t, 0.0f, segLen);

	Math::Vector3 closestPoint = ws + segN * t;

	DirectX::BoundingSphere capsuleSphere;
	capsuleSphere.Center = closestPoint;
	capsuleSphere.Radius = m_radius;

	bool isHit = capsuleSphere.Intersects(target);
	if (!pRes || !isHit) return isHit;

	// 押し返し処理（SphereCollision と同じ）
	Math::Vector3 hitDir = Math::Vector3(target.Center) - closestPoint;
	float betweenDistance = hitDir.Length();

	hitDir.Normalize();
	pRes->m_hitDir = hitDir;
	pRes->m_overlapDistance = (m_radius + target.Radius) - betweenDistance;
	pRes->m_hitPos = closestPoint + pRes->m_hitDir * (m_radius - pRes->m_overlapDistance * 0.5f);

	return true;
}

//=======================================================
// カプセル vs Box(AABB)の当たり判定
//=======================================================
bool KdBoxCollision::Intersects(const KdCollider::CapsuleInfo& target,const Math::Matrix& world,KdCollider::CollisionResult* pRes)
{
	if (!m_enable) return false;

	// BOXのワールド形状
	DirectX::BoundingBox aabbWS;
	DirectX::BoundingOrientedBox obbWS;

	m_Abox.Transform(aabbWS, world);
	m_Obox.Transform(obbWS, world);

	// カプセル線分のワールド変換
	Math::Vector3 s = Math::Vector3::Transform(target.m_start, world);
	Math::Vector3 e = Math::Vector3::Transform(target.m_end, world);

	Math::Vector3 seg = e - s;
	float segLen = seg.Length();
	Math::Vector3 segN = seg / segLen;

	// BOX中心との最近接点
	Math::Vector3 c = (!m_IsOriented) ? Math::Vector3(aabbWS.Center)
		: Math::Vector3(obbWS.Center);

	float t = (c - s).Dot(segN);
	t = std::clamp(t, 0.0f, segLen);

	Math::Vector3 capsulePoint = s + segN * t;

	// カプセルを球として扱う
	DirectX::BoundingSphere sphere;
	sphere.Center = capsulePoint;
	sphere.Radius = target.m_radius;

	bool isHit = (!m_IsOriented)
		? sphere.Intersects(aabbWS)
		: sphere.Intersects(obbWS);

	if (!pRes || !isHit) return isHit;

	// 最近接点計算（AABB / OBB 共通）
	Math::Vector3 localPoint;

	if (!m_IsOriented)
	{
		localPoint = capsulePoint - Math::Vector3(aabbWS.Center);
	}
	else
	{
		DirectX::XMFLOAT4 q = obbWS.Orientation;
		localPoint = XMVector3InverseRotate(capsulePoint - Math::Vector3(obbWS.Center), Math::Vector4(q));
	}

	Math::Vector3 outPos = { 0,0,0 };
	Math::Vector3 ext = (!m_IsOriented) ? Math::Vector3(aabbWS.Extents)
		: Math::Vector3(obbWS.Extents);

	for (int i = 0; i < 3; i++)
	{
		float dist = (&localPoint.x)[i];
		if (dist > (&ext.x)[i]) dist = (&ext.x)[i];
		else if (dist < -(&ext.x)[i]) dist = -(&ext.x)[i];
		(&outPos.x)[i] = dist;
	}

	if (m_IsOriented)
	{
		DirectX::XMFLOAT4 q = obbWS.Orientation;
		outPos = XMVector3Rotate(outPos, Math::Vector4(q));
		outPos += Math::Vector3(obbWS.Center);
	}
	else
	{
		outPos += Math::Vector3(aabbWS.Center);
	}

	Math::Vector3 hitDir = outPos - capsulePoint;
	float betweenDistance = hitDir.Length();

	hitDir.Normalize();
	pRes->m_hitDir = hitDir;
	pRes->m_overlapDistance = target.m_radius - betweenDistance;

	pRes->m_hitPos =
		capsulePoint + pRes->m_hitDir * (target.m_radius + pRes->m_overlapDistance * 0.5f);

	pRes->m_hitNDir = pRes->m_hitDir;

	return true;
}


//=======================================================
// カプセル vs Box(OBB)の当たり判定
//=======================================================

bool KdCapsuleCollision::Intersects(
	const DirectX::BoundingOrientedBox& target,
	const Math::Matrix& world,
	KdCollider::CollisionResult* pRes)
{
	if (!m_enable) return false;

	// ★ target をワールド変換（KdBoxCollision と同じ）
	DirectX::BoundingOrientedBox obbWS;
	target.Transform(obbWS, world);

	// カプセルのワールド座標
	Math::Vector3 s = Math::Vector3::Transform(m_start, world);
	Math::Vector3 e = Math::Vector3::Transform(m_end, world);

	Math::Vector3 seg = e - s;
	float segLen = seg.Length();
	Math::Vector3 segN = seg / segLen;

	// OBB中心との最近接点
	Math::Vector3 c = Math::Vector3(obbWS.Center);
	float t = (c - s).Dot(segN);   // ★ Dot はメンバー関数
	t = std::clamp(t, 0.0f, segLen);

	Math::Vector3 capsulePoint = s + segN * t;

	// OBB の回転
	DirectX::XMFLOAT4 obbQuat = obbWS.Orientation;

	// カプセルの球を OBB のローカル空間へ
	Math::Vector3 localPoint =
		XMVector3InverseRotate(capsulePoint - Math::Vector3(obbWS.Center), Math::Vector4(obbQuat));

	// ローカル空間で AABB として扱う
	DirectX::BoundingBox localBox;
	localBox.Center = { 0,0,0 };
	localBox.Extents = obbWS.Extents;

	DirectX::BoundingSphere sphere;
	sphere.Center = localPoint;
	sphere.Radius = m_radius;

	bool isHit = localBox.Intersects(sphere);
	if (!pRes || !isHit) return isHit;

	// AABB の最近接点
	Math::Vector3 outPos = { 0,0,0 };
	for (int i = 0; i < 3; i++)
	{
		float dist = (&localPoint.x)[i];
		float ext = (&localBox.Extents.x)[i];

		if (dist > ext) dist = ext;
		else if (dist < -ext) dist = -ext;

		(&outPos.x)[i] = dist;
	}

	// ローカル → ワールドへ戻す
	Math::Vector3 rotated = Math::Vector3(XMVector3Rotate(outPos, Math::Vector4(obbQuat)));
	Math::Vector3 worldHitPos = rotated + Math::Vector3(obbWS.Center);

	// 押し返し方向
	Math::Vector3 hitDir = worldHitPos - capsulePoint;
	float betweenDistance = hitDir.Length();

	hitDir.Normalize();
	pRes->m_hitDir = hitDir;
	pRes->m_overlapDistance = m_radius - betweenDistance;

	pRes->m_hitPos =
		capsulePoint + pRes->m_hitDir * (m_radius + pRes->m_overlapDistance * 0.5f);

	return true;
}

//=======================================================
// カプセル vs カプセルの当たり判定
//=======================================================

bool KdCapsuleCollision::Intersects(const KdCollider::CapsuleInfo& target,const Math::Matrix& world,KdCollider::CollisionResult* pRes)
{
	if (!m_enable) return false;

	// 自分のカプセル（ワールド）
	Math::Vector3 p1 = Math::Vector3::Transform(m_start, world);
	Math::Vector3 q1 = Math::Vector3::Transform(m_end, world);

	// 相手のカプセル（ワールド）
	Math::Vector3 p2 = Math::Vector3::Transform(target.m_start, world);
	Math::Vector3 q2 = Math::Vector3::Transform(target.m_end, world);

	// 線分同士の最近接点を求める
	Math::Vector3 c1, c2;
	float dist = SegmentSegmentDistance(p1, q1, p2, q2, c1, c2);

	float limit = m_radius + target.m_radius;

	if (dist > limit) return false;

	if (!pRes) return true;

	// 押し返し方向
	Math::Vector3 hitDir = c2 - c1;
	float betweenDistance = hitDir.Length();

	hitDir.Normalize();
	pRes->m_hitDir = hitDir;
	pRes->m_overlapDistance = limit - betweenDistance;

	// 衝突位置（線分1側）
	pRes->m_hitPos = c1 + pRes->m_hitDir * (m_radius - pRes->m_overlapDistance * 0.5f);

	return true;
}
//=======================================================
// カプセル vs レイの当たり判定
//=======================================================

bool KdCapsuleCollision::Intersects(const KdCollider::RayInfo& target,const Math::Matrix& world,KdCollider::CollisionResult* pRes)
{
	if (!m_enable) return false;

	// カプセル線分のワールド変換
	Math::Vector3 s = Math::Vector3::Transform(m_start, world);
	Math::Vector3 e = Math::Vector3::Transform(m_end, world);

	// Ray の情報
	Math::Vector3 rayPos = target.m_pos;
	Math::Vector3 rayDir = target.m_dir;

	Math::Vector3 segPoint, rayPoint;

	float dist = SegmentRayDistance(s, e, rayPos, rayDir, segPoint, rayPoint);

	// 半径以内なら衝突
	if (dist > m_radius) return false;

	// 詳細不要なら true だけ返す
	if (!pRes) return true;

	// 押し返し方向
	Math::Vector3 hitDir = rayPoint - segPoint;
	float betweenDistance = hitDir.Length();

	hitDir.Normalize();
	pRes->m_hitDir = hitDir;
	pRes->m_overlapDistance = m_radius - betweenDistance;

	// 衝突位置（カプセル側）
	pRes->m_hitPos = segPoint + pRes->m_hitDir * (m_radius - pRes->m_overlapDistance * 0.5f);

	// 法線方向（レイ → カプセル）
	pRes->m_hitNDir = pRes->m_hitDir;

	return true;
}

//=======================================================
// カプセル vs モデル・ポリゴンの当たり判定（ダミー）
//=======================================================

bool KdModelCollision::Intersects(const KdCollider::CapsuleInfo& target,const Math::Matrix& world,KdCollider::CollisionResult* pRes)
{
	if (!m_enable || !m_shape) return false;

	auto spModelData = m_shape->GetData();
	if (!spModelData) return false;

	const auto& dataNodes = spModelData->GetOriginalNodes();
	const auto& workNodes = m_shape->GetNodes();

	// カプセル線分のワールド変換
	Math::Vector3 s = Math::Vector3::Transform(target.m_start, world);
	Math::Vector3 e = Math::Vector3::Transform(target.m_end, world);

	Math::Vector3 seg = e - s;
	float segLen = seg.Length();
	Math::Vector3 segN = seg / segLen;

	const int samples = 10;
	float step = segLen / samples;

	bool isHit = false;
	CollisionMeshResult nearest;
	float nearestOverlap = 0.0f;

	// モデルのコリジョンメッシュノードを走査
	for (int nodeIndex : spModelData->GetCollisionMeshNodeIndices())
	{
		const auto& dataNode = dataNodes[nodeIndex];
		const auto& workNode = workNodes[nodeIndex];

		if (!dataNode.m_spMesh) continue;

		auto mesh = dataNode.m_spMesh;
		Math::Matrix meshWorld = workNode.m_worldTransform * world;

		DirectX::BoundingSphere sphere;
		sphere.Radius = target.m_radius;

		// カプセル線分を球に分解して判定
		for (int i = 0; i <= samples; i++)
		{
			float t = step * i;
			Math::Vector3 pos = s + segN * t;
			sphere.Center = pos;

			CollisionMeshResult tmp;
			if (MeshIntersect(*mesh, sphere, meshWorld, &tmp))
			{
				isHit = true;

				if (tmp.m_overlapDistance > nearestOverlap)
				{
					nearestOverlap = tmp.m_overlapDistance;
					nearest = tmp;
				}
			}
		}
	}

	if (!isHit) return false;

	if (pRes)
	{
		pRes->m_hitPos = nearest.m_hitPos;
		pRes->m_hitDir = nearest.m_hitDir;
		pRes->m_hitNDir = nearest.m_hitNDir;
		pRes->m_overlapDistance = nearest.m_overlapDistance;
	}

	return true;
}

bool KdPolygonCollision::Intersects(const KdCollider::CapsuleInfo& target,const Math::Matrix& world,KdCollider::CollisionResult* pRes)
{
	if (!m_enable || !m_polygon) return false;

	// カプセル線分のワールド変換
	Math::Vector3 s = Math::Vector3::Transform(target.m_start, world);
	Math::Vector3 e = Math::Vector3::Transform(target.m_end, world);

	Math::Vector3 seg = e - s;
	float segLen = seg.Length();
	Math::Vector3 segN = seg / segLen;

	// ポリゴンの中心（頂点の平均）
	Math::Vector3 polyCenter = Math::Vector3::Zero;
	const auto& verts = m_polygon->GetVertices();
	for (auto& v : verts)
	{
		polyCenter += Math::Vector3::Transform(v.pos, world);
	}
	polyCenter /= (float)verts.size();

	// 線分上の最近接点
	float t = (polyCenter - s).Dot(segN);
	t = std::clamp(t, 0.0f, segLen);

	Math::Vector3 capsulePoint = s + segN * t;

	// カプセルを球として扱う
	DirectX::BoundingSphere sphere;
	sphere.Center = capsulePoint;
	sphere.Radius = target.m_radius;

	// ★ 既存の SphereVSPolygon 判定を使う
	return Intersects(sphere, world, pRes);
}


float KdCapsuleCollision::SegmentSegmentDistance(const Math::Vector3& p1, const Math::Vector3& q1, const Math::Vector3& p2, const Math::Vector3& q2, Math::Vector3& c1, Math::Vector3& c2)
{
	Math::Vector3 d1 = q1 - p1;   // 線分1の方向
	Math::Vector3 d2 = q2 - p2;   // 線分2の方向
	Math::Vector3 r = p1 - p2;

	float a = d1.Dot(d1);         // |d1|^2
	float e = d2.Dot(d2);         // |d2|^2
	float f = d2.Dot(r);

	float s, t;

	// 両方の線分が点でない場合
	if (a > 0.00001f && e > 0.00001f)
	{
		float b = d1.Dot(d2);
		float c = d1.Dot(r);
		float denom = a * e - b * b;

		// 平行でない場合
		if (denom != 0.0f)
		{
			s = (b * f - c * e) / denom;
		}
		else
		{
			s = 0.0f; // 平行
		}

		s = std::clamp(s, 0.0f, 1.0f);

		t = (b * s + f) / e;
		t = std::clamp(t, 0.0f, 1.0f);
	}
	else
	{
		// どちらかが点
		s = 0.0f;
		t = std::clamp(f / e, 0.0f, 1.0f);
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;

	return (c1 - c2).Length();
}

float KdCapsuleCollision::SegmentRayDistance(const Math::Vector3& segA,const Math::Vector3& segB,const Math::Vector3& rayPos,
											 const Math::Vector3& rayDir,Math::Vector3& outSegPoint,Math::Vector3& outRayPoint)
{
	Math::Vector3 u = segB - segA;
	Math::Vector3 v = rayDir;
	Math::Vector3 w = segA - rayPos;

	float a = u.Dot(u);
	float b = u.Dot(v);
	float c = v.Dot(v);
	float d = u.Dot(w);
	float e = v.Dot(w);

	float denom = a * c - b * b;

	float s, t;

	if (denom > 0.00001f)
	{
		s = (b * e - c * d) / denom;
		s = std::clamp(s, 0.0f, 1.0f);
	}
	else
	{
		s = 0.0f;
	}

	t = (b * s + e) / c;
	if (t < 0.0f) t = 0.0f;

	outSegPoint = segA + u * s;
	outRayPoint = rayPos + v * t;

	return (outSegPoint - outRayPoint).Length();
}

