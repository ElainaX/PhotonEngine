#include "ResourceManager.h"

namespace photon 
{

	void ResourceManager::Initialize(RHI* rhi)
	{
		m_Rhi = rhi;
		m_ResourceLoader = std::make_unique<ResourceLoader>(rhi);
		m_ShaderFactory = std::make_unique<ShaderFactory>();
	}

	std::shared_ptr<photon::Texture2D> ResourceManager::LoadTexture2D(const std::filesystem::path& filepath, bool forceLoadSRGB)
	{
		auto find_it = m_LoadedTextures.find(std::filesystem::canonical(filepath).generic_wstring());
		if(find_it != m_LoadedTextures.end())
		{
			return m_Textures[find_it->second];
		}
		auto texData = m_ResourceLoader->LoadTexture(filepath, forceLoadSRGB);
		auto upload = texData.first;
		auto tex = texData.second;
		m_LoadedTextures[filepath.generic_wstring()] = tex->guid;
		m_Texture2DToUploadBuffer.insert({ tex.get(), upload });
		m_Textures.insert({ tex->guid, tex });
		return tex;
	}

	std::shared_ptr<photon::Texture2D> ResourceManager::CreateTexture2D(Texture2DDesc desc)
	{
		std::shared_ptr<Texture2D> newTex = m_Rhi->CreateTexture2D(desc);
		m_Textures.insert({ newTex->guid, newTex });

		return newTex;
	}

	std::shared_ptr<photon::Buffer> ResourceManager::CreateBuffer(BufferDesc desc)
	{
		std::shared_ptr<Buffer> newBuffer = m_Rhi->CreateBuffer(desc);
		if (desc.cpuResource != nullptr && desc.heapProp == ResourceHeapProperties::Default)
		{
			desc.heapProp = ResourceHeapProperties::Upload;
			std::shared_ptr<Buffer> uploadBuffer = m_Rhi->CreateBuffer(desc);
			m_Rhi->CopyDataGpuToGpu(newBuffer.get(), uploadBuffer.get());
			m_Buffers.insert({ uploadBuffer->guid, uploadBuffer });
			m_BufferToUploadBuffer[newBuffer.get()] = uploadBuffer;
		}
		m_Buffers.insert({ newBuffer->guid, newBuffer });
		
		return newBuffer;
	}

	std::shared_ptr<photon::Mesh> ResourceManager::CreateMesh(MeshDesc desc)
	{
		std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(desc);

		m_Meshs.insert({ newMesh->guid, newMesh });

		return newMesh;
	}

	std::shared_ptr<photon::Material> ResourceManager::CreateMaterial(StaticModelMaterialDataConstants data, UINT64 texGuid, const std::wstring& name)
	{
		if (m_Textures.find(texGuid) == m_Textures.end())
			return nullptr;
		auto newMat = std::make_shared<Material>(data, m_Textures[texGuid].get());
		m_Materials.insert({ newMat->guid, newMat });
		newMat->name = name;
		return newMat;
	}

	std::shared_ptr<photon::Material> ResourceManager::CreateMaterial(StaticModelMaterialDataConstants data, UINT64 diffuseGuid, UINT64 normalGuid, UINT64 roughnessGuid, const std::wstring& name)
	{
		if (m_Textures.find(diffuseGuid) == m_Textures.end() || m_Textures.find(normalGuid) == m_Textures.end() || m_Textures.find(roughnessGuid) == m_Textures.end())
			return nullptr;
		auto newMat = std::make_shared<Material>(data, m_Textures[diffuseGuid].get(), m_Textures[normalGuid].get(), m_Textures[roughnessGuid].get());
		m_Materials.insert({ newMat->guid, newMat });
		newMat->name = name;
		return newMat;
	}

	std::shared_ptr<photon::Material> ResourceManager::CreateMaterial(StaticModelMaterialDataConstants data, UINT64 diffuseGuid, UINT64 normalGuid, const std::wstring& name)
	{
		if (m_Textures.find(diffuseGuid) == m_Textures.end() || m_Textures.find(normalGuid) == m_Textures.end())
			return nullptr;
		auto newMat = std::make_shared<Material>(data, m_Textures[diffuseGuid].get(), m_Textures[normalGuid].get());
		m_Materials.insert({ newMat->guid, newMat });
		newMat->name = name;
		return newMat;
	}

	std::shared_ptr<photon::Material> ResourceManager::CreateMaterial(StaticModelMaterialDataConstants data, Texture2D* diffuse, Texture2D* normal, Texture2D* roughness, const std::wstring& name)
	{
		assert(diffuse);
		auto newMat = std::make_shared<Material>(data, diffuse, normal, roughness);
		newMat->name = name;
		m_Materials.insert({ newMat->guid, newMat });
		return newMat;
	}

	photon::Shader* ResourceManager::LoadShader(const std::wstring& shaderName)
	{
		return m_ShaderFactory->Create(shaderName);
	}

	std::shared_ptr<photon::Model> ResourceManager::LoadModel(const std::filesystem::path& path)
	{
		auto model = m_ResourceLoader->PreLoadModel(path);
		LoadModelToGpu(model.get());
		m_Models.insert({ model->guid, model });
		return model;
	}

	std::shared_ptr<photon::Texture2D> ResourceManager::GetTexture2D(UINT64 guid)
	{
		if(m_Textures.find(guid) != m_Textures.end())
		{
			return m_Textures[guid];
		}
		return nullptr;
	}

	std::shared_ptr<photon::Texture2D> ResourceManager::GetLoadedTexture2D(const std::filesystem::path& filepathRelateToAssetFolder)
	{
		auto path = g_AssetFolder / filepathRelateToAssetFolder;
		return LoadTexture2D(filepathRelateToAssetFolder);
	}

	std::shared_ptr<photon::Buffer> ResourceManager::GetBuffer(UINT64 guid)
	{
		if (m_Buffers.find(guid) != m_Buffers.end())
		{
			return m_Buffers[guid];
		}
		return nullptr;
	}

	std::shared_ptr<photon::Mesh> ResourceManager::GetMesh(UINT64 guid)
	{
		if (m_Meshs.find(guid) != m_Meshs.end())
		{
			return m_Meshs[guid];
		}
		return nullptr;
	}

	std::shared_ptr<photon::Buffer> ResourceManager::GetBufferUploadBuffer(Buffer* buffer)
	{
		if (m_BufferToUploadBuffer.find(buffer) != m_BufferToUploadBuffer.end())
		{
			return m_BufferToUploadBuffer[buffer];
		}
		return nullptr;
	}

	std::shared_ptr<photon::Buffer> ResourceManager::GetBufferUploadBuffer(UINT64 bufferGuid)
	{
		Buffer* buffer = GetBuffer(bufferGuid).get();
		if (m_BufferToUploadBuffer.find(buffer) != m_BufferToUploadBuffer.end())
		{
			return m_BufferToUploadBuffer[buffer];
		}
		return nullptr;
	}

	std::shared_ptr<photon::Buffer> ResourceManager::GetTextureUploadBuffer(Texture2D* tex)
	{
		if (m_Texture2DToUploadBuffer.find(tex) != m_Texture2DToUploadBuffer.end())
		{
			return m_Texture2DToUploadBuffer[tex];
		}
		return nullptr;
	}

	std::shared_ptr<photon::Buffer> ResourceManager::GetTextureUploadBuffer(UINT64 texGuid)
	{
		Texture2D* tex = GetTexture2D(texGuid).get();
		if (m_Texture2DToUploadBuffer.find(tex) != m_Texture2DToUploadBuffer.end())
		{
			return m_Texture2DToUploadBuffer[tex];
		}
		return nullptr;
	}

	void ResourceManager::BindBufferUploadBuffer(Buffer* buffer, std::shared_ptr<Buffer> uploadBuffer)
	{
		m_BufferToUploadBuffer[buffer] = uploadBuffer;
	}

	void ResourceManager::DestoryTexture2D(UINT64 guid)
	{
		if(m_Meshs.find(guid) != m_Meshs.end())
		{
			m_Meshs[guid]->gpuResource->Release();
			m_Meshs[guid]->gpuResource = nullptr;
			m_Meshs.erase(guid);
		}
	}

	void ResourceManager::DestoryTexture2D(Resource* resource)
	{
		UINT64 guid = resource->guid;
		DestoryTexture2D(guid);
	}

	void ResourceManager::LoadModelToGpu(Model* model)
	{
		for(auto& meshInfo : model->meshes)
		{
			meshInfo->mesh = CreateMesh(meshInfo->meshDesc);
			std::shared_ptr<Texture2D> diffuseTex;
			std::shared_ptr<Texture2D> normalTex;
			std::shared_ptr<Texture2D> roughnessTex;
			if(!meshInfo->diffuseTexturePaths.empty())
			{
				diffuseTex = LoadTexture2D(meshInfo->diffuseTexturePaths[0], true);
				meshInfo->diffuseTextures.push_back(diffuseTex);
			}
			if (!meshInfo->normalTexturePaths.empty())
			{
				normalTex = LoadTexture2D(meshInfo->normalTexturePaths[0], true);
				meshInfo->normalTextures.push_back(normalTex);
			}
			if(!meshInfo->roughnessTexturePaths.empty())
			{
				roughnessTex = LoadTexture2D(meshInfo->roughnessTexturePaths[0], true);
				meshInfo->roughnessTextures.push_back(roughnessTex);
			}

			StaticModelMaterialDataConstants data;
			data.diffuseAlbedo = { 1.0f, 0.0f, 1.0f, 1.0f };
			data.bInverseRoughness = meshInfo->bInverseRoughness;
			data.fresnelR0 = { 0.04, 0.04, 0.04 };
			data.roughness = 1.0f;
			auto mat = CreateMaterial(data, diffuseTex.get(), normalTex.get(), roughnessTex.get(), meshInfo->name);
			meshInfo->mat = mat.get();
		}
	}

}