#pragma once

class CameraBase : public KdGameObject
{
public:
	CameraBase()						{}
	virtual ~CameraBase()	override	{}

	void Init()				override;
	void PreDraw()			override;
	void SetTarget(const std::shared_ptr<KdGameObject>& target);

	//virtual bool IsFPS() const { return false; }

	// 「絶対変更しません！見るだけ！」な書き方
	const std::shared_ptr<KdCamera>& GetCamera() const
	{
		return m_spCamera;
	}

	// 「中身弄るかもね」な書き方
	std::shared_ptr<KdCamera> WorkCamera() const
	{
		return m_spCamera;
	}

	const Math::Matrix GetRotationMatrix()const
	{
		return Math::Matrix::CreateFromYawPitchRoll(
		       DirectX::XMConvertToRadians(m_DegAng.y),
		       DirectX::XMConvertToRadians(m_DegAng.x),
		       DirectX::XMConvertToRadians(m_DegAng.z));
	}

	const Math::Matrix GetRotationYMatrix() const
	{
		return Math::Matrix::CreateRotationY(
			   DirectX::XMConvertToRadians(m_DegAng.y));
	}

	void RegistHitObject(const std::shared_ptr<KdGameObject>& object)
	{
		m_wpHitObjectList.push_back(object);
	}

	void SetActive(bool active) { m_active = active; }
	bool IsActive() const { return m_active; }


	Math::Matrix GetBillboardMatrix() const
	{
		auto cam = m_spCamera;
		if (!cam) return Math::Matrix::Identity;

		Math::Matrix camWorld = cam->GetCameraMatrix();

		// 平行移動を消す（回転だけ残す）
		camWorld.Translation(Math::Vector3::Zero);

		//  回転だけ反転
		return camWorld.Invert();
	}

	Math::Vector3 GetCameraDir() const
	{
		// カメラのワールド行列
		Math::Matrix camMat = m_spCamera->GetCameraMatrix();

		// 前方方向（Z軸の反転）
		Math::Vector3 forward = -Math::Vector3(camMat.Forward());

		forward.Normalize();
		return forward;
	}

	Math::Vector3 GetCameraPos() const
	{
		return Math::Vector3(m_spCamera->GetCameraMatrix().Translation());
	}


	Math::Vector2 WorldToScreen(const Math::Vector3& worldPos) const;

protected:
	// カメラ回転用角度
	Math::Vector3								m_DegAng		= Math::Vector3::Zero;

	void UpdateRotateByMouse();

	std::shared_ptr<KdCamera>					m_spCamera		= nullptr;
	std::weak_ptr<KdGameObject>					m_wpTarget;
	std::vector<std::weak_ptr<KdGameObject>>	m_wpHitObjectList{};

	Math::Matrix								m_mLocalPos		= Math::Matrix::Identity;
	Math::Matrix								m_mRotation		= Math::Matrix::Identity;

	// カメラ回転用マウス座標の差分
	POINT										m_FixMousePos{};

	bool m_active = false;

};