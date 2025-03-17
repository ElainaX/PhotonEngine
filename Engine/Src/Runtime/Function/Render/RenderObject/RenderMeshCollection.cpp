﻿#include "RenderMeshCollection.h"
#include "Function/Render/DX12RHI/DX12Resource/VertexType.h"
#include "Macro.h"
namespace photon 
{

	void RenderMeshCollection::PushMesh(std::shared_ptr<Mesh> mesh)
	{
		if(mesh->bIsLoad)
		{
			LOG_ERROR("mesh {} can't load in two meshCollection!", mesh->name);
			return;
		}
		m_Meshes.insert({ mesh->guid, mesh });
		vertexType = mesh->type;
		vertexSizeInBytes += mesh->vertexRawData->GetBufferSize();
		indexSizeInBytes += mesh->indexRawData->GetBufferSize();
		mesh->bIsLoad = true;
	}

	void RenderMeshCollection::EndPush(RHI* rhi)
	{
		UINT vertexStride = VertexTypeToSizeInBytes(vertexType);
		UINT indexStride = 4;

		char* AllVertBuffer = new char[vertexSizeInBytes];
		char* AllIndexBuffer = new char[indexSizeInBytes];

		UINT vertOffset = 0;
		UINT indexOffset = 0;
		

		for(auto& meshKeyAndVal : m_Meshes)
		{
			auto& mesh = meshKeyAndVal.second;

			UINT meshVertSize = mesh->vertexRawData->GetBufferSize();
			UINT meshIndexSize = mesh->indexRawData->GetBufferSize();

			char* vertDst = AllVertBuffer + vertOffset;
			char* indexDst = AllIndexBuffer + indexOffset;
			CopyMemory(vertDst, mesh->vertexRawData->GetBufferPointer(), meshVertSize);
			CopyMemory(indexDst, mesh->indexRawData->GetBufferPointer(), meshIndexSize);

			mesh->baseVertexLocation = vertOffset;
			mesh->startIndexLocation = indexOffset;
			mesh->indexCount = meshIndexSize / indexStride;

			vertOffset += meshVertSize;
			indexOffset += meshIndexSize;
		}

		vertexBufferGpu = std::make_shared<VertexBuffer>(rhi, vertexType, AllVertBuffer, vertexSizeInBytes);
		indexBufferGpu = std::make_shared<IndexBuffer>(rhi, AllIndexBuffer, indexSizeInBytes);

		delete[] AllVertBuffer;
		delete[] AllIndexBuffer;
		AllVertBuffer = nullptr;
		AllIndexBuffer = nullptr;
	}

	photon::Mesh* RenderMeshCollection::GetMesh(UINT64 guid)
	{
		if(m_Meshes.find(guid) != m_Meshes.end())
		{
			return m_Meshes[guid].get();
		}
		return nullptr;
	}

}