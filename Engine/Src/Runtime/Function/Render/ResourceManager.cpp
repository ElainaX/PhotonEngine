#include "ResourceManager.h"

namespace photon 
{

	void ResourceManager::Initialize(RHI* rhi)
	{
		m_Rhi = rhi;
		m_ResourceLoader = std::make_unique<ResourceLoader>(rhi);
		m_ShaderFactory = std::make_unique<ShaderFactory>();
	}

	std::shared_ptr<photon::Texture2D> ResourceManager::LoadTexture2D(const std::filesystem::path& filepath, bool isSRGB /*= true*/, bool isHDR)
	{
		auto find_it = m_LoadedTextures.find(filepath.generic_wstring());
		if(find_it != m_LoadedTextures.end())
		{
			return m_Textures[find_it->second];
		}
		auto texData = m_ResourceLoader->LoadTexture(filepath);
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

	photon::Shader* ResourceManager::LoadShader(const std::wstring& shaderName)
	{
		return m_ShaderFactory->Create(shaderName);
	}

	std::shared_ptr<photon::Texture2D> ResourceManager::GetTexture2D(UINT64 guid)
	{
		if(m_Textures.find(guid) != m_Textures.end())
		{
			return m_Textures[guid];
		}
		return nullptr;
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

}