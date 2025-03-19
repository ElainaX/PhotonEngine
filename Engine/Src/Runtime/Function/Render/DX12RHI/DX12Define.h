#pragma once 
#include <cstdint>
namespace photon 
{
	constexpr int g_FrameResourceCount = 3;
	constexpr int g_SwapChainCount = 3;


	constexpr int g_CbvSrvUavHeapSize = 5;
	constexpr int g_SamplerHeapSize = 32;
	constexpr int g_RenderTargetHeapSize = 64;
	constexpr int g_DepthStencilHeapSize = 64;
}