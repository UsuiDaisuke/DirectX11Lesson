#include"StageObject.h"

#include"Application/Component/ModelComponent.h"

void StageObject::Deserialize()
{
	m_spModelComponent->SetModel(KdResFac.GetModel("Data/StageMap/StageMap.gltf"));
}