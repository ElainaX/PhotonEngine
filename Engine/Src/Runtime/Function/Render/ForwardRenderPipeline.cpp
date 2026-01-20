#include "ForwardRenderPipeline.h"
#include "RenderPass/MainCameraPass.h"
#include "RenderPass/PreprocessPass.h"
#include "RenderResourceData.h"
#include "DX12RHI/DX12RHI.h"

namespace photon 
{

	void ForwardRenderPipeline::Initialize(RenderPipelineCreateInfo* createInfo)
	{
		ForwardPipelineCreateInfo* info = dynamic_cast<ForwardPipelineCreateInfo*>(createInfo);
		m_Rhi = info->rhi;
		m_WindowSystem = info->windowSystem;

		m_PreprocessRenderPass = std::make_shared<PreprocessPass>();
		m_PreprocessRenderPass->Initialize(m_Rhi);

		m_MainCameraRenderPass = std::make_shared<MainCameraPass>();
		m_MainCameraRenderPass->Initialize(m_Rhi, m_WindowSystem);

		m_CsmMgr = std::make_shared<CascadedShadowManager>();
		m_CsmMgr->Initialize({ 1024, 1024 }, 3);
	}

	void photon::ForwardRenderPipeline::PrepareContext(EG_FrameContext* frame)
	{
		m_UI->PreRender();
		m_BB.Clear();
		m_BB.Set("csm_mgr", m_CsmMgr);
		// Update RenderItem Constants

		// PreprocessPass
		PreprocessPassRenderResourceData preprocessPassData;
		preprocessPassData.frame = frame;
		preprocessPassData.bb = &m_BB;
		////preprocessPassData.spliters = { 0.003f, 0.01f };
		////preprocessPassData.mainCamera = frame->mainCamera;
		////preprocessPassData.mainLight = frame->mainLight;
		////preprocessPassData.allRenderItems = frame->allRenderItems;
		m_PreprocessRenderPass->PrepareContext(&preprocessPassData);

		// MainCameraPass 
		MainPassRenderResourceData mainCameraPassData;
		mainCameraPassData.frame = frame;
		mainCameraPassData.bb = &m_BB;
		//mainCameraPassData.allRenderItems = resourceData->allRenderItems;
		//mainCameraPassData.diffuseMap = resourceData->diffuseMap;
		//mainCameraPassData.renderTarget = resourceData->renderTarget;
		//mainCameraPassData.depthStencil = resourceData->depthStencil;
		//mainCameraPassData.mainCamera = resourceData->mainCamera;
		//mainCameraPassData.gameTimer = resourceData->gameTimer;
		//mainCameraPassData.cubemap = resourceData->cubemap;
		//mainCameraPassData.directionalLights = std::move(resourceData->directionalLights);
		//mainCameraPassData.pointLights = std::move(resourceData->pointLights);
		//mainCameraPassData.spotLights = std::move(resourceData->spotLights);
		//mainCameraPassData.cascadedShadowManager = preprocessPassData.cascadedShadowManager;
		m_MainCameraRenderPass->PrepareContext(&mainCameraPassData);



	}

	void ForwardRenderPipeline::Render(EG_FrameContext* frame)
	{
		m_PreprocessRenderPass->Draw(frame, &m_BB);
		m_MainCameraRenderPass->Draw(frame, &m_BB);
	}



	void ForwardRenderPipeline::Stop()
	{
		m_bStop = true;
		m_MainCameraRenderPass->OnlyUI(true);
	}

	void ForwardRenderPipeline::ReStart()
	{
		m_bStop = false;
		m_MainCameraRenderPass->OnlyUI(false);
	}

	std::shared_ptr<CascadedShadowManager> ForwardRenderPipeline::GetCSMMgr()
	{
		return m_CsmMgr;
	}

	void ForwardRenderPipeline::SetCurrEditorUI(WindowUI* ui)
	{
		m_UI = ui;
	}

}


