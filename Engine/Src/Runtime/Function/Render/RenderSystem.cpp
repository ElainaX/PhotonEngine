#include "RenderSystem.h"
#include "DX12RHI/DX12RHI.h"
#include "ForwardRenderPipeline.h"
#include "Function/Util/RenderUtil.h"

namespace photon 
{

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

		//m_Rhi->BeginSingleRenderPass();
		m_TestShader = std::make_shared<TestShader>(L"E:/Code/PhotonEngine/Engine/Src/Runtime/Function/Render/Shaders/TestShader.hlsl");

		ForwardPipelineCreateInfo forwardCreateInfo;
		forwardCreateInfo.rhi = m_Rhi.get();
		forwardCreateInfo.windowSystem = m_WindowSystem.get();
		m_RenderPipelines[RenderPipelineType::ForwardPipeline] = 
			std::make_shared<ForwardRenderPipeline>();
		m_RenderPipelines[RenderPipelineType::ForwardPipeline]->Initialize(&forwardCreateInfo);

		//m_ResourceData = std::make_shared<RenderResourceData>();
		Texture2DDesc texDesc;
		Vector2i swapchainWidthAndHeight = m_WindowSystem->GetClientWidthAndHeight();
		texDesc.width = swapchainWidthAndHeight.x;
		texDesc.height = swapchainWidthAndHeight.y;
		texDesc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.heapProp = ResourceHeapProperties::Default;
		texDesc.flag = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
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

		VertexSimple vertices[] =
		{
			VertexSimple{Vector3{-0.5f, -0.5f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
			VertexSimple{Vector3{0.0f, 0.5f, 0.0f}  , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
			VertexSimple{Vector3{0.5f, -0.5f, 0.0f} , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		};

		//VertexSimple vertices2[] =
		//{
		//	VertexSimple{Vector3{-0.0f, -0.5f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		//	VertexSimple{Vector3{0.5f, 0.5f, 0.0f}  , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		//	VertexSimple{Vector3{1.0f, -0.5f, 0.0f} , Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
		//};

		uint32_t indices[] = { 0, 1, 2 };


		MeshDesc triMeshDesc;
		triMeshDesc.name = "Triangle";
		triMeshDesc.type = VertexType::VertexSimple;
		triMeshDesc.vertexRawData = RenderUtil::CreateD3DBlob(vertices, sizeof(VertexSimple) * 3);
		triMeshDesc.indexRawData = RenderUtil::CreateD3DBlob(indices, sizeof(uint32_t) * 3);
		std::shared_ptr<Mesh> triMesh = m_ResourceManager->CreateMesh(triMeshDesc);

		//MeshDesc triMeshDesc2;
		//triMeshDesc2.name = "Triangle2";
		//triMeshDesc2.type = VertexType::VertexSimple;
		//triMeshDesc2.vertexRawData = RenderUtil::CreateD3DBlob(vertices2, sizeof(VertexSimple) * 3);
		//triMeshDesc2.indexRawData = RenderUtil::CreateD3DBlob(indices, sizeof(uint32_t) * 3);
		//std::shared_ptr<Mesh> triMesh2 = m_ResourceManager->CreateMesh(triMeshDesc2);

		m_RenderMeshCollection = std::make_shared<RenderMeshCollection>();
		m_RenderMeshCollection->PushMesh(triMesh);
		//m_RenderMeshCollection->PushMesh(triMesh2);
		m_RenderMeshCollection->EndPush(m_Rhi.get());

		m_CommomRenderItem = std::make_shared<CommonRenderItem>();
		m_CommomRenderItem->meshCollection = m_RenderMeshCollection.get();
		m_CommomRenderItem->meshGuid = triMesh->guid;
		m_CommomRenderItem->renderLayer = RenderLayer::Opaque;
		m_CommomRenderItem->shader = m_TestShader.get();
		m_CommomRenderItem->frameResourceInfo.objConstantIdx = 0;
		StaticModelObjectConstants objConstant;
		objConstant.color = Vector4{ 1.0, 0.0, 0.0, 1.0 };
		m_CommomRenderItem->frameResourceInfo.objectConstants = objConstant;
		m_RenderItems.push_back(m_CommomRenderItem.get());

		StaticModelFrameResourceDesc resourceDesc;
		resourceDesc.allObjectNum = 1;
		resourceDesc.allPassNum = 1;
		m_Rhi->CreateFrameResource(FrameResourceType::StaticModelFrameResource, &resourceDesc);

		//m_RenderItem.meshCollection = m_RenderMeshCollection.get();
		//m_RenderItem.meshGuid = triMesh->guid;
		//m_RenderItem2.meshCollection = m_RenderMeshCollection.get();
		//m_RenderItem2.meshGuid = triMesh2->guid;


		//m_Rhi->EndSingleRenderPass();
	}

	void RenderSystem::Tick(GameTimer& gt)
	{
		if(m_WindowSystem->ShouldClose())
		{
			m_Rhi->Clear();
			return;
		}
		// Bind Global Info
		m_Rhi->CmdSetDescriptorHeaps();

		ForwardPipelineRenderResourceData forwardPipelineData;
		forwardPipelineData.allRenderItems.resize(m_RenderItems.size());
		for(int i = 0; i< m_RenderItems.size(); ++i)
		{
			//UpdateFrameResource()
			auto ritem = dynamic_cast<CommonRenderItem*>(m_RenderItems[i]);
			if(ritem->numFrameDirty > 0)
			{
				auto frameResource = (CommonRenderItem::TFrameResource*)m_Rhi->GetCurrFrameResource(CommonRenderItem::s_FrameResourceType);
				frameResource->UpdateObjectConstantBuffer(ritem->frameResourceInfo.objConstantIdx, &ritem->frameResourceInfo.objectConstants);
				ritem->numFrameDirty--;
			}
			forwardPipelineData.allRenderItems[i] = ritem;
		}
		forwardPipelineData.depthStencil = m_DepthStencil;
		forwardPipelineData.renderTarget = m_RenderTarget;
		m_RenderPipelines[RenderPipelineType::ForwardPipeline]->PrepareContext(&forwardPipelineData);


		m_RenderPipelines[RenderPipelineType::ForwardPipeline]->Render();
	}

	std::shared_ptr<photon::RHI> RenderSystem::GetRHI()
	{
		return m_Rhi;
	}

	void RenderSystem::SetRenderPipelineType(RenderPipelineType renderType)
	{

	}

}