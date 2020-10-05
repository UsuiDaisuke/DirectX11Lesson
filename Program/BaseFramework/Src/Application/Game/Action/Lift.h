#pragma once

#include "../GameObject.h"

class Lift :public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj) override;
	virtual void Update() override;

private:
	KdMatrix m_mStart;

	KdVec3 m_vGoal = {};

	float m_speed = 0.0f;

	float m_progress = 0.0f;

	bool m_goTo = true;
};