#include "IndexBuffer.h"
#include "Function/Render/DX12RHI/DX12RHI.h"
#include "Function/Util/RenderUtil.h"

namespace photon 
{

	IndexBuffer::IndexBuffer(std::shared_ptr<Buffer> _indexBuffer)
	{
		CreateBuffer(_indexBuffer);
	}

	void IndexBuffer::CreateBuffer(std::shared_ptr<Buffer> _indexBuffer)
	{
		indexBuffer = _indexBuffer;

		view.BufferLocation = indexBuffer->gpuResource->GetGPUVirtualAddress();
		view.Format = DXGI_FORMAT_R32_UINT;
		view.SizeInBytes = indexBuffer->gpuResource->GetDesc().Width;
	}

}