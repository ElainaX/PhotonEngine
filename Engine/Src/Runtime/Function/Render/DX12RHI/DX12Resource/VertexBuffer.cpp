#include "VertexBuffer.h"

#include "Function/Render/DX12RHI/d3dx12.h"
#include "Function/Render/DX12RHI/DX12RHI.h"
#include "Function/Util/RenderUtil.h"

#include <d3d12.h>

namespace photon
{

	VertexBuffer::VertexBuffer(std::shared_ptr<Buffer> _vertexBuffer, VertexType type)
	{
		CreateBuffer(_vertexBuffer, type);
	}

	void VertexBuffer::CreateBuffer(std::shared_ptr<Buffer> _vertexBuffer, VertexType type)
	{
		vertexBuffer = _vertexBuffer;
		vertexType = type;

		view.BufferLocation = vertexBuffer->gpuResource->GetGPUVirtualAddress();
		view.SizeInBytes = vertexBuffer->gpuResource->GetDesc().Width;
		view.StrideInBytes = VertexTypeToSizeInBytes(vertexType);
	}

}