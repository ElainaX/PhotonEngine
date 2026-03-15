#include "PreprocessPass.h"
#include "Function/Global/RuntimeGlobalContext.h"
#include "Function/Render/RenderSystem.h"
#include "Function//Util/RenderUtil.h"

namespace photon 
{


	void PreprocessPass::Initialize(RHI* rhi)
	{
		m_Rhi = rhi;

		for(int i = 0; i < MaxCascadedNum; ++i)
		{
			m_PassConstantsIndices.push_back(StaticModelPassConstants::s_CurrPassIndex++);
		}
		m_PassConstantses.resize(MaxCascadedNum);
		//m_CascadedShadowManager = std::make_shared<CascadedShadowManager>();
		//m_CascadedShadowManager->Initialize({ 1024, 1024 }, 3);

		m_ShadowSubPass = std::make_shared<DrawShadowSubPass>();
		m_ShadowSubPass->Initialize(m_Rhi);
		m_ShadowShader = g_RuntimeGlobalContext.renderSystem->GetShaderFactory()->Create(L"DrawShadowMap");
	}

	void photon::PreprocessPass::PrepareContext(RenderResourceData * data)
	{
		auto preprocessData = static_cast<PreprocessPassRenderResourceData*>(data);
		auto frame = preprocessData->frame;
		auto cascadedShadowMgr = preprocessData->bb->Get<CascadedShadowManager>("csm_mgr");

		cascadedShadowMgr->Track(frame->uniforms.mainDirLight, frame->uniforms.mainCamera);
		preprocessData->bb->Set("csm_spliters", std::make_shared<std::pair<std::vector<float>, std::vector<float>>>
			( RenderUtil::LogLinearCascadedSplit(frame->uniforms.znear, frame->uniforms.zfar, 50.0f, MaxCascadedNum) ));
		//m_CascadedShadowManager->Track(preprocessData->mainLight, preprocessData->mainCamera);
		//preprocessData->cascadedShadowManager = m_CascadedShadowManager;

		auto splitersPair = preprocessData->bb->Get <std::pair<std::vector<float>, std::vector<float>>>("csm_spliters");
		auto viewProjMatrices = cascadedShadowMgr->GetViewAndProjMatrices(splitersPair->second);
		preprocessData->bb->Set("light_view_proj_matrices", std::make_shared<std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>>>
			(viewProjMatrices));

		// 拙劣的实现方式：手动删除一下
		auto& currPassFrameResource = 
			m_PassFrameResources[frame->services.rhi->GetCurrFrameResource(FrameResourceType::StaticModelFrameResource)];
		for (auto& res : currPassFrameResource)
		{
			if (dynamic_cast<Mesh*>(res) != nullptr)
			{
				frame->services.resMgr->DestoryMesh(dynamic_cast<Mesh*>(res));
			}
		}
		currPassFrameResource.clear();
		// Fill Frustum RenderItem
		for (int i = 0; i < viewProjMatrices.size(); ++i)
		{
			auto& [view, proj] = viewProjMatrices[i];
			auto corners = RenderUtil::ProjViewToCorners(proj, view);
			GeometryGenerator::MeshData meshData = frame->services.geoGen->CreateFrustum(corners);
			MeshDesc meshDesc;
			meshDesc.name = L"csm_frustum_" + std::to_wstring(i);
			meshDesc.type = VertexType::VertexSimple;
			meshDesc.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			meshDesc.vertexRawData = RenderUtil::CreateD3DBlob(meshData.Vertices.data(), sizeof(VertexSimple) * meshData.Vertices.size());
			meshDesc.indexRawData = RenderUtil::CreateD3DBlob(meshData.Indices32.data(), sizeof(UINT32) * meshData.Indices32.size());
			// 删不了这个mesh，咋办？
			// 拙劣的实现方式：手动删除一下
			auto mesh = frame->services.resMgr->CreateMesh(meshDesc);
			frame->services.innerMeshCollection->PushMesh(mesh);
			currPassFrameResource.push_back(mesh.get());
			
			auto ritem = std::make_shared<CommonRenderItem>();
			ritem->bCastShadow = false;
			ritem->meshCollection = frame->services.innerMeshCollection;
			ritem->meshGuid = mesh->guid;
			ritem->shader = frame->services.shaderFactory->Create(L"DebugDrawLight");
			ritem->objConstantIdx = g_objIdxHolder.GetObjIndex();
			ritem->wireframeOn = true;
			DirectX::XMMATRIX worldMat = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
			DirectX::XMStoreFloat4x4(&ritem->objectConstants.world, worldMat);

			frame->renderlists.innerRitems[frame->services.rhi
				->GetCurrFrameResource(FrameResourceType::StaticModelFrameResource)].push_back(ritem);

			frame->uniforms.staticFrameResource->UpdateObjectConstantBuffer(ritem->objConstantIdx, &ritem->objectConstants);
		}

		DrawShadowSubPassData shadowData;
		shadowData.frame = frame;
		shadowData.bb = preprocessData->bb;

		// Do Shadow Render
		for(int i = 0; i < viewProjMatrices.size(); ++i)
		{
			auto[viewMat, projMat] = viewProjMatrices[i];
			auto viewProj = XMMatrixMultiply(viewMat, projMat);

			XMStoreFloat4x4(&m_PassConstantses[i].view, viewMat);
			XMStoreFloat4x4(&m_PassConstantses[i].proj, projMat);
			XMStoreFloat4x4(&m_PassConstantses[i].viewProj, viewProj);


			StaticModelFrameResource* frameResource = (StaticModelFrameResource*)m_Rhi->GetCurrFrameResource(FrameResourceType::StaticModelFrameResource);
			frameResource->UpdatePassConstantBuffer(m_PassConstantsIndices[i], &m_PassConstantses[i]);

			shadowData.passConstantses.push_back(m_PassConstantsIndices[i]);

		}

		
		//shadowData.cascadedShadowManager = m_CascadedShadowManager;
		//shadowData.renderItems = preprocessData->allRenderItems;

		shadowData.shadowShader = m_ShadowShader;
		m_ShadowSubPass->PrepareForData(&shadowData);
	}

	void photon::PreprocessPass::Draw(EG_FrameContext* frame, PassBlackboard* bb)
	{
		m_ShadowSubPass->Draw(frame, bb);
	}

}