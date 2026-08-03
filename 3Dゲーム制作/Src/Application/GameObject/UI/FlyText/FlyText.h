#pragma once
class CameraBase;
class FlyText : public KdGameObject
{
public:
	FlyText() {}
	~FlyText() override {}

	void Init(const Math::Vector3& worldPos, int value);	
	void Update() override;
	void DrawSprite() override;
	void SetCamera(const std::shared_ptr<CameraBase>& cam)
	{
		m_wpCamera = cam;
	}

private:
	std::weak_ptr<CameraBase> m_wpCamera;

	Math::Vector3 m_worldPos;

	int m_value = 0;

	float m_life = 1.0f;          // 残り時間(秒)
	float m_offsetY = 0.0f;       // 上昇量
	float m_alpha = 255.0f;       // 透明度
};