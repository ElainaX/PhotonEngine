#pragma once
#include "RenderPipeline.h"
#include "RenderResourceData/ForwardPipelineRenderResourceData.h"
#include "RenderResourceData/MainPassRenderResourceData.h"
#include "RenderPass/MainCameraPass.h"

namespace photon 
{
	class RenderPass;
	class Texture2D;

	struct ForwardPipelineCreateInfo : public RenderPipelineCreateInfo
	{
		RHI* rhi;
		WindowSystem* windowSystem;
	};

	class ForwardRenderPipeline : public RenderPipeline
	{
	public:
		ForwardRenderPipeline() = default;

		ForwardRenderPipeline(RenderPipelineCreateInfo* createInfo)
			: RenderPipeline(RenderPipelineType::ForwardPipeline) 
		{
			Initialize(createInfo);
		}


		void PrepareContext(RenderResourceData* data) override;
		void Initialize(RenderPipelineCreateInfo* createInfo) override final;
		void Render() override final;
		


	private:

		//std::shared_ptr<RenderPass> m_DirectionalLightShadowPass;
		//std::shared_ptr<RenderPass> m_PointLightShadowPass;
		std::shared_ptr<MainCameraPass> m_MainCameraRenderPass;
		//std::shared_ptr<RenderPass> m_ColorGradingPass;

		std::shared_ptr<Texture2D> m_RenderTarget;
		RHI* m_Rhi = nullptr;
		WindowSystem* m_WindowSystem = nullptr;

	};
}