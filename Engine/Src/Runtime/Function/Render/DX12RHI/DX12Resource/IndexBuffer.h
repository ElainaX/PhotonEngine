#pragma once
#include "Resource/Texture/Buffer.h"
#include "VertexType.h"

#include <memory>

namespace photon 
{
	class DX12RHI;

	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		IndexBuffer(DX12RHI* rhi,
			const void* indexData, UINT64 sizeInBytes);
		
		void CreateBuffer(DX12RHI* rhi,
			const void* indexData, UINT64 sizeInBytes);

		std::shared_ptr<Buffer> uploadBuffer;
		std::shared_ptr<Buffer> indexBuffer;

		D3D12_INDEX_BUFFER_VIEW view;
	};
}