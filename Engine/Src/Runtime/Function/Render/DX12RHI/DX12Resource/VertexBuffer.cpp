#include "VertexBuffer.h"

#include "Function/Render/DX12RHI/d3dx12.h"
#include "Function/Render/DX12RHI/DX12RHI.h"

namespace photon
{

	VertexBuffer::VertexBuffer(DX12RHI* rhi, VertexType vertType, const void* vertexData, UINT64 sizeInBytes)
	{
		CreateBuffer(rhi, vertType, vertexData, sizeInBytes);
	}

	void VertexBuffer::CreateBuffer(DX12RHI* rhi,
		VertexType vertType, const void* vertexData, UINT64 sizeInBytes)
	{
		vertexType = vertType;

		BufferDesc defaultBufferDesc;
		defaultBufferDesc.bufferSizeInBytes = sizeInBytes;
		defaultBufferDesc.heapProp = ResourceHeapProperties::Static;
		vertexBuffer = rhi->CreateBuffer(defaultBufferDesc);

		BufferDesc uploadBufferDesc;
		uploadBufferDesc.bufferSizeInBytes = sizeInBytes;
		uploadBufferDesc.heapProp = ResourceHeapProperties::Upload;
		uploadBuffer = rhi->CreateBuffer(uploadBufferDesc, vertexData, sizeInBytes);

		rhi->CopyDataGpuToGpu(vertexBuffer.get(), uploadBuffer.get());
	
		view.BufferLocation = vertexBuffer->gpuResource->GetGPUVirtualAddress();
		view.SizeInBytes = sizeInBytes;
		view.StrideInBytes = VertexTypeToSizeInBytes(vertexType);

	}

}