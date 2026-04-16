#pragma once
#include "Resource/DXResourceHeader.h"
#include "VertexType.h"

#include <memory>

#include "Function/Render/DX12RHI/GpuResource.h"
#include "Function/Render/DX12RHI/GpuResourceManager.h"
#include "Function/Render/DX12RHI/ViewDesc.h"
#include "Function/Util/RenderUtil.h"

namespace photon 
{

	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		
		IndexBufferRange range;
		GpuResourceHandle buffer;
		std::string debugName = "IndexBuffer";

		D3D12_INDEX_BUFFER_VIEW GetView(GpuResourceManager* gpuResMgr) const
		{
			D3D12_INDEX_BUFFER_VIEW view = {};
			if (!gpuResMgr || !buffer.IsValid())
				return view;

			DXResource* res = gpuResMgr->GetResource(buffer);
			if (!res || !res->gpuResource)
				return view;

			const uint32_t indexStride = (range.format == IndexFormat::UInt16) ? 2u : 4u;

			view.BufferLocation = res->gpuResource->GetGPUVirtualAddress() +
				static_cast<uint64_t>(range.firstIndex) * indexStride;
			view.SizeInBytes = range.indexCount * indexStride;
			view.Format = RenderUtil::ToDxgiFormat(range.format);
			return view;
		}
	};
}
