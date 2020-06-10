#include"KdResourceFactory.h"

std::shared_ptr<KdModel> KdResourceFactory::GetModel(const std::string& filename)
{
	//filenameのモデルがあるか？
	auto itFound = m_modelMap.find(filename);
	//ない場合
	if (itFound == m_modelMap.end())
	{
		//生成 & 読み込み
		auto newModel = std::make_shared<KdModel>();
		if (newModel->Load(filename) == false)
		{
			//読み込み失敗は、nullを返す
			return nullptr;
		}

		//リスト(map)に登録
		m_modelMap[filename] = newModel;
		//リソースを返す
		return newModel;
	}
	//ある場合
	{
		return (*itFound).second;
	}
}