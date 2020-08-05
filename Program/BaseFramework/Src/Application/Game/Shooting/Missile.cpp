﻿#include"Missile.h"

#include"Application/main.h"
#include"Application\Game\Scene.h"
#include"Aircraft.h"
#include"EffectObject.h"
#include"../../Component/ModelComponent.h"
#include "AnimationEffect.h"

void Missile::Deserialize(const json11::Json& jsonObj)
{
	m_lifeSpan = APP.m_maxFps * 10;

	if (jsonObj.is_null()) { return; }
	GameObject::Deserialize(jsonObj);

	if (jsonObj["Speed"].is_null() == false)
	{
		m_speed = jsonObj["Speed"].number_value();
	}

	if (jsonObj["Power"].is_null() == false)
	{
		m_attackPow = jsonObj["Power"].int_value();
	}

	// 煙テクスチャ
	m_texture = KdResFac.GetTexture("Data/Texture/smokeline2.png");
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

	//1フレーム前の座標保存
	m_prevPos = m_mWorld.GetTranslation();

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

	UpdateCollision();

	// 軌跡の更新
	UpdateTrail();
}

void Missile::UpdateCollision()
{
	bool isHit = false;

	// 一回の移動量と移動方向を計算
	KdVec3 moveVec = m_mWorld.GetTranslation() - m_prevPos;	// 動く前→今の場所のベクトル
	float moveDistance = moveVec.Length();	//１回の移動量

	// 動いていないなら判定しない
	if (moveDistance == 0.0f) { return; }

	// 発射した主人をshared_ptr取得
	auto spOwner = m_wpOwner.lock();

	//球情報の作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_colRadius;

	//レイ情報の作成
	RayInfo rayInfo;
	rayInfo.m_pos = m_prevPos;			// 一つ前の場所から
	rayInfo.m_dir = moveVec;			// 動いた方向に向かって
	rayInfo.m_maxRange = moveDistance;	//動いた分だけ判定を行う

	rayInfo.m_dir.Normalize();

	KdRayResult rayResult;

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		// 自分自身を無視
		if (obj.get() == this) { continue; }
		// 発射した主人を無視
		if (obj.get() == spOwner.get()) { continue; }

		//TAG_Characterとは球判定を行う
		if (obj->GetTag() & TAG_Character)
		{
			if (obj->HitCheckBySphere(info))
			{
				isHit = true;

				std::shared_ptr<Aircraft> aircraft = std::dynamic_pointer_cast<Aircraft>(obj);
				if (aircraft)
				{
					aircraft->OnNotify_Damage(m_attackPow);
				}
			}
		}

		//TAG_StageObjectとはレイ判定を行う
		if (obj->GetTag() & TAG_StageObject)
		{
			if (obj->HitCheckByRay(rayInfo, rayResult))
			{
				isHit = true;
			}
		}

		//当たったら
		if (isHit) {
			Explosion();
			Destroy();
		}
	}
}


void Missile::Explosion()
{
	// アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	// 爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(
		KdResFac.GetTexture("Data/Texture/Explosion00.png"), 10.0f, 5, 5, rand() % 360
	);

	// 爆発をミサイル(自分)の位置に合わせる
	effect->SetMatrix(m_mWorld);

	// リストに追加
	Scene::GetInstance().AddObject(effect);
}

void Missile::UpdateTrail()
{
	m_trailRotate += 1.0f;

	KdMatrix mTrail;
	mTrail.RotateZ(m_trailRotate);
	mTrail *= m_mWorld;

	// 軌道座標を先頭に追加
	m_pointList.push_front(mTrail);

	// 軌道の数制限(100以前は消去する)
	if (m_pointList.size() > 100)
	{
		m_pointList.pop_back();
	}
}

void Missile::DrawEffect()
{
	if (!m_alive) { return; }

	// ポイントが2つ以下の場合は描画不可
	if (m_pointList.size() < 2) { return; }

	// 軌道画像の分割数
	float sliceCount = (float)(m_pointList.size() - 1);

	// 頂点行列
	std::vector<Vertex> vertex;
	// ポイント数分のサイズ確保
	vertex.resize(m_pointList.size() * 2);

	// 横幅
	float width = 0.5f;
	
	//------------------------------
	// 頂点データ作成
	//------------------------------
	for (UINT i = 0; i < m_pointList.size(); i++)
	{
		// 登録行列の参照(ショートカット)
		Math::Matrix& mat = m_pointList[i];

		// 2つの頂点の参照(ショートカット)
		Vertex& v1 = vertex[i * 2];
		Vertex& v2 = vertex[i * 2 + 1];

		// X方向
		Math::Vector3 axisX = mat.Right();
		axisX.Normalize();

		// 座標
		v1.Pos = mat.Translation() + axisX * width * 0.5f;
		v2.Pos = mat.Translation() - axisX * width * 0.5f;

		// UV
		float uvY = i / sliceCount;
		v1.UV = { 0, uvY };
		v2.UV = { 1, uvY };
	}

	SHADER.m_effectShader.SetWorldMatrix(KdMatrix());

	SHADER.m_effectShader.WriteToCB();

	// テクスチャセット
	if (m_texture)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, m_texture->GetSRViewAddress());
	}
	else
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	// 指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));
}
