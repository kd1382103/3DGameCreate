#pragma once
class BaseObject :public KdGameObject
{
public:
	BaseObject() {}
	~BaseObject() override { Release(); }

	void Init()							override;
	void Update()						override;
	void PostUpdate()					override;
	void DrawLit()						override;
	void GenerateDepthMapFromLight()	override;

private:

};