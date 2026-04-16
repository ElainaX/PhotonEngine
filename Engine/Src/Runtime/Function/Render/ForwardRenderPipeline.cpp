#include "ForwardRenderPipeline.h"

namespace photon
{
	void ForwardRenderPipeline::Initialize(const RenderPipelineCreateInfo& createInfo)
	{
		m_services = createInfo.services;

		m_preprocessPass = std::make_shared<PreprocessPass>();
		m_preprocessPass->Initialize(m_services);

		m_mainCameraPass = std::make_shared<MainCameraPass>();
		m_mainCameraPass->Initialize(m_services);

		m_csmMgr = std::make_shared<CascadedShadowManager>();
		m_csmMgr->Initialize({ ShadowMapSize, ShadowMapSize }, MaxCascadedNum);
	}

	void ForwardRenderPipeline::Prepare(EG_FrameContext& frame)
	{
		if (m_ui && m_services.imguiSystem)
		{
			m_services.imguiSystem->BeginFrame();
			m_ui->PreRender();              // 这里只负责搭 UI
			m_services.imguiSystem->EndFrame();
		}

		m_blackboard.Clear();
		m_blackboard.Set("csm_mgr", m_csmMgr);

		PassPrepareContext preprocessCtx = {};
		preprocessCtx.frame = &frame;
		preprocessCtx.blackboard = &m_blackboard;
		preprocessCtx.services = &m_services;
		preprocessCtx.renderScene = frame.renderScene;
		preprocessCtx.camera = frame.uniforms.mainCamera;
		preprocessCtx.visibleItems = &frame.renderlists.shadowCasters;
		m_preprocessPass->Prepare(preprocessCtx);

		PassPrepareContext mainCtx = {};
		mainCtx.frame = &frame;
		mainCtx.blackboard = &m_blackboard;
		mainCtx.services = &m_services;
		mainCtx.renderScene = frame.renderScene;
		mainCtx.camera = frame.uniforms.mainCamera;
		mainCtx.visibleItems = &frame.renderlists.opaque;
		m_mainCameraPass->Prepare(mainCtx);
	}

	void ForwardRenderPipeline::Execute(EG_FrameContext& frame)
	{
		PassExecuteContext execCtx = {};
		execCtx.frame = &frame;
		execCtx.blackboard = &m_blackboard;
		execCtx.services = &m_services;
		execCtx.cmd = frame.services.graphicsCmd;

		m_preprocessPass->Execute(execCtx);
		m_mainCameraPass->Execute(execCtx);
	}

	void ForwardRenderPipeline::Stop()
	{
		m_stop = true;
		if (m_mainCameraPass)
		{
			m_mainCameraPass->OnlyUI(true);
		}
	}

	void ForwardRenderPipeline::ReStart()
	{
		m_stop = false;
		if (m_mainCameraPass)
		{
			m_mainCameraPass->OnlyUI(false);
		}
	}

	void ForwardRenderPipeline::SetCurrEditorUI(WindowUI* ui)
	{
		m_ui = ui;
	}
}