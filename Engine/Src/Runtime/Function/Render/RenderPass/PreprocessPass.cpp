#include "PreprocessPass.h"
#include "Function/Render/EGFrameContext.h"
#include "Function/Render/CascadedShadowManager.h"
#include "Function/Util/RenderUtil.h"

namespace photon
{
	void PreprocessPass::Initialize(const RenderPipelineServices& services)
	{
		RenderPass::Initialize(services);
	}

	void PreprocessPass::Prepare(const PassPrepareContext& ctx)
	{
		if (!ctx.frame || !ctx.blackboard || !ctx.camera)
			return;

		auto csmMgr = ctx.blackboard->Get<CascadedShadowManager>("csm_mgr");
		if (!csmMgr || !ctx.frame->uniforms.mainDirLight)
			return;

		csmMgr->Track(ctx.frame->uniforms.mainDirLight, ctx.camera);

		auto spliters = RenderUtil::LogLinearCascadedSplit(
			ctx.frame->uniforms.znear,
			ctx.frame->uniforms.zfar,
			50.0f,
			MaxCascadedNum);

		ctx.blackboard->Set(
			"csm_spliters",
			std::make_shared<std::pair<std::vector<float>, std::vector<float>>>(spliters));

		auto viewProjMatrices = csmMgr->GetViewAndProjMatrices(spliters.second);
		ctx.blackboard->Set(
			"light_view_proj_matrices",
			std::make_shared<std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>>>(viewProjMatrices));
	}

	void PreprocessPass::Execute(const PassExecuteContext& ctx)
	{
		// 这里先不接真正 shadow subpass。
		// 先保证主链联调通，后面再把 DrawShadowSubPass 迁过来。
	}
}