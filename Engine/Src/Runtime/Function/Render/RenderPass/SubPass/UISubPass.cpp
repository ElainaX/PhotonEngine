#include "UISubPass.h"
#include "Function/Render/EGFrameContext.h"
#include "Function/Render/ResourceManager.h"
#include "Function/UI/ImGuiSystem.h"

namespace photon
{
	void UISubPass::Prepare(const PassPrepareContext& ctx)
	{
		ClearDrawList();
	}

	void UISubPass::Execute(const PassExecuteContext& ctx)
	{
		if (!ctx.frame || !ctx.cmd || !ctx.services)
			return;

		ResourceManager& rm = *ctx.services->resourceManager;

		const TextureRenderResource* colorRR = rm.GetTextureRenderResource(ctx.frame->targets.sceneColor);
		const TextureRenderResource* depthRR = rm.GetTextureRenderResource(ctx.frame->targets.sceneDepth);
		if (!colorRR || !depthRR)
			return;

		D3D12_CPU_DESCRIPTOR_HANDLE colorRtv =
			ctx.services->descriptorSystem->GetCpuHandle(colorRR->rtv);
		D3D12_CPU_DESCRIPTOR_HANDLE depthDsv =
			ctx.services->descriptorSystem->GetCpuHandle(depthRR->dsv);

		D3D12_RECT scissorRect = {
			0, 0,
			ctx.frame->uniforms.viewportSize.x,
			ctx.frame->uniforms.viewportSize.y
		};

		D3D12_VIEWPORT viewport = {
			0.0f,
			0.0f,
			static_cast<float>(ctx.frame->uniforms.viewportSize.x),
			static_cast<float>(ctx.frame->uniforms.viewportSize.y),
			0.0f,
			1.0f
		};

		ctx.cmd->SetViewportsAndScissorRects(scissorRect, viewport);
		ctx.cmd->SetRenderTargets(1, &colorRtv, true, &depthDsv);
		
		if (ctx.services->imguiSystem)
		{
			ctx.services->imguiSystem->Render(*ctx.cmd);
		}
	}
}