#pragma once
#include "RenderPipeline.h"

#include "RenderPass.h"

namespace photon 
{
	class RenderPass;
	class Texture2D;

	class ForwardRenderPipeline : public RenderPipeline
	{
	public:
		ForwardRenderPipeline() = default;

		ForwardRenderPipeline(std::shared_ptr<RHI> rhi)
			: RenderPipeline(RenderPipelineType::ForwardPipeline) 
		{
			Initialize(rhi);
		}


		void Initialize(std::shared_ptr<RHI> rhi) override final;
		void Render(std::shared_ptr<RHI> rhi, std::shared_ptr<RenderResourceData> renderData) override final;
		
	private:

		//std::shared_ptr<RenderPass> m_DirectionalLightShadowPass;
		//std::shared_ptr<RenderPass> m_PointLightShadowPass;
		std::shared_ptr<RenderPass> m_MainCameraRenderPass;
		//std::shared_ptr<RenderPass> m_ColorGradingPass;

		std::shared_ptr<Texture2D> m_RenderTarget;


	};
}