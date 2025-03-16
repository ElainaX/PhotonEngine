#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <windows.h>
#include <string>

namespace photon 
{
	enum class ResourceHeapProperties : uint32_t
	{
		Static = D3D12_HEAP_TYPE_DEFAULT,
		Upload = D3D12_HEAP_TYPE_UPLOAD
	};

	class Resource 
	{
	public:
		Resource() = default;
		virtual ~Resource() = default;
	public:

		bool operator==(const Resource& rhs) const
		{
			return guid == rhs.guid;
		}

		size_t GetHashValue() const
		{
			size_t h1 = std::hash<std::string>{}(name);
			size_t h2 = std::hash<UINT64>{}(guid);
			return h1 ^ (h2 << 1);
		}

	public:
		UINT64 guid = 0;
		std::string name = "Basic Resource";
		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;


		bool bStatic = false;
		ResourceHeapProperties heapProp;
		D3D12_RESOURCE_DESC dxDesc;
		Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource;
		Microsoft::WRL::ComPtr<ID3DBlob> cpuResource;
	};
	//class Texture3D;
	 
	//class Material;

	//class Mesh;
}

template<>
struct std::hash<photon::Resource>
{
	size_t operator()(const photon::Resource& rhs) const noexcept { return rhs.GetHashValue(); }
};