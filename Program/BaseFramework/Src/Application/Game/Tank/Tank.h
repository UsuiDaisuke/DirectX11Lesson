#pragma once

#include"../GameObject.h"

class Tank : public GameObject
{
public:
	virtual void Deserialize(const json11::Json& jsonObj) override;
	virtual void Update() override;

private:
	void UpdateMove();
	float m_moveSpeed = 0.1f;
	KdVec3 m_pos;

	void UpdateRotate();
	float m_rotateAngle = 1.0f;
	KdVec3 m_rot;

	float m_gravity = 0.01f;
	KdVec3 m_force;

	static const float s_allowToStepHeight;	//歩いて乗り越えられる段差の高さ
	static const float s_landingHeight;		//地面から足が離れていても着地しているとする高さ

	// static const intはヘッダー内でも値を入れられるがそれ以外は不可

	void UpdateCollision();
	bool CheckGround(float& rDstDistance);

	KdVec3 m_prevPos;
	bool m_isGround;
};