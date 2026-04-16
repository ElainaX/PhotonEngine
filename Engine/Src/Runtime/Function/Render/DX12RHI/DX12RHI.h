#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>
#include <array>



#include "d3dx12.h"
#include "Function/Render/RHI.h"
#include "DX12Define.h"
#include "FrameSyncSystem.h"
#include "Function/Render/WindowSystem.h"
#include "Resource/DXResourceHeader.h"
#include "ViewDesc.h"


namespace photon 
{
	class ResourceManager;


	//struct FrameContext
	//{
	//	std::unordered_map<FrameResourceType, std::shared_ptr<FrameResource>> frameResources;
	//	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator = nullptr;
	//	UINT64 fenceValue = 0;
	//};

	struct SwapChainContent
	{
		std::shared_ptr<DXTexture2D> backBuffer;
	};


	// DX12RHI以后只负责
	// 1. 资源创建底层函数
	// 2. queue/fence/present/execute持有
	// 3. 为CommandContextManager提供底层支持
	class DX12RHI final : public RHI 
	{
	public:
		~DX12RHI() override;

		bool Initialize(const RHIInitInfo& initializeInfo) override;
		void Shutdown() override;

		void Present() override;
		void ResizeSwapChain(uint32_t width, uint32_t height) override;


		uint64_t GetCompletedFenceValue(QueueType queueType) const override;
		uint64_t SignalQueue(QueueType queueType) override;
		void WaitForFenceValue(QueueType queueType, uint64_t fenceValue) override;

	public:
		ID3D12Device* GetDevice() const { return m_device.Get(); }
		IDXGISwapChain3* GetSwapChain() const { return m_swapChain.Get(); }
		ID3D12CommandQueue* GetGraphicsQueue() const { return m_graphicsQueue.Get(); }
		ID3D12CommandQueue* GetComputeQueue() const { return m_computeQueue.Get(); }
		ID3D12CommandQueue* GetCopyQueue() const { return m_copyQueue.Get(); }

		UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

		// 四大DX GPU资源
		HRESULT CreateDXBuffer(const DXBufferDesc& desc, DXBuffer& outResource);
		HRESULT CreateDXTexture2D(const DXTexture2DDesc& desc, DXTexture2D& outResource);
		HRESULT CreateDXTexture2DArray(const DXTexture2DArrayDesc& desc, DXTexture2DArray& outResource);
		HRESULT CreateDXTexture3D(const DXTexture3DDesc& desc, DXTexture3D& outResource);

		Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc);

		uint32_t GetCurrentBackBufferIndex() const;
		DXTexture2D* GetCurrBackbuffer();
		DXTexture2D* GetBackbuffer(uint32_t idx);

		uint64_t GetResourceSizeInBytes(ID3D12Resource* pResource);

	private:
		ID3D12CommandQueue* GetQueueByType(QueueType type) const;
		ID3D12Fence* GetFenceByType(QueueType type) const;


		void FlushCommandQueue();

		void CreateSwapChain();
		void ReCreateSwapChain();
		void CreateFactory();
		void CreateDevice();
		void CreateFence();
		void CreateCommandObjects();
		void CreateSwapChainRenderTarget();
		void CreateDebugManager();
		void OnWindowResize(const WindowResizeEvent& e);


	private:
		Microsoft::WRL::ComPtr<IDXGIFactory7> m_factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_adapter;
		Microsoft::WRL::ComPtr<ID3D12Device3> m_device;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
		std::array<SwapChainContent, g_SwapChainCount> m_swapChainContents;
		HANDLE m_swapChainWaitableObject = {};

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_graphicsQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_computeQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_copyQueue;

		Microsoft::WRL::ComPtr<ID3D12Fence1> m_graphicsFence;
		Microsoft::WRL::ComPtr<ID3D12Fence1> m_computeFence;
		Microsoft::WRL::ComPtr<ID3D12Fence1> m_copyFence;



		uint64_t m_graphicsFenceValue = 1;
		uint64_t m_computeFenceValue = 1;
		uint64_t m_copyFenceValue = 1;


		WindowSystem* m_windowSystem = nullptr;
		GpuResourceManager* m_gpuResManager = nullptr;
		FrameSyncSystem* m_frameSyncSystem = nullptr;

	};
}