﻿#pragma once

//===============================
//  リソース管理クラス
//  ・デザインパターンのFlyweightパターンを採用
//===============================

class KdResourceFactory
{
public:
	//モデルデータの取得
	std::shared_ptr<KdModel> GetModel(const std::string& filename);

	//テクスチャデータ取得
	std::shared_ptr<KdTexture> GetTexture(const std::string& filename);

	json11::Json GetJSON(const std::string& filename);

	std::shared_ptr<KdSoundEffect> GetSound(const std::string& filename);

	//管理を破棄する
	void Clear()
	{
		m_modelMap.clear();
		m_texMap.clear();
		m_jsonMap.clear();

		m_soundMap.clear();
	}

private:
	//JSON読み込み
	json11::Json LoadJSON(const std::string& filename);

	//モデルデータ管理マップ
	std::unordered_map<std::string, std::shared_ptr<KdModel>> m_modelMap;

	// テクスチャ管理マップ
	std::unordered_map< std::string, std::shared_ptr<KdTexture>> m_texMap;

	// JSON管理マップ
	std::unordered_map< std::string, json11::Json > m_jsonMap;

	// JSON管理マップ
	std::unordered_map< std::string, std::shared_ptr<KdSoundEffect> > m_soundMap;

//====================
//  シングルトン
//====================
private:
	KdResourceFactory() {};

public:
	static KdResourceFactory& GetInstance()
	{
		static KdResourceFactory instance;
		return instance;
	}

};

#define KdResFac KdResourceFactory::GetInstance()