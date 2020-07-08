#include "GameObject.h"
#include "../Game/Shooting/Aircraft.h"

#include "../Component/CameraComponent.h"
#include "../Component/InputComponent.h"
#include "../Component/ModelComponent.h"

GameObject::GameObject()
{

}

GameObject::~GameObject()
{
	Release();
}

void GameObject::Deserialize(const json11::Json& jsonObj)
{
	if (jsonObj.is_null()) { return; }

	if (jsonObj["Name"].is_null() == false)
	{
		m_name = jsonObj["Name"].string_value();
	}

	//タグ
	if (jsonObj["Tag"].is_null() == false)
	{
		m_tag = jsonObj["Tag"].number_value();
	}

	//モデル--------------------

	if (m_spModelComponent)
	{
		m_spModelComponent->SetModel(KdResFac.GetModel(jsonObj["ModelFileName"].string_value()));
	}

	//行列--------------------
	KdMatrix mTrans, mRotate, mScale;

	//座標
	const std::vector<json11::Json>& rPos = jsonObj["Pos"].array_items();
	if (rPos.size() == 3)
	{
		mTrans.CreateTranslation((float)rPos[0].number_value(), (float)rPos[1].number_value(), (float)rPos[2].number_value());
	}

	//回転
	const std::vector<json11::Json>& rRot = jsonObj["Rot"].array_items();
	if (rRot.size() == 3)
	{
		mRotate.CreateRotationX((float)rRot[0].number_value() * KdToRadians);
		mRotate.RotateY((float)rRot[1].number_value() * KdToRadians);
		mRotate.RotateZ((float)rRot[2].number_value() * KdToRadians);
	}

	//拡縮
	const std::vector<json11::Json>& rScale = jsonObj["Scale"].array_items();
	if (rScale.size() == 3)
	{
		mScale.CreateScalling((float)rScale[0].number_value(), (float)rScale[1].number_value(), (float)rScale[2].number_value());
	}

	//当たり判定の半径
	if (jsonObj["Radius"].is_null() == false)
	{
		m_colRadius = jsonObj["Radius"].number_value();
	}

	m_mWorld = mScale * mRotate * mTrans;
}

void GameObject::Update()
{

}

void GameObject::Draw()
{
	if (m_spModelComponent == nullptr) { return; }

	m_spModelComponent->Draw();
}

//球による当たり判定(距離判定)
bool GameObject::HitCheckBySphere(const SphereInfo& rInfo)
{
	// 当たったとする距離の計算
	float hitRange = rInfo.m_radius + m_colRadius;

	// 自分の座標ベクトル
	KdVec3 myPos = m_mWorld.GetTranslation();

	// 二点間のベクトルを計算
	KdVec3 betweenVec = rInfo.m_pos - myPos;

	// 二点間の距離を計算
	float distance = betweenVec.Length();

	bool isHit = false;
	if (distance <= hitRange)
	{
		isHit = true;
	}

	return isHit;
}

bool GameObject::HitCheckByRay(const RayInfo& rInfo)
{
	// 判定する対象のモデルがない場合は当たっていないとして帰る
	if (!m_spModelComponent) { return false; }

	// 発射方向は正規化されていないと正しく判定できないので正規化
	KdVec3 rayDir = rInfo.m_dir;
	rayDir.Normalize();

	// 面情報の取得
	const std::shared_ptr<KdMesh>& mesh = m_spModelComponent->GetMesh();	//モデル(メッシュ)情報の取得
	const KdMeshFace* pFaces = &mesh->GetFaces()[0];	// 面情報の先頭を取得
	UINT faceNum = mesh->GetFaces().size();

	// すべての面(三角形)と当たり判定
	for (UINT faceIdx = 0; faceIdx < faceNum; ++faceIdx)
	{
		// 三角形を構成する3つの頂点のIndex
		const UINT* idx = pFaces[faceIdx].Idx;

		// レイと三角形の当たり判定
		float triDist = FLT_MAX;
		bool bHit = DirectX::TriangleTests::Intersects(
			rInfo.m_pos,	// 発射場所
			rayDir,			// 発射方向

			// 判定する三角形の頂点情報
			mesh->GetVertexPositions()[idx[0]],
			mesh->GetVertexPositions()[idx[1]],
			mesh->GetVertexPositions()[idx[2]],

			triDist	// 当たった場合の距離
		);

		// ヒットしていなかったらスキップ
		if (bHit == false) { continue; }

		// 最大距離以内か
		if (triDist <= rInfo.m_maxRange)
		{
			return true;
		}
	}

	return false;
}

void GameObject::Release()
{

}

std::shared_ptr<GameObject> CreateGameObject(const std::string& name)
{
	if (name == "GameObject") {
		return std::make_shared<GameObject>();
	}
	else if(name == "Aircraft"){
		return std::make_shared<Aircraft>();
	}

	// 文字列が既存クラスに一致しなかった
	assert(0 && "存在しないGameObjectクラスです！");

	return nullptr;
}
