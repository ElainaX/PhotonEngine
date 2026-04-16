#pragma once

#include <d3d12.h>
#include "Function/Render/DX12RHI/DXPipeline/GraphicsPipelineState.h"

namespace photon
{
	class Dx12RasterState
	{
	public:
		Dx12RasterState() = default;
		explicit Dx12RasterState(const RasterStateDesc& desc)
		{
			Build(desc);
		}

		void Build(const RasterStateDesc& desc);
		const D3D12_RASTERIZER_DESC& GetRasterizerDesc() const { return m_desc; }

		static D3D12_RASTERIZER_DESC ToNative(const RasterStateDesc& desc);

	private:
		static D3D12_FILL_MODE ToD3D12FillMode(FillMode fillMode);
		static D3D12_CULL_MODE ToD3D12CullMode(CullMode cullMode);

	private:
		D3D12_RASTERIZER_DESC m_desc = {};
	};
}
