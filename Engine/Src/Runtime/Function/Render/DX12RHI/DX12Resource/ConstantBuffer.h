#pragma once

#include "Resource/Texture/Buffer.h"
#include "Function/Render/DX12RHI/DescriptorHeap/Descriptor.h"

namespace photon 
{
	class RHI;
	class ConstantBuffer
	{
	public:
		ConstantBuffer(std::shared_ptr<Buffer> _constantBuffer, std::shared_ptr<Buffer> _uploadBuffer,
			unsigned int _elementCount, unsigned int _singleElementSizeInBytes, unsigned int _constantBufferStrideInBytes, UINT64 _bufferSize);


		void UpdateElements(RHI* rhi, unsigned int startIndex, const void* data, UINT64 sizeInBytes);
		D3D12_GPU_VIRTUAL_ADDRESS GetConstantGPUAddressByIndex(UINT64 Idx);



		std::shared_ptr<Buffer> constantBuffer;
		std::shared_ptr<Buffer> uploadBuffer;

		unsigned int elementCount = 0;
		unsigned int singleElementSizeInBytes = 0;
		unsigned int constantBufferStrideInBytes = 0;
		UINT64 bufferSizeInBytes = 0;
	};


}