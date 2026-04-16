#pragma once 
#include <cstdint>
#include <d3d12.h>

namespace photon 
{
	//constexpr int g_FrameContextCount = 3;
	constexpr int g_SwapChainCount = 3;


	constexpr int g_CbvSrvUavHeapSize = 20000;
	constexpr int g_SamplerHeapSize = 32;
	constexpr int g_RenderTargetHeapSize = 64;
	constexpr int g_DepthStencilHeapSize = 128;

	enum class ViewType : uint8_t
	{
		Unknown = 0,
		SRV,
		UAV,
		RTV,
		DSV,
		CBV,
		Sampler
	};

	enum ViewKeyFlags : uint32_t
	{
		ViewKeyFlag_None = 0,

		// DSV
		ViewKeyFlag_ReadOnlyDepth = 1u << 0,
		ViewKeyFlag_ReadOnlyStencil = 1u << 1,

		// Buffer SRV/UAV
		ViewKeyFlag_RawBuffer = 1u << 2,
	};

	enum class ViewDimension : uint8_t
	{
		Unknown = 0,

		Buffer,

		Texture1D,
		Texture1DArray,

		Texture2D,
		Texture2DArray,

		Texture2DMS,
		Texture2DMSArray,

		Texture3D,

		TextureCube,
		TextureCubeArray
	};

	enum class DXResourceDimension : uint8_t
	{
		Unknown = 0,

		Buffer,
		Texture1D,
		Texture2D,
		Texture2DArray,
		Texture3D,
	};

	enum class DescriptorHeapKind : uint8_t
	{
		Unknown = 0,
		Cbv,
		Srv,
		Uav,
		Sampler,
		Rtv,
		Dsv
	};

	enum class HeapProp : uint32_t
	{
		Default = D3D12_HEAP_TYPE_DEFAULT,
		Upload = D3D12_HEAP_TYPE_UPLOAD,
		Readback = D3D12_HEAP_TYPE_READBACK,
		Unknown
	};

}
