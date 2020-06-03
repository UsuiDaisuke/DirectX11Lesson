#pragma once

// 前方宣言
class StageObject;
class EditorCamera;
class Aircraft;
class Missile;

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
	void Release();		//解放
	void Update();		//更新処理
	void Draw();		//描画処理

	void AddMissile(Missile* pMissile);

	void ImGuiUpdate();	//ImGuiの更新

	// デバッグライン描画
	void AddDebugLines(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Color& color = { 1,1,1,1 });

	// デバッグスフィア描画
	void AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color = { 1,1,1,1 });

	// デバッグ軸描画
	void AddDebugCoordinateAxisLine(const Math::Vector3& pos, float scale = 1.0f);

private:
	Scene();

	KdModel m_sky;	// スカイスフィア
	EditorCamera*	m_pCamera;
	StageObject*	m_pGround = nullptr;	// ステージ
	Aircraft*		m_pAircraft = nullptr;	// 飛行機
	bool			m_editorCameraEnable = true;

	std::list<Missile*> m_missiles;

	// デバッグライン描画用の頂点配列
	std::vector<KdEffectShader::Vertex> m_debugLines;
};