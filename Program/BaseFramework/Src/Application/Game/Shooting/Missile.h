#pragma once
#include"../GameObject.h"

class Missile : public GameObject
{
public:

	void Deserialize(const json11::Json& jsonObj) override;	// 初期化:オブジェクト精製用の外部データの解釈
	void Update();		// 更新

	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }

	void UpdateCollision();
	inline void SetOwner(const std::shared_ptr<GameObject>& spOwner) { m_wpOwner = spOwner; }

	void Explosion();

	void UpdateTrail();			// 軌道の更新
	void DrawEffect() override;	// 透明物の描画

private:

	KdVec3		m_prevPos;

	float		m_speed = 0.5f;
	int			m_lifeSpan = 0;

	std::weak_ptr<GameObject> m_wpTarget;
	std::weak_ptr<GameObject> m_wpOwner;

	int			m_attackPow = 5;

	// 頂点形式
	KdTrailPolygon	m_trailSmoke;

	float m_trailRotate = 0.0f;
};