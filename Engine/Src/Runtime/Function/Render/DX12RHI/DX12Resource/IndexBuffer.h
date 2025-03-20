#pragma once
#include "Resource/Texture/Buffer.h"
#include "VertexType.h"

#include <memory>

namespace photon 
{
	class RHI;

	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		IndexBuffer(std::shared_ptr<Buffer> _indexBuffer);
		
		void CreateBuffer(std::shared_ptr<Buffer> _indexBuffer);

		std::shared_ptr<Buffer> indexBuffer;

		D3D12_INDEX_BUFFER_VIEW view;
	};
}