#pragma once
#include "Function/Render/DX12RHI/GpuResource.h"
#include "Function/Render/DX12RHI/ViewDesc.h"
#include "Function/Render/DX12RHI/DescriptorHeap/Descriptor.h"

namespace photon 
{
	enum class FrameResourceType
	{
		Unknown = 0,
		DefaultBuffer,
		UploadBuffer
	};

	// buffer以及对应的指针
	struct FrameResourceRange
	{
		FrameResourceType frameResType = FrameResourceType::Unknown;
		GpuResourceHandle buffer = {};
		ConstantBufferRange range;
	};


	struct FrameDescriptorRange
	{
		FrameDescriptorHandle startHandle;
		uint32_t count = 1;
		uint32_t descriptorPerSize = 0;
	};
}
