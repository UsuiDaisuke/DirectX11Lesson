#include "Aircraft.h"
#include "Missile.h"

#include "../Scene.h"
#include "../../Component/CameraComponent.h"
#include "../../Component/InputComponent.h"
#include "../../Component/ModelComponent.h"

void Aircraft::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);

	if (m_spCameraComponent)
	{
		m_spCameraComponent->GetOffsetMatrix().CreateTranslation(0.0f, 1.5f, -10.0f);
	}

	if ((GetTag() & OBJECT_TAG::TAG_Player) != 0)
	{
		Scene::GetInstance().SetTargetCamera(m_spCameraComponent);

		//プレイヤー入力
		m_spInputComponent = std::make_shared<PlayerInputComponent>(*this);
	}
	else
	{
		//敵飛行機入力
		m_spInputComponent = std::make_shared<EnemyInputComponent>(*this);
	}

}

void Aircraft::Update()
{
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	UpdateMove();

	UpdateShoot();

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}
}

void Aircraft::ImGuiUpdate()
{
	if (ImGui::TreeNodeEx("Aircraft", ImGuiTreeNodeFlags_DefaultOpen))
	{
		KdVec3 pos;
		pos = m_mWorld.GetTranslation();

		//ImGui::Text("Position [x:%.2f] [y:%.2f] [z:%.2f]", pos.x, pos.y, pos.z);

		if (ImGui::DragFloat3("Position", &pos.x, 0.01f))
		{
			KdMatrix mTrans;
			mTrans.CreateTranslation(pos.x, pos.y, pos.z);

			//m_mWorld = mTrans;
			m_mWorld.SetTranslation(mTrans.GetTranslation());
		}

		ImGui::TreePop();
	}
}

void Aircraft::UpdateMove()
{
	if (m_spInputComponent == nullptr)
	{
		return;
	}

	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);
	//移動ベクトル作成
	KdVec3 move = { inputMove.x, 0.0f, inputMove.y };

	move.Normalize();

	//移動角度補正
	move *= m_speed;

	/*
	m_mWorld._41 += move.x;
	m_mWorld._42 += move.y;
	m_mWorld._43 += move.z;
	*/

	//移動行列作成
	//Math::Matrix moveMat = DirectX::XMMatrixTranslation(move.x, move.y, move.z);
	KdMatrix moveMat;
	moveMat.CreateTranslation(move.x, move.y, move.z);

	//ワールド行列に合成
	//m_mWorld = DirectX::XMMatrixMultiply(moveMat, m_mWorld);
	m_mWorld = moveMat * m_mWorld;

	//回転ベクトル作成
	const Math::Vector2& inputRotate = m_spInputComponent->GetAxis(Input::Axes::R);
	KdVec3 rotate = { inputRotate.y, 0.0f, -inputRotate.x };

	//回転行列作成
	//Math::Matrix rotateMat = DirectX::XMMatrixRotationX(rotate.x * KdToRadians);
	//rotateMat = DirectX::XMMatrixMultiply(rotateMat, DirectX::XMMatrixRotationZ(rotate.z * KdToRadians));
	KdMatrix rotateMat;
	rotateMat.CreateRotationX(rotate.x * KdToRadians);
	rotateMat.RotateZ(rotate.z * KdToRadians);

	//ワールド行列に合成
	//m_mWorld = DirectX::XMMatrixMultiply(rotateMat, m_mWorld);
	m_mWorld = rotateMat * m_mWorld;
}

void Aircraft::UpdateShoot()
{
	if (m_spInputComponent == nullptr)
	{
		return;
	}

	if (m_spInputComponent->GetButton(Input::Buttons::A))
	{
		if (m_canShoot)
		{
			std::shared_ptr<Missile> spMissile = std::make_shared<Missile>();
			if (spMissile)
			{
				spMissile->Deserialize(KdLoadJson("Data/Scene/Missile.json"));
				spMissile->SetMatrix(m_mWorld);

				Scene::GetInstance().AddObject(spMissile);
			}

			m_canShoot = false;
		}
	}
	else
	{
		m_canShoot = true;
	}
}