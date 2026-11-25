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
private:

};