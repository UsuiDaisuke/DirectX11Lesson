#include "GameObject.h"

#include "../Component/CameraComponent.h"
#include "../Component/InputComponent.h"
#include "../Component/ModelComponent.h"

GameObject::GameObject()
{

}

GameObject::~GameObject()
{
	
}

void GameObject::Deserialize()
{

}

void GameObject::Update()
{

}

void GameObject::Draw()
{
	if (m_spModelComponent == nullptr){ return; }

	m_spModelComponent->Draw();
}

void GameObject::Release()
{

}