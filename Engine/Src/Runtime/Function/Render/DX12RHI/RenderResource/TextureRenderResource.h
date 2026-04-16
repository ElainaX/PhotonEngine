#pragma once
#include "Function/Render/ResourceHandle.h"
#include "Function/Render/ResourceTypes.h"
#include "Function/Render/DX12RHI/GpuResource.h"
#include "Function/Render/DX12RHI/DescriptorHeap/Descriptor.h"

namespace photon
{
	struct TextureRenderResource
	{
		TextureHandle handle;
		Guid assetGuid;

		GpuResourceHandle texture;

		DescriptorHandle srv;
		DescriptorHandle rtv;
		DescriptorHandle dsv;
		DescriptorHandle uav;

		TextureDimension dimension = TextureDimension::Tex2D;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t depth = 1;
		uint32_t arraySize = 1;
		uint32_t mipCount = 1;
	};

}
