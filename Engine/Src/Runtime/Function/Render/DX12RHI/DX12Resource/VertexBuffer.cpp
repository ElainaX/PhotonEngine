#include "VertexBuffer.h"

#include "Function/Render/DX12RHI/d3dx12.h"
#include "Function/Render/DX12RHI/DX12RHI.h"
#include "Function/Util/RenderUtil.h"

#include <d3d12.h>

namespace photon
{

	VertexBuffer::VertexBuffer(RHI* rhi, VertexType vertType, const void* vertexData, UINT64 sizeInBytes)
	{
		CreateBuffer(rhi, vertType, vertexData, sizeInBytes);
	}

	void VertexBuffer::CreateBuffer(RHI* rhi,
		VertexType vertType, const void* vertexData, UINT64 sizeInBytes)
	{
		vertexType = vertType;

		BufferDesc defaultBufferDesc;
		defaultBufferDesc.bufferSizeInBytes = sizeInBytes;
		defaultBufferDesc.cpuResource = nullptr;
		defaultBufferDesc.heapProp = ResourceHeapProperties::Default;
		vertexBuffer = rhi->CreateBuffer(defaultBufferDesc);

		BufferDesc uploadBufferDesc;
		uploadBufferDesc.bufferSizeInBytes = sizeInBytes;
		uploadBufferDesc.cpuResource = RenderUtil::CreateD3DBlob(vertexData, sizeInBytes);
		uploadBufferDesc.heapProp = ResourceHeapProperties::Upload;
		uploadBuffer = rhi->CreateBuffer(uploadBufferDesc);

		rhi->CopyDataGpuToGpu(vertexBuffer.get(), uploadBuffer.get());
	
		view.BufferLocation = vertexBuffer->gpuResource->GetGPUVirtualAddress();
		view.SizeInBytes = sizeInBytes;
		view.StrideInBytes = VertexTypeToSizeInBytes(vertexType);

	}

}