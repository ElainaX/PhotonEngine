#pragma once
#include "Resource/ResourceType.h"
#include <cstdint>

namespace photon 
{
	struct ViewBase 
	{
		ViewBase()
		{
			viewGuid = ++s_ViewGuid;
		}
		uint64_t viewGuid;
		uint32_t offsetCountInHeap = 0; // not the byte
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleInHeap;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleInHeap;

		static uint64_t s_ViewGuid;
	};

	struct ConstantBufferView : public ViewBase
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc;
	};

	struct ShaderResourceView : public ViewBase
	{
		Resource* resource;
		D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc;
	};

	struct UnorderedAccessView : public ViewBase
	{
		Resource* resource;
		Resource* counterResource;
		D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc;
	};

	struct SamplerView : public ViewBase
	{
		D3D12_SAMPLER_DESC viewDesc;
	};



	struct DepthStencilView : public ViewBase
	{
		Resource* resource;
		D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc;
	};

	struct RenderTargetView : public ViewBase
	{
		Resource* resource;
		D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
	};


	

}