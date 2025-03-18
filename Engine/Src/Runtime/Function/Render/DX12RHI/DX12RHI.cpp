#include "DX12RHI.h"
#include "Function/Render/WindowSystem.h"
#include "Macro.h"
#include "d3dx12.h"
#include "Platform/FileSystem/FileSystem.h"
#include "../Shader/TestShader.h"
#include "../ResourceManager.h"


#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <cmath>

namespace photon 
{

	DX12RHI::~DX12RHI()
	{
	}

	void DX12RHI::Initialize(RHIInitInfo initializeInfo)
	{
		// 等待被删除
		m_ResourceManager = std::make_shared<ResourceManager>();
		m_ResourceManager->Initialize(this);


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

		// 做一些Test的准备
		FlushCommandQueue();

		BeginSingleRenderPass();

		CompileShaders();


		Texture2DDesc desc;
		desc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.flag = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		desc.heapProp = ResourceHeapProperties::Default;
		desc.clearValue = { 1.0f, 1.0f, 1.0f, 1.0f };
		desc.width = m_WindowSystem->GetClientWidthAndHeight().x;
		desc.height = m_WindowSystem->GetClientWidthAndHeight().y;

		m_RenderTex = m_ResourceManager->CreateTexture2D(desc);
		m_ResourceToViews.insert({m_RenderTex.get(), m_RtvHeap->CreateRenderTargetView(m_RenderTex.get(), nullptr)});

		Texture2DDesc dsvtex;
		dsvtex.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvtex.flag = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		dsvtex.heapProp = ResourceHeapProperties::Default;
		dsvtex.width = desc.width;
		dsvtex.height = desc.height;
		dsvtex.clearValue = { 1.0f, 0.0f, 0.0f, -1.0f };
		m_DepthStencilTex = m_ResourceManager->CreateTexture2D(dsvtex);
		m_ResourceToViews.insert({ m_DepthStencilTex.get(), m_DsvHeap->CreateDepthStencilView(m_DepthStencilTex.get(), nullptr) });
		ResourceStateTransform(m_DepthStencilTex.get(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

		VertexSimple vertices[] =
		{
			VertexSimple{Vector3{-0.5f, -0.5f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
			VertexSimple{Vector3{0.0f, 0.5f, 0.0f}  , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
			VertexSimple{Vector3{0.5f, -0.5f, 0.0f} , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		};

		uint32_t indices[] = { 0, 1, 2 };


		MeshDesc triMeshDesc;
		triMeshDesc.name = "Triangle";
		triMeshDesc.type = VertexType::VertexSimple;
		DX_LogIfFailed(D3DCreateBlob(sizeof(VertexSimple) * 3, &triMeshDesc.vertexRawData));
		CopyMemory(triMeshDesc.vertexRawData->GetBufferPointer(), (void*)vertices, triMeshDesc.vertexRawData->GetBufferSize());

		DX_LogIfFailed(D3DCreateBlob(sizeof(uint32_t) * 3, &triMeshDesc.indexRawData));
		CopyMemory(triMeshDesc.indexRawData->GetBufferPointer(), (void*)indices, triMeshDesc.indexRawData->GetBufferSize());
	
		std::shared_ptr<Mesh> triMesh = m_ResourceManager->CreateMesh(triMeshDesc);


		m_RenderMeshCollection = std::make_shared<RenderMeshCollection>();
		m_RenderMeshCollection->PushMesh(triMesh);
		m_RenderMeshCollection->EndPush(this);

		m_RenderItem.meshCollection = m_RenderMeshCollection.get();
		m_RenderItem.meshGuid = triMesh->guid;


		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(
			0,      // 无参数
			nullptr, // 无描述符
			0,      // 无静态采样器
			nullptr, // 无静态采样器描述
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT // 允许输入装配器使用输入布局
		);

		using namespace Microsoft::WRL;
		// 序列化 Root Signature
		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
		if (FAILED(hr))
		{
			if (error)
			{
				OutputDebugStringA((char*)error->GetBufferPointer());
			}
			throw std::runtime_error("Failed to serialize root signature");
		}

		// 创建 Root Signature
		hr = m_Device->CreateRootSignature(
			0,                              // 节点掩码（单 GPU 为 0）
			signature->GetBufferPointer(),  // 序列化后的 Root Signature 数据
			signature->GetBufferSize(),     // 数据大小
			IID_PPV_ARGS(&m_RootSignature)  // 返回的 Root Signature 对象
		);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create root signature");
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		auto& inputLayout = VertexSimple::GetInputLayout();
		psoDesc.InputLayout = { inputLayout.data(), (unsigned int)inputLayout.size() };
		psoDesc.pRootSignature = m_RootSignature.Get();
		psoDesc.VS = {
		reinterpret_cast<BYTE*>(m_TestShaderBlob->vsBlob->GetBufferPointer()),
		m_TestShaderBlob->vsBlob->GetBufferSize()
		};
		psoDesc.PS = {
		reinterpret_cast<BYTE*>(m_TestShaderBlob->psBlob->GetBufferPointer()),
		m_TestShaderBlob->psBlob->GetBufferSize()
		};
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DX_LogIfFailed(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

		EndSingleRenderPass();

		FlushCommandQueue();
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

		for(int i = 0; i < g_SwapChainCount; ++i)
		{
			m_SwapChainContents[i] = SwapChainContent();
			m_SwapChainContents[i].view = m_RtvHeap->CreateRenderTargetView();
		}

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
			if (m_SwapChainContents[i].view != nullptr)
			{
				m_SwapChainContents[i].backBuffer->gpuResource->Release();
				m_SwapChainContents[i].backBuffer->gpuResource = nullptr;
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
			m_SwapChainContents[i].backBuffer = std::make_shared<Texture2D>(pBackBuffer->GetDesc(), ResourceHeapProperties::Default, pBackBuffer, nullptr);
			m_SwapChainContents[i].backBuffer->name = "SwapChain Buffer" + std::to_string(i);
			m_RtvHeap->CreateRenderTargetView(m_SwapChainContents[i].backBuffer.get(), nullptr, m_SwapChainContents[i].view);
		}
	}

	void DX12RHI::CreateDescriptorHeaps()
	{
		//m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		//D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
		//heapDesc.NumDescriptors = g_SwapChainCount + 1;
		//heapDesc.NodeMask = 0;
		//heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		//heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		//DX_LogIfFailed(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_RtvHeap)));

		//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		//for(int i = 0; i < g_SwapChainCount; ++i)
		//{
		//	m_SwapChainContents[i] = std::make_shared<SwapChainContent>();
		//	m_SwapChainContents[i]->cpuDescriptor = rtvHeapHandle;
		//	rtvHeapHandle.Offset(m_RtvDescriptorSize);
		//}

		//m_DsvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		//D3D12_DESCRIPTOR_HEAP_DESC dsvheapDesc;
		//dsvheapDesc.NumDescriptors = 1;
		//dsvheapDesc.NodeMask = 0;
		//dsvheapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		//dsvheapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		//DX_LogIfFailed(m_Device->CreateDescriptorHeap(&dsvheapDesc, IID_PPV_ARGS(&m_DsvHeap)));

		m_RtvHeap = std::make_shared<RtvDescriptorHeap>(m_Device.Get(), g_RenderTargetHeapSize);
		m_DsvHeap = std::make_shared<DsvDescriptorHeap>(m_Device.Get(), g_DepthStencilHeapSize);
		m_CbvUavSrvHeap = std::make_shared<CbvSrvUavDescriptorHeap>(m_Device.Get(),
			g_CbvSrvUavHeapSize, static_cast<int>(g_CbvSrvUavHeapSize * 0.4), static_cast<int>(g_CbvSrvUavHeapSize * 0.4));
		m_SamplerHeap = std::make_shared<SamplerDescriptorHeap>(m_Device.Get(), g_SamplerHeapSize);
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
		D3D12_RESOURCE_DESC dxDesc = Texture2D::ToDxDesc(desc);

		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES((D3D12_HEAP_TYPE)desc.heapProp);
		D3D12_CLEAR_VALUE optClearValue;
		optClearValue.Color[0] = desc.clearValue.x;
		optClearValue.Color[1] = desc.clearValue.y;
		optClearValue.Color[2] = desc.clearValue.z;
		optClearValue.Color[3] = desc.clearValue.w;
		optClearValue.Format = desc.format;
		D3D12_RESOURCE_STATES state = desc.heapProp == ResourceHeapProperties::Upload ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COMMON;
		DX_LogIfFailed(m_Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &dxDesc,
			state, &optClearValue, IID_PPV_ARGS(&resource)));

		std::shared_ptr<Texture2D> tex = std::make_shared<Texture2D>(desc, resource);

		if (desc.cpuResource != nullptr)
		{
			CopyDataCpuToGpu(tex.get(), desc.cpuResource->GetBufferPointer(), desc.cpuResource->GetBufferSize());
			tex->cpuResource = desc.cpuResource;
		}

		return tex;
	}

	std::shared_ptr<Buffer> DX12RHI::CreateBuffer(BufferDesc desc)
	{
		D3D12_RESOURCE_DESC dxDesc = Buffer::ToDxDesc(desc);
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES((D3D12_HEAP_TYPE)desc.heapProp);
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_RESOURCE_STATES state = desc.heapProp == ResourceHeapProperties::Upload ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COMMON;
		DX_LogIfFailed(m_Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &dxDesc, state, nullptr, IID_PPV_ARGS(&resource)));
		std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(desc, resource);


		if (desc.cpuResource != nullptr)
		{
			CopyDataCpuToGpu(buffer.get(), desc.cpuResource->GetBufferPointer(), desc.cpuResource->GetBufferSize());
			buffer->cpuResource = desc.cpuResource;
		}


		return buffer;
	}

	std::shared_ptr<Buffer> DX12RHI::CreateBuffer(BufferDesc desc, const void* data, UINT64 sizeInBytes)
	{
		if (desc.heapProp == ResourceHeapProperties::Default)
		{
			LOG_ERROR("Static Heap Type Can't Get Resource Directly!");
		}
		DX_LogIfFailed(D3DCreateBlob(sizeInBytes, &desc.cpuResource));
		std::shared_ptr<Buffer> buffer = CreateBuffer(desc);
		return buffer;
	}

	void DX12RHI::CompileShaders()
	{
		m_TestShader = std::make_shared<TestShader>(L"E:/Code/PhotonEngine/Engine/Src/Runtime/Function/Render/Shaders/TestShader.hlsl");
		m_TestShaderBlob = m_TestShader->Compile({ MacroInfo{"MEME", "3"} });
	}

	void DX12RHI::CopyTextureToSwapChain(std::shared_ptr<Texture2D> tex)
	{
		ResourceStateTransform(tex.get(), D3D12_RESOURCE_STATE_COPY_SOURCE);
		ResourceStateTransform(m_SwapChainContents[m_CurrBackBufferIndex].backBuffer.get(), D3D12_RESOURCE_STATE_COPY_DEST);

		if(tex->dxDesc.SampleDesc.Count == 1)
		{
			m_MainCmdList->CopyResource(m_SwapChainContents[m_CurrBackBufferIndex].backBuffer->gpuResource.Get(), tex->gpuResource.Get());
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
		ResourceStateTransform(m_SwapChainContents[m_CurrBackBufferIndex].backBuffer.get(), D3D12_RESOURCE_STATE_PRESENT);
	}



	void DX12RHI::TestRender()
	{
		BeginSingleRenderPass();

		ResourceStateTransform(m_RenderTex.get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		auto [width, height] = m_WindowSystem->GetClientWidthAndHeight();
		D3D12_RECT d3dScissorRect = { 0, 0, (LONG)width, (LONG)height };
		D3D12_VIEWPORT d3dViewport{ (FLOAT)0,(FLOAT)0, (FLOAT)width, (FLOAT)height,
			0.0f, 1.0f};
		m_MainCmdList->RSSetViewports(1, &d3dViewport);
		m_MainCmdList->RSSetScissorRects(1, &d3dScissorRect);
		m_MainCmdList->SetPipelineState(m_PipelineState.Get());
		m_MainCmdList->SetGraphicsRootSignature(m_RootSignature.Get());

		auto rtv = m_ResourceToViews[m_RenderTex.get()];
		auto dsv = m_ResourceToViews[m_DepthStencilTex.get()];

		Vector4 clearValue = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_MainCmdList->ClearRenderTargetView(rtv->cpuHandleInHeap, (float*)&clearValue, 0, nullptr);
		m_MainCmdList->ClearDepthStencilView(dsv->cpuHandleInHeap,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		m_MainCmdList->OMSetRenderTargets(1, &rtv->cpuHandleInHeap, true, &dsv->cpuHandleInHeap);


		m_MainCmdList->IASetVertexBuffers(0, 1, &m_RenderMeshCollection->VertexBufferView());
		m_MainCmdList->IASetIndexBuffer(&m_RenderMeshCollection->IndexBufferView());

		auto mesh = m_RenderMeshCollection->GetMesh(m_RenderItem.meshGuid);
		m_MainCmdList->IASetPrimitiveTopology(m_RenderItem.primitiveType);

		m_MainCmdList->DrawIndexedInstanced(mesh->indexCount, 1, mesh->startIndexLocation, mesh->baseVertexLocation, 0);



		CopyTextureToSwapChain(m_RenderTex);

		PrepareForPresent();

		EndSingleRenderPass();

		Present();
	}

	void DX12RHI::CopyDataGpuToGpu(Resource* dstResource, Resource* srcResource)
	{
		ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_COPY_DEST);
		ResourceStateTransform(srcResource, D3D12_RESOURCE_STATE_GENERIC_READ);
		m_MainCmdList->CopyResource(dstResource->gpuResource.Get(), srcResource->gpuResource.Get());
	}

	void DX12RHI::CopyDataCpuToGpu(Resource* dstResource, const void* data, UINT64 sizeInBytes)
	{
		auto dst = dstResource->gpuResource.Get();
		void* mappedData = nullptr;
		dst->Map(0, nullptr, &mappedData);
		CopyMemory(mappedData, data, sizeInBytes);
		dst->Unmap(0, nullptr);
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