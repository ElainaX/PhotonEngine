#include "Dx12RasterState.h"

namespace photon
{
	void Dx12RasterState::Build(const RasterStateDesc& desc)
	{
		m_desc = ToNative(desc);
	}

	D3D12_RASTERIZER_DESC Dx12RasterState::ToNative(const RasterStateDesc& inDesc)
	{
		RasterStateDesc desc = inDesc;
		desc.Canonicalize();

		D3D12_RASTERIZER_DESC nativeDesc = {};
		nativeDesc.FillMode = ToD3D12FillMode(desc.fillMode);
		nativeDesc.CullMode = ToD3D12CullMode(desc.cullMode);
		nativeDesc.FrontCounterClockwise = desc.frontCCW ? TRUE : FALSE;
		nativeDesc.DepthBias = desc.depthBias;
		nativeDesc.DepthBiasClamp = desc.depthBiasClamp;
		nativeDesc.SlopeScaledDepthBias = desc.slopeScaledDepthBias;
		nativeDesc.DepthClipEnable = desc.depthClipEnable ? TRUE : FALSE;
		nativeDesc.MultisampleEnable = desc.multisampleEnable ? TRUE : FALSE;
		nativeDesc.AntialiasedLineEnable = desc.antialiasedLineEnable ? TRUE : FALSE;
		nativeDesc.ForcedSampleCount = desc.forcedSampleCount;
		nativeDesc.ConservativeRaster = desc.conservativeRaster ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		return nativeDesc;
	}

	D3D12_FILL_MODE Dx12RasterState::ToD3D12FillMode(FillMode fillMode)
	{
		return fillMode == FillMode::Wireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
	}

	D3D12_CULL_MODE Dx12RasterState::ToD3D12CullMode(CullMode cullMode)
	{
		switch (cullMode)
		{
		case CullMode::None: return D3D12_CULL_MODE_NONE;
		case CullMode::Front: return D3D12_CULL_MODE_FRONT;
		case CullMode::Back: return D3D12_CULL_MODE_BACK;
		default: return D3D12_CULL_MODE_BACK;
		}
	}
}
