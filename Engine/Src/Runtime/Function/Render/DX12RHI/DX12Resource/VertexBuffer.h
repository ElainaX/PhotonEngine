#pragma once 
#include "Resource/Texture/Buffer.h"
#include "VertexType.h"

#include <memory>

namespace photon 
{
	class RHI;

	class VertexBuffer 
	{
	public:
		VertexBuffer() = default;
		VertexBuffer(RHI* rhi,
			VertexType vertType, const void* vertexData, UINT64 sizeInBytes);

		void CreateBuffer(RHI* rhi,
			VertexType vertType, const void* vertexData, UINT64 sizeInBytes);



		std::shared_ptr<Buffer> uploadBuffer;
		std::shared_ptr<Buffer> vertexBuffer;
		VertexType vertexType;
		D3D12_VERTEX_BUFFER_VIEW view;
	};
}