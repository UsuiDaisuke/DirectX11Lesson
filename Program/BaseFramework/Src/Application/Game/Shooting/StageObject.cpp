#include"StageObject.h"

void StageObject::Deserialize()
{
	m_spModel = KdResourceFactory::GetInstance().GetModel("Data/StageMap/StageMap.gltf");
}