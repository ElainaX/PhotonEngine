#pragma once
#include "Function/Render/DX12RHI/RenderResource/VertexType.h"

namespace photon
{
	struct MeshCpuData
	{
		VertexLayoutDesc vertexLayout;

		std::vector<std::byte> vertexData;
		std::vector<std::byte> indexData;

		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		IndexFormat indexFormat = IndexFormat::UInt32;

		bool Empty() const
		{
			return vertexCount == 0 || indexCount == 0;
		}

		void Clear()
		{
			vertexData.clear();
			indexData.clear();
			vertexCount = 0;
			indexCount = 0;
		}

		uint32_t GetVertexBufferSize() const
		{
			return static_cast<uint32_t>(vertexData.size());
		}

		uint32_t GetIndexBufferSize() const
		{
			return static_cast<uint32_t>(indexData.size());
		}
	};


}
