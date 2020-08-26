#pragma once

#include "../GameObject.h"

class Human : public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj) override;
	virtual void Update() override;

private:
	void UpdateMove();

	float m_moveSpeed = 0.1f;
	KdVec3 m_pos;

	void UpdateCamera();
	float m_camRotSpeed = 0.2f;
	KdMatrix m_mCamMat;
};