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

	if (m_spModelComponent)
	{
		m_spModelComponent->SetModel(KdResFac.GetModel(jsonObj["ModelFileName"].string_value()));
	}
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