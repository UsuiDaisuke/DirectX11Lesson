#pragma once

//===============================
//  リソース管理クラス
//  ・デザインパターンのFlyweightパターンを採用
//===============================

class KdResourceFactory
{
public:
	//モデルデータの取得
	std::shared_ptr<KdModel> GetModel(const std::string& filename);

	//管理を破棄する
	void Clear()
	{
		m_modelMap.clear();
	}

private:
	//モデルデータ管理マップ
	std::unordered_map<std::string, std::shared_ptr<KdModel>> m_modelMap;

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