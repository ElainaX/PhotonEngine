#include "IndexBuffer.h"
#include "Function/Render/DX12RHI/DX12RHI.h"
#include "Function/Util/RenderUtil.h"

namespace photon 
{

	IndexBuffer::IndexBuffer(RHI* rhi, const void* indexData, UINT64 sizeInBytes)
	{
		CreateBuffer(rhi, indexData, sizeInBytes);
	}

	void IndexBuffer::CreateBuffer(RHI* rhi, const void* indexData, UINT64 sizeInBytes)
	{
		BufferDesc defaultBufferDesc;
		defaultBufferDesc.bufferSizeInBytes = sizeInBytes;
		defaultBufferDesc.heapProp = ResourceHeapProperties::Default;
		indexBuffer = rhi->CreateBuffer(defaultBufferDesc);

		BufferDesc uploadBufferDesc;
		uploadBufferDesc.bufferSizeInBytes = sizeInBytes;
		uploadBufferDesc.heapProp = ResourceHeapProperties::Upload;
		uploadBufferDesc.cpuResource = RenderUtil::CreateD3DBlob(indexData, sizeInBytes);
		uploadBuffer = rhi->CreateBuffer(uploadBufferDesc);

		rhi->CopyDataGpuToGpu(indexBuffer.get(), uploadBuffer.get());

		view.BufferLocation = indexBuffer->gpuResource->GetGPUVirtualAddress();
		view.Format = DXGI_FORMAT_R32_UINT;
		view.SizeInBytes = sizeInBytes;
	}

}