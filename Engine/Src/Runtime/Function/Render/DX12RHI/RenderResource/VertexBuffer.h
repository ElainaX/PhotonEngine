#pragma once 
#include "Function/Render/DX12RHI/GpuResource.h"
#include "Function/Render/DX12RHI/GpuResourceManager.h"
#include "Function/Render/DX12RHI/ViewDesc.h"

namespace photon 
{
	class RHI;

	class VertexBuffer 
	{
	public:
		VertexBuffer() = default;

		VertexBufferRange range;
		GpuResourceHandle buffer;
		std::string debugName = "VertexBuffer";

		D3D12_VERTEX_BUFFER_VIEW GetView(GpuResourceManager* gpuResMgr) const
		{
			D3D12_VERTEX_BUFFER_VIEW view = {};
			if (!gpuResMgr || !buffer.IsValid() || range.vertexCount == 0)
				return view;

			DXResource* res = gpuResMgr->GetResource(buffer);
			if (!res || !res->gpuResource)
				return view;

			view.BufferLocation = res->gpuResource->GetGPUVirtualAddress() +
				static_cast<uint64_t>(range.firstVertex) * range.strideInBytes;

			view.SizeInBytes = range.vertexCount * range.strideInBytes;
			view.StrideInBytes = range.strideInBytes;
			return view;
		}

	};

	using SubVertexBuffer = VertexBuffer;
	using SubIndexBuffer = IndexBuffer;
}
