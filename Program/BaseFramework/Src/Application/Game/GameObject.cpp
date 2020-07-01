#include "GameObject.h"

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

void GameObject::Release()
{

}