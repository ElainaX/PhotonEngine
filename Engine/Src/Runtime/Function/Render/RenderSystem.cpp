#include "RenderSystem.h"
#include "DX12RHI/DX12RHI.h"
#include "ForwardRenderPipeline.h"
#include "Function/Util/RenderUtil.h"
#include "Shader/ShaderFactory.h"

namespace photon
{
	using namespace DirectX;

	RenderSystem::~RenderSystem() = default;

	void RenderSystem::Initialize(RenderSystemInitInfo initInfo)
	{
		LOG_INFO("RenderSystemInit");
		m_WindowSystem = initInfo.windowSystem;

		m_rhi = std::make_shared<DX12RHI>();
		m_frameSyncSystem = std::make_shared<FrameSyncSystem>();

		RHIInitInfo rhiInitInfo = { .windowSystem = m_WindowSystem, .frameSyncSystem = m_frameSyncSystem.get() };
		m_rhi->Initialize(rhiInitInfo);
		m_frameSyncSystem->Initialize(m_rhi.get());

		m_descriptorSystem = std::make_shared<DescriptorSystem>();
		m_descriptorSystem->Initialize(m_rhi->GetDevice(), m_rhi.get());

		m_gpuResMgr = std::make_shared<GpuResourceManager>();
		m_gpuResMgr->Initialize(m_rhi.get(), m_descriptorSystem.get());

		m_frameAllocSystem = std::make_shared<FrameAllocatorSystem>();
		m_frameAllocSystem->Initialize(m_descriptorSystem.get(), m_gpuResMgr.get(), m_rhi.get());

		m_CmdCtxMgr = std::make_shared<CommandContextManager>();
		m_CmdCtxMgr->Initialize(m_rhi.get());

		auto& initCtx = m_CmdCtxMgr->BeginInitContext();
		m_frameAllocSystem->BeginBootstrapFrame();

		m_resourceManager = std::make_shared<ResourceManager>();
		m_resourceManager->Initialize(
			m_rhi.get(),
			m_gpuResMgr.get(),
			m_descriptorSystem.get(),
			m_frameAllocSystem.get(),
			m_CmdCtxMgr.get());

		ForwardPipelineCreateInfo forwardCreateInfo;
		forwardCreateInfo.services.rhi = m_rhi.get();
		forwardCreateInfo.services.windowSystem = m_WindowSystem;
		forwardCreateInfo.services.resourceManager = m_resourceManager.get();
		forwardCreateInfo.services.gpuResManager = m_gpuResMgr.get();
		forwardCreateInfo.services.descriptorSystem = m_descriptorSystem.get();
		forwardCreateInfo.services.frameAllocator = m_frameAllocSystem.get();
		forwardCreateInfo.services.cmdCtxMgr = m_CmdCtxMgr.get();
		forwardCreateInfo.services.pipelineCache = m_resourceManager->GetPipelineStateCache();
		forwardCreateInfo.services.rootSignatureCache = m_resourceManager->GetRootSignatureCache();

		m_RenderPipelines[RenderPipelineType::ForwardPipeline] =
			std::make_shared<ForwardRenderPipeline>();
		m_RenderPipelines[RenderPipelineType::ForwardPipeline]->Initialize(forwardCreateInfo);
		m_CurrRenderPipeline = m_RenderPipelines[RenderPipelineType::ForwardPipeline].get();

		Vector2i size = m_WindowSystem->GetClientWidthAndHeight();
		CreateOrResizeMainViewportTargets(size);

		auto camera = std::make_shared<RenderCamera>(size.x / static_cast<float>(size.y));
		m_RenderScene.push_back(std::make_shared<RenderScene>(camera));
		m_RenderScene[0]->AddDirectionalLight(
			Vector3{ 1.0f, 1.0f, 1.0f },
			Vector3{ 20.0f, -50.0f, 30.0f });

		// 这里用新版 ResourceManager + handle
		TextureHandle baseColor = m_resourceManager->LoadTexture(g_AssetTextureFolder / "rustediron2_basecolor.png");
		TextureHandle roughness = m_resourceManager->LoadTexture(g_AssetTextureFolder / "rustediron2_roughness.png");
		TextureHandle normal = m_resourceManager->LoadTexture(g_AssetTextureFolder / "rustediron2_normal.png");

		ShaderProgramLoadDesc pbrDesc = ShaderFactory::BuildPbrShaderDesc();
		pbrDesc.entryPoints[ToIndex(ShaderStage::VS)] = "VSMain";
		pbrDesc.entryPoints[ToIndex(ShaderStage::PS)] = "PSMain";

		ShaderHandle pbrShader =
			m_resourceManager->LoadShaderProgram(
				g_AssetShaderFolder / "Pbr.hlsl",
				pbrDesc);
		MaterialHandle pbrMat = m_resourceManager->CreatePBRMaterial(pbrShader, "SimplePBR");
		m_resourceManager->SetMaterialTextureBinding(pbrMat, "baseColorMap", baseColor);
		m_resourceManager->SetMaterialTextureBinding(pbrMat, "normalMap", normal);
		m_resourceManager->SetMaterialTextureBinding(pbrMat, "roughnessMap", roughness);

		auto houseModel = m_resourceManager->LoadModel(g_AssetModelFolder / "SingleHouse/HouseSuburban.obj");
		RenderItem* houseItem = m_RenderScene[0]->CreateRenderItem();
		houseItem->mesh = m_resourceManager->GetMeshHandleByGuid(houseModel->meshGuid);
		houseItem->layers = RenderLayer::Opaque | RenderLayer::ShadowCaster;
		houseItem->flags.castShadow = true;
		houseItem->overrideMaterials.push_back(pbrMat);
		houseItem->overrideMaterials.push_back(pbrMat);
		houseItem->overrideMaterials.push_back(pbrMat);
		m_resourceManager->SetMeshShader(houseItem->mesh, pbrShader);
		{
			XMMATRIX world = XMMatrixScaling(0.001f, 0.001f, 0.001f);
			XMStoreFloat4x4(&houseItem->objConstant.world, world);
		}

		auto floorModel = m_resourceManager->LoadModel(g_AssetModelFolder / "Floor/floor.obj");
		RenderItem* floorItem = m_RenderScene[0]->CreateRenderItem();
		floorItem->mesh = m_resourceManager->GetMeshHandleByGuid(floorModel->meshGuid);
		floorItem->layers = RenderLayer::Opaque | RenderLayer::ShadowCaster;
		floorItem->flags.castShadow = true;
		floorItem->overrideMaterials.push_back(pbrMat);
		floorItem->overrideMaterials.push_back(pbrMat);
		floorItem->overrideMaterials.push_back(pbrMat);
		m_resourceManager->SetMeshShader(floorItem->mesh, pbrShader);
		{
			XMMATRIX world =
				XMMatrixScaling(1.4f, 1.4f, 1.4f) *
				XMMatrixTranslation(0.0f, -1.9f, 0.0f);
			XMStoreFloat4x4(&floorItem->objConstant.world, world);
		}

		CreateOrRefreshBackBufferRtvs();

		m_frameAllocSystem->EndBootsstrapFrame();
		m_CmdCtxMgr->SubmitInitContextAndWait(initCtx);
	}

	void RenderSystem::InitializeEditorUI(WindowUI* windowUI)
	{
		if (!m_imguiSystem)
		{
			m_imguiSystem = std::make_shared<ImGuiSystem>();
			m_imguiSystem->Initialize(m_rhi.get(), m_WindowSystem);
		}

		for (auto& [type, pipeline] : m_RenderPipelines)
		{
			if (pipeline)
			{
				pipeline->SetImGuiSystem(m_imguiSystem.get());
				pipeline->SetCurrEditorUI(windowUI);
			}
		}
	}

	void RenderSystem::CreateOrRefreshBackBufferRtvs()
	{
		for (auto& h : m_backbufferRtvs)
		{
			if (h.IsValid())
			{
				m_descriptorSystem->FreeDescriptor(h);
				h = {};
			}
		}

		for (uint32_t i = 0; i < FrameSyncSystem::kMaxFramesInFlight; ++i)
		{
			DXTexture2D* backbuffer = m_rhi->GetBackbuffer(i);
			if (!backbuffer)
				continue;

			ViewDesc rtvDesc = {};
			rtvDesc.type = ViewType::RTV;
			rtvDesc.dimension = ViewDimension::Texture2D;
			rtvDesc.format = backbuffer->dxDesc.Format;

			m_backbufferRtvs[i] =
				m_descriptorSystem->CreateDescriptor(backbuffer, rtvDesc);
		}
	}

	void RenderSystem::CreateOrResizeMainViewportTargets(Vector2i size)
	{
		if (size.x <= 0 || size.y <= 0)
			return;

		if (m_mainViewportSize == size &&
			m_sceneColor.handle.IsValid() &&
			m_sceneDepth.handle.IsValid())
		{
			return;
		}

		if (m_sceneColor.handle.IsValid())
		{
			m_resourceManager->DestroyTexture(m_sceneColor, false);
			m_sceneColor = {};
		}
		if (m_sceneDepth.handle.IsValid())
		{
			m_resourceManager->DestroyTexture(m_sceneDepth, false);
			m_sceneDepth = {};
		}

		m_mainViewportSize = size;

		DXTexture2DDesc colorDesc = {};
		colorDesc.width = size.x;
		colorDesc.height = size.y;
		colorDesc.maxMipLevels = 1;
		colorDesc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		colorDesc.heapProp = HeapProp::Default;
		colorDesc.flag = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		colorDesc.hasClearValue = true;
		colorDesc.clearValue = { 0.1f, 0.1f, 0.1f, 1.0f };

		m_sceneColor = m_resourceManager->CreateRuntimeTexture2D(
			colorDesc, TextureDimension::Tex2D, "SceneColor");

		DXTexture2DDesc depthDesc = {};
		depthDesc.width = size.x;
		depthDesc.height = size.y;
		depthDesc.maxMipLevels = 1;
		depthDesc.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.heapProp = HeapProp::Default;
		depthDesc.flag = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		depthDesc.hasClearValue = true;
		depthDesc.depthStencil = { 1.0f, 0.0f };

		m_sceneDepth = m_resourceManager->CreateRuntimeTexture2D(
			depthDesc, TextureDimension::Tex2D, "SceneDepth");
	}

	void RenderSystem::BuildEGFrameContext(
		EG_FrameContext& frameCtx,
		GameTimer* timer,
		DX12CommandContext* graphicsCmd)
	{
		RenderScene* currScene = m_RenderScene[0].get();
		RenderCamera* mainCamera = currScene->GetMainCamera();

		auto viewMat = mainCamera->GetViewMatrix();
		auto projMat = mainCamera->GetProjMatrix();
		auto viewProj = XMMatrixMultiply(viewMat, projMat);

		auto viewDet = XMMatrixDeterminant(viewMat);
		auto projDet = XMMatrixDeterminant(projMat);
		auto viewProjDet = XMMatrixDeterminant(viewProj);

		auto invViewMat = XMMatrixInverse(&viewDet, viewMat);
		auto invProjMat = XMMatrixInverse(&projDet, projMat);
		auto invViewProjMat = XMMatrixInverse(&viewProjDet, viewProj);

		frameCtx.frameIndex = m_frameSyncSystem->GetCurrentFrameIndex();
		frameCtx.renderScene = currScene;

		frameCtx.uniforms.timer = timer;
		frameCtx.uniforms.mainCamera = mainCamera;

		XMStoreFloat4x4(&frameCtx.uniforms.view, viewMat);
		XMStoreFloat4x4(&frameCtx.uniforms.proj, projMat);
		XMStoreFloat4x4(&frameCtx.uniforms.viewProj, viewProj);
		XMStoreFloat4x4(&frameCtx.uniforms.invView, invViewMat);
		XMStoreFloat4x4(&frameCtx.uniforms.invProj, invProjMat);
		XMStoreFloat4x4(&frameCtx.uniforms.invViewProj, invViewProjMat);

		frameCtx.uniforms.camPosWS = mainCamera->pos;
		frameCtx.uniforms.znear = mainCamera->znear;
		frameCtx.uniforms.zfar = mainCamera->zfar;
		frameCtx.uniforms.viewportSize = m_mainViewportSize;
		frameCtx.uniforms.invViewportSize = {
			1.0f / static_cast<float>(m_mainViewportSize.x),
			1.0f / static_cast<float>(m_mainViewportSize.y)
		};

		frameCtx.uniforms.dirLights = currScene->directionalLights;
		frameCtx.uniforms.pointLights = currScene->pointLights;
		frameCtx.uniforms.spotLights = currScene->spotLights;

		if (!frameCtx.uniforms.dirLights.empty())
			frameCtx.uniforms.mainDirLight = &frameCtx.uniforms.dirLights[0];
		if (!frameCtx.uniforms.pointLights.empty())
			frameCtx.uniforms.mainPointLight = &frameCtx.uniforms.pointLights[0];

		frameCtx.services.rhi = m_rhi.get();
		frameCtx.services.resourceManager = m_resourceManager.get();
		frameCtx.services.gpuResManager = m_gpuResMgr.get();
		frameCtx.services.descriptorSystem = m_descriptorSystem.get();
		frameCtx.services.frameAllocator = m_frameAllocSystem.get();
		frameCtx.services.cmdCtxMgr = m_CmdCtxMgr.get();
		frameCtx.services.pipelineCache = m_resourceManager->GetPipelineStateCache();
		frameCtx.services.rootSignatureCache = m_resourceManager->GetRootSignatureCache();
		frameCtx.services.graphicsCmd = graphicsCmd;

		frameCtx.targets.sceneColor = m_sceneColor;
		frameCtx.targets.sceneDepth = m_sceneDepth;
		frameCtx.skybox = currScene->GetCubemap();

		const auto allItems = currScene->GatherRenderItems();
		frameCtx.renderlists.all = allItems;

		for (const RenderItem* item : allItems)
		{
			if (!item || !item->IsVisible())
				continue;

			const bool transparent =
				item->IsTransparent() || HasLayer(item->layers, RenderLayer::Transparent);

			if (transparent)
				frameCtx.renderlists.transparent.push_back(item);
			else
				frameCtx.renderlists.opaque.push_back(item);

			if (item->flags.castShadow || HasLayer(item->layers, RenderLayer::ShadowCaster))
				frameCtx.renderlists.shadowCasters.push_back(item);
		}

		PrepareFrameObjectBindings(frameCtx);
	}

	void RenderSystem::PrepareFrameObjectBindings(EG_FrameContext& frameCtx)
	{
		auto* cmd = frameCtx.services.graphicsCmd;
		auto* gpuResMgr = frameCtx.services.gpuResManager;
		auto* descriptorSystem = frameCtx.services.descriptorSystem;
		auto* frameAllocator = frameCtx.services.frameAllocator;

		if (!cmd || !gpuResMgr || !descriptorSystem || !frameAllocator)
			return;

		const auto items = frameCtx.renderScene->GatherMutableRenderItems();
		for (RenderItem* item : items)
		{
			if (!item || !item->IsVisible())
				continue;

			FrameResourceRange upload =
				frameAllocator->AllocateFrameUploadBuffer256<ObjectData>(1);

			DXResource* uploadRes = gpuResMgr->GetResource(upload.buffer);
			if (!uploadRes)
				continue;

			cmd->CopyBufferCpuToUpload(
				uploadRes,
				upload.range.cbvOffsetInBytes,
				&item->objConstant,
				sizeof(ObjectData));

			FrameDescriptorHandle frameCbv =
				frameAllocator->AllocateFrameCbvDescriptor();

			ViewDesc cbvDesc = {};
			cbvDesc.type = ViewType::CBV;
			cbvDesc.dimension = ViewDimension::Buffer;
			cbvDesc.cbvOffsetInBytes = upload.range.cbvOffsetInBytes;
			cbvDesc.cbvSizeInBytes = upload.range.cbvSizeInBytes;

			descriptorSystem->CreateDescriptorAtCpuHandle(
				uploadRes,
				cbvDesc,
				frameAllocator->GetCpuHandle(frameCbv));

			item->objCbvHandle = frameCbv;
		}
	}

	void RenderSystem::ExecutePresentPass(EG_FrameContext& frameCtx, DX12CommandContext& gfxCtx)
	{
		const TextureRenderResource* colorRR =
			m_resourceManager->GetTextureRenderResource(frameCtx.targets.sceneColor);
		if (!colorRR)
			return;

		DXResource* sceneColor = m_gpuResMgr->GetResource(colorRR->texture);
		DXTexture2D* backbuffer = m_rhi->GetCurrBackbuffer();
		if (!sceneColor || !backbuffer)
			return;

		gfxCtx.ResourceStateTransform(sceneColor, D3D12_RESOURCE_STATE_COPY_SOURCE);
		gfxCtx.ResourceStateTransform(backbuffer, D3D12_RESOURCE_STATE_COPY_DEST);
		gfxCtx.CopyGpuResource(backbuffer, sceneColor);
		gfxCtx.ResourceStateTransform(backbuffer, D3D12_RESOURCE_STATE_PRESENT);
	}

	void RenderSystem::Tick(GameTimer& gt)
	{
		if (m_WindowSystem->ShouldClose())
		{
			m_rhi->Shutdown();
			return;
		}

		Vector2i size = m_WindowSystem->GetClientWidthAndHeight();
		if (size != m_mainViewportSize)
		{
			m_rhi->ResizeSwapChain(size.x, size.y);
			CreateOrResizeMainViewportTargets(size);
			CreateOrRefreshBackBufferRtvs();
		}

		m_frameSyncSystem->BeginFrame();
		uint32_t frameIndex = m_frameSyncSystem->GetCurrentFrameIndex();

		m_CmdCtxMgr->BeginFrame(frameIndex);
		m_frameAllocSystem->BeginFrame(frameIndex);

		auto& gfxCtx = m_CmdCtxMgr->BeginGraphicsContext(frameIndex);

		EG_FrameContext frameCtx = {};
		BuildEGFrameContext(frameCtx, &gt, &gfxCtx);

		m_CurrRenderPipeline->Prepare(frameCtx);

		// 任何可能导致 frame descriptor heap identity 改变的操作，只能发生在 Execute 之前。
		// 所有 subpass / pass 在 Prepare 阶段把 descriptor 都准备好
		m_frameAllocSystem->FinalizeFrameDescriptors();

		gfxCtx.SetDescriptorHeaps({ m_frameAllocSystem->GetCurrentCbvSrvUavHeap() });

		m_CurrRenderPipeline->Execute(frameCtx);

		if (!IsEditorMode())
			ExecutePresentPass(frameCtx, gfxCtx);
		else
			ExecuteEditorUIPass(frameCtx, gfxCtx);

		uint64_t fenceVal = m_CmdCtxMgr->SubmitGraphicsContext(gfxCtx);
		m_frameSyncSystem->EndFrame(fenceVal);

		m_gpuResMgr->ProcessDeferredRelease();
		m_rhi->Present();
	}

	bool RenderSystem::IsEditorMode() const
	{
		return m_imguiSystem != nullptr;
	}

	void RenderSystem::ExecuteEditorUIPass(EG_FrameContext& frameCtx, DX12CommandContext& gfxCtx)
	{
		if (!m_imguiSystem)
			return;

		const uint32_t backbufferIndex = m_rhi->GetCurrentBackBufferIndex();
		if (backbufferIndex >= m_backbufferRtvs.size())
			return;

		DXTexture2D* backbuffer = m_rhi->GetCurrBackbuffer();
		if (!backbuffer)
			return;

		DescriptorHandle rtvHandle = m_backbufferRtvs[backbufferIndex];
		if (!rtvHandle.IsValid())
			return;

		D3D12_CPU_DESCRIPTOR_HANDLE rtv =
			m_descriptorSystem->GetCpuHandle(rtvHandle);

		gfxCtx.ResourceStateTransform(backbuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);

		D3D12_RECT scissorRect = {
			0, 0,
			frameCtx.uniforms.viewportSize.x,
			frameCtx.uniforms.viewportSize.y
		};

		D3D12_VIEWPORT viewport = {
			0.0f,
			0.0f,
			static_cast<float>(frameCtx.uniforms.viewportSize.x),
			static_cast<float>(frameCtx.uniforms.viewportSize.y),
			0.0f,
			1.0f
		};

		gfxCtx.SetViewportsAndScissorRects(scissorRect, viewport);
		gfxCtx.SetRenderTargets(1, &rtv, true, nullptr);

		m_imguiSystem->Render(gfxCtx);

		gfxCtx.ResourceStateTransform(backbuffer, D3D12_RESOURCE_STATE_PRESENT);
	}

	void RenderSystem::Stop()
	{
		m_bStopRenderContent = true;
		m_CurrRenderPipeline->Stop();
	}

	void RenderSystem::ReStart()
	{
		m_bStopRenderContent = false;
		m_CurrRenderPipeline->ReStart();
	}

	void RenderSystem::SetRenderPipelineType(RenderPipelineType renderType)
	{
		m_CurrPipelineType = renderType;
		auto it = m_RenderPipelines.find(renderType);
		m_CurrRenderPipeline = (it != m_RenderPipelines.end()) ? it->second.get() : nullptr;
	}

	RenderCamera* RenderSystem::GetRenderCamera()
	{
		return m_RenderScene.empty() ? nullptr : m_RenderScene[0]->GetMainCamera();
	}

	ResourceManager* RenderSystem::GetResourceManager()
	{
		return m_resourceManager.get();
	}

	RenderScene* RenderSystem::GetRenderScene()
	{
		return m_RenderScene.empty() ? nullptr : m_RenderScene[0].get();
	}
}
