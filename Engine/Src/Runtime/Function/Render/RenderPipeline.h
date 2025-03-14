#pragma once
#include "RenderType.h"

#include <memory>

namespace photon 
{
	class RHI;
	class RenderResourceData;

	class RenderPipeline
	{
	public:
		RenderPipeline(RenderPipelineType renderPipelineType = RenderPipelineType::ForwardPipeline)
			: m_Type(renderPipelineType){}

		virtual void Render(std::shared_ptr<RHI> rhi, std::shared_ptr<RenderResourceData> renderData) = 0;
		virtual void Initialize() = 0;

	protected:
		RenderPipelineType m_Type = RenderPipelineType::ForwardPipeline;


	};

}