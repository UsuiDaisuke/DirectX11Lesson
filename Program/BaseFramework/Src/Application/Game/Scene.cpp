#include "Scene.h"
#include "GameObject.h"
#include "../Component/CameraComponent.h"

#include "EditorCamera.h"
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
	//Jsonファイルを開く
	std::ifstream ifs("Data/test.json");
	if (ifs.fail()) { assert(0 && "Jsonファイルのパスが間違っています"); }

	//文字列として全読み込み
	std::string strJson((std::istream_iterator<char>(ifs)), std::istream_iterator<char>());

	//文字列のJSONを解析(パース)する
	std::string err;
	json11::Json jsonObj = json11::Json::parse(strJson, err);
	if (err.size() > 0) { assert(0 && "読み込んだファイルのJson変換に失敗"); }
	
	//値アクセス
	{
		OutputDebugStringA(jsonObj["Name"].string_value().append("\n").c_str());
		//auto name = jsonObj["Name"].string_value();本来値を取得するならこれだけで良い
		OutputDebugStringA(std::to_string(jsonObj["Hp"].int_value()).append("\n").c_str());
	}

	//配列アクセス
	{
		auto pos = jsonObj["Position"].array_items();
		for (auto&& p : pos)
		{
			OutputDebugStringA(std::to_string(p.number_value()).append("\n").c_str());
		}
		//配列添え字アクセス
		OutputDebugStringA(std::to_string(pos[0].number_value()).append("\n").c_str());
		OutputDebugStringA(std::to_string(pos[1].number_value()).append("\n").c_str());
		OutputDebugStringA(std::to_string(pos[2].number_value()).append("\n").c_str());

	}

	//Object取得
	{
		auto& object = jsonObj["monster"].object_items();
		OutputDebugStringA(object["name"].string_value().append("\n").c_str());
		OutputDebugStringA(std::to_string(object["hp"].int_value()).append("\n").c_str());
		OutputDebugStringA(std::to_string(object["pos"][0].number_value()).append("\n").c_str());
		OutputDebugStringA(std::to_string(object["pos"][1].number_value()).append("\n").c_str());
		OutputDebugStringA(std::to_string(object["pos"][2].number_value()).append("\n").c_str());
	}

	//Object配列取得
	{
		auto& objects = jsonObj["techniques"].array_items();
		for (auto&& object : objects)
		{
			//共通の要素はチェックなしでアクセス
			OutputDebugStringA(object["name"].string_value().append("\n").c_str());
			OutputDebugStringA(std::to_string(object["atk"].int_value()).append("\n").c_str());
			OutputDebugStringA(std::to_string(object["hitrate"].number_value()).append("\n").c_str());

			//固有パラメータはチェックしてアクセス
			if (object["effect"].is_string())
			{
				OutputDebugStringA(object["effect"].string_value().append("\n").c_str());
			}
		}
	}

	m_spSky = KdResourceFactory::GetInstance().GetModel("Data/Sky/Sky.gltf");

	m_spCamera = std::make_shared<EditorCamera>();

	Deserialize();
}

void Scene::Deserialize()
{
	m_poly.Init(10.0f, 10.0f, { 1,1,1,1 });
	m_poly.SetTexture(KdResFac.GetTexture("Data/Texture/Explosion00.png"));

	LoadScene("Data/Scene/ShootingGame.json");
}

void Scene::Release()
{
	if (m_spSky)
	{
		m_spSky.reset();
	}

	if (m_spCamera)
	{
		m_spCamera.reset();
	}
	
	m_spObjects.clear();
}

void Scene::Update()
{
	if (m_editorCameraEnable) {
		if (m_spCamera)
		{
			m_spCamera->Update();
		}
	}

	for (auto pObjects : m_spObjects)
	{
		pObjects->Update();
	}

	for (auto spObjectItr = m_spObjects.begin(); spObjectItr != m_spObjects.end();)
	{
		//寿命が尽きていたらリストから除外
		if ((*spObjectItr)->IsAlive() == false)
		{
			spObjectItr = m_spObjects.erase(spObjectItr);
		}
		else
		{
			++spObjectItr;
		}
	}
}

void Scene::Draw()
{
	// エディターカメラをシェーダーにセット
	if (m_editorCameraEnable) {
		if (m_spCamera)
		{
			m_spCamera->SetToShader();
		}
	}
	else 
	{
		std::shared_ptr<CameraComponent> spCamera = m_wpTargetCamera.lock();
		if (spCamera)
		{
			spCamera->SetToShader();
		}
	}

	// ライトの情報をリセット
	SHADER.m_cb8_Light.Write();

	// エフェクトシェーダーを描画デバイスにセット
	SHADER.m_effectShader.SetToDevice();

	// スカイスフィア拡大
	Math::Matrix skyScale = DirectX::XMMatrixScaling(100.0f, 100.0f, 100.0f);

	SHADER.m_effectShader.SetWorldMatrix(skyScale);

	// モデルの描画(メッシュの情報とマテリアルの情報を渡す)
	if (m_spSky) {
		SHADER.m_effectShader.DrawMesh(m_spSky->GetMesh().get(), m_spSky->GetMaterials());
	}

	//不透明物描画
	SHADER.m_standardShader.SetToDevice();

	for (auto pObjects : m_spObjects)
	{
		pObjects->Draw();
	}

	//半透明物描画
	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());

	{//ポリゴンの描画
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullNone);

		KdMatrix tempMat;
		tempMat.SetTranslation({ 0.0f, 5.0f, 0.0f });

		SHADER.m_effectShader.SetWorldMatrix(tempMat);
		SHADER.m_effectShader.WriteToCB();
		m_poly.Draw(0);

		tempMat.SetTranslation({ 5.0f, 10.0f, 1.0f });
		SHADER.m_effectShader.SetWorldMatrix(tempMat);
		SHADER.m_effectShader.WriteToCB();
		m_poly.Draw(0);

		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);
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

void Scene::LoadScene(const std::string& sceneFilename)
{
	// GameObjectリストを空にする
	m_spObjects.clear();

	//JSON読み込み
	json11::Json json = KdResFac.GetJSON(sceneFilename);
	if (json.is_null())
	{
		assert(0 && "[LoadScene]jsonファイル読み込み失敗");
		return;
	}

	// オブジェクトリスト取得
	auto& ObjectDataList = json["GameObjects"].array_items();

	//オブジェクト生成ループ
	for (auto&& objJsonData : ObjectDataList)
	{
		//オブジェクト作成
		auto newGameObj = CreateGameObject(objJsonData["ClassName"].string_value());

		KdMergePrefab(objJsonData);

		//オブジェクトのデシリアライズ
		newGameObj->Deserialize(objJsonData);

		//リストへ追加
		AddObject(newGameObj);
	}
}

void Scene::AddObject(std::shared_ptr<GameObject> pObject)
{
	if (pObject == nullptr)
	{
		return;
	}

	m_spObjects.push_back(pObject);
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
