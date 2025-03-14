﻿#include "ForwardRenderPipeline.h"
#include "RenderPass/MainCameraPass.h"
#include "RenderResourceData.h"
#include "DX12RHI/DX12RHI.h"

namespace photon 
{

	void ForwardRenderPipeline::Initialize()
	{

	}

	void ForwardRenderPipeline::Render(std::shared_ptr<RHI> rhi, std::shared_ptr<RenderResourceData> renderData)
	{
		rhi->BeginSingleRenderPass();

		auto tex = renderData->texA;
		rhi->CopyTextureToSwapChain(tex);

		rhi->PrepareForPresent();

		rhi->EndSingleRenderPass();
		
		rhi->Present();
	}

}


