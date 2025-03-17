#pragma once
#include "../DX12RHI/DX12Resource/VertexBuffer.h"
#include "../DX12RHI/DX12Resource/IndexBuffer.h"
#include "Function/Render/RHI.h"
#include "Mesh.h"

#include <memory>
#include <string>
#include <map>



namespace photon 
{
	class RenderMeshCollection
	{
	public:
		void PushMesh(std::shared_ptr<Mesh> mesh);

		void EndPush(RHI* rhi);

		Mesh* GetMesh(UINT64 guid);


		D3D12_VERTEX_BUFFER_VIEW& VertexBufferView()const
		{
			return vertexBufferGpu->view;
		}

		D3D12_INDEX_BUFFER_VIEW& IndexBufferView()const
		{
			return indexBufferGpu->view;
		}


		std::string name;
		std::shared_ptr<VertexBuffer> vertexBufferGpu;
		std::shared_ptr<IndexBuffer> indexBufferGpu;
		VertexType vertexType;

		UINT vertexSizeInBytes = 0;
		UINT indexSizeInBytes = 0;

	private:

		std::map<UINT64, std::shared_ptr<Mesh>> m_Meshes;

	};
}