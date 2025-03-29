#include "RenderMeshCollection.h"
#include "Function/Render/DX12RHI/DX12Resource/VertexType.h"
#include "Macro.h"
namespace photon 
{

	void RenderMeshCollection::PushMesh(std::shared_ptr<Mesh> mesh)
	{
		if(mesh->bIsLoad)
		{
			LOG_ERROR("mesh {} can't load in two meshCollection!", WString2String(mesh->name));
			return;
		}
		m_Meshes.insert({ mesh->guid, mesh });
		vertexType = mesh->type;
		vertexSizeInBytes += mesh->vertexRawData->GetBufferSize();
		indexSizeInBytes += mesh->indexRawData->GetBufferSize();
		mesh->bIsLoad = true;
		bShouldRecreateBuffer = true;
	}

	void RenderMeshCollection::EndPush(RHI* rhi)
	{
		if (!bShouldRecreateBuffer)
			return;
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

			mesh->baseVertexLocation = vertOffset / vertexStride;
			mesh->startIndexLocation = indexOffset / indexStride;
			mesh->indexCount = meshIndexSize / indexStride;

			vertOffset += meshVertSize;
			indexOffset += meshIndexSize;
		}

		vertexBufferGpu = rhi->CreateVertexBuffer(vertexType, AllVertBuffer, vertexSizeInBytes);
		indexBufferGpu = rhi->CreateIndexBuffer(AllIndexBuffer, indexSizeInBytes);

		delete[] AllVertBuffer;
		delete[] AllIndexBuffer;
		AllVertBuffer = nullptr;
		AllIndexBuffer = nullptr;

		bShouldRecreateBuffer = false;
	}

	photon::Mesh* RenderMeshCollection::GetMesh(UINT64 guid)
	{
		if(m_Meshes.find(guid) != m_Meshes.end())
		{
			return m_Meshes[guid].get();
		}
		return nullptr;
	}

	bool RenderMeshCollection::IsMeshLoaded(UINT64 guid)
	{
		return m_Meshes.find(guid) != m_Meshes.end();
	}

}