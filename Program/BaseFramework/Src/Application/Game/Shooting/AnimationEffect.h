#pragma once

#include "../GameObject.h"

class AnimationEffect : public GameObject
{
public:
	AnimationEffect();
	~AnimationEffect();

	void SetAnimationInfo(const std::shared_ptr<KdTexture>& rTex, float size, int splitX, int splitY, float angle);

	virtual void Update() override;
	virtual void DrawEffect() override;

private:

	KdSquarePolygon m_poly;

	float m_angleZ = 0;
};