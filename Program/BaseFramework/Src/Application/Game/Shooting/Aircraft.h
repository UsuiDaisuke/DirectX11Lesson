#pragma once
#include "../GameObject.h"

class Missile;

class Aircraft : public GameObject
{
public:

	void Deserialize();	// 初期化:オブジェクト精製用の外部データの解釈
	void Update();		// 更新

	void ImGuiUpdate(); //Aircraftクラス専用のImGui更新

	void SetCameraToShader();	//追従カメラのビュー行列・射影行列をシェーダーにセット

	void UpdateMove();	//移動の更新処理
	void UpdateShoot();	//発射関数

private:

	float			m_speed = 0.2f;			// 移動スピード

	KdMatrix		m_mOffset;				//追従カメラからの相対座標行列
	KdMatrix		m_mProj;				//追従カメラの射影行列

	bool			m_canShoot = true;		//発射可能フラグ
};