#pragma once

class GameObject
{
public:

	GameObject();
	~GameObject();

	void Deserialize();
	void Update();
	void Draw();

protected:

	void Release();

	KdModel* m_pModel = nullptr;
	KdMatrix m_mWorld;

};