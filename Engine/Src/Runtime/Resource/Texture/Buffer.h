﻿#pragma once
#include "Resource/ResourceType.h"

namespace photon 
{
	struct BufferDesc 
	{
		UINT64 bufferSizeInBytes;

		Microsoft::WRL::ComPtr<ID3DBlob> cpuResource = nullptr;
		ResourceHeapProperties heapProp = ResourceHeapProperties::Default;
	};


	class Buffer : public Resource
	{
	public:
		Buffer() = default;
		Buffer(BufferDesc bufferDesc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource);
		Buffer(D3D12_RESOURCE_DESC desc, ResourceHeapProperties prop, 
			Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource, Microsoft::WRL::ComPtr<ID3DBlob> _cpuResource);
		
		static D3D12_RESOURCE_DESC ToDxDesc(BufferDesc desc);
		static BufferDesc ToPhotonDesc(D3D12_RESOURCE_DESC dxDesc, ResourceHeapProperties heapProp);
	};

}