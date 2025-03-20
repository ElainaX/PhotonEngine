#include "DepthStencilState.h"

namespace photon 
{

	D3D12_DEPTH_STENCIL_DESC DepthStencilState::GetDepthStencilDesc() const
	{
		D3D12_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = depthState.isDepthTestEnbale;
		desc.DepthWriteMask = depthState.isDepthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL: D3D12_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = (D3D12_COMPARISON_FUNC)depthState.depthCompareOp;
		
		desc.StencilEnable = stencilState.isStencilTestEnable;
		desc.StencilReadMask = stencilState.stencilReadMask;
		desc.StencilWriteMask = stencilState.stencilWriteMask;

		desc.FrontFace.StencilFailOp = (D3D12_STENCIL_OP)stencilState.onFrontStencilTestFailed;
		desc.FrontFace.StencilDepthFailOp = (D3D12_STENCIL_OP)stencilState.onFrontStencilTestPassButDepthTestFailed;
		desc.FrontFace.StencilPassOp = (D3D12_STENCIL_OP)stencilState.onFrontStencilDepthTestPass;
		desc.FrontFace.StencilFunc = (D3D12_COMPARISON_FUNC)stencilState.frontStencilCompareOp;

		desc.BackFace.StencilFailOp = (D3D12_STENCIL_OP)stencilState.onBackStencilTestFailed;
		desc.BackFace.StencilDepthFailOp = (D3D12_STENCIL_OP)stencilState.onBackStencilTestPassButDepthTestFailed;
		desc.BackFace.StencilPassOp = (D3D12_STENCIL_OP)stencilState.onBackStencilDepthTestPass;
		desc.BackFace.StencilFunc = (D3D12_COMPARISON_FUNC)stencilState.backStencilCompareOp;

		return desc;
	}

}