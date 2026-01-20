#pragma once
#include "RenderType.h"
#include "PassBlackboard.h"

#include "EGFrameContext.h"
#include <memory>

namespace photon 
{
	class RHI;
	class RenderResourceData;
	class WindowUI;
	class EG_FrameContext;

	struct RenderPipelineCreateInfo
	{
		virtual ~RenderPipelineCreateInfo() {}
	};

	class RenderPipeline
	{
	public:
		RenderPipeline(RenderPipelineType renderPipelineType = RenderPipelineType::ForwardPipeline)
			: m_Type(renderPipelineType){}

		virtual void Render(EG_FrameContext* frame) = 0;
		virtual void Initialize(RenderPipelineCreateInfo* createInfo) = 0;
		virtual void PrepareContext(EG_FrameContext* frame) = 0;
		virtual void SetCurrEditorUI(WindowUI* ui) = 0;
		virtual void Stop() = 0;
		virtual void ReStart() = 0;

	protected:
		RenderPipelineType m_Type = RenderPipelineType::ForwardPipeline;

		PassBlackboard m_BB;
	};

}