#include "Model.h"
#include "Platform/FileSystem/FileSystem.h"

namespace photon 
{

	void Model::AddAssimpTexturePathToBack(std::filesystem::path texturePath, aiTextureType type)
	{
		auto folder = Path::GetFileFolder(name);
		texturePath = folder / texturePath;
		auto backInfo = meshes.back();
		switch(type)
		{
		case aiTextureType_DIFFUSE:
			backInfo->diffuseTexturePaths.push_back(texturePath);
			break;
		case aiTextureType_NORMALS:
			backInfo->normalTexturePaths.push_back(texturePath);
			break;
		case aiTextureType_SPECULAR:
			backInfo->roughnessTexturePaths.push_back(texturePath);
			backInfo->bInverseRoughness = true;
			break;
		}
	}

	bool Model::IsLoadedOver()
	{
		return m_bLoadOver;
	}

	

	std::string Model::GetGameObjectType()
	{
		return "Model";
	}

}



