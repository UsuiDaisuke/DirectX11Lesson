#pragma once
#include"../GameObject.h"

class Missile : public GameObject
{
public:

	void Deserialize(const json11::Json& jsonObj) override;	// 初期化:オブジェクト精製用の外部データの解釈
	void Update();		// 更新

	

private:

	float		m_speed = 0.5f;
	int			m_lifeSpan = 0;

};