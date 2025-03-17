﻿#pragma once 

#include "Resource/ResourceType.h"
#include "Resource/Texture/Texture2D.h"
#include "Resource/Texture/Buffer.h"
#include "RenderObject/Mesh.h"
#include "RenderObject/Material.h"
#include "RHI.h"

#include <map>
#include <memory>

namespace photon 
{
	class ResourceManager 
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;
		void Initialize(RHI* rhi);

		std::shared_ptr<Texture2D> CreateTexture2D(Texture2DDesc desc);
		std::shared_ptr<Buffer> CreateBuffer(BufferDesc desc);
		std::shared_ptr<Mesh> CreateMesh(MeshDesc desc);

		std::shared_ptr<Texture2D> GetTexture2D(UINT64 guid);
		std::shared_ptr<Buffer> GetBuffer(UINT64 guid);
		std::shared_ptr<Mesh> GetMesh(UINT64 guid);

	private:
		std::map<UINT64, std::shared_ptr<Texture2D>> m_Textures;
		std::map<UINT64, std::shared_ptr<Buffer>> m_Buffers;
		std::map<UINT64, std::shared_ptr<Mesh>> m_Meshs;

		RHI* m_Rhi;
	};


}