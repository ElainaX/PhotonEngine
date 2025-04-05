#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.h"
#include "Material.h"
#include "Resource/Texture/Texture2D.h"
#include "Resource/Texture/Buffer.h"

#include <vector>
#include <filesystem>
//#include <future>
#include <memory>
#include <utility>

namespace photon 
{

	struct ModelMeshRelateInfo
	{
		std::wstring name = L"Mesh";

		MeshDesc meshDesc;
		std::vector<std::filesystem::path> diffuseTexturePaths;
		std::vector<std::filesystem::path> roughnessTexturePaths;
		std::vector<std::filesystem::path> normalTexturePaths;

		Material* mat;
		std::shared_ptr<Mesh> mesh;

		std::vector<std::shared_ptr<Texture2D>> diffuseTextures;
		std::vector<std::shared_ptr<Texture2D>> roughnessTextures;
		std::vector<std::shared_ptr<Texture2D>> normalTextures;
		bool bInverseRoughness = false;
	};

	class Model : public Resource, public GameObject
	{
	public:
		Model()
		{
			GameObjectName = GetGameObjectType();
		}
		

		void AddAssimpTexturePathToBack(std::filesystem::path texturePath, aiTextureType type);

		//void SetLoadFuture(std::future<bool>&& loadFuture)
		//{
		//	m_LoadOverFuture = std::move(loadFuture);
		//}
		void SetLoadOver(bool loadOver = true)
		{
			m_bLoadOver = loadOver;
		}
		bool IsLoadedOver();

		VertexType vertexType = VertexType::VertexSimple; 
		std::vector<std::shared_ptr<ModelMeshRelateInfo>> meshes;

		bool bHasDiffuseMap = false;
		bool bHasNormalMap = false;
		bool bHasRoughnessMap = false;

		


		std::string GetGameObjectType() override;

	private:


		//std::future<bool> m_LoadOverFuture;
		bool m_bLoadOver = false;
	};


}