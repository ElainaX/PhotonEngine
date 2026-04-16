#include "DX12RHI.h"
#include "Function/Render/WindowSystem.h"
#include "Macro.h"
#include "d3dx12.h"
#include "Platform/FileSystem/FileSystem.h"
#include "Function/Util/RenderUtil.h"
#include <dxgi1_6.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

namespace photon 
{

	DX12RHI::~DX12RHI()
	{
	}

	bool DX12RHI::Initialize(const RHIInitInfo& initializeInfo)
	{
		m_windowSystem = initializeInfo.windowSystem;
		m_frameSyncSystem = initializeInfo.frameSyncSystem;
		PHOTON_ASSERT(m_windowSystem  && m_frameSyncSystem, "WindowSystem Is NullPtr! DX12RHI Init Error!");

		CreateDebugManager();

		CreateFactory();

		CreateDevice();

		CreateFence();

		CreateCommandObjects();

		CreateSwapChain();

		return true;
	}

	// 注意，这里只需要创建SwapChain的颜色缓冲，因为我们会把ImGui的图直接复制回SwapChain，不需要深度缓冲
	void DX12RHI::CreateSwapChain()
	{
		DXGI_SWAP_CHAIN_DESC1 scDesc;
		{
			Vector2i widthAndHeight = m_windowSystem->GetClientWidthAndHeight();
			ZeroMemory(&scDesc, sizeof(scDesc));
			scDesc.BufferCount = g_SwapChainCount;
			scDesc.Width = widthAndHeight.x;
			scDesc.Height = widthAndHeight.y;
			scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scDesc.SampleDesc.Count = 1;
			scDesc.SampleDesc.Quality = 0;
			scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			scDesc.Scaling = DXGI_SCALING_STRETCH;
			scDesc.Stereo = FALSE;
		}
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		DX_LogIfFailed(m_factory->CreateSwapChainForHwnd(m_graphicsQueue.Get(), m_windowSystem->GetHwnd(), 
			&scDesc, nullptr, nullptr, &swapChain1));
		DX_LogIfFailed(swapChain1->QueryInterface(IID_PPV_ARGS(&m_swapChain)));

		DX_LogIfFailed(m_swapChain->SetMaximumFrameLatency(g_SwapChainCount));
		m_swapChainWaitableObject = m_swapChain->GetFrameLatencyWaitableObject();

		for(int i = 0; i < g_SwapChainCount; ++i)
		{
			m_swapChainContents[i] = SwapChainContent();
		}

		CreateSwapChainRenderTarget();
	}

	void DX12RHI::ReCreateSwapChain()
	{
		LOG_INFO("Recreate SwapChain");
		auto [width, height] = m_windowSystem->GetClientWidthAndHeight();
		ResizeSwapChain(width, height);
	}

	void DX12RHI::CreateFactory()
	{
		DX_LogIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory)));
	}

	void DX12RHI::CreateDevice()
	{
		PHOTON_ASSERT(m_factory.Get() != nullptr, "Factory Not Created!");
		
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter1;
		DX_LogIfFailed(m_factory->EnumAdapters1(0, &adapter1));
		// 高级功能需要Query
		DX_LogIfFailed(adapter1->QueryInterface(IID_PPV_ARGS(&m_adapter)));

		DXGI_ADAPTER_DESC gpuDesc;
		DX_LogIfFailed(m_adapter->GetDesc(&gpuDesc));
		LOG_INFO("Successfully Connect To GPU: {}", WString2String(gpuDesc.Description));

		DX_LogIfFailed(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_device)));
	}

	void DX12RHI::CreateFence()
	{
		PHOTON_ASSERT(m_device.Get() != nullptr, "Device Not Created!");
		Microsoft::WRL::ComPtr<ID3D12Fence> fence0;
		DX_LogIfFailed(m_device->CreateFence(0u, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence0)));
		DX_LogIfFailed(fence0->QueryInterface(IID_PPV_ARGS(&m_graphicsFence)));
		DX_LogIfFailed(fence0->QueryInterface(IID_PPV_ARGS(&m_computeFence)));
		DX_LogIfFailed(fence0->QueryInterface(IID_PPV_ARGS(&m_copyFence)));
	}

	void DX12RHI::CreateCommandObjects()
	{
		PHOTON_ASSERT(m_device.Get() != nullptr, "Device Not Created!");

		D3D12_COMMAND_QUEUE_DESC queueDesc;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = 0;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;
		
		DX_LogIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_graphicsQueue)));

		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		DX_LogIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_computeQueue)));

		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		DX_LogIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_copyQueue)));

	}

	void DX12RHI::CreateSwapChainRenderTarget()
	{
		for (UINT i = 0; i < g_SwapChainCount; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			DX_LogIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

			D3D12_RESOURCE_DESC dxDesc = backBuffer->GetDesc();
			DXTexture2DDesc desc = {};
			desc.width = dxDesc.Width;
			desc.height = dxDesc.Height;
			desc.maxMipLevels = dxDesc.MipLevels;
			desc.format = dxDesc.Format;
			desc.flag = dxDesc.Flags;
			desc.heapProp = HeapProp::Default;
			desc.hasClearValue = false;

			auto name = "SwapChain DXTexture2D" + std::to_string(i);

			m_swapChainContents[i].backBuffer = std::make_shared<DXTexture2D>();
			m_swapChainContents[i].backBuffer->Initialize(desc, backBuffer, name);
			m_swapChainContents[i].backBuffer->state = D3D12_RESOURCE_STATE_PRESENT;
		}
	}

	void DX12RHI::FlushCommandQueue()
	{
		UINT fenceValue = ++m_graphicsFenceValue;
		m_graphicsQueue->Signal(m_graphicsFence.Get(), fenceValue);
		if(m_graphicsFence->GetCompletedValue() < fenceValue)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			m_graphicsFence->SetEventOnCompletion(fenceValue, eventHandle);
			HANDLE waitableObjects[] = { eventHandle, m_swapChainWaitableObject };
			WaitForMultipleObjectsEx(2, waitableObjects, TRUE, INFINITE, TRUE);
			CloseHandle(eventHandle);
		}
	}

	void DX12RHI::WaitForFenceValue(QueueType queueType, uint64_t fenceValue)
	{
		if (fenceValue == 0)
			return;
		auto fence = GetFenceByType(queueType);

		if (m_graphicsFence->GetCompletedValue() < fenceValue)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			DX_LogIfFailed(m_graphicsFence->SetEventOnCompletion(fenceValue, eventHandle));
			WaitForSingleObjectEx(eventHandle, INFINITE, false);
			bool closed = CloseHandle(eventHandle);
		}
	}

	void DX12RHI::CreateDebugManager()
	{
#ifdef DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
		else {
			// 调试层不可用，可能是未安装Graphics Tools
			return;
		}
#endif
	}

	void DX12RHI::Present()
	{
		DX_LogIfFailed(m_swapChain->Present(0, 0));
	}

	void DX12RHI::ResizeSwapChain(uint32_t width, uint32_t height)
	{
		if (!m_swapChain || !m_device || width == 0 || height == 0)
			return;

		LOG_INFO("DX12RHI ResizeSwapChain: {} x {}", width, height);

		// 先确保 GPU 不再使用旧 backbuffer
		FlushCommandQueue();

		// 释放旧的 swapchain buffer 包装
		for (UINT i = 0; i < g_SwapChainCount; ++i)
		{
			if (m_swapChainContents[i].backBuffer)
			{
				m_swapChainContents[i].backBuffer.reset();
			}
		}

		// 某些驱动下 ResizeBuffers 后 waitable object 也应重新获取
		if (m_swapChainWaitableObject)
		{
			CloseHandle(m_swapChainWaitableObject);
			m_swapChainWaitableObject = nullptr;
		}

		DX_LogIfFailed(
			m_swapChain->ResizeBuffers(
				g_SwapChainCount,
				width,
				height,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT));

		DX_LogIfFailed(m_swapChain->SetMaximumFrameLatency(g_SwapChainCount));
		m_swapChainWaitableObject = m_swapChain->GetFrameLatencyWaitableObject();

		CreateSwapChainRenderTarget();
	}

	uint64_t DX12RHI::GetCompletedFenceValue(QueueType queueType) const
	{
		auto fence = GetFenceByType(queueType);
		return fence->GetCompletedValue();
	}

	uint64_t DX12RHI::SignalQueue(QueueType queueType)
	{
		switch (queueType)
		{
		case QueueType::Graphics:
			m_graphicsQueue->Signal(m_graphicsFence.Get(), m_graphicsFenceValue);
			return m_graphicsFenceValue++;
		case QueueType::Compute:
			m_computeQueue->Signal(m_computeFence.Get(), m_computeFenceValue);
			return m_computeFenceValue++;
		case QueueType::Copy:
			m_copyQueue->Signal(m_copyFence.Get(), m_copyFenceValue);
			return m_copyFenceValue++;
		}

		return 0;
	}
	
	uint64_t DX12RHI::GetResourceSizeInBytes(ID3D12Resource* pResource)
	{
		if (!pResource)
			return 0;

		D3D12_RESOURCE_DESC desc = pResource->GetDesc();
		// 如果是buffer，直接查width即可
		if (desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
			return desc.Width;

		// 否则直接调用API查找即可
		D3D12_RESOURCE_ALLOCATION_INFO info = m_device->GetResourceAllocationInfo(0, 1, &desc);
		return info.SizeInBytes;
	}

	UINT DX12RHI::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const
	{
		return m_device->GetDescriptorHandleIncrementSize(type);
	}

	HRESULT DX12RHI::CreateDXBuffer(const DXBufferDesc& desc, DXBuffer& outResource)
	{
		static uint64_t name_id = 1;

		Microsoft::WRL::ComPtr<ID3D12Resource> res;
		D3D12_RESOURCE_DESC dxDesc = DXBuffer::ToDxDesc(desc);
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES((D3D12_HEAP_TYPE)desc.heapProp);

		D3D12_RESOURCE_STATES state = DXBuffer::GetInitialResourceState(desc);
		auto ret = m_device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &dxDesc,
			state, nullptr, IID_PPV_ARGS(&res));
		DX_LogIfFailed(ret);

		outResource.Initialize(desc, res, outResource.GetClassNameID() + "_" + std::to_string(name_id++));

		return ret;
	}

	HRESULT DX12RHI::CreateDXTexture2D(const DXTexture2DDesc& desc, DXTexture2D& outResource)
	{
		static uint64_t name_id = 1;

		Microsoft::WRL::ComPtr<ID3D12Resource> res;
		D3D12_RESOURCE_DESC dxDesc = DXTexture2D::ToDxDesc(desc);
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES((D3D12_HEAP_TYPE)desc.heapProp);

		D3D12_RESOURCE_STATES state = DXTexture2D::GetInitialResourceState(desc);
		D3D12_CLEAR_VALUE clearValue = DXTexture2D::ToDXTextureDesc(desc).clearValue;
		HRESULT hr = m_device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &dxDesc,
			state, desc.hasClearValue ? &clearValue : nullptr, IID_PPV_ARGS(&res));

		DX_LogIfFailed(hr);

		outResource.Initialize(desc, res, outResource.GetClassNameID() + "_" + std::to_string(name_id++));

		return hr;
	}

	HRESULT DX12RHI::CreateDXTexture2DArray(const DXTexture2DArrayDesc& desc, DXTexture2DArray& outResource)
	{
		static uint64_t name_id = 1;

		Microsoft::WRL::ComPtr<ID3D12Resource> res;
		D3D12_RESOURCE_DESC dxDesc = DXTexture2DArray::ToDxDesc(desc);
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES((D3D12_HEAP_TYPE)desc.heapProp);
		// 创建在upload堆的资源只能被GPU读，无法进行其他操作
		D3D12_RESOURCE_STATES state = DXTexture2DArray::GetInitialResourceState(desc);
		D3D12_CLEAR_VALUE clearValue = DXTexture2DArray::ToDXTextureDesc(desc).clearValue;
		HRESULT hr = m_device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &dxDesc,
			state, desc.hasClearValue ? &clearValue : nullptr, IID_PPV_ARGS(&res));

		DX_LogIfFailed(hr);

		outResource.Initialize(desc, res, outResource.GetClassNameID() + "_" + std::to_string(name_id++));

		return hr;
	}

	HRESULT DX12RHI::CreateDXTexture3D(const DXTexture3DDesc& desc, DXTexture3D& outResource)
	{
		static uint64_t name_id = 1;

		Microsoft::WRL::ComPtr<ID3D12Resource> res;
		D3D12_RESOURCE_DESC dxDesc = DXTexture3D::ToDxDesc(desc);
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES((D3D12_HEAP_TYPE)desc.heapProp);

		D3D12_RESOURCE_STATES state = DXTexture3D::GetInitialResourceState(desc);
		auto ret = m_device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &dxDesc,
			state, nullptr, IID_PPV_ARGS(&res));
		DX_LogIfFailed(ret);

		outResource.Initialize(desc, res, outResource.GetClassNameID() + "_" + std::to_string(name_id++));

		return ret;
	}


	Microsoft::WRL::ComPtr<ID3D12PipelineState> DX12RHI::CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc)
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> ret;
		DX_LogIfFailed(m_device->CreateGraphicsPipelineState(desc, IID_PPV_ARGS(&ret)));
		return ret;
	}

	uint32_t DX12RHI::GetCurrentBackBufferIndex() const
	{
		return m_frameSyncSystem->GetCurrentFrameIndex();
	}

	DXTexture2D* DX12RHI::GetCurrBackbuffer()
	{
		return m_swapChainContents[GetCurrentBackBufferIndex()].backBuffer.get();
	}

	DXTexture2D* DX12RHI::GetBackbuffer(uint32_t idx)
	{
		return m_swapChainContents[idx].backBuffer.get();
	}


	ID3D12CommandQueue* DX12RHI::GetQueueByType(QueueType type) const
	{
		switch (type)
		{
		case QueueType::Graphics:
			return m_graphicsQueue.Get();
		case QueueType::Compute:
			return m_computeQueue.Get();
		case QueueType::Copy:
			return m_copyQueue.Get();
		default:
			return nullptr;
		}
	}

	ID3D12Fence* DX12RHI::GetFenceByType(QueueType type) const
	{
		switch (type)
		{
		case QueueType::Graphics:
			return m_graphicsFence.Get();
		case QueueType::Compute:
			return m_computeFence.Get();
		case QueueType::Copy:
			return m_copyFence.Get();
		default:
			return nullptr;
		}
	}

	void DX12RHI::Shutdown()
	{
		FlushCommandQueue();
		// Clean DX12 DXResource
		if(m_swapChain)
		{
			m_swapChain->SetFullscreenState(false, nullptr); 
		}
	}


	void DX12RHI::OnWindowResize(const WindowResizeEvent& e)
	{
		// 新体系里由 RenderSystem 统一处理 resize：
			// 1. Resize swapchain
			// 2. Recreate sceneColor / sceneDepth
			// 3. Recreate postprocess targets
	}

}
