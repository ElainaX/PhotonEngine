#include "DXResource.h"

namespace photon 
{
	bool DXResource::Initialize(DXResourceDimension _dim, const std::string& _name, HeapProp _heapProp,
	                            D3D12_RESOURCE_STATES _state, D3D12_RESOURCE_DESC _desc, const D3D12_CLEAR_VALUE* _clearValue,
	                            Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource)
	{
		dimension = _dim;
		name = _name;
		heapProp = _heapProp;
		state = _state;
		dxDesc = _desc;
		if (_clearValue)
		{
			clearValue = *_clearValue;
		}
		gpuResource = _gpuResource;

		return gpuResource != nullptr;
	}
}
