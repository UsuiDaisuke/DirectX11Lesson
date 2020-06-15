#pragma once
#include "../GameObject.h"

class Missile;

class Aircraft : public GameObject
{
public:

	void Deserialize();	// 初期化:オブジェクト精製用の外部データの解釈
	void Update();		// 更新

	void ImGuiUpdate(); //Aircraftクラス専用のImGui更新

	void UpdateMove();	//移動の更新処理
	void UpdateShoot();	//発射関数

private:

	float			m_speed = 0.2f;			// 移動スピード
	bool			m_canShoot = true;		//発射可能フラグ
};