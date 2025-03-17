#include "ResourceManager.h"

namespace photon 
{

	void ResourceManager::Initialize(RHI* rhi)
	{
		m_Rhi = rhi;
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
		m_Buffers.insert({ newBuffer->guid, newBuffer });
		return newBuffer;
	}

	std::shared_ptr<photon::Mesh> ResourceManager::CreateMesh(MeshDesc desc)
	{
		std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(desc);

		m_Meshs.insert({ newMesh->guid, newMesh });

		return newMesh;
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

}