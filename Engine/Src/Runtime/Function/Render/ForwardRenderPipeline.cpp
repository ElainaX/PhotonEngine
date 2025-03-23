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
		ForwardPipelineRenderResourceData* resourceData = dynamic_cast<ForwardPipelineRenderResourceData*>(data);


		// MainCameraPass 
		MainPassRenderResourceData mainCameraPassData;
		mainCameraPassData.allRenderItems = resourceData->allRenderItems;
		mainCameraPassData.renderTarget = resourceData->renderTarget;
		mainCameraPassData.depthStencil = resourceData->depthStencil;
		m_MainCameraRenderPass->PrepareContext(&mainCameraPassData);



	}

	void ForwardRenderPipeline::Render()
	{
		m_MainCameraRenderPass->Draw();
	}



}


