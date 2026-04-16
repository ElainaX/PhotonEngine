#pragma once
#include "Function/Render/RenderPass.h"
#include "SubPass/OpaqueSubPass.h"
#include "SubPass/UISubPass.h"

namespace photon
{
	class MainCameraPass : public RenderPass
	{
	public:
		void Initialize(const RenderPipelineServices& services) override;
		void Prepare(const PassPrepareContext& ctx) override;
		void Execute(const PassExecuteContext& ctx) override;

		void OnlyUI(bool onlyUI = true)
		{
			m_onlyUI = onlyUI;
		}

	private:
		void BeginMainColorPass(const PassExecuteContext& ctx);
		void EndMainColorPass(const PassExecuteContext& ctx);

	private:
		OpaqueSubPass m_opaqueSubPass;
		UISubPass m_uiSubPass;
		bool m_onlyUI = false;
	};
}