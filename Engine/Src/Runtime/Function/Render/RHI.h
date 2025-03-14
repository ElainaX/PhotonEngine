#pragma once
#include "RhiStruct.h"
#include "Resource/ResourceType.h"
#include "Resource/Texture/Texture2D.h"

#include <memory>
#include <cstdint>
#include <utility>
#include <d3d12.h>

namespace photon 
{
	class WindowSystem;

	// 特定RHI的Init如果需要其他的信息就在这个struct中添加
	struct RHIInitInfo
	{
		std::shared_ptr<WindowSystem> window_System;
	};

	// RHI Interface
	class RHI
	{
	public:
		virtual ~RHI(){}
		
		// 关于创建的相关函数
		virtual void Initialize(RHIInitInfo initializeInfo) = 0;
		virtual void CreateSwapChain() = 0;
		virtual void ReCreateSwapChain() = 0;
		virtual void CreateFactory() = 0;
		virtual void CreateDevice() = 0;
		virtual void CreateFence() = 0;

		virtual void CreateCommandObjects() = 0;
		virtual void CreateSwapChainRenderTarget() = 0;
		virtual void CreateDescriptorHeaps() = 0;

		virtual void CreateAssetAllocator() = 0;

		virtual void CreateDebugManager() = 0;

		virtual void Clear() = 0;

		// 常用功能
		virtual void FlushCommandQueue() = 0;
		virtual void WaitForFenceValue(uint64_t fenceValue) = 0;

		virtual void CopyTextureToSwapChain(std::shared_ptr<Texture2D> tex) = 0;
		virtual void PrepareForPresent() = 0;
		virtual void Present() = 0;

		virtual void BeginSingleRenderPass() = 0;
		virtual void EndSingleRenderPass() = 0;


		virtual unsigned int GetCurrBackBufferIndex() = 0;
		// virtual Resource* GetCurrBackBufferResource() = 0;

		
		// 资源相关函数
		virtual std::shared_ptr<Texture2D> CreateTexture2D(Texture2DDesc desc) = 0;

		virtual void ResourceStateTransform(Resource* resource, D3D12_RESOURCE_STATES stateAfter) = 0;

	private:
		// No Private
	};
}