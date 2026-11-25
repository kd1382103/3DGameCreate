#pragma once
class BaseScene;
class BaseObject :public KdGameObject
{
public:
	BaseObject() {}
	~BaseObject() override { Release(); }

	virtual void Damage(
		float dmg,
		bool isUltimate = false,
		bool finalHit = false, 
		float knockBackRate = 1.0f
	) {}

protected:

	//==============================================================
	// 方向ベクトル計算
	//==============================================================
	static Math::Vector3 GetForwardFromAngle(float angleY)
	{
		Math::Matrix rot =
			Math::Matrix::CreateRotationY(angleY);

		return rot.Forward();
	}

	static Math::Vector3 GetRightFromAngle(float angleY)
	{
		Math::Matrix rot =
			Math::Matrix::CreateRotationY(angleY);

		return rot.Right();
	}

	//==============================================================
	// モデルアニメーション更新
	//==============================================================
	template <class TAnimator, class TModel>
	void AdvanceModelAnimation(
		TAnimator& animator,
		const std::shared_ptr<TModel>& model,
		float dt
	)
	{
		if (!model) return;

		animator.AdvanceTime(
			model->WorkNodes(),
			dt
		);

		if (model->NeedCalcNodeMatrices())
		{
			model->CalcNodeMatrices();
		}
	}

	//==============================================================
	// 重力処理
	//==============================================================
	static void ApplyGravity(
		float& gravity,
		Math::Vector3& position,
		float dt
	)
	{
		gravity += 0.005f * dt;
		position.y -= gravity * dt;
	}

	//==============================================================
	// スロー時間更新
	//==============================================================
	static void UpdateSlowTimer(
		bool& isSlow,
		float& slowTimer,
		float dt
	)
	{
		if (!isSlow)
		{
			return;
		}

		slowTimer -= dt;

		if (slowTimer <= 0.0f)
		{
			slowTimer = 0.0f;
			isSlow = false;
		}
	}

	// ワールド用ポリゴンの初期設定
	template <class TPolygon>
	static void SetupWorldPolygon(
		TPolygon& polygon,
		const std::string& texture,
		float scale
	)
	{
		polygon.SetMaterial(texture);
		polygon.Set2DObject(false);
		polygon.SetScale(scale);
	}
};