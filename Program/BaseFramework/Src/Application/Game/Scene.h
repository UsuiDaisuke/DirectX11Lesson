﻿#pragma once

// 前方宣言
class GameObject;
class EditorCamera;

class Scene
{
public:

	static Scene& GetInstance()
	{
		static Scene instance;
		return instance;
	}

	~Scene();

	void Init();		//初期化
	void Deserialize();
	void Release();		//解放
	void Update();		//更新処理
	void Draw();		//描画処理

	void AddObject(std::shared_ptr<GameObject> pObject);

	void ImGuiUpdate();	//ImGuiの更新

	// デバッグライン描画
	void AddDebugLines(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Color& color = { 1,1,1,1 });

	// デバッグスフィア描画
	void AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color = { 1,1,1,1 });

	// デバッグ軸描画
	void AddDebugCoordinateAxisLine(const Math::Vector3& pos, float scale = 1.0f);

private:
	Scene();

	std::shared_ptr<KdModel> m_spSky = nullptr;	// スカイスフィア
	std::shared_ptr <EditorCamera>	m_spCamera = nullptr;
	bool			m_editorCameraEnable = true;

	std::list<std::shared_ptr<GameObject>> m_objects;

	// デバッグライン描画用の頂点配列
	std::vector<KdEffectShader::Vertex> m_debugLines;
};