﻿#include "ForwardRenderPipeline.h"
#include "RenderPass/MainCameraPass.h"
#include "RenderResourceData.h"
#include "DX12RHI/DX12RHI.h"

namespace photon 
{

	void ForwardRenderPipeline::Initialize(RenderPipelineCreateInfo* createInfo)
	{
		ForwardPipelineCreateInfo* info = dynamic_cast<ForwardPipelineCreateInfo*>(createInfo);
		m_Rhi = info->rhi;
		m_WindowSystem = info->windowSystem;

		m_MainCameraRenderPass = std::make_shared<MainCameraPass>();
		m_MainCameraRenderPass->Initialize(m_Rhi, m_WindowSystem);
	}

	void ForwardRenderPipeline::PrepareContext(RenderResourceData* data)
	{
		m_UI->PreRender();


		ForwardPipelineRenderResourceData* resourceData = dynamic_cast<ForwardPipelineRenderResourceData*>(data);
		// Update RenderItem Constants

		// MainCameraPass 
		MainPassRenderResourceData mainCameraPassData;
		mainCameraPassData.allRenderItems = resourceData->allRenderItems;
		//mainCameraPassData.diffuseMap = resourceData->diffuseMap;
		mainCameraPassData.renderTarget = resourceData->renderTarget;
		mainCameraPassData.depthStencil = resourceData->depthStencil;
		mainCameraPassData.mainCamera = resourceData->mainCamera;
		mainCameraPassData.gameTimer = resourceData->gameTimer;
		mainCameraPassData.directionalLights = std::move(resourceData->directionalLights);
		mainCameraPassData.pointLights = std::move(resourceData->pointLights);
		mainCameraPassData.spotLights = std::move(resourceData->spotLights);
		m_MainCameraRenderPass->PrepareContext(&mainCameraPassData);



	}

	void ForwardRenderPipeline::Render()
	{
		m_MainCameraRenderPass->Draw();
	}



	void ForwardRenderPipeline::SetCurrEditorUI(WindowUI* ui)
	{
		m_UI = ui;
	}

}


