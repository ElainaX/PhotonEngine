#include "UISubPass.h"
#include "Function/Util/RenderUtil.h"

namespace photon 
{



	void UISubPass::Initialize(RHI* _rhi)
	{
		m_Rhi = _rhi;
	}

	void UISubPass::PrepareForData(RenderResourceData* _data)
	{
		auto data = dynamic_cast<UISubPassData*>(_data);
		renderTargetView = data->renderTargetView;
		depthStencilView = data->depthStencilView;
	}

	void UISubPass::Draw(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport)
	{
		auto RenderTex = (Texture2D*)renderTargetView->resource;
		auto DepthTex = (Texture2D*)depthStencilView->resource;

		m_Rhi->ResourceStateTransform(RenderTex, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_Rhi->ResourceStateTransform(DepthTex, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		m_Rhi->CmdClearRenderTarget(renderTargetView, RenderTex->clearValue);
		m_Rhi->CmdClearDepthStencil(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, DepthTex->clearValue.x, RenderUtil::FloatRoundToUINT(DepthTex->clearValue.y));

		m_Rhi->CmdSetViewportsAndScissorRects(scissorRect, viewport);

		m_Rhi->CmdSetRenderTargets(1, &renderTargetView->cpuHandleInHeap, true, &depthStencilView->cpuHandleInHeap);
		m_Rhi->CmdDrawImGui();
	}

}