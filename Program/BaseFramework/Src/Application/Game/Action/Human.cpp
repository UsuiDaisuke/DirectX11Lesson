#include"Human.h"

#include"../Scene.h"
#include"../../Component/CameraComponent.h"
#include"../../Component/InputComponent.h"

// static const変数の初期化
const float Human::s_allowToStepHeight = 0.8f;
const float Human::s_landingHeight = 0.1f;

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

	// 移動前の座標を覚える
	m_prevPos = m_pos;

	UpdateMove();

	// 重力をキャラクターのYの移動力に加える
	m_force.y -= m_gravity;

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
		UpdateCamera();
	}
}

void Human::UpdateMove()
{
	if (!m_spCameraComponent) { return; }

	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);

	KdVec3 moveSide = m_spCameraComponent->GetCameraMatrix().GetAxisX() * inputMove.x;
	KdVec3 moveForward = m_spCameraComponent->GetCameraMatrix().GetAxisZ() * inputMove.y;

	moveForward.y = 0.0f;

	// 移動ベクトルの計算
	KdVec3 moveVec = moveSide + moveForward;

	// 正規化
	moveVec.Normalize();

	UpdateRotate(moveVec);

	moveVec *= m_moveSpeed;

	m_force.x = moveVec.x;
	m_force.z = moveVec.z;

	if (m_spInputComponent->GetButton(Input::Buttons::A))
	{
		if (m_isGround)
		{
			m_force.y = 0.2f;
		}
	}
}

void Human::UpdateCamera()
{
	float deltaY = (float)(m_spInputComponent->GetAxis(Input::Axes::R).x) * m_camRotSpeed;

	KdMatrix mRotate;
	mRotate.RotateY(deltaY * KdToRadians);
	m_mCamMat *= mRotate;

	KdMatrix trans;
	trans.CreateTranslation(m_pos.x, m_pos.y, m_pos.z);
	m_spCameraComponent->SetCameraMatrix(m_mCamMat * trans);
}

void Human::UpdateRotate(const KdVec3& rMoveDir)
{
	// 移動していなければ帰る
	if (rMoveDir.LengthSquared() == 0.0f) { return; }

	// 今キャラクターの方向ベクトル
	KdVec3 nowDir = m_mWorld.GetAxisZ();
	nowDir.Normalize();

	// キャラクターが今向いている方角の角度を求める(ラジアン角)
	float nowRadian = atan2(nowDir.x, nowDir.z);

	// 移動方向へのベクトルの角度を求める(ラジアン角)
	float targetRadian = atan2(rMoveDir.x, rMoveDir.z);
	float rotateRadian = targetRadian - nowRadian;

	// -π～π(-180度～180度)
	// 180度の角度で数値の切れ目
	if (rotateRadian > M_PI)
	{
		rotateRadian -= 2 * float(M_PI);
	}
	else if (rotateRadian < -M_PI)
	{
		rotateRadian += 2 * float(M_PI);
	}

	// 一回の回転角度をm_rotateAngle内に収める
	rotateRadian = std::clamp(rotateRadian, -m_rotateAngle * KdToRadians, m_rotateAngle * KdToRadians);

	m_rot.y += rotateRadian;
}

void Human::UpdateCollision()
{
	float distanceFromGround = FLT_MAX;

	if (CheckGround(distanceFromGround))
	{
		m_pos.y += s_allowToStepHeight - distanceFromGround;

		m_force.y = 0.0f;
	}
}

bool Human::CheckGround(float& rDstDistance)
{
	RayInfo rayInfo;
	rayInfo.m_pos = m_pos;

	rayInfo.m_pos.y += s_allowToStepHeight;

	rayInfo.m_pos.y += m_prevPos.y - m_pos.y;

	rayInfo.m_dir = { 0.0f, -1.0f, 0.0f };

	rayInfo.m_maxRange = FLT_MAX;
	KdRayResult finalRayResult;

	std::shared_ptr<GameObject> hitObj = nullptr;

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

				hitObj = obj;
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

	if (hitObj)
	{
		auto mOneMove = hitObj->GetOneMove();
		auto vOneMove = mOneMove.GetTranslation();

		m_pos += vOneMove;
	}

	return m_isGround;
}
