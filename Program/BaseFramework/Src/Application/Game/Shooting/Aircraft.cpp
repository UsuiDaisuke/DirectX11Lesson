#include "Aircraft.h"
#include "Missile.h"
#include "../Scene.h"

void Aircraft::Deserialize()
{
	m_spModel = std::make_shared<KdModel>();
	if (m_spModel == nullptr)
	{
		return;
	}

	if (m_spModel->Load("Data/Aircraft/Aircraft_body.gltf") == false)
	{
		m_spModel = nullptr;
	}

	// 初期配置座標を地面から少し浮いた位置にする
	m_mWorld.CreateTranslation(0.0f, 5.0f, 0.0f);

	m_mOffset = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(15));
	m_mOffset *= Math::Matrix::CreateTranslation(0.0f, 2.0f, -10.0f);

	m_mProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60),
		D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 5000.0f);
}

void Aircraft::Update()
{
	UpdateMove();

	UpdateShoot();
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

void Aircraft::SetCameraToShader()
{
	//追従カメラ行列の作成 : 追従カメラからの相対座標行列に、ゲーム上の飛行機の絶対座標行列を合成
	KdMatrix mCam = m_mOffset * m_mWorld;

	//追従カメラ座標をシェーダーにセット
	SHADER.m_cb7_Camera.Work().CamPos = mCam.GetTranslation();

	//追従カメラのビュー行列をシェーダーにセット
	SHADER.m_cb7_Camera.Work().mV = mCam.Invert();

	//追従カメラの射影行列をシェーダーにセット
	SHADER.m_cb7_Camera.Work().mP = m_mProj;

	//カメラ情報(ビュー行列、射影行列)を、シェーダーの定数バッファへセット
	SHADER.m_cb7_Camera.Write();
}

void Aircraft::UpdateMove()
{
	//移動ベクトル作成
	KdVec3 move;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		move.z = 1.0f;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		move.z = -1.0f;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		move.x = 1.0f;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		move.x = -1.0f;
	}

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
	KdVec3 rotate;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		rotate.x = 1.0f;
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		rotate.z = 1.0f;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		rotate.x = -1.0f;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		rotate.z = -1.0f;
	}

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
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (m_canShoot)
		{
			std::shared_ptr<Missile> spMissile = std::make_shared<Missile>();
			if (spMissile)
			{
				spMissile->Deserialize();
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