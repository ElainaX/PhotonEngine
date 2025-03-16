#include "IndexBuffer.h"
#include "Function/Render/DX12RHI/DX12RHI.h"

namespace photon 
{

	IndexBuffer::IndexBuffer(DX12RHI* rhi, const void* indexData, UINT64 sizeInBytes)
	{
		CreateBuffer(rhi, indexData, sizeInBytes);
	}

	void IndexBuffer::CreateBuffer(DX12RHI* rhi, const void* indexData, UINT64 sizeInBytes)
	{
		BufferDesc defaultBufferDesc;
		defaultBufferDesc.bufferSizeInBytes = sizeInBytes;
		defaultBufferDesc.heapProp = ResourceHeapProperties::Static;
		indexBuffer = rhi->CreateBuffer(defaultBufferDesc);

		BufferDesc uploadBufferDesc;
		uploadBufferDesc.bufferSizeInBytes = sizeInBytes;
		uploadBufferDesc.heapProp = ResourceHeapProperties::Upload;
		uploadBuffer = rhi->CreateBuffer(uploadBufferDesc, indexData, sizeInBytes);

		rhi->CopyDataGpuToGpu(indexBuffer.get(), uploadBuffer.get());

		view.BufferLocation = indexBuffer->gpuResource->GetGPUVirtualAddress();
		view.Format = DXGI_FORMAT_R32_UINT;
		view.SizeInBytes = sizeInBytes;
	}

}