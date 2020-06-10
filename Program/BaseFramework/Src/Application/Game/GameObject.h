#pragma once

class GameObject
{
public:

	GameObject();
	virtual ~GameObject();

	virtual void Deserialize();
	virtual void Update();
	virtual void Draw();

	inline void SetMatrix(const KdMatrix& rMat)
	{
		m_mWorld = rMat;
	}
	inline bool IsAlive() const
	{
		return m_alive;
	}
	inline void Destroy()
	{
		m_alive = false;
	}

protected:

	virtual void Release();

	std::shared_ptr<KdModel> m_spModel = nullptr;
	KdMatrix m_mWorld;

	bool m_alive = true;
};