#pragma once
#include "../GameObject.h"

class Missile;

class Aircraft : public GameObject
{
public:

	void Deserialize(const json11::Json& jsonObj) override;	// 初期化:オブジェクト精製用の外部データの解釈
	void Update();		// 更新

	void ImGuiUpdate(); //Aircraftクラス専用のImGui更新

	void Draw() override;	// 描画

	void OnNotify_Damage(int damage);

private:

	void UpdateMove();	//移動の更新処理
	void UpdateShoot();	//発射関数
	void UpdateCollision();	//当たり判定処理
	void UpdatePropeller();// プロペラ更新

	std::shared_ptr<GameObject> m_spPropeller;	// プロペラ用オブジェクト

	// プロペラが飛行機本体からどれだけ離れているか
	KdMatrix m_mPropLocal;
	float m_propRotSpeed;

	float			m_speed = 0.2f;			// 移動スピード
	bool			m_canShoot = true;		// 発射可能フラグ

	KdVec3			m_prevPos = {};			// 1フレーム前の座標

	bool	m_laser = false;
	float	m_laserRange = 1000.0f;

	int		m_hp = 10;
	int			m_attackPow = 1;

	// 基底アクションステート
	class BaseAction
	{
	public:
		virtual void Update(Aircraft& owner) = 0;
	};

	// 飛行中
	class ActionFly : public BaseAction
	{
	public:
		virtual void Update(Aircraft& owner) override;
	};

	// 墜落中
	class ActionCrash : public BaseAction
	{
	public:
		virtual void Update(Aircraft& owner) override;

		int m_timer = 180;
	};

	// 現在実行するアクションステート
	std::shared_ptr<BaseAction> m_spActionState;
};