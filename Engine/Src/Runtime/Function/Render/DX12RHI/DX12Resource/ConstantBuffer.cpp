#include "ConstantBuffer.h"
#include "Function/Render/DX12RHI/DX12RHI.h"
namespace photon
{

	ConstantBuffer::ConstantBuffer(std::shared_ptr<Buffer> _constantBuffer, std::shared_ptr<Buffer> _uploadBuffer, 
		unsigned int _elementCount, unsigned int _singleElementSizeInBytes, unsigned int _constantBufferStrideInBytes, UINT64 _bufferSize)
		: constantBuffer(_constantBuffer), uploadBuffer(_uploadBuffer), 
		elementCount(_elementCount), singleElementSizeInBytes(_singleElementSizeInBytes), 
		constantBufferStrideInBytes(_constantBufferStrideInBytes), bufferSizeInBytes(_bufferSize)
	{

	}

	void ConstantBuffer::UpdateElements(RHI* rhi, unsigned int startIndex, const void* data, UINT64 sizeInBytes)
	{
		unsigned int count = sizeInBytes / singleElementSizeInBytes;
		UINT64 totalSizeInBytesGpu = count * constantBufferStrideInBytes;
		UINT64 startPosInGpu = startIndex * constantBufferStrideInBytes;
		for(int i = 0; i < count; ++i)
		{
			UINT64 offset = i * singleElementSizeInBytes;
			UINT64 offsetGpu = i * constantBufferStrideInBytes;
			rhi->CopyDataCpuToGpu(uploadBuffer.get(), startPosInGpu + offsetGpu, (const char*)data + offset, singleElementSizeInBytes);
		}
		rhi->CopyDataGpuToGpu(constantBuffer.get(), uploadBuffer.get(), startPosInGpu, startPosInGpu, totalSizeInBytesGpu);
	}

	D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetConstantGPUAddressByIndex(UINT64 Idx)
	{
		UINT64 startPosInGpu = Idx * constantBufferStrideInBytes;
		return constantBuffer->gpuResource->GetGPUVirtualAddress() + startPosInGpu;
	}

}