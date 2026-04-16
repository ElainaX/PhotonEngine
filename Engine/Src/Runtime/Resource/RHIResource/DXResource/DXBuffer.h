#pragma once
#include "DXResource.h"

namespace photon 
{



	class DXBuffer : public DXResource
	{
	public:
		DXBuffer() = default;
		bool Initialize(const DXBufferDesc& desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuRes, const std::string& _name = "DXBuffer");

		static D3D12_RESOURCE_DESC ToDxDesc(const DXBufferDesc& desc);
		static D3D12_RESOURCE_FLAGS GetResourceFlags(const DXBufferDesc& desc);
		static D3D12_RESOURCE_STATES GetInitialResourceState(const DXBufferDesc& desc);
		static DXResourceDimension GetResourceDim();
		//static DXBufferDesc ToPhotonDesc(D3D12_RESOURCE_DESC dxDesc, HeapProp heapProp);

		bool allowUav = false;
	};

}