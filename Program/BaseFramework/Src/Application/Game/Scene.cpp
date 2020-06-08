﻿#include "Scene.h"
#include "GameObject.h"

#include "EditorCamera.h"
#include "Shooting/StageObject.h"
#include "Shooting/Aircraft.h"
#include "Shooting/Missile.h"

Scene::Scene()		//コンストラタ
{

}

Scene::~Scene()		//デストラクタ
{

}

void Scene::Init()
{
	m_sky.Load("Data/Sky/Sky.gltf");

	m_pCamera = new EditorCamera();

	StageObject* pGround = new StageObject();

	if (pGround)
	{
		pGround->Deserialize();
		m_objects.push_back(pGround);
	}

	Aircraft* pAircraft = new Aircraft();

	if (pAircraft)
	{
		pAircraft->Deserialize();
		m_objects.push_back(pAircraft);
	}
}

void Scene::Release()
{
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}
	
	for (auto pObjects : m_objects)
	{
		delete pObjects;
	}
	m_objects.clear();
}

void Scene::Update()
{
	if (m_editorCameraEnable) {
		if (m_pCamera)
		{
			m_pCamera->Update();
		}
	}

	for (auto pObjects : m_objects)
	{
		pObjects->Update();
	}

	for (auto pObjectItr = m_objects.begin(); pObjectItr != m_objects.end();)
	{
		//寿命が尽きていたらリストから除外
		if ((*pObjectItr)->IsAlive() == false)
		{
			delete (*pObjectItr);
			pObjectItr = m_objects.erase(pObjectItr);
		}
		else
		{
			++pObjectItr;
		}
	}
}

void Scene::Draw()
{
	// エディターカメラをシェーダーにセット
	if (m_editorCameraEnable) {
		if (m_pCamera)
		{
			m_pCamera->SetToShader();
		}
	}
	else 
	{

	}

	// ライトの情報をリセット
	SHADER.m_cb8_Light.Write();

	// エフェクトシェーダーを描画デバイスにセット
	SHADER.m_effectShader.SetToDevice();

	// スカイスフィア拡大
	Math::Matrix skyScale = DirectX::XMMatrixScaling(100.0f, 100.0f, 100.0f);

	SHADER.m_effectShader.SetWorldMatrix(skyScale);

	// モデルの描画(メッシュの情報とマテリアルの情報を渡す)
	SHADER.m_effectShader.DrawMesh(m_sky.GetMesh(), m_sky.GetMaterials());


	//不透明物描画
	SHADER.m_standardShader.SetToDevice();

	

	for (auto pObjects : m_objects)
	{
		pObjects->Draw();
	}

	// デバッグライン描画
	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());

	{
		AddDebugLines(Math::Vector3(), Math::Vector3(0.0f, 10.0f, 0.0f));

		AddDebugSphereLine(Math::Vector3(5.0f, 5.0f, 0.0f), 2.0f);

		AddDebugCoordinateAxisLine(Math::Vector3(0.0f, 5.0f, 5.0f), 3.0f);

		//Zバッファ使用OFF + 書き込みOFF
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

		if (m_debugLines.size() >= 1) {
			SHADER.m_effectShader.SetWorldMatrix(Math::Matrix());

			SHADER.m_effectShader.DrawVertices(m_debugLines, D3D_PRIMITIVE_TOPOLOGY_LINELIST);

			m_debugLines.clear();
		}

		//Zバッファ使用ON + 書き込みON
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);
	}
}

void Scene::AddObject(GameObject* pObject)
{
	if (pObject == nullptr)
	{
		return;
	}

	m_objects.push_back(pObject);
}

void Scene::ImGuiUpdate()
{
	if (ImGui::Begin("Scene"))
	{
		//ImGui::Text(u8"今日は天気がいいから\n飛行機の座標でも表示しようかな");

		ImGui::Checkbox("EditorCamera", &m_editorCameraEnable);

	}
	ImGui::End();
}

void Scene::AddDebugLines(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Color& color)
{
	// ラインの開始頂点
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0.0f, 0.0f };
	ver.Pos = p1;

	m_debugLines.push_back(ver);

	// ラインの終端頂点
	ver.Pos = p2;

	m_debugLines.push_back(ver);
}

void Scene::AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color)
{
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0.0f, 0.0f };

	static constexpr int kDetail = 32;
	for (UINT i = 0; i < kDetail + 1; ++i)
	{
		//XZ平面
		ver.Pos = pos;
		ver.Pos.x += cos((float)i * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)i * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.Pos = pos;
		ver.Pos.x += cos((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		//XY平面
		ver.Pos = pos;
		ver.Pos.x += cos((float)i * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.y += sin((float)i * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.Pos = pos;
		ver.Pos.x += cos((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.y += sin((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		//YZ平面
		ver.Pos = pos;
		ver.Pos.y += cos((float)i * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)i * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.Pos = pos;
		ver.Pos.y += cos((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);
	}
}

void Scene::AddDebugCoordinateAxisLine(const Math::Vector3& pos, float scale)
{
	// X軸
	KdEffectShader::Vertex ver;
	ver.Color = { 1,0,0,1 };
	ver.UV = { 0.0f, 0.0f };
	ver.Pos = pos;

	m_debugLines.push_back(ver);

	ver.Pos.x += scale;

	m_debugLines.push_back(ver);

	// Y軸
	ver.Color = { 0,1,0,1 };
	ver.UV = { 0.0f, 0.0f };
	ver.Pos = pos;

	m_debugLines.push_back(ver);

	ver.Pos.y += scale;

	m_debugLines.push_back(ver);

	// Z軸
	ver.Color = { 0,0,1,1 };
	ver.UV = { 0.0f, 0.0f };
	ver.Pos = pos;

	m_debugLines.push_back(ver);

	ver.Pos.z += scale;

	m_debugLines.push_back(ver);

	m_debugLines.push_back(ver);
}