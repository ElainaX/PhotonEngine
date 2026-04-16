#pragma once
#include "RenderTypes.h"
#include "RenderPass/PassBlackboard.h"
#include "EGFrameContext.h"
#include "RenderPass/PassContext.h"
#include "Function/UI/ImGuiSystem.h"

namespace photon 
{
	class WindowUI;
	class EG_FrameContext;

	struct RenderPipelineCreateInfo
	{
		virtual ~RenderPipelineCreateInfo() = default;
		RenderPipelineServices services = {};
	};

	class RenderPipeline
	{
	public:
		RenderPipeline(RenderPipelineType renderPipelineType = RenderPipelineType::ForwardPipeline)
			: m_type(renderPipelineType){}

		virtual ~RenderPipeline() = default;

		virtual void Initialize(const RenderPipelineCreateInfo& createInfo) = 0;
		virtual void Prepare(EG_FrameContext& frame) = 0;
		virtual void Execute(EG_FrameContext& frame) = 0;


		virtual void SetCurrEditorUI(WindowUI* ui) { m_ui = ui; }
		virtual void SetImGuiSystem(ImGuiSystem* imguiSystem)
		{
			m_services.imguiSystem = imguiSystem;
		}
		virtual void Stop() {};
		virtual void ReStart() {};

	protected:
		RenderPipelineType m_type = RenderPipelineType::ForwardPipeline;
		RenderPipelineServices m_services = {};
		PassBlackboard m_blackboard = {};
		WindowUI* m_ui = nullptr;
	};

}
