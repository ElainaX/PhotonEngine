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
		ForwardRenderPipeline()
			: RenderPipeline(RenderPipelineType::ForwardPipeline) 
		{
			Initialize();
		}


		void Initialize() override final;
		void Render(std::shared_ptr<RHI> rhi, std::shared_ptr<RenderResourceData> renderData) override final;
		
	private:

		//std::shared_ptr<RenderPass> m_DirectionalLightShadowPass;
		//std::shared_ptr<RenderPass> m_PointLightShadowPass;
		std::shared_ptr<RenderPass> m_MainCameraRenderPass;
		//std::shared_ptr<RenderPass> m_ColorGradingPass;

		std::shared_ptr<Texture2D> m_RenderTarget;

	};
}