#include "Tank.h"

#include"../Scene.h"
#include"../../Component/CameraComponent.h"
#include"../../Component/InputComponent.h"

// static const変数の初期化
const float Tank::s_allowToStepHeight = 0.8f;
const float Tank::s_landingHeight = 0.1f;

void Tank::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	// カメラコンポーネントの設定
	if (m_spCameraComponent)
	{
		m_spCameraComponent->GetOffsetMatrix().CreateTranslation(0.0f, 1.5f, -10.0f);
		m_spCameraComponent->GetOffsetMatrix().RotateX(25.0f * KdToRadians);
	}

	// プレイヤーであればInputComponentを作成
	if ((GetTag() & TAG_Player) != 0)
	{
		Scene::GetInstance().SetTargetCamera(m_spCameraComponent);
		m_spInputComponent = std::make_shared<ActionPlayerInputComponent>(*this);
	}
}

void Tank::Update()
{// Inputコンポーネントの更新
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	// 移動前の座標を覚える
	m_prevPos = m_pos;

	UpdateMove();

	// 重力をキャラクターのYの移動力に加える
	//m_force.y -= m_gravity;

	// 移動力をキャラクターの座標に足しこむ
	m_pos.x += m_force.x;
	m_pos.y += m_force.y;
	m_pos.z += m_force.z;

	UpdateCollision();

	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.Move(m_pos);

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}
}

void Tank::UpdateMove()
{
	if (!m_spCameraComponent) { return; }

	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);
	KdVec3 moveForward = m_spCameraComponent->GetCameraMatrix().GetAxisZ() * inputMove.y;

	moveForward.y = 0.0f;

	// 移動ベクトルの計算
	KdVec3 moveVec = moveForward;

	// 正規化
	moveVec.Normalize();

	UpdateRotate();

	moveVec *= m_moveSpeed;

	m_force.x = moveVec.x;
	m_force.z = moveVec.z;
}

void Tank::UpdateRotate()
{
	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);

	m_rot.y += (inputMove.x * m_rotateAngle) * KdToRadians;
}

void Tank::UpdateCollision()
{
	float distanceFromGround = FLT_MAX;

	if (CheckGround(distanceFromGround))
	{
		m_pos.y += s_allowToStepHeight - distanceFromGround;

		m_force.y = 0.0f;
	}
}

bool Tank::CheckGround(float& rDstDistance)
{
	RayInfo rayInfo;
	rayInfo.m_pos = m_pos;

	rayInfo.m_pos.y += s_allowToStepHeight;

	rayInfo.m_pos.y += m_prevPos.y - m_pos.y;

	rayInfo.m_dir = { 0.0f, -1.0f, 0.0f };

	rayInfo.m_maxRange = FLT_MAX;
	KdRayResult finalRayResult;

	for (auto& obj : Scene::GetInstance().GetObjects())
	{
		if (obj.get() == this) { continue; }

		if (!(obj->GetTag() & (TAG_StageObject))) { continue; }

		KdRayResult rayResult;

		if (obj->HitCheckByRay(rayInfo, rayResult))
		{
			if (rayResult.m_distance < finalRayResult.m_distance)
			{
				finalRayResult = rayResult;
			}
		}
	}

	float distanceFromGround = FLT_MAX;

	if (finalRayResult.m_hit)
	{
		distanceFromGround = finalRayResult.m_distance - (m_prevPos.y - m_pos.y);
	}

	if (m_force.y > 0.0f)
	{
		m_isGround = false;
	}
	else
	{
		m_isGround = (distanceFromGround < (s_allowToStepHeight + s_landingHeight));
	}

	rDstDistance = distanceFromGround;

	return m_isGround;
}
