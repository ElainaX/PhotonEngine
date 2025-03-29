#include "DX12RHI.h"
#include "Function/Render/WindowSystem.h"
#include "Macro.h"
#include "d3dx12.h"
#include "Platform/FileSystem/FileSystem.h"
#include "../Shader/TestShader.h"
#include "../ResourceManager.h"
#include "Function/Util/RenderUtil.h"
#include "DirectXTK/WICTextureLoader12.h"


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
		//FlushCommandQueue();

		//BeginSingleRenderPass();

		//CompileShaders();


		//Texture2DDesc desc;
		//desc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//desc.flag = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		//desc.heapProp = ResourceHeapProperties::Default;
		//desc.clearValue = { 1.0f, 1.0f, 1.0f, 1.0f };
		//desc.width = m_WindowSystem->GetClientWidthAndHeight().x;
		//desc.height = m_WindowSystem->GetClientWidthAndHeight().y;

		//m_RenderTex = m_ResourceManager->CreateTexture2D(desc);
		//m_ResourceToViews.insert({m_RenderTex.get(), m_RtvHeap->CreateRenderTargetView(m_RenderTex.get(), nullptr)});

		//Texture2DDesc dsvtex;
		//dsvtex.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//dsvtex.flag = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		//dsvtex.heapProp = ResourceHeapProperties::Default;
		//dsvtex.width = desc.width;
		//dsvtex.height = desc.height;
		//dsvtex.clearValue = { 1.0f, 0.0f, 0.0f, 0.0f };
		//m_DepthStencilTex = m_ResourceManager->CreateTexture2D(dsvtex);
		//m_ResourceToViews.insert({ m_DepthStencilTex.get(), m_DsvHeap->CreateDepthStencilView(m_DepthStencilTex.get(), nullptr) });
		//ResourceStateTransform(m_DepthStencilTex.get(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

		//VertexSimple vertices[] =
		//{
		//	VertexSimple{Vector3{-0.5f, -0.5f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		//	VertexSimple{Vector3{0.0f, 0.5f, 0.0f}  , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		//	VertexSimple{Vector3{0.5f, -0.5f, 0.0f} , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		//};

		//VertexSimple vertices2[] =
		//{
		//	VertexSimple{Vector3{-0.0f, -0.5f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		//	VertexSimple{Vector3{0.5f, 0.5f, 0.0f}  , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		//	VertexSimple{Vector3{1.0f, -0.5f, 0.0f} , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		//};

		//uint32_t indices[] = { 0, 1, 2 };


		//MeshDesc triMeshDesc;
		//triMeshDesc.name = "Triangle";
		//triMeshDesc.type = VertexType::VertexSimple;
		//triMeshDesc.vertexRawData = RenderUtil::CreateD3DBlob(vertices, sizeof(VertexSimple) * 3);
		//triMeshDesc.indexRawData = RenderUtil::CreateD3DBlob(indices, sizeof(uint32_t) * 3);
		//std::shared_ptr<Mesh> triMesh = m_ResourceManager->CreateMesh(triMeshDesc);

		//MeshDesc triMeshDesc2;
		//triMeshDesc2.name = "Triangle2";
		//triMeshDesc2.type = VertexType::VertexSimple;
		//triMeshDesc2.vertexRawData = RenderUtil::CreateD3DBlob(vertices2, sizeof(VertexSimple) * 3);
		//triMeshDesc2.indexRawData = RenderUtil::CreateD3DBlob(indices, sizeof(uint32_t) * 3);
		//std::shared_ptr<Mesh> triMesh2 = m_ResourceManager->CreateMesh(triMeshDesc2);

		//m_RenderMeshCollection = std::make_shared<RenderMeshCollection>();
		//m_RenderMeshCollection->PushMesh(triMesh);
		//m_RenderMeshCollection->PushMesh(triMesh2);
		//m_RenderMeshCollection->EndPush(this);

		//m_RenderItem.meshCollection = m_RenderMeshCollection.get();
		//m_RenderItem.meshGuid = triMesh->guid;
		//m_RenderItem2.meshCollection = m_RenderMeshCollection.get();
		//m_RenderItem2.meshGuid = triMesh2->guid;

		//// 创建一个cbv的view
		//Vector4 gcolor[2] = { { 1.0, 0.5, 0.5, 0.5 },  { 0.5, 0.9, 0.9, 0.5 } };
		//BufferDesc constantBufferDesc;
		//constantBufferDesc.bufferSizeInBytes = 2 * RenderUtil::GetConstantBufferByteSize(sizeof(gcolor));
		//constantBufferDesc.cpuResource = RenderUtil::CreateD3DBlob(&gcolor[0], constantBufferDesc.bufferSizeInBytes, sizeof(Vector4));
		//void* dst = (char*)constantBufferDesc.cpuResource->GetBufferPointer() + 256;
		//CopyMemory(dst, (const void*)&gcolor[1], sizeof(Vector4));
		//constantBufferDesc.heapProp = ResourceHeapProperties::Default;
		//m_ConstantBuffer = m_ResourceManager->CreateBuffer(constantBufferDesc);


		//D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
		//constantBufferViewDesc.BufferLocation = m_ConstantBuffer->gpuResource->GetGPUVirtualAddress();
		//constantBufferViewDesc.SizeInBytes = 256;
		//m_ColorAView = m_CbvUavSrvHeap->CreateConstantBufferView(&constantBufferViewDesc);
		//constantBufferViewDesc.BufferLocation = m_ConstantBuffer->gpuResource->GetGPUVirtualAddress() + 256;
		//constantBufferViewDesc.SizeInBytes = 256;
		//m_ColorBView = m_CbvUavSrvHeap->CreateConstantBufferView(&constantBufferViewDesc);

		//m_RootSignature = CreateRootSignature(m_TestShader.get());
		//m_GraphicsPipeline = std::make_shared<DXGraphicsPipeline>();
		//m_GraphicsPipeline->SetShaderMust(m_TestShader.get(), {}, m_RootSignature.Get());
		//m_GraphicsPipeline->SetRenderTargetMust({ DXGI_FORMAT_R8G8B8A8_UNORM });

		//m_GraphicsPipeline->FinishOffRenderSet(this);

		//m_GraphicsPipeline2 = std::make_shared<DXGraphicsPipeline>(*m_GraphicsPipeline);

		////BlendState
		//BlendColorEquation colorEquation(BlendSrc{}, BlendSrcF{ BlendFactorValueType::srcAlpha }, BlendOp::add, BlendDst{}, BlendDstF{ BlendFactorValueType::oneMinusSrcAlpha });
		//BlendEquation equation(colorEquation);
		//BlendState blendState(equation);
		//m_GraphicsPipeline2->SetBlendState(blendState);
	
		//DepthState dpState;
		//dpState.isDepthTestEnbale = false;
		//DepthStencilState depthStencilState;
		//depthStencilState.depthState = dpState;
		//m_GraphicsPipeline2->SetDepthStencilState(depthStencilState);
		//m_GraphicsPipeline2->FinishOffRenderSet(this);


		//auto& inputLayout = m_TestShader->GetShaderInputLayout();
		//auto shaderBlob = m_TestShader->Compile({});

		//D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		//ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		//psoDesc.InputLayout = { inputLayout.data(), (unsigned int)inputLayout.size() };
		//psoDesc.pRootSignature = m_RootSignature.Get();
		//psoDesc.VS = shaderBlob->GetVSShaderByteCode();
		//psoDesc.PS = shaderBlob->GetPSShaderByteCode();
		//psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		//psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		//psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		//psoDesc.SampleMask = UINT_MAX;
		//psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//psoDesc.NumRenderTargets = 1;
		//psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		//psoDesc.SampleDesc.Count = 1;
		//psoDesc.SampleDesc.Quality = 0;
		//psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//DX_LogIfFailed(m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

		//EndSingleRenderPass();

		//FlushCommandQueue();
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
		for(int i = 0; i < g_FrameContextCount; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
			DX_LogIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)));
			m_FrameContexts[i] = std::make_shared<FrameContext>();  
			m_FrameContexts[i]->cmdAllocator = allocator;
		}

		// CmdList刚开启的时候是打开状态，需要手动关闭
		DX_LogIfFailed(m_Device->CreateCommandList(0u, D3D12_COMMAND_LIST_TYPE_DIRECT, m_MainCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_MainCmdList)));
		DX_LogIfFailed(m_Device->CreateCommandList(0u, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameContexts[m_CurrFrameContextIndex]->cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_FrameResourceCmdList)));
		DX_LogIfFailed(m_MainCmdList->Close());
		m_CurrCmdList = m_FrameResourceCmdList.Get();
		m_CurrFrameContext = m_FrameContexts[m_CurrFrameContextIndex].get();
	}

	void DX12RHI::CreateSwapChainRenderTarget()
	{
		for (UINT i = 0; i < g_SwapChainCount; i++)
		{
			ID3D12Resource* pBackBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
			m_SwapChainContents[i].backBuffer = std::make_shared<Texture2D>(pBackBuffer->GetDesc(), ResourceHeapProperties::Default, pBackBuffer, nullptr);
			m_SwapChainContents[i].backBuffer->name = L"SwapChain Buffer" + std::to_wstring(i);
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


	Microsoft::WRL::ComPtr<ID3D12RootSignature> DX12RHI::CreateRootSignature(Shader* shader, int samplerCount /*= 0*/, const D3D12_STATIC_SAMPLER_DESC* samplerDesc /*= nullptr*/)
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> ret;
		auto signature = shader->GetDXSerializedRootSignatureBlob(samplerCount, samplerDesc);
		DX_LogIfFailed(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&ret)));
		return ret;
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
		if (fenceValue <= 0)
			return;

		if (m_Fence->GetCompletedValue() < fenceValue)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			DX_LogIfFailed(m_Fence->SetEventOnCompletion(fenceValue, eventHandle));
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
			if(desc.heapProp == ResourceHeapProperties::Upload)
			{
				CopyDataCpuToGpu(buffer.get(), desc.cpuResource->GetBufferPointer(), desc.cpuResource->GetBufferSize());
				buffer->cpuResource = desc.cpuResource;
			}
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
		CopyMemory(desc.cpuResource->GetBufferPointer(), data, sizeInBytes);
		std::shared_ptr<Buffer> buffer = CreateBuffer(desc);
		return buffer;
	}

	void DX12RHI::CopyTextureToSwapChain(Texture2D* tex)
	{
		ResourceStateTransform(tex, D3D12_RESOURCE_STATE_COPY_SOURCE);
		ResourceStateTransform(m_SwapChainContents[m_CurrBackBufferIndex].backBuffer.get(), D3D12_RESOURCE_STATE_COPY_DEST);

		if(tex->dxDesc.SampleDesc.Count == 1)
		{
			m_CurrCmdList->CopyResource(m_SwapChainContents[m_CurrBackBufferIndex].backBuffer->gpuResource.Get(), tex->gpuResource.Get());
		}
	}

	void DX12RHI::Present()
	{
		DX_LogIfFailed(m_FrameResourceCmdList->Close());
		ID3D12CommandList* cmdsLists[] = { m_FrameResourceCmdList.Get() };
		m_CmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		DX_LogIfFailed(m_SwapChain->Present(0, 0));
		m_CurrBackBufferIndex = (m_CurrBackBufferIndex + 1) % g_SwapChainCount;
		m_CurrFrameContext->fenceValue = ++m_FenceValue;
		m_CmdQueue->Signal(m_Fence.Get(), m_CurrFrameContext->fenceValue);

		m_CurrFrameContextIndex = (m_CurrFrameContextIndex + 1) % g_FrameContextCount;
		m_CurrFrameContext = m_FrameContexts[m_CurrFrameContextIndex].get();
		WaitForFenceValue(m_CurrFrameContext->fenceValue);
		DX_LogIfFailed(m_CurrFrameContext->cmdAllocator->Reset());
		m_FrameResourceCmdList->Reset(m_CurrFrameContext->cmdAllocator.Get(), nullptr);
		m_CurrCmdList = m_FrameResourceCmdList.Get();
		

		/*FlushCommandQueue();*/
	}

	void DX12RHI::BeginSingleRenderPass()
	{
		DX_LogIfFailed(m_MainCmdAllocator->Reset());
		DX_LogIfFailed(m_MainCmdList->Reset(m_MainCmdAllocator.Get(), nullptr));
		m_CurrCmdList = m_MainCmdList.Get();
	}

	void DX12RHI::EndSingleRenderPass()
	{
		DX_LogIfFailed(m_MainCmdList->Close());
		ID3D12CommandList* cmdsLists[] = { m_MainCmdList.Get() };
		m_CmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
		FlushCommandQueue();
		m_CurrCmdList = m_FrameResourceCmdList.Get();
	}

	void DX12RHI::ResourceStateTransform(Resource* resource, D3D12_RESOURCE_STATES stateAfter)
	{
		D3D12_RESOURCE_STATES stateBefore = resource->state;
		if (stateBefore == stateAfter)
			return;
		
		auto resourceTrans = CD3DX12_RESOURCE_BARRIER::Transition(resource->gpuResource.Get(), stateBefore, stateAfter);
		m_CurrCmdList->ResourceBarrier(1, &resourceTrans);

		resource->state = stateAfter;
	}

	void DX12RHI::PrepareForPresent()
	{
		ResourceStateTransform(m_SwapChainContents[m_CurrBackBufferIndex].backBuffer.get(), D3D12_RESOURCE_STATE_PRESENT);
	}


	void DX12RHI::CmdSetViewportsAndScissorRects(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport)
	{
		m_CurrCmdList->RSSetViewports(1, &viewport);
		m_CurrCmdList->RSSetScissorRects(1, &scissorRect);
	}

	void DX12RHI::CmdSetPipelineState(ID3D12PipelineState* pipeline)
	{
		m_CurrCmdList->SetPipelineState(pipeline);
	}

	void DX12RHI::CmdSetGraphicsRootSignature(ID3D12RootSignature* rootSignature)
	{
		m_CurrCmdList->SetGraphicsRootSignature(rootSignature);
	}

	void DX12RHI::CmdSetRenderTargets(UINT numRenderTargetViews, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, bool RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor)
	{
		m_CurrCmdList->OMSetRenderTargets(numRenderTargetViews, pRenderTargetDescriptors, RTsSingleHandleToDescriptorRange, pDepthStencilDescriptor);
	}

	void DX12RHI::CmdSetDescriptorHeaps(const std::vector<ID3D12DescriptorHeap*>& descriptorHeaps)
	{
		m_CurrCmdList->SetDescriptorHeaps(descriptorHeaps.size(), &descriptorHeaps[0]);
	}

	void DX12RHI::CmdSetDescriptorHeaps()
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvUavSrvHeap->GetDXHeapPtr(), m_SamplerHeap->GetDXHeapPtr() };
		m_CurrCmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	}

	void DX12RHI::CmdSetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
	{
		m_CurrCmdList->SetGraphicsRootDescriptorTable(RootParameterIndex, BaseDescriptor);
	}

	void DX12RHI::CmdSetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		m_CurrCmdList->SetGraphicsRootConstantBufferView(RootParameterIndex, BufferLocation);
	}

	void DX12RHI::CmdSetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews)
	{
		m_CurrCmdList->IASetVertexBuffers(startSlot, numViews, pViews);
	}

	void DX12RHI::CmdSetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView)
	{
		m_CurrCmdList->IASetIndexBuffer(pView);
	}

	void DX12RHI::CmdSetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology)
	{
		m_CurrCmdList->IASetPrimitiveTopology(PrimitiveTopology);
	}

	void DX12RHI::CmdDrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
	{
		m_CurrCmdList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
	}

	photon::FrameResource* DX12RHI::GetCurrFrameResource(FrameResourceType type)
	{
		return m_FrameContexts[m_CurrFrameContextIndex]->frameResources[type].get();
	}

	void DX12RHI::CreateFrameResource(FrameResourceType type, FrameResourceDesc* desc)
	{
		for(int i = 0; i < g_FrameContextCount; ++i)
		{
			switch (type)
			{
			case photon::FrameResourceType::StaticModelFrameResource: 
			{
				auto _desc = dynamic_cast<StaticModelFrameResourceDesc*>(desc);
				auto frameResource = std::make_shared<StaticModelFrameResource>(this, *_desc);
				m_FrameContexts[i]->frameResources[type] = frameResource;
				break;
			}
			case photon::FrameResourceType::DynamicModelFrameResource:
				break;
			default:
				break;
			}


		}
	}

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> DX12RHI::GetStaticSamplers()
	{
		const CD3DX12_STATIC_SAMPLER_DESC pointWrap(0, D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		const CD3DX12_STATIC_SAMPLER_DESC pointClamp(1, D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(2, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(3, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
		const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(4, D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(5, D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

		return { pointWrap, pointClamp,
				linearWrap, linearClamp,
				anisotropicWrap, anisotropicClamp };
	}

	std::shared_ptr<photon::Texture2D> DX12RHI::LoadTextureFromFile(const std::wstring& filepath, std::unique_ptr<uint8_t[]>& decodedData, D3D12_SUBRESOURCE_DATA& subresource, size_t maxsize /*= 0*/)
	{
		Texture2DDesc desc;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		DX_LogIfFailed(DirectX::LoadWICTextureFromFile(m_Device.Get(), filepath.c_str(), &resource, decodedData, subresource)); 
		
		D3D12_HEAP_PROPERTIES heapProp;
		D3D12_HEAP_FLAGS heapFlags;
		resource->GetHeapProperties(&heapProp, &heapFlags);
		desc = Texture2D::ToPhotonDesc(resource->GetDesc(), (ResourceHeapProperties)(heapProp.Type));
		std::shared_ptr<Texture2D> retTex = std::make_shared<Texture2D>(desc, resource);
		return retTex;
	}

	void DX12RHI::CopySubResourceDataCpuToGpu(Resource* dest, Resource* upload, UINT64 uploadOffsetInBytes, D3D12_SUBRESOURCE_DATA* resources, UINT resourcesStartIdx /*= 0*/, UINT resourcesNum /*= 1*/)
	{
		ResourceStateTransform(upload, D3D12_RESOURCE_STATE_GENERIC_READ);
		ResourceStateTransform(dest, D3D12_RESOURCE_STATE_COPY_DEST);
		UpdateSubresources(m_CurrCmdList, dest->gpuResource.Get(), upload->gpuResource.Get(), uploadOffsetInBytes, resourcesStartIdx, resourcesNum, resources);
	}

	void DX12RHI::CmdClearDepthStencil(DepthStencilView* view, D3D12_CLEAR_FLAGS ClearFlags, float depth, UINT8 stencil, UINT numRects, const D3D12_RECT* clearRect /*= nullptr*/)
	{
		ResourceStateTransform(view->resource, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_CurrCmdList->ClearDepthStencilView(view->cpuHandleInHeap, ClearFlags, depth, stencil, numRects, clearRect);
	}

	void DX12RHI::CmdClearRenderTarget(RenderTargetView* view, Vector4 clearRGBA, UINT numRects, const D3D12_RECT* clearRect /*= nullptr*/)
	{
		ResourceStateTransform(view->resource, D3D12_RESOURCE_STATE_RENDER_TARGET);
		float clearRGBAFloats[4] = { clearRGBA.x, clearRGBA.y, clearRGBA.z, clearRGBA.w };
		m_CurrCmdList->ClearRenderTargetView(view->cpuHandleInHeap, clearRGBAFloats, numRects, clearRect);
	}

	photon::ConstantBufferView* DX12RHI::CreateConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, ConstantBufferView* thisView /*= nullptr*/)
	{
		return m_CbvUavSrvHeap->CreateConstantBufferView(pDesc, thisView);
	}

	photon::ConstantBufferView* DX12RHI::CreateConstantBufferView()
	{
		return m_CbvUavSrvHeap->CreateConstantBufferView();
	}

	photon::ShaderResourceView* DX12RHI::CreateShaderResourceView(Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, ShaderResourceView* thisView /*= nullptr*/)
	{
		return m_CbvUavSrvHeap->CreateShaderResourceView(resource, pDesc, thisView);
	}

	photon::ShaderResourceView* DX12RHI::CreateShaderResourceView()
	{
		return m_CbvUavSrvHeap->CreateShaderResourceView();
	}

	photon::UnorderedAccessView* DX12RHI::CreateUnorderedAccessView(Resource* resource, Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, UnorderedAccessView* thisView /*= nullptr*/)
	{
		return m_CbvUavSrvHeap->CreateUnorderedAccessView(resource, counterResource, pDesc, thisView);
	}

	photon::UnorderedAccessView* DX12RHI::CreateUnorderedAccessView()
	{
		return m_CbvUavSrvHeap->CreateUnorderedAccessView();
	}

	photon::DepthStencilView* DX12RHI::CreateDepthStencilView(Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* pDesc, DepthStencilView* thisView /*= nullptr*/)
	{
		return m_DsvHeap->CreateDepthStencilView(resource, pDesc, thisView);
	}

	photon::DepthStencilView* DX12RHI::CreateDepthStencilView()
	{
		return m_DsvHeap->CreateDepthStencilView();
	}

	photon::RenderTargetView* DX12RHI::CreateRenderTargetView(Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, RenderTargetView* thisView /*= nullptr*/)
	{
		return m_RtvHeap->CreateRenderTargetView(resource, pDesc, thisView);
	}

	photon::RenderTargetView* DX12RHI::CreateRenderTargetView()
	{
		return m_RtvHeap->CreateRenderTargetView();
	}

	photon::SamplerView* DX12RHI::CreateSampler(const D3D12_SAMPLER_DESC* pDesc, SamplerView* thisView /*= nullptr*/)
	{
		return m_SamplerHeap->CreateSampler(pDesc, thisView);
	}

	photon::SamplerView* DX12RHI::CreateSampler()
	{
		return m_SamplerHeap->CreateSampler();
	}

	std::shared_ptr<photon::ConstantBuffer> DX12RHI::CreateConstantBuffer(unsigned int elementCount, unsigned int singleElementSizeInBytes)
	{
		unsigned int constantBufferStrideBytes = RenderUtil::GetConstantBufferByteSize(singleElementSizeInBytes);
		UINT64 constantBufferSizeInBytes = elementCount * constantBufferStrideBytes;
		
		BufferDesc defaultBufferDesc;
		defaultBufferDesc.bufferSizeInBytes = constantBufferSizeInBytes;
		defaultBufferDesc.cpuResource = nullptr;
		defaultBufferDesc.heapProp = ResourceHeapProperties::Default;
		auto buffer = m_ResourceManager->CreateBuffer(defaultBufferDesc);

		BufferDesc uploadBufferDesc;
		uploadBufferDesc.bufferSizeInBytes = constantBufferSizeInBytes;
		defaultBufferDesc.cpuResource = nullptr;
		defaultBufferDesc.heapProp = ResourceHeapProperties::Upload;
		auto uploadBuffer = m_ResourceManager->CreateBuffer(defaultBufferDesc);

		return std::make_shared<ConstantBuffer>(buffer, uploadBuffer, elementCount, singleElementSizeInBytes, constantBufferStrideBytes, constantBufferStrideBytes);
	}

	std::shared_ptr<photon::ResourceManager> DX12RHI::GetResourceManager()
	{
		return m_ResourceManager;
	}

	std::shared_ptr<photon::VertexBuffer> DX12RHI::CreateVertexBuffer(VertexType type, const void* data, UINT64 sizeInBytes)
	{

		BufferDesc defaultBufferDesc;
		defaultBufferDesc.bufferSizeInBytes = sizeInBytes;
		defaultBufferDesc.cpuResource = RenderUtil::CreateD3DBlob(data, sizeInBytes);
		defaultBufferDesc.heapProp = ResourceHeapProperties::Default;
		auto vertexBuffer = m_ResourceManager->CreateBuffer(defaultBufferDesc);
		ResourceStateTransform(vertexBuffer.get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		
		return std::make_shared<VertexBuffer>(vertexBuffer, type);
	}

	std::shared_ptr<photon::IndexBuffer> DX12RHI::CreateIndexBuffer(const void* data, UINT64 sizeInBytes)
	{
		BufferDesc defaultBufferDesc;
		defaultBufferDesc.bufferSizeInBytes = sizeInBytes;
		defaultBufferDesc.cpuResource = RenderUtil::CreateD3DBlob(data, sizeInBytes);
		defaultBufferDesc.heapProp = ResourceHeapProperties::Default;
		auto indexBuffer = m_ResourceManager->CreateBuffer(defaultBufferDesc);
		ResourceStateTransform(indexBuffer.get(), D3D12_RESOURCE_STATE_INDEX_BUFFER);
		return std::make_shared<IndexBuffer>(indexBuffer);
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> DX12RHI::CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc)
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> ret;
		DX_LogIfFailed(m_Device->CreateGraphicsPipelineState(desc, IID_PPV_ARGS(&ret)));
		return ret;
	}

	void DX12RHI::CopyDataGpuToGpu(Resource* dstResource, Resource* srcResource)
	{
		ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_COPY_DEST);
		ResourceStateTransform(srcResource, D3D12_RESOURCE_STATE_GENERIC_READ);
		m_CurrCmdList->CopyResource(dstResource->gpuResource.Get(), srcResource->gpuResource.Get());
	}

	void DX12RHI::CopyDataGpuToGpu(Resource* dstResource, Resource* srcResource, UINT64 dstStartPosInBytes, UINT64 srcStartPosInBytes, UINT64 sizeInBytes)
	{
		ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_COPY_DEST);
		ResourceStateTransform(srcResource, D3D12_RESOURCE_STATE_GENERIC_READ);
		m_CurrCmdList->CopyBufferRegion(dstResource->gpuResource.Get(), dstStartPosInBytes, srcResource->gpuResource.Get(), srcStartPosInBytes, sizeInBytes);
	}

	void DX12RHI::CopyDataCpuToGpu(Resource* dstResource, const void* data, UINT64 sizeInBytes)
	{
		auto dst = dstResource->gpuResource.Get();
		void* mappedData = nullptr;
		dst->Map(0, nullptr, &mappedData);
		CopyMemory(mappedData, data, sizeInBytes);
		dst->Unmap(0, nullptr);
	}


	void DX12RHI::CopyDataCpuToGpu(Resource* dstResource, UINT64 startPosInBytes, const void* data, UINT64 sizeInBytes)
	{
		auto dst = dstResource->gpuResource.Get();
		void* mappedData = nullptr;
		dst->Map(0, nullptr, &mappedData);
		CopyMemory((char*)mappedData + startPosInBytes, data, sizeInBytes);
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