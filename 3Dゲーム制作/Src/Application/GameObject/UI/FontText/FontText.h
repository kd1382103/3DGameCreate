#pragma once
class CameraBase;
class FontText : public KdGameObject
{
public:
	FontText() {}
	~FontText() override {}

	// フライテキスト
	void Init(const Math::Vector3& worldPos, int value);

	// 画面上のメッセージ
	void InitMessage(
		const std::string& text,
		const Math::Vector2& pos = { 0.0f, 0.0f },
		float scale = 2.0f
	);

	void Update() override;
	void DrawSprite() override;

	void SetCamera(const std::shared_ptr<CameraBase>& cam)
	{
		m_wpCamera = cam;
	}

	void SetExpired()
	{
		m_isExpired = true;
	}

	void SetVisible(bool visible)
	{
		m_visible = visible;
	}

	bool IsVisible() const
	{
		return m_visible;
	}

public:

	//FlyTextの表示時間
	static constexpr float LifeTime = 3.0f;

	//拡大縮小にかける時間
	static constexpr float ScaleTime = 0.5f;

	//「constexpr」はコンパイル時に定数を定義する
	// プログラムの実行時ではなくコンパイル時に値が決まる

private:

	//=======================================
	// 共通
	//=======================================

	bool m_isMessage = false;
	bool m_visible = true;

	//=======================================
	// フライテキスト
	//=======================================

	std::weak_ptr<CameraBase> m_wpCamera;

	Math::Vector3 m_worldPos;
	int m_value = 0;

	float m_life = 0.0f;
	float m_offsetY = 0.0f;
	float m_offsetX = 0.0f;
	float m_velocityX = 0.0f;

	float m_alpha = 255.0f;
	float m_scale = 0.0f;

	//=======================================
	// メッセージ
	//=======================================

	std::string m_text;

	Math::Vector2 m_screenPos = { 0.0f, 0.0f };
	float m_messageScale = 2.0f;
};