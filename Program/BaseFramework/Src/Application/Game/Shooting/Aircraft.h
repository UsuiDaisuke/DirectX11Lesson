﻿#pragma once
#include "../GameObject.h"

class Missile;

class Aircraft : public GameObject
{
public:

	void Deserialize(const json11::Json& jsonObj) override;	// 初期化:オブジェクト精製用の外部データの解釈
	void Update();		// 更新

	void ImGuiUpdate(); //Aircraftクラス専用のImGui更新

	void UpdateMove();	//移動の更新処理
	void UpdateShoot();	//発射関数
	void UpdateCollision();	//当たり判定処理

	void Draw() override;	// 描画

	void OnNotify_Damage(int damage);

private:

	float			m_speed = 0.2f;			// 移動スピード
	bool			m_canShoot = true;		// 発射可能フラグ

	KdVec3			m_prevPos = {};			// 1フレーム前の座標

	bool	m_laser = false;
	float	m_laserRange = 1000.0f;

	int		m_hp = 10;
	int			m_attackPow = 1;
};