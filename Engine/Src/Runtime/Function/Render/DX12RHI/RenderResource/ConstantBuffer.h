#pragma once
#include "ResourceCommon.h"
#include "Function/Render/DX12RHI/GpuResource.h"
#include "Function/Render/DX12RHI/ViewDesc.h"

namespace photon 
{
	class RHI;
	class ConstantBuffer
	{
	public:
		ResourceUsage usage = ResourceUsage::Static;
		ConstantBufferRange range;
		GpuResourceHandle buffer;
		std::string debugName = "ConstantBuffer";
	};


}