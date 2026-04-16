#pragma once
#include "RenderPipeline.h"
#include "RenderPass/MainCameraPass.h"
#include "RenderPass/PreprocessPass.h"
#include "Function/UI/WindowUI.h"
#include "CascadedShadowManager.h"


namespace photon 
{
	class RenderPass;
	class DXTexture2D;

	struct ForwardPipelineCreateInfo : public RenderPipelineCreateInfo
	{
	};

	class ForwardRenderPipeline : public RenderPipeline
	{
	public:
		ForwardRenderPipeline() = default;

		void Initialize(const RenderPipelineCreateInfo& createInfo) override;
		void Prepare(EG_FrameContext& frame) override;
		void Execute(EG_FrameContext& frame) override;

		void Stop() override;
		void ReStart() override;

		std::shared_ptr<CascadedShadowManager> GetCSMMgr() const
		{
			return m_csmMgr;
		}

		void SetCurrEditorUI(WindowUI* ui) override;

	private:

		std::shared_ptr<MainCameraPass> m_mainCameraPass;
		std::shared_ptr<PreprocessPass> m_preprocessPass;
		std::shared_ptr<CascadedShadowManager> m_csmMgr;

		bool m_stop = false;
	};
}