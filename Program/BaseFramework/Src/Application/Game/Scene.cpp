﻿#include "Scene.h"
#include "GameObject.h"
#include "../Component/CameraComponent.h"

#include "../main.h"

#include "EditorCamera.h"
#include "Shooting/Aircraft.h"
#include "Shooting/Missile.h"
#include "Shooting/AnimationEffect.h"

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

	m_EditorLog = std::make_shared<ImGuiLogWindow>();

	// サウンド関連の処理
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;

	// サウンドエンジンの作成
	m_audioEng = std::make_unique<DirectX::AudioEngine>(eflags);
	m_audioEng->SetReverb(DirectX::Reverb_Default);

	// サウンドの読み込み
	if (m_audioEng != nullptr)
	{
		try
		{
			// wstringに変換(const char* は受け取ってくれない)
			std::wstring wFilename = sjis_to_wide("Data/Audio/BGM/Castle.wav");

			// BGMの読み込み
			m_soundEffects.push_back(std::make_unique<DirectX::SoundEffect>(m_audioEng.get(), wFilename.c_str()));
			
			// SEの読み込み
			wFilename = sjis_to_wide("Data/Audio/SE/ItemGet.wav");
			m_soundEffects.push_back(std::make_unique<DirectX::SoundEffect>(m_audioEng.get(), wFilename.c_str()));
		}
		catch (...)
		{
			assert(0 && "Sound File Load Error");
		}
	}

	// BGMサウンドの再生
	if (m_soundEffects[0] != nullptr)
	{
		// 再生オプション
		DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default;

		// サウンドエフェクトからサウンドインスタンスの作成
		auto instance = (m_soundEffects[0]->CreateInstance(flags));
		// サウンドインスタンスを使って再生
		if (instance)
		{
			//instance->Play();
		}
		m_instances.push_back(std::move(instance));
	}

	Deserialize();
}

void Scene::Deserialize()
{
	LoadScene("Data/Scene/ActionGame.json");

	//LoadScene("Data/Scene/ActionGame.json");

	std::shared_ptr<AnimationEffect> spExp = std::make_shared<AnimationEffect>();
	spExp->SetAnimationInfo(KdResFac.GetTexture("Data/Texture/Explosion00.png"), 10.0f, 5, 5, 0.0f);
	AddObject(spExp);
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

	// サウンドの後処理
	m_audioEng = nullptr;

	for (auto& ins : m_instances)
	{
		ins = nullptr;
	}
}

void Scene::Update()
{
	if (m_editorCameraEnable) {
		if (m_spCamera)
		{
			m_spCamera->Update();
		}
	}

	{
		//疑似的な太陽の表示
		const KdVec3 sunPos = { 0.f,5.f,0.f };
		KdVec3 sunDir = m_lightDir;
		sunDir.Normalize();
		KdVec3 color = m_lightColor;
		color.Normalize();
		Math::Color sunColor = color;
		sunColor.w = 1.0f;
		AddDebugLines(sunPos, sunPos + sunDir * 2, sunColor);
		AddDebugSphereLine(sunPos, 0.5f, sunColor);
	}


	if (pauseFlag < PAUSE_ON_DOWN)
	{
		auto selectObject = m_wpImguiSelectObj.lock();

		for (auto pObjects : m_spObjects)
		{
			if (pObjects == selectObject) { continue; }
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

		// シーンの遷移リクエストがあったらロードする
		if (m_isRequestChangeScene)
		{
			ExecChangeScene();
		}
	}

	if (GetAsyncKeyState('P'))
	{
		// SEならせるかどうか
		if (m_canPlaySE)
		{
			// サウンドエフェクトからサウンドインスタンスを作成
			DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default;

			// 再生
			auto instance = (m_soundEffects[1]->CreateInstance(flags));

			if (instance)
			{
				instance->Play();
			}

			// 再生中インスタンスリストに加える
			m_instances.push_back(std::move(instance));

			m_canPlaySE = false;
		}
	}
	else
	{
		m_canPlaySE = true;
	}

	// サウンドエンジンの更新
	if (m_audioEng == nullptr)
	{
		m_audioEng->Update();
	}

	// 再生中ではないインスタンスは終了したとしてリストから削除
	for (auto iter = m_instances.begin(); iter != m_instances.end();)
	{
		if (iter->get()->GetState() != DirectX::SoundState::PLAYING)
		{
			// リストから削除
			iter = m_instances.erase(iter);
			continue;
		}

		++iter;
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
		SHADER.m_effectShader.DrawMesh(m_spSky->GetMesh(0).get(), m_spSky->GetMaterials());
	}

	//不透明物描画
	//SHADER.m_standardShader.SetToDevice();
	SHADER.m_modelShader.SetToDevice();

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

		for (auto spObj : m_spObjects)
		{
			spObj->DrawEffect();
		}

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

void Scene::Reset()
{
	m_spObjects.clear();
	m_wpImguiSelectObj.reset();
	m_wpTargetCamera.reset();
	m_spSky = nullptr;
}

void Scene::LoadScene(const std::string& sceneFilename)
{
	// 各項目を空にする
	Reset();

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

	m_spSky = KdResourceFactory::GetInstance().GetModel("Data/Sky/Sky.gltf");
}

void Scene::AddObject(std::shared_ptr<GameObject> pObject)
{
	if (pObject == nullptr)
	{
		return;
	}

	m_spObjects.push_back(pObject);
}

std::shared_ptr<GameObject> Scene::FindObjectWithName(const std::string& name)
{
	for (auto&& obj : m_spObjects)
	{
		if (obj->GetName() == name) { return obj; }
	}

	// 見つからなかったらnullを返す
	return nullptr;
}

void Scene::ImGuiUpdate()
{
	if (ImGui::Begin("Scene"))
	{
		//ImGui::Text(u8"今日は天気がいいから\n飛行機の座標でも表示しようかな");

		ImGui::Checkbox("EditorCamera", &m_editorCameraEnable);

		if (ImGui::CollapsingHeader("Object List", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto&& rObj : m_spObjects)
			{
				// 選択したものかどうかを判断する
				bool selected = (rObj == m_wpImguiSelectObj.lock());

				ImGui::PushID(rObj.get());

				if (ImGui::Selectable(rObj->GetName().c_str(), selected))
				{
					m_wpImguiSelectObj = rObj;
				}

				ImGui::PopID();
			}
		}
	}
	ImGui::End();

	// インスペクターウィンド
	if (ImGui::Begin("Inspector"))
	{
		auto selectObject = m_wpImguiSelectObj.lock();

		if (selectObject)
		{
			selectObject->ImGuiUpdate();
		}
	}
	ImGui::End();

	// プレハブウィンド
	if (ImGui::Begin("PrefabFactory"))
	{
		ImGui::InputText("", &Createpath);

		ImGui::SameLine();
		if (ImGui::Button(u8"Jsonパス取得"))
		{
			std::string filepath = "";
			if (APP.m_window.OpenFileDialog
					(filepath, 
					"ファイルを開く", 
					"JSONファイル\0*.json\0")
				)
			{
				Createpath = filepath;
			}
		}

		if (ImGui::Button("Create"))
		{
			json11::Json json = KdResFac.GetJSON(Createpath);
			if (json.is_null())
			{
				m_EditorLog->AddLog(u8"%s 読み込み失敗", Createpath.c_str());
				return;
			}

			if (json["ClassName"].is_null())
			{
				m_EditorLog->AddLog(u8"%s 生成失敗", Createpath.c_str());
				return;
			}

			auto newGameObj = CreateGameObject(json["ClassName"].string_value());

			KdMergePrefab(json);

			//オブジェクトのデシリアライズ
			newGameObj->Deserialize(json);

			//リストへ追加
			AddObject(newGameObj);
			m_EditorLog->AddLog(u8"%s 生成完了", Createpath.c_str());
		}
	}
	ImGui::End();

	m_EditorLog->ImGuiUpdate("Log Window");

	if (ImGui::Begin("LightSettings"))
	{

		if (ImGui::DragFloat3("Direction", &m_lightDir.x, 0.01f))
		{
			SHADER.m_cb8_Light.Work().DL_Dir = m_lightDir;
			SHADER.m_cb8_Light.Work().DL_Dir.Normalize();
		}
		if (ImGui::DragFloat3("Color", &m_lightColor.x, 0.01f))
		{
			SHADER.m_cb8_Light.Work().DL_Color = m_lightColor;
		}
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

void Scene::RequestChangeScene(const std::string& fileName)
{
	m_nextSceneFileName = fileName;

	m_isRequestChangeScene = true;
}

void Scene::ExecChangeScene()
{
	LoadScene(m_nextSceneFileName);

	m_isRequestChangeScene = false;
}