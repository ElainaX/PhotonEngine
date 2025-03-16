#pragma once 
#include "Resource/Texture/Buffer.h"
#include "VertexType.h"

#include <memory>

namespace photon 
{
	class DX12RHI;

	class VertexBuffer 
	{
	public:
		VertexBuffer() = default;
		VertexBuffer(DX12RHI* rhi,
			VertexType vertType, const void* vertexData, UINT64 sizeInBytes);

		void CreateBuffer(DX12RHI* rhi, 
			VertexType vertType, const void* vertexData, UINT64 sizeInBytes);



		std::shared_ptr<Buffer> uploadBuffer;
		std::shared_ptr<Buffer> vertexBuffer;
		VertexType vertexType;
		D3D12_VERTEX_BUFFER_VIEW view;
	};
}