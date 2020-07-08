#include"Missile.h"

#include"Application/main.h"
#include"../../Component/ModelComponent.h"

void Missile::Deserialize(const json11::Json& jsonObj)
{
	m_lifeSpan = APP.m_maxFps * 10;

	if (jsonObj.is_null()) { return; }
	GameObject::Deserialize(jsonObj);

	if (jsonObj["Speed"].is_null() == false)
	{
		m_speed = jsonObj["Speed"].number_value();
	}
}

void Missile::Update()
{
	if (m_alive == false)
	{
		return;
	}

	if (--m_lifeSpan <= 0)
	{
		Destroy();
	}

	// ターゲットをshared_ptr化
	auto target = m_wpTarget.lock();

	if (target)
	{
		// 自身からターゲットへのベクトル
		KdVec3 vTarget = target->GetMatrix().GetTranslation() - m_mWorld.GetTranslation();

		// 単位ベクトル化：自身からターゲットへ向かう長さ1のベクトル
		vTarget.Normalize();

		// 自分のZ方向(前方向)
		KdVec3 vZ = m_mWorld.GetAxisZ();

		// 拡大率が入っていると計算がおかしくなるため単位ベクトル化
		vZ.Normalize();

		// ※※※※※回転軸作成（この軸で開店する）※※※※※
		KdVec3 vRotAxis = KdVec3::Cross(vZ, vTarget);

		// 0ベクトルなら回転しない
		if (vRotAxis.LengthSquared() != 0)
		{
			// 自分のZ方向ベクトルと自身からターゲットへ向かうベクトルの内積
			float d = KdVec3::Dot(vZ, vTarget);

			// 誤差で-1～1以外になる可能性大なので、クランプする
			if (d > 1.0f)d = 1.0f;
			else if (d < -1.0f)d = -1.0f;

			// 自分の前方向ベクトルと自身からターゲットへ向かうベクトル間の角度(radian)
			float radian = acos(d);

			// 角度制限 １フレームにつき最大で１度以上回転しない
			if (radian > 1.0f * KdToRadians)
			{
				radian = 1.0f * KdToRadians;
			}

			// ※※※※※radian（ここまでで回転角度が求まった）※※※※※

			KdMatrix mRot;
			mRot.CreateRotationAxis(vRotAxis, radian);
			auto pos = m_mWorld.GetTranslation();
			m_mWorld.SetTranslation({ 0,0,0 });
			m_mWorld *= mRot;
			m_mWorld.SetTranslation(pos);
		}
	}

	KdVec3 move = m_mWorld.GetAxisZ();
	move.Normalize();

	move *= m_speed;

	m_mWorld.Move(move);
}