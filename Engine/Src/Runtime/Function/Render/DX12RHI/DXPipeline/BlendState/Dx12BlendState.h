#pragma once

#include <d3d12.h>
#include "Function/Render/DX12RHI/DXPipeline/GraphicsPipelineState.h"

namespace photon
{
	class Dx12BlendState
	{
	public:
		Dx12BlendState() = default;
		explicit Dx12BlendState(const BlendStateDesc& desc, uint32_t renderTargetCount = 1)
		{
			Build(desc, renderTargetCount);
		}

		void Build(const BlendStateDesc& desc, uint32_t renderTargetCount = 1);
		const D3D12_BLEND_DESC& GetBlendDesc() const { return m_desc; }

		static D3D12_BLEND_DESC ToNative(const BlendStateDesc& desc, uint32_t renderTargetCount = 1);

	private:
		static D3D12_BLEND ToD3D12Blend(BlendFactor factor);
		static D3D12_BLEND_OP ToD3D12BlendOp(BlendOp op);
		static D3D12_LOGIC_OP ToD3D12LogicOp(LogicOp op);

	private:
		D3D12_BLEND_DESC m_desc = {};
	};
}
