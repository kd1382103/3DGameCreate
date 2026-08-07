#pragma once
class BaseScene;
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
	void DrawSprite()					override;

	virtual void Damage(float dmg, bool isUltimate = false, bool finalHit = false) {}
private:

};