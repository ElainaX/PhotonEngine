#include "DX12RHI.h"
#include "Function/Render/WindowSystem.h"
#include "Macro.h"
#include "d3dx12.h"

#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXMath.h>

namespace photon 
{

	DX12RHI::~DX12RHI()
	{
	}

	void DX12RHI::Initialize(RHIInitInfo initializeInfo)
	{
		m_WindowSystem = initializeInfo.window_System;
		PHOTON_ASSERT(m_WindowSystem != nullptr, "WindowSystem Is NullPtr! DX12RHI Init Error!");

		CreateDebugManager();

		CreateFactory();

		CreateDevice();

		CreateFence();

		CreateCommandObjects();

		CreateDescriptorHeaps();

		CreateSwapChain();

		// RegisterWindowClass
		auto OnWindowResizeLambda = [this](WindowResizeEvent& event)
			{
				this->OnWindowResize(event);
			};
		m_WindowSystem->RegisterOnWindowResizeCallback(OnWindowResizeLambda);

	}

	void DX12RHI::CreateSwapChain()
	{
		DXGI_SWAP_CHAIN_DESC1 scDesc;
		{
			Vector2i widthAndHeight = m_WindowSystem->GetClientWidthAndHeight();
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
		DX_LogIfFailed(m_Factory->CreateSwapChainForHwnd(m_CmdQueue.Get(), m_WindowSystem->GetHwnd(), 
			&scDesc, nullptr, nullptr, &swapChain1));
		DX_LogIfFailed(swapChain1->QueryInterface(IID_PPV_ARGS(&m_SwapChain)));

		DX_LogIfFailed(m_SwapChain->SetMaximumFrameLatency(g_SwapChainCount));
		m_SwapChainWaitableObject = m_SwapChain->GetFrameLatencyWaitableObject();

		CreateSwapChainRenderTarget();
	}

	void DX12RHI::ReCreateSwapChain()
	{
		LOG_INFO("Recreate SwapChain");
		auto [width, height] = m_WindowSystem->GetClientWidthAndHeight();
		// ReCreate之前需要等待所有FrameResource已经渲染结束
		FlushCommandQueue();
		m_CurrBackBufferIndex = 0;

		// Clean Up SwapChain Resource
		for (UINT i = 0; i < g_SwapChainCount; i++)
		{
			if (m_SwapChainContents[i])
			{
				m_SwapChainContents[i]->backBuffer->gpuResource->Release();
				m_SwapChainContents[i]->backBuffer->gpuResource = nullptr;
			}
		}
		// 0: maintain, DXGI_FORMAT_UNKNOWN: maintain
		DX_LogIfFailed(m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 
							DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT));
		CreateSwapChainRenderTarget();
	}

	void DX12RHI::CreateFactory()
	{
		DX_LogIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_Factory)));
	}

	void DX12RHI::CreateDevice()
	{
		PHOTON_ASSERT(m_Factory.Get() != nullptr, "Factory Not Created!");
		
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter1;
		DX_LogIfFailed(m_Factory->EnumAdapters1(0, &adapter1));
		// 高级功能需要Query
		DX_LogIfFailed(adapter1->QueryInterface(IID_PPV_ARGS(&m_Adapter)));

		DXGI_ADAPTER_DESC gpuDesc;
		DX_LogIfFailed(m_Adapter->GetDesc(&gpuDesc));
		LOG_INFO("Successfully Connect To GPU: {}", WString2String(gpuDesc.Description));

		DX_LogIfFailed(D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_Device)));
	}

	void DX12RHI::CreateFence()
	{
		PHOTON_ASSERT(m_Device.Get() != nullptr, "Device Not Created!");
		Microsoft::WRL::ComPtr<ID3D12Fence> fence0;
		DX_LogIfFailed(m_Device->CreateFence(0u, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence0)));
		DX_LogIfFailed(fence0->QueryInterface(IID_PPV_ARGS(&m_Fence)));
	}

	void DX12RHI::CreateCommandObjects()
	{
		PHOTON_ASSERT(m_Device.Get() != nullptr, "Device Not Created!");

		D3D12_COMMAND_QUEUE_DESC queueDesc;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = 0;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;
		
		DX_LogIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CmdQueue)));


		DX_LogIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_MainCmdAllocator)));
		for(int i = 0; i < g_FrameResourceCount; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
			DX_LogIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)));
			m_FrameContexts[i] = std::make_shared<FrameContext>(FrameContext{ allocator , 0});  
		}

		// CmdList刚开启的时候是打开状态，需要手动关闭
		DX_LogIfFailed(m_Device->CreateCommandList(0u, D3D12_COMMAND_LIST_TYPE_DIRECT, m_MainCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_MainCmdList)));
		DX_LogIfFailed(m_MainCmdList->Close());
	}

	void DX12RHI::CreateSwapChainRenderTarget()
	{
		for (UINT i = 0; i < g_SwapChainCount; i++)
		{
			ID3D12Resource* pBackBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			m_SwapChainContents[i]->backBuffer = std::make_shared<Texture2D>(pBackBuffer->GetDesc(), pBackBuffer);
			m_SwapChainContents[i]->backBuffer->name = "SwapChain Buffer" + std::to_string(i);
			m_Device->CreateRenderTargetView(pBackBuffer, nullptr, m_SwapChainContents[i]->cpuDescriptor);
		}
	}

	void DX12RHI::CreateDescriptorHeaps()
	{
		m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
		heapDesc.NumDescriptors = g_SwapChainCount * 1;
		heapDesc.NodeMask = 0;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DX_LogIfFailed(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_RtvHeap)));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		for(int i = 0; i < g_SwapChainCount; ++i)
		{
			m_SwapChainContents[i] = std::make_shared<SwapChainContent>();
			m_SwapChainContents[i]->cpuDescriptor = rtvHeapHandle;
			rtvHeapHandle.Offset(m_RtvDescriptorSize);
		}

	}

	void DX12RHI::CreateAssetAllocator()
	{
	}

	void DX12RHI::FlushCommandQueue()
	{
		UINT fenceValue = ++m_FenceValue;
		m_CmdQueue->Signal(m_Fence.Get(), fenceValue);
		if(m_Fence->GetCompletedValue() < fenceValue)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			m_Fence->SetEventOnCompletion(fenceValue, eventHandle);
			HANDLE waitableObjects[] = { eventHandle, m_SwapChainWaitableObject };
			WaitForMultipleObjectsEx(2, waitableObjects, TRUE, INFINITE, TRUE);
			CloseHandle(eventHandle);
		}
	}

	void DX12RHI::WaitForFenceValue(uint64_t fenceValue)
	{
		if (m_Fence->GetCompletedValue() < fenceValue)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			m_Fence->SetEventOnCompletion(fenceValue, eventHandle);
			WaitForSingleObjectEx(eventHandle, TRUE, INFINITE);
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

	unsigned int DX12RHI::GetCurrBackBufferIndex()
	{
		return m_CurrBackBufferIndex;
	}

	std::shared_ptr<Texture2D> DX12RHI::CreateTexture2D(Texture2DDesc desc)
	{
		D3D12_RESOURCE_DESC dxDesc;
		dxDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dxDesc.Format = desc.format;
		dxDesc.MipLevels = desc.maxMipLevel;
		dxDesc.Alignment = 0;
		dxDesc.DepthOrArraySize = 1;
		dxDesc.Width = desc.width;
		dxDesc.Height = desc.height;
		dxDesc.SampleDesc.Count = desc.sampleCount;
		dxDesc.SampleDesc.Quality = desc.sampleQuality;
		dxDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dxDesc.Flags = desc.flag;

		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES((D3D12_HEAP_TYPE)desc.heapProp);
		CD3DX12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(desc.format, m_ClearColor.ptr());
		DX_LogIfFailed(m_Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &dxDesc,
			D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(&resource)));

		std::shared_ptr<Texture2D> tex = std::make_shared<Texture2D>(dxDesc, resource);

		// 交给ResourceManager管理
		// ResourceManager::PushResource

		return tex;
	}

	void DX12RHI::CopyTextureToSwapChain(std::shared_ptr<Texture2D> tex)
	{
		ResourceStateTransform(tex.get(), D3D12_RESOURCE_STATE_COPY_SOURCE);
		ResourceStateTransform(m_SwapChainContents[m_CurrBackBufferIndex]->backBuffer.get(), D3D12_RESOURCE_STATE_COPY_DEST);

		if(tex->dxDesc.SampleDesc.Count == 1)
		{
			m_MainCmdList->CopyResource(m_SwapChainContents[m_CurrBackBufferIndex]->backBuffer->gpuResource.Get(), tex->gpuResource.Get());
		}
	}

	void DX12RHI::Present()
	{
		DX_LogIfFailed(m_SwapChain->Present(0, 0));
		m_CurrBackBufferIndex = (m_CurrBackBufferIndex + 1) % g_SwapChainCount;
		FlushCommandQueue();
	}

	void DX12RHI::BeginSingleRenderPass()
	{
		DX_LogIfFailed(m_MainCmdAllocator->Reset());
		DX_LogIfFailed(m_MainCmdList->Reset(m_MainCmdAllocator.Get(), nullptr));
	}

	void DX12RHI::EndSingleRenderPass()
	{
		DX_LogIfFailed(m_MainCmdList->Close());
		ID3D12CommandList* cmdsLists[] = { m_MainCmdList.Get() };
		m_CmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	}

	void DX12RHI::ResourceStateTransform(Resource* resource, D3D12_RESOURCE_STATES stateAfter)
	{
		D3D12_RESOURCE_STATES stateBefore = resource->state;
		if (stateBefore == stateAfter)
			return;
		
		auto resourceTrans = CD3DX12_RESOURCE_BARRIER::Transition(resource->gpuResource.Get(), stateBefore, stateAfter);
		m_MainCmdList->ResourceBarrier(1, &resourceTrans);

		resource->state = stateAfter;
	}

	void DX12RHI::PrepareForPresent()
	{
		ResourceStateTransform(m_SwapChainContents[m_CurrBackBufferIndex]->backBuffer.get(), D3D12_RESOURCE_STATE_PRESENT);
	}

	void DX12RHI::Clear()
	{
		FlushCommandQueue();
		// Clean DX12 Resource
		if(m_SwapChain)
		{
			m_SwapChain->SetFullscreenState(false, nullptr); 
		}
	}

	void DX12RHI::OnWindowResize(const WindowResizeEvent& e)
	{
		ReCreateSwapChain();
	}

}