#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <windows.h>
#include <string>

#include "GuidAllocator.h"
#include "Macro.h"
#include "Core/Math/Vector4.h"
#include "Function/Render/DX12RHI/DX12Define.h"
#include "magic_enum/magic_enum.hpp"

namespace photon 
{


	struct DXBufferDesc
	{
		UINT64 bufferSizeInBytes = 0;

		//Microsoft::WRL::ComPtr<ID3DBlob> cpuResource = nullptr;
		HeapProp heapProp = HeapProp::Default;
		bool allowUav = false;
	};

	struct DXTextureDesc
	{
		uint32_t width = 1;
		uint32_t height = 1;
		uint16_t depthOrArraySize = 1;
		uint16_t mipLevels = 1;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		uint32_t sampleCount = 1;
		uint32_t sampleQuality = 0;
		
		D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE;
		D3D12_RESOURCE_DIMENSION resDim = D3D12_RESOURCE_DIMENSION_UNKNOWN;

		// 不需要layout
		HeapProp heapProp = HeapProp::Default;
		bool hasClearValue = false;
		D3D12_CLEAR_VALUE clearValue = {};
	};

	struct DXTexture2DDesc
	{
		unsigned int width = 1;
		unsigned int height = 1;
		unsigned int maxMipLevels = 1; // 从1开始
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE;

		HeapProp heapProp = HeapProp::Default;
		bool hasClearValue = false;
		union
		{
			Vector4 clearValue = { 1.0f, 1.0f, 1.0f, 1.0f };
			Vector2 depthStencil;
		};
		
	};

	struct DXTexture2DArrayDesc
	{
		unsigned int width = 1;
		unsigned int height = 1;
		unsigned int arraySize = 1;
		unsigned int maxMipLevels = 1;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE;

		HeapProp heapProp = HeapProp::Default;
		bool hasClearValue = false;
		union
		{
			Vector4 clearValue = { 1.0f, 1.0f, 1.0f, 1.0f };
			Vector2 depthStencil;
		};
	};

	struct DXTexture3DDesc
	{
		unsigned int width = 1;
		unsigned int height = 1;
		unsigned int depth = 1;
		unsigned int maxMipLevels = 1;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE;

		HeapProp heapProp = HeapProp::Default;
	};

	class DXResource
	{
	public:
		DXResource() = default;
		bool Initialize(DXResourceDimension _dim, const std::string& _name, HeapProp _heapProp, D3D12_RESOURCE_STATES _state,
		                D3D12_RESOURCE_DESC _desc, const D3D12_CLEAR_VALUE* _clearValue, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource);
		virtual ~DXResource() = default;
		
		
		ID3D12Resource* GetNative() const { return gpuResource.Get(); }
		std::string GetClassNameID() const { return "DX" + std::string(magic_enum::enum_name(dimension)); }
		std::string GetStateString() const { return std::string(magic_enum::enum_name(state)); }
		std::string GetHeapPropString() const { return std::string(magic_enum::enum_name(heapProp)); }



	public:
		std::string name = "Basic DXResource";
		HeapProp heapProp = HeapProp::Unknown;
		// 具体的类名可以通过magic_enum来取
		DXResourceDimension dimension = DXResourceDimension::Unknown;
		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
		D3D12_RESOURCE_DESC dxDesc = {};
		D3D12_CLEAR_VALUE clearValue = {};
		Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource = nullptr;
		bool pendingDestory = false;


		//bool operator==(const DXResource& rhs) const
		//{
		//	return guid == rhs.guid;
		//}

		//size_t GetHashValue() const
		//{
		//	size_t h1 = std::hash<std::wstring>{}(name);
		//	size_t h2 = std::hash<UINT64>{}(guid);
		//	return h1 ^ (h2 << 1);
		//}




	//	static UINT64 s_Guid;

	//public:
	//	UINT64 guid = 0;
	//	
	//	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

	//	bool bStatic = false;
	//	HeapProp heapProp = HeapProp::Unknown;
	//	D3D12_RESOURCE_DESC dxDesc = {};
	//	Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource;
	//	Microsoft::WRL::ComPtr<ID3DBlob> cpuResource;
	};
	//class Texture3D;
	 
	//class Material;

	//class Mesh;
}

//template<>
//struct std::hash<photon::DXResource>
//{
//	size_t operator()(const photon::DXResource& rhs) const noexcept { return rhs.GetHashValue(); }
//};