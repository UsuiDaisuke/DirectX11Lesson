#pragma once

class CameraComponent;

//タグ定数
enum OBJECT_TAG
{
	TAG_None		=	0x00000000,		//属性なし:初期設定用
	TAG_Character	=	0x00000001,		//キャラクター設定用
	TAG_Player		=	0x00000002,		//プレイヤー設定用
};

class GameObject : public std::enable_shared_from_this<GameObject>
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

	inline void SetTag(UINT tag) { m_tag = tag; }
	inline UINT GetTag() const{ return m_tag; }

	//カメラコンポーネントの取得
	std::shared_ptr<CameraComponent> getCameraComponent() { return m_spCameraComponent; }

protected:

	virtual void Release();

	//カメラコンポーネント
	std::shared_ptr<CameraComponent> m_spCameraComponent = std::make_shared<CameraComponent>(*this);

	std::shared_ptr<KdModel> m_spModel = nullptr;
	KdMatrix	m_mWorld;
	bool		m_alive		=	true;
	UINT		m_tag		=	OBJECT_TAG::TAG_None;
};