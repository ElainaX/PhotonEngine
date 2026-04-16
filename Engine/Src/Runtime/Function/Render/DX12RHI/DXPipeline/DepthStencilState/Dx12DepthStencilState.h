#pragma once

#include <d3d12.h>
#include "Function/Render/DX12RHI/DXPipeline/GraphicsPipelineState.h"

namespace photon
{
	class Dx12DepthStencilState
	{
	public:
		Dx12DepthStencilState() = default;
		explicit Dx12DepthStencilState(const DepthStencilStateDesc& desc)
		{
			Build(desc);
		}

		void Build(const DepthStencilStateDesc& desc);
		const D3D12_DEPTH_STENCIL_DESC& GetDepthStencilDesc() const { return m_desc; }

		static D3D12_DEPTH_STENCIL_DESC ToNative(const DepthStencilStateDesc& desc);

	private:
		static D3D12_COMPARISON_FUNC ToD3D12Compare(CompareOp op);
		static D3D12_DEPTH_WRITE_MASK ToD3D12WriteMask(DepthWriteMask mask);
		static D3D12_STENCIL_OP ToD3D12StencilOp(StencilOp op);
		static D3D12_DEPTH_STENCILOP_DESC ToD3D12StencilFace(const StencilFaceDesc& face);

	private:
		D3D12_DEPTH_STENCIL_DESC m_desc = {};
	};
}
