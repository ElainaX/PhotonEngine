#include "MainCameraPass.h"
#include "Function/Render/EGFrameContext.h"
#include "Function/Render/ResourceManager.h"
#include "Function/Render/DX12RHI/DX12CommandContext.h"
#include "Function/Render/Shader/ShaderParameter/ShaderConstantBuffer.h"

namespace photon
{
	namespace
	{
		PassData BuildMainPassData(const EG_FrameContext& frame)
		{
			PassData data = {};
			data.view = frame.uniforms.view;
			data.invView = frame.uniforms.invView;
			data.proj = frame.uniforms.proj;
			data.invProj = frame.uniforms.invProj;
			data.viewProj = frame.uniforms.viewProj;
			data.invViewProj = frame.uniforms.invViewProj;
			data.eyePos = frame.uniforms.camPosWS;
			data.renderTargetSize = {
				static_cast<float>(frame.uniforms.viewportSize.x),
				static_cast<float>(frame.uniforms.viewportSize.y)
			};
			data.invRenderTargetSize = {
				frame.uniforms.invViewportSize.x,
				frame.uniforms.invViewportSize.y
			};
			data.znear = frame.uniforms.znear;
			data.zfar = frame.uniforms.zfar;
			data.totalTime = frame.uniforms.timer ? static_cast<float>(frame.uniforms.timer->TotalTime()) : 0.0f;
			data.deltaTime = frame.uniforms.timer ? static_cast<float>(frame.uniforms.timer->DeltaTime()) : 0.0f;
			data.ambientLight = { 0.15f, 0.15f, 0.15f, 1.0f };
			return data;
		}
	}

	void MainCameraPass::Initialize(const RenderPipelineServices& services)
	{
		RenderPass::Initialize(services);
		m_opaqueSubPass.Initialize();
		m_uiSubPass.Initialize();
	}

	void MainCameraPass::Prepare(const PassPrepareContext& ctx)
	{
		if (!ctx.frame || !ctx.services)
			return;

		PassData passData = BuildMainPassData(*ctx.frame);

		FrameResourceRange upload =
			ctx.services->frameAllocator->AllocateFrameUploadBuffer256<PassData>(1);

		DXResource* uploadRes =
			ctx.services->gpuResManager->GetResource(upload.buffer);
		if (!uploadRes)
			return;

		ctx.frame->services.graphicsCmd->CopyBufferCpuToUpload(
			uploadRes,
			upload.range.cbvOffsetInBytes,
			&passData,
			sizeof(PassData));

		FrameDescriptorHandle passCbv =
			ctx.services->frameAllocator->AllocateFrameCbvDescriptor();

		ViewDesc cbvDesc = {};
		cbvDesc.type = ViewType::CBV;
		cbvDesc.dimension = ViewDimension::Buffer;
		cbvDesc.cbvOffsetInBytes = upload.range.cbvOffsetInBytes;
		cbvDesc.cbvSizeInBytes = upload.range.cbvSizeInBytes;

		ctx.services->descriptorSystem->CreateDescriptorAtCpuHandle(
			uploadRes,
			cbvDesc,
			ctx.services->frameAllocator->GetCpuHandle(passCbv));

		ctx.blackboard->Set(
			"main_pass_cbv",
			std::make_shared<FrameDescriptorHandle>(passCbv));

		m_opaqueSubPass.Prepare(ctx);
		m_uiSubPass.Prepare(ctx);
	}

	void MainCameraPass::Execute(const PassExecuteContext& ctx)
	{
		if (!ctx.frame || !ctx.cmd || !ctx.services)
			return;

		BeginMainColorPass(ctx);

		if (!m_onlyUI)
			m_opaqueSubPass.Execute(ctx);

		// Editor 模式下不把 ImGui 画到 sceneColor
		const bool hasEditorUI = (ctx.services->imguiSystem != nullptr);
		if (!hasEditorUI)
		{
			m_uiSubPass.Execute(ctx);
		}

		EndMainColorPass(ctx);
	}

	void MainCameraPass::BeginMainColorPass(const PassExecuteContext& ctx)
	{
		ResourceManager& rm = *ctx.services->resourceManager;
		auto* frame = ctx.frame;
		auto& cmd = *ctx.cmd;

		const TextureRenderResource* colorRR = rm.GetTextureRenderResource(frame->targets.sceneColor);
		const TextureRenderResource* depthRR = rm.GetTextureRenderResource(frame->targets.sceneDepth);
		if (!colorRR || !depthRR)
			return;

		DXResource* sceneColor = ctx.services->gpuResManager->GetResource(colorRR->texture);
		DXResource* sceneDepth = ctx.services->gpuResManager->GetResource(depthRR->texture);
		if (!sceneColor || !sceneDepth)
			return;

		D3D12_CPU_DESCRIPTOR_HANDLE colorRtv =
			ctx.services->descriptorSystem->GetCpuHandle(colorRR->rtv);
		D3D12_CPU_DESCRIPTOR_HANDLE depthDsv =
			ctx.services->descriptorSystem->GetCpuHandle(depthRR->dsv);

		cmd.ResourceStateTransform(sceneColor, D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmd.ResourceStateTransform(sceneDepth, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		D3D12_RECT scissorRect = {
			0, 0,
			frame->uniforms.viewportSize.x,
			frame->uniforms.viewportSize.y
		};

		D3D12_VIEWPORT viewport = {
			0.0f, 0.0f,
			static_cast<float>(frame->uniforms.viewportSize.x),
			static_cast<float>(frame->uniforms.viewportSize.y),
			0.0f, 1.0f
		};

		cmd.SetViewportsAndScissorRects(scissorRect, viewport);
		cmd.SetRenderTargets(1, &colorRtv, true, &depthDsv);
		cmd.ClearRenderTarget(colorRtv, { 0.1f, 0.1f, 0.1f, 1.0f });
		cmd.ClearDepthStencil(
			depthDsv,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			1.0f,
			0);
	}

	void MainCameraPass::EndMainColorPass(const PassExecuteContext& ctx)
	{
		const TextureRenderResource* colorRR =
			ctx.services->resourceManager->GetTextureRenderResource(ctx.frame->targets.sceneColor);
		if (!colorRR)
			return;

		DXResource* sceneColor =
			ctx.services->gpuResManager->GetResource(colorRR->texture);
		if (!sceneColor)
			return;

		const bool hasEditorUI = (ctx.services->imguiSystem != nullptr);
		if (hasEditorUI)
		{
			ctx.cmd->ResourceStateTransform(sceneColor, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
		else
		{
			ctx.cmd->ResourceStateTransform(sceneColor, D3D12_RESOURCE_STATE_COPY_SOURCE);
		}
	}
}