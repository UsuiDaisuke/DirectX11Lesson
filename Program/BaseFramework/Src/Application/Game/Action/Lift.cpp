#include "Lift.h"

void Lift::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	m_mStart = m_mWorld;

	if (jsonObj["MoveTo"].is_array())
	{
		auto& p = jsonObj["MoveTo"].array_items();
		m_vGoal = KdVec3(p[0].number_value(), p[1].number_value(), p[2].number_value());
	}

	if (jsonObj["Speed"].is_number())
	{
		m_speed = jsonObj["Speed"].number_value();
	}
}

void Lift::Update()
{
	GameObject::Update();

	auto& vStart = m_mStart.GetTranslation();
	auto& vGoal = m_vGoal;

	KdVec3 vTo = vGoal - vStart;
	KdVec3 vNow = vStart + vTo * m_progress;

	m_mWorld.SetTranslation(vNow);

	if (m_goTo)
	{
		m_progress += m_speed;
		if (m_progress >= 1.0f)
		{
			m_goTo = false;
			m_progress = 1.0f;
		}
	}
	else
	{
		m_progress -= m_speed;
		if (m_progress <= 0.0f)
		{
			m_goTo = true;
			m_progress = 0.0f;
		}
	}
}
