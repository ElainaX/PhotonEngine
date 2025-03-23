#pragma once
#include "RenderType.h"
#include <memory>

namespace photon 
{
	class RHI;
	class RenderResourceData;

	struct RenderPipelineCreateInfo
	{
		virtual ~RenderPipelineCreateInfo() {}
	};

	class RenderPipeline
	{
	public:
		RenderPipeline(RenderPipelineType renderPipelineType = RenderPipelineType::ForwardPipeline)
			: m_Type(renderPipelineType){}

		virtual void Render() = 0;
		virtual void Initialize(RenderPipelineCreateInfo* createInfo) = 0;
		virtual void PrepareContext(RenderResourceData* data) = 0;

	protected:
		RenderPipelineType m_Type = RenderPipelineType::ForwardPipeline;


	};

}