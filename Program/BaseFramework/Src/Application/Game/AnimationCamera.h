#pragma once

#include "GameObject.h"

class AnimationCamera : public GameObject
{
public:
	AnimationCamera() { m_name = "AnimationCamera"; }

	// 二点間の補完を行う
	void Update()override;

	void Set(const std::shared_ptr<GameObject>& start, const std::shared_ptr<GameObject>& end, float spd = 0.01f)
	{
		if (!start->GetCameraComponent() || !end->GetCameraComponent()) { return; }

		m_progress = 0.0f;
		m_wpStart = start;
		m_wpEnd = end;
		m_speed = spd;
	}

private:
	std::weak_ptr<GameObject> m_wpStart;
	std::weak_ptr<GameObject> m_wpEnd;
	float m_progress = 0.0f;
	float m_speed = 0.0f;
};