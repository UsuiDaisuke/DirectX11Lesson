#pragma once

class CameraComponent;
class InputComponent;
class ModelComponent;

struct SphereInfo;
struct RayInfo;

//タグ定数
enum OBJECT_TAG
{
	TAG_None		=	0x00000000,		//属性なし:初期設定用
	TAG_Character	=	0x00000001,		//キャラクター設定用
	TAG_Player		=	0x00000002,		//プレイヤー設定用
	TAG_StageObject	=	0x00000004,		//背景オブジェクト設定用
	TAG_AttackHit	=	0x00000010,		//攻撃が当たる属性
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:

	GameObject();
	virtual ~GameObject();

	virtual void Deserialize(const json11::Json& jsonObj);
	virtual void Update();
	virtual void Draw();

	// 半透明物の描画
	virtual void DrawEffect() {};

	inline void SetMatrix(const KdMatrix& rMat)
	{
		m_mWorld = rMat;
	}
	inline KdMatrix GetMatrix() const
	{
		return m_mWorld;
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
	std::shared_ptr<CameraComponent> GetCameraComponent() { return m_spCameraComponent; }
	//入力コンポーネントの取得
	std::shared_ptr<InputComponent> GetInputComponent() { return m_spInputComponent; }
	//モデルコンポーネントの取得
	std::shared_ptr<ModelComponent> GetModelComponent() { return m_spModelComponent; }

	bool HitCheckBySphere(const SphereInfo& rInfo);
	bool HitCheckByRay(const RayInfo& rInfo, KdRayResult& rResult);

protected:

	virtual void Release();

	//カメラコンポーネント
	std::shared_ptr<CameraComponent> m_spCameraComponent = std::make_shared<CameraComponent>(*this);
	//インプットコンポーネント
	std::shared_ptr<InputComponent> m_spInputComponent = std::make_shared<InputComponent>(*this);
	//モデルコンポーネント
	std::shared_ptr<ModelComponent> m_spModelComponent = std::make_shared<ModelComponent>(*this);

	KdMatrix	m_mWorld;
	bool		m_alive		=	true;
	UINT		m_tag		=	OBJECT_TAG::TAG_None;

	std::string m_name = "GameObject";

	float		m_colRadius = 2.0f;
};

// クラス名からGameObjectを生成する関数
std::shared_ptr<GameObject> CreateGameObject(const std::string& name);

// 球判定に使うデータ
struct SphereInfo
{
	KdVec3 m_pos = {};
	float m_radius = 0.0f;
};

struct RayInfo
{
	KdVec3 m_pos;
	KdVec3 m_dir;
	float m_maxRange = 0.0f;
};