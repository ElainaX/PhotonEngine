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
		VertexBuffer(std::shared_ptr<Buffer> _vertexBuffer, VertexType type);
		void CreateBuffer(std::shared_ptr<Buffer> _vertexBuffer, VertexType type);


		std::shared_ptr<Buffer> vertexBuffer;
		VertexType vertexType;
		D3D12_VERTEX_BUFFER_VIEW view;
	};
}