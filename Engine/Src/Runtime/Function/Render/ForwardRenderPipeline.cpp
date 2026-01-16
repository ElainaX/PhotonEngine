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
	}

	void ForwardRenderPipeline::PrepareContext(RenderResourceData* data)
	{
		m_UI->PreRender();


		ForwardPipelineRenderResourceData* resourceData = dynamic_cast<ForwardPipelineRenderResourceData*>(data);
		// Update RenderItem Constants

		// PreprocessPass
		PreprocessPassRenderResourceData preprocessPassData;
		preprocessPassData.spliters = { 0.003f, 0.01f };
		preprocessPassData.mainCamera = resourceData->mainCamera;
		preprocessPassData.mainLight = resourceData->mainLight;
		preprocessPassData.allRenderItems = resourceData->allRenderItems;
		m_PreprocessRenderPass->PrepareContext(&preprocessPassData);

		// MainCameraPass 
		MainPassRenderResourceData mainCameraPassData;
		mainCameraPassData.allRenderItems = resourceData->allRenderItems;
		//mainCameraPassData.diffuseMap = resourceData->diffuseMap;
		mainCameraPassData.renderTarget = resourceData->renderTarget;
		mainCameraPassData.depthStencil = resourceData->depthStencil;
		mainCameraPassData.mainCamera = resourceData->mainCamera;
		mainCameraPassData.gameTimer = resourceData->gameTimer;
		mainCameraPassData.cubemap = resourceData->cubemap;
		mainCameraPassData.directionalLights = std::move(resourceData->directionalLights);
		mainCameraPassData.pointLights = std::move(resourceData->pointLights);
		mainCameraPassData.spotLights = std::move(resourceData->spotLights);
		mainCameraPassData.cascadedShadowManager = preprocessPassData.cascadedShadowManager;
		m_MainCameraRenderPass->PrepareContext(&mainCameraPassData);



	}

	void ForwardRenderPipeline::Render()
	{
		m_PreprocessRenderPass->Draw();
		m_MainCameraRenderPass->Draw();
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

	void ForwardRenderPipeline::SetCurrEditorUI(WindowUI* ui)
	{
		m_UI = ui;
	}

}


