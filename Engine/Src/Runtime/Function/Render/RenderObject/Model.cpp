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

	

	void Model::GlobalSwitchDiffuseMap(std::shared_ptr<Texture2D> tex)
	{
		std::filesystem::path texPath = tex->name;
		for(auto& meshInfo : meshes)
		{
			meshInfo->diffuseTextures.push_back(tex);
			meshInfo->diffuseTexturePaths.push_back(texPath);
			meshInfo->mat->diffuseMap = tex.get();
		}
	}

	void Model::GlobalSwitchSpecularMap(std::shared_ptr<Texture2D> tex)
	{
		std::filesystem::path texPath = tex->name;
		for (auto& meshInfo : meshes)
		{
			meshInfo->roughnessTextures.push_back(tex);
			meshInfo->roughnessTexturePaths.push_back(texPath);
			meshInfo->mat->roughnessMap = tex.get();
			meshInfo->bOldInverseRoughness = meshInfo->bInverseRoughness;
			meshInfo->bInverseRoughness = true;
			meshInfo->mat->matCBufferData.bInverseRoughness = true;
		}
	}

	void Model::GlobalSwitchNormalMap(std::shared_ptr<Texture2D> tex)
	{
		std::filesystem::path texPath = tex->name;
		for (auto& meshInfo : meshes)
		{
			meshInfo->normalTextures.push_back(tex);
			meshInfo->normalTexturePaths.push_back(texPath);
			meshInfo->mat->normalMap = tex.get();
		}
	}

	void Model::GlobalRebackRawDiffuseMap()
	{
		for (auto& meshInfo : meshes)
		{
			if (meshInfo->diffuseTextures.empty())
				continue;
			meshInfo->mat->diffuseMap = meshInfo->diffuseTextures.front().get();
		}
	}

	void Model::GlobalRebackRawNormalMap()
	{
		for (auto& meshInfo : meshes)
		{
			if (meshInfo->normalTextures.empty())
				continue;
			meshInfo->mat->normalMap = meshInfo->normalTextures.front().get();
		}
	}

	void Model::GlobalRebackRawSpecularMap()
	{
		for (auto& meshInfo : meshes)
		{
			if (meshInfo->roughnessTextures.empty())
				continue;
			meshInfo->mat->roughnessMap = meshInfo->roughnessTextures.front().get();
			meshInfo->bInverseRoughness = meshInfo->bOldInverseRoughness;
		}
	}

	std::string Model::GetGameObjectType()
	{
		return "Model";
	}

}



