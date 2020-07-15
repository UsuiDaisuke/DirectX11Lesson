#include "EffectObject.h"
#include "../Scene.h"

void EffectObject::Update()
{
	if (m_alive == false) { return; }

	if (--m_lifeSpan <= 0)
	{
		Destroy();

		return;
	}
	//徐々に大きくなっていく更新
	{
		m_scale += m_lifeSpan * 0.001f;

		// 色をバクハツっぽい色(黄色、オレンジ)
		m_color.R(m_lifeSpan / 30.0f);
		m_color.G(m_lifeSpan / 60.0f);
	}
}

void EffectObject::Draw()
{
	if (m_alive == false) { return; }

	Scene::GetInstance().AddDebugSphereLine(m_mWorld.GetTranslation(), m_scale, m_color);
}
