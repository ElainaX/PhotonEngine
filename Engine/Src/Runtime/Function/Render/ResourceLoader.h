#pragma once
#include "Resource/Texture/Texture2D.h"
#include "RenderObject/Model.h"
#include "Resource/Texture/Buffer.h"
#include "DirectXTK/WICTextureLoader12.h"
#include "RHI.h"
#include "assimp/Importer.hpp"
#include <filesystem>
#include <utility>
#include <string>
#include <array>

namespace photon 
{
	class ResourceLoader
	{
	public:
		ResourceLoader(RHI* rhi)
			: m_Rhi(rhi) {
		}

		std::pair<std::shared_ptr<Buffer>, std::shared_ptr<Texture2D>> LoadTexture(const std::filesystem::path& filepath, bool bForceLoadSRGB);
		std::shared_ptr<Model> PreLoadModel(const std::filesystem::path& filepath);

		RHI* m_Rhi;

	private:
		bool LoadAssimpSceneModel(const std::filesystem::path& filepath, std::shared_ptr<photon::Model> model);
		void ProcessSingleAssimpNode(aiNode* node, const aiScene* scene, std::shared_ptr<photon::Model> model);
		void LoadAssimpMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<photon::Model> model);
		void LoadAssimpMaterialTextures(aiMaterial* mat, aiTextureType type, std::shared_ptr<photon::Model> model);
		void TryLoadModelTexturesFromFile(std::shared_ptr<photon::Model> model);

		const std::wstring diffuseMapSuffix = { L"_c" };
		const std::wstring normalMapSuffix = { L"_n" };
		const std::wstring roughnessMapSuffix = { L"_r" };
		const std::wstring specularMapSuffix = { L"_g" };
	};


}