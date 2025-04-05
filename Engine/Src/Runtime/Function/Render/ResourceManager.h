#pragma once 

#include "Resource/ResourceType.h"
#include "Resource/Texture/Texture2D.h"
#include "Resource/Texture/Buffer.h"
#include "RenderObject/Mesh.h"
#include "RenderObject/Model.h"
#include "RenderObject/Material.h"
#include "Shader/ShaderFactory.h"
#include "RHI.h"
#include "ResourceLoader.h"
#include "Platform/FileSystem/FileSystem.h"

#include <map>
#include <unordered_map>
#include <memory>
#include <filesystem>

namespace photon 
{
	class ResourceManager 
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;
		void Initialize(RHI* rhi);

		std::shared_ptr<Texture2D> LoadTexture2D(const std::filesystem::path& filepath, bool forceLoadSRGB = false);
		std::shared_ptr<Texture2D> CreateTexture2D(Texture2DDesc desc);
		std::shared_ptr<Buffer> CreateBuffer(BufferDesc desc);
		std::shared_ptr<Mesh> CreateMesh(MeshDesc desc);
		std::shared_ptr<Material> CreateMaterial(StaticModelMaterialDataConstants data, UINT64 texGuid, const std::wstring& name);
		std::shared_ptr<Material> CreateMaterial(StaticModelMaterialDataConstants data, UINT64 diffuseGuid, UINT64 normalGuid, const std::wstring& name);
		std::shared_ptr<Material> CreateMaterial(StaticModelMaterialDataConstants data, UINT64 diffuseGuid, UINT64 normalGuid, UINT64 roughnessGuid, const std::wstring& name);
		std::shared_ptr<Material> CreateMaterial(StaticModelMaterialDataConstants data, Texture2D* diffuse, Texture2D* normal, Texture2D* roughness, const std::wstring& name);
		Shader* LoadShader(const std::wstring& shaderName);
		std::shared_ptr<Model> LoadModel(const std::filesystem::path& path);

		std::shared_ptr<Texture2D> GetTexture2D(UINT64 guid);
		std::shared_ptr<Texture2D> GetLoadedTexture2D(const std::filesystem::path& filepathRelateToAssetFolder);
		std::shared_ptr<Buffer> GetBuffer(UINT64 guid);
		std::shared_ptr<Mesh> GetMesh(UINT64 guid);

		std::shared_ptr<Buffer> GetBufferUploadBuffer(Buffer* buffer);
		std::shared_ptr<Buffer> GetBufferUploadBuffer(UINT64 bufferGuid);
		std::shared_ptr<Buffer> GetTextureUploadBuffer(Texture2D* tex);
		std::shared_ptr<Buffer> GetTextureUploadBuffer(UINT64 texGuid);


		void BindBufferUploadBuffer(Buffer* buffer, std::shared_ptr<Buffer> uploadBuffer);


		void DestoryTexture2D(UINT64 guid);
		void DestoryTexture2D(Resource* resource);

	private:
		void LoadModelToGpu(Model* model);

		std::unique_ptr<ShaderFactory> m_ShaderFactory;
		std::unique_ptr<ResourceLoader> m_ResourceLoader;
		std::unordered_map<Buffer*, std::shared_ptr<Buffer>> m_BufferToUploadBuffer;
		std::unordered_map<Texture2D*, std::shared_ptr<Buffer>> m_Texture2DToUploadBuffer;
		std::map<UINT64, std::shared_ptr<Texture2D>> m_Textures;
		std::map<UINT64, std::shared_ptr<Buffer>> m_Buffers;
		std::map<UINT64, std::shared_ptr<Mesh>> m_Meshs;
		std::map<UINT64, std::shared_ptr<Material>> m_Materials;
		std::map<UINT64, std::shared_ptr<Model>> m_Models;
		std::map<std::wstring, UINT64> m_LoadedTextures;


		RHI* m_Rhi = nullptr;
	};


}