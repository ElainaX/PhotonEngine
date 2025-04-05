#include "RenderSystem.h"
#include "DX12RHI/DX12RHI.h"
#include "ForwardRenderPipeline.h"
#include "Function/Util/RenderUtil.h"

namespace photon 
{
	using namespace DirectX;

	RenderSystem::~RenderSystem()
	{
		
	}

	void RenderSystem::Initialize(RenderSystemInitInfo initInfo)
	{
		LOG_INFO("RenderSystemInit");
		m_WindowSystem = initInfo.windowSystem;

		m_Rhi = std::make_shared<DX12RHI>();
		RHIInitInfo rhiInitInfo;
		rhiInitInfo.window_System = m_WindowSystem;
		m_Rhi->Initialize(rhiInitInfo);

		m_ResourceManager = m_Rhi->GetResourceManager();

		m_ShaderFactory = std::make_shared<ShaderFactory>();


		ForwardPipelineCreateInfo forwardCreateInfo;
		forwardCreateInfo.rhi = m_Rhi.get();
		forwardCreateInfo.windowSystem = m_WindowSystem.get();
		m_RenderPipelines[RenderPipelineType::ForwardPipeline] = 
			std::make_shared<ForwardRenderPipeline>();
		m_RenderPipelines[RenderPipelineType::ForwardPipeline]->Initialize(&forwardCreateInfo);
		m_CurrRenderPipeline = m_RenderPipelines[RenderPipelineType::ForwardPipeline].get();

		//m_ResourceData = std::make_shared<RenderResourceData>();
		Texture2DDesc texDesc;
		Vector2i swapchainWidthAndHeight = m_WindowSystem->GetClientWidthAndHeight();
		texDesc.width = swapchainWidthAndHeight.x;
		texDesc.height = swapchainWidthAndHeight.y;
		texDesc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.heapProp = ResourceHeapProperties::Default;
		texDesc.flag = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		texDesc.clearValue = { 0.1f, 0.1f, 0.1f, 1.0f };
		m_RenderTarget = m_ResourceManager->CreateTexture2D(texDesc);

		Texture2DDesc dsvtex;
		dsvtex.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvtex.flag = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		dsvtex.heapProp = ResourceHeapProperties::Default;
		dsvtex.width = swapchainWidthAndHeight.x;
		dsvtex.height = swapchainWidthAndHeight.y;
		dsvtex.clearValue = { 1.0f, 0.0f, 0.0f, 0.0f };
		m_DepthStencil = m_ResourceManager->CreateTexture2D(dsvtex);
		m_Rhi->ResourceStateTransform(m_DepthStencil.get(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

		auto camera = std::make_shared<RenderCamera>(swapchainWidthAndHeight.x / (float)swapchainWidthAndHeight.y);
		m_RenderScene.push_back(std::make_shared<RenderScene>(camera));
		m_RenderScene[0]->AddDirectionalLight(Vector3{ 0.7f, 0.2f, 0.2f }, Vector3{ 1.0f, -1.0f, 0.4f });
		m_RenderScene[0]->AddPointLight(Vector3{ 1.0f, 1.0f, 1.0f }, Vector3{ 1.0f, 2.0f, 3.0f }, 3.0f, 8.0f);
		m_RenderScene[0]->AddSpotLight(Vector3{ 1.0f, 1.0f, 0.8f }, Vector3{ 0.0f, 3.0f, 0.0f }, Vector3{0.0f, 0.0f, 0.0f}, 1.0f, 8.0f, 32.0f);
		//VertexSimple vertices[] =
		//{
		//	VertexSimple{Vector3{-0.5f, -0.5f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 1.0f}},
		//	VertexSimple{Vector3{0.0f, 0.5f, 0.0f}  , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.5f, 0.0f}},
		//	VertexSimple{Vector3{0.5f, -0.5f, 0.0f} , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{1.0f, 1.0f}},
		//};
		//uint32_t indices[] = { 0, 1, 2 };

		auto sphereMeshData = m_GeometryGenerator->CreateSphere(0.5f, 40, 40);
		auto sphereIndices = sphereMeshData.Indices32;
		std::vector<VertexSimple> sphereVertices(sphereMeshData.Vertices.size());
		for (int i = 0; i < sphereMeshData.Vertices.size(); ++i)
		{
			auto vert = sphereMeshData.Vertices[i];
			sphereVertices[i].position = vert.Position;
			sphereVertices[i].normal = vert.Normal;
			sphereVertices[i].tangent = vert.TangentU;
			sphereVertices[i].texCoord = vert.TexC;
		}

		auto gridMeshData = m_GeometryGenerator->CreateGrid(5.0f, 5.0f, 10, 10);
		auto gridIndices = gridMeshData.Indices32;
		std::vector<VertexSimple> gridVertices(gridMeshData.Vertices.size());
		for (int i = 0; i < gridMeshData.Vertices.size(); ++i)
		{
			auto vert = gridMeshData.Vertices[i];
			gridVertices[i].position = vert.Position;
			gridVertices[i].normal = vert.Normal;
			gridVertices[i].tangent = vert.TangentU;
			gridVertices[i].texCoord = vert.TexC;
		}

		MeshDesc sphereDesc;
		sphereDesc.name = L"Standard Sphere";
		sphereDesc.type = VertexType::VertexSimple;
		sphereDesc.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		sphereDesc.vertexRawData = RenderUtil::CreateD3DBlob((void*)sphereVertices.data(), sphereVertices.size() * sizeof(VertexSimple));
		sphereDesc.indexRawData = RenderUtil::CreateD3DBlob((void*)sphereIndices.data(), sphereIndices.size() * sizeof(uint32_t));
		auto sphereMesh = m_ResourceManager->CreateMesh(sphereDesc);

		MeshDesc gridDesc;
		gridDesc.name = L"Standard Grid";
		gridDesc.type = VertexType::VertexSimple;
		gridDesc.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gridDesc.vertexRawData = RenderUtil::CreateD3DBlob((void*)gridVertices.data(), gridVertices.size() * sizeof(VertexSimple));
		gridDesc.indexRawData = RenderUtil::CreateD3DBlob((void*)gridIndices.data(), gridIndices.size() * sizeof(uint32_t));
		auto gridMesh = m_ResourceManager->CreateMesh(gridDesc);


		auto diffuseMap = m_ResourceManager->LoadTexture2D(g_AssetTextureFolder / L"rustediron2_basecolor.png", true);
		auto roughnessMap = m_ResourceManager->LoadTexture2D(g_AssetTextureFolder / L"rustediron2_roughness.png");
		auto normalMap = m_ResourceManager->LoadTexture2D(g_AssetTextureFolder / L"rustediron2_normal.png");


		StaticModelMaterialDataConstants matData;
		matData.diffuseAlbedo = { 1.0f, 0.5f, 0.5f, 1.0f };
		matData.fresnelR0 = {0.03f, 0.03f, 0.03f};
		matData.roughness = 0.5f;
		matData.bInverseRoughness = false;
		auto sphereMat = m_ResourceManager->CreateMaterial(matData, diffuseMap->guid, normalMap->guid, roughnessMap->guid,  L"Simple PBR Mat");
	
		

		
		StaticFrameResourceEditor standardEditor;

		auto testShader = m_ShaderFactory->Create(L"TestShader");

		m_RenderScene[0]->AddCommonRenderItem(sphereMesh, sphereMat.get(), testShader, RenderLayer::Opaque, standardEditor);

		standardEditor.translation = { 0.0f, -1.0f, 0.0f };
		m_RenderScene[0]->AddCommonRenderItem(gridMesh, sphereMat.get(), testShader, RenderLayer::Opaque, standardEditor);

		auto model = m_ResourceManager->LoadModel(g_AssetModelFolder / L"FarmerHouse/FarmerHouse.fbx");
		m_RenderScene[0]->AddModel(model, testShader, RenderLayer::Opaque, StaticFrameResourceEditor{});

		StaticModelFrameResourceDesc resourceDesc;
		resourceDesc.allObjectNum = StaticModelObjectConstants::s_CurrObjectIndex + 100;
		resourceDesc.allPassNum = StaticModelPassConstants::s_CurrPassIndex + 100;
		resourceDesc.allMatDatasNum = StaticModelMaterialDataConstants::s_CurrMatDataIndex;
		m_Rhi->CreateFrameResource(FrameResourceType::StaticModelFrameResource, &resourceDesc);

	}

	void RenderSystem::InitializeEditorUI(WindowUI* windowUI)
	{
		m_Rhi->InitializeImGui();
		m_CurrRenderPipeline->SetCurrEditorUI(windowUI);
	}

	void RenderSystem::Tick(GameTimer& gt)
	{
		if(m_WindowSystem->ShouldClose())
		{
			m_Rhi->Clear();
			return;
		}

		if (m_WindowSystem->GetClientWidthAndHeight() != Vector2i(m_RenderTarget->dxDesc.Width, m_RenderTarget->dxDesc.Height))
		{
			ReCreateRenderTargetTexAndDepthStencilTex(m_WindowSystem->GetClientWidthAndHeight());
		}

		// Bind Global Info
		m_Rhi->CmdSetDescriptorHeaps();
		RenderScene* currRenderScene = m_RenderScene[0].get();
		ForwardPipelineRenderResourceData forwardPipelineData;

		auto ritems = currRenderScene->GetCommonRenderItems(m_Rhi.get(), true);
		forwardPipelineData.allRenderItems.resize(ritems.size());
		for(int i = 0; i< ritems.size(); ++i)
		{
			//UpdateFrameResource()
			auto ritem = dynamic_cast<CommonRenderItem*>(ritems[i]);
			if(ritem->numFrameDirty > 0)
			{
				auto frameResource = (CommonRenderItem::TFrameResource*)m_Rhi->GetCurrFrameResource(CommonRenderItem::s_FrameResourceType);
				frameResource->UpdateObjectConstantBuffer(ritem->frameResourceInfo.objConstantIdx, &ritem->frameResourceInfo.objectConstants);
				frameResource->UpdateMatDataConstantBuffer(ritem->material->matCBufferIdx, &ritem->material->matCBufferData);
				ritem->numFrameDirty--;
			}
			forwardPipelineData.allRenderItems[i] = ritem;
		}

		auto& dirLights = m_RenderScene[0]->directionalLights;
		auto& pointLights = m_RenderScene[0]->pointLights;
		auto& spotLights = m_RenderScene[0]->spotLights;
		forwardPipelineData.directionalLights.resize(dirLights.size());
		forwardPipelineData.pointLights.resize(pointLights.size());
		forwardPipelineData.spotLights.resize(spotLights.size());
		for(int i = 0; i < dirLights.size(); ++i)
		{
			forwardPipelineData.directionalLights[i] = &dirLights[i].data;
		}
		for (int i = 0; i < pointLights.size(); ++i)
		{
			forwardPipelineData.pointLights[i] = &pointLights[i].data;
		}
		for (int i = 0; i < spotLights.size(); ++i)
		{
			forwardPipelineData.spotLights[i] = &spotLights[i].data;
		}
		
		forwardPipelineData.depthStencil = m_DepthStencil;
		forwardPipelineData.renderTarget = m_RenderTarget;
		forwardPipelineData.gameTimer = &gt;
		forwardPipelineData.mainCamera = currRenderScene->GetMainCamera();
		
		
		
		m_CurrRenderPipeline->PrepareContext(&forwardPipelineData);




		m_CurrRenderPipeline->Render();
	}

	void RenderSystem::ReCreateRenderTargetTexAndDepthStencilTex(Vector2i size)
	{
		m_Rhi->FlushCommandQueue();

		Texture2DDesc texDesc;
		texDesc.width = size.x;
		texDesc.height = size.y;
		texDesc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.heapProp = ResourceHeapProperties::Default;
		texDesc.flag = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		texDesc.clearValue = { 0.1f, 0.1f, 0.1f, 1.0f };
		m_RenderTarget = m_ResourceManager->CreateTexture2D(texDesc);

		Texture2DDesc dsvtex;
		dsvtex.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvtex.flag = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		dsvtex.heapProp = ResourceHeapProperties::Default;
		dsvtex.width = size.x;
		dsvtex.height = size.y;
		dsvtex.clearValue = { 1.0f, 0.0f, 0.0f, 0.0f };
		m_DepthStencil = m_ResourceManager->CreateTexture2D(dsvtex);
		m_Rhi->ResourceStateTransform(m_DepthStencil.get(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}

	std::shared_ptr<photon::RHI> RenderSystem::GetRHI()
	{
		return m_Rhi;
	}

	photon::ShaderResourceView* RenderSystem::GetFinalOutputShaderResourceView()
	{
		m_RenderTargetSRV = m_Rhi->CreateShaderResourceView(m_RenderTarget.get(), nullptr, m_RenderTargetSRV);
		return m_RenderTargetSRV;
	}

	void RenderSystem::SetRenderPipelineType(RenderPipelineType renderType)
	{

	}

	photon::RenderCamera* RenderSystem::GetRenderCamera()
	{
		return m_RenderScene[0]->GetMainCamera();
	}

	photon::ResourceManager* RenderSystem::GetResourceManager()
	{
		return m_ResourceManager.get();
	}

	photon::GeometryGenerator* RenderSystem::GetGeometryGenerator()
	{
		return m_GeometryGenerator.get();
	}

	photon::RenderScene* RenderSystem::GetRenderScene()
	{
		return m_RenderScene[0].get();
	}

	photon::ShaderFactory* RenderSystem::GetShaderFactory()
	{
		return m_ShaderFactory.get();
	}

}