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

	void UpdateRotate(const KdVec3& rMoveDir);
	float m_rotateAngle = 10.0f;
	KdVec3 m_rot;

	float m_gravity = 0.01f;
	KdVec3 m_force;

	static const float s_allowToStepHeight;	//歩いて乗り越えられる段差の高さ
	static const float s_landingHeight;		//地面から足が離れていても着地しているとする高さ
	
	// static const intはヘッダー内でも値を入れられるがそれ以外は不可

	void UpdateCollision();
	bool CheckGround(float& rDstDistance);
	bool CheckBump();

	KdVec3 m_prevPos;
	bool m_isGround;

	std::shared_ptr<KdAnimationData>	m_spAnimation = nullptr;
	float m_animationTime = 0.0f;

	class AnimationBase
	{
	public:
		virtual void Update(Human& rOwner) = 0;
	};

	class ActionWait : public AnimationBase
	{
	public:
		void Update(Human& rOwner) override;
	};

	class ActionWalk : public AnimationBase
	{
	public:
		void Update(Human& rOwner) override;
	};

	class ActionJump : public AnimationBase
	{
	public:
		void Update(Human& rOwner) override;
	};

	std::shared_ptr<AnimationBase> m_spActionState = nullptr;
};