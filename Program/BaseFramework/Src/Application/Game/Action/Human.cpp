#include"Human.h"

#include"../Scene.h"
#include"../../Component/CameraComponent.h"
#include"../../Component/InputComponent.h"

void Human::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	// カメラコンポーネントの設定
	if (m_spCameraComponent)
	{
		m_spCameraComponent->GetOffsetMatrix().CreateTranslation(0.0f, 1.5f, -5.0f);
		m_spCameraComponent->GetOffsetMatrix().RotateX(25.0f * KdToRadians);
	}

	// プレイヤーであればInputComponentを作成
	if ((GetTag() & TAG_Player) != 0)
	{
		Scene::GetInstance().SetTargetCamera(m_spCameraComponent);
		m_spInputComponent = std::make_shared<ActionPlayerInputComponent>(*this);
	}
}

void Human::Update()
{
	// Inputコンポーネントの更新
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	UpdateMove();

	m_mWorld.SetTranslation(m_pos);

	if (m_spCameraComponent)
	{
		UpdateCamera();
	}
}

void Human::UpdateMove()
{
	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);

	// 移動ベクトルの計算
	KdVec3 moveVec = { inputMove.x, 0.0f, inputMove.y };

	// 正規化
	moveVec.Normalize();

	moveVec *= m_moveSpeed;

	m_pos.x += moveVec.x;
	m_pos.z += moveVec.z;
}

void Human::UpdateCamera()
{
	float deltaY = (float)(m_spInputComponent->GetAxis(Input::Axes::R).x) * m_camRotSpeed;

	KdMatrix mRotate;
	mRotate.RotateY(deltaY * KdToRadians);
	m_mCamMat *= mRotate;

	m_spCameraComponent->SetCameraMatrix(m_mCamMat * m_mWorld);
}
