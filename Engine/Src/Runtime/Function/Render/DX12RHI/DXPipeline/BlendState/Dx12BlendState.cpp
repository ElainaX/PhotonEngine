#include "Dx12BlendState.h"

namespace photon
{
	void Dx12BlendState::Build(const BlendStateDesc& desc, uint32_t renderTargetCount)
	{
		m_desc = ToNative(desc, renderTargetCount);
	}

	D3D12_BLEND_DESC Dx12BlendState::ToNative(const BlendStateDesc& inDesc, uint32_t renderTargetCount)
	{
		BlendStateDesc desc = inDesc;
		desc.Canonicalize();

		if (renderTargetCount == 0)
			renderTargetCount = 1;
		if (renderTargetCount > 8)
			renderTargetCount = 8;

		D3D12_BLEND_DESC nativeDesc = {};
		nativeDesc.AlphaToCoverageEnable = desc.alphaToCoverageEnable ? TRUE : FALSE;
		nativeDesc.IndependentBlendEnable = FALSE;

		D3D12_RENDER_TARGET_BLEND_DESC rtDesc = {};
		rtDesc.BlendEnable = desc.blendEnable ? TRUE : FALSE;
		rtDesc.LogicOpEnable = desc.logicOpEnable ? TRUE : FALSE;
		rtDesc.SrcBlend = ToD3D12Blend(desc.srcColor);
		rtDesc.DestBlend = ToD3D12Blend(desc.dstColor);
		rtDesc.BlendOp = ToD3D12BlendOp(desc.colorOp);
		rtDesc.SrcBlendAlpha = ToD3D12Blend(desc.srcAlpha);
		rtDesc.DestBlendAlpha = ToD3D12Blend(desc.dstAlpha);
		rtDesc.BlendOpAlpha = ToD3D12BlendOp(desc.alphaOp);
		rtDesc.LogicOp = ToD3D12LogicOp(desc.logicOp);
		rtDesc.RenderTargetWriteMask = desc.colorWriteMask;

		for (uint32_t i = 0; i < 8; ++i)
		{
			nativeDesc.RenderTarget[i] = {};
		}
		for (uint32_t i = 0; i < renderTargetCount; ++i)
		{
			nativeDesc.RenderTarget[i] = rtDesc;
		}

		return nativeDesc;
	}

	D3D12_BLEND Dx12BlendState::ToD3D12Blend(BlendFactor factor)
	{
		switch (factor)
		{
		case BlendFactor::Zero: return D3D12_BLEND_ZERO;
		case BlendFactor::One: return D3D12_BLEND_ONE;
		case BlendFactor::SrcColor: return D3D12_BLEND_SRC_COLOR;
		case BlendFactor::InvSrcColor: return D3D12_BLEND_INV_SRC_COLOR;
		case BlendFactor::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
		case BlendFactor::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
		case BlendFactor::DstAlpha: return D3D12_BLEND_DEST_ALPHA;
		case BlendFactor::InvDstAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
		case BlendFactor::DstColor: return D3D12_BLEND_DEST_COLOR;
		case BlendFactor::InvDstColor: return D3D12_BLEND_INV_DEST_COLOR;
		case BlendFactor::SrcAlphaSat: return D3D12_BLEND_SRC_ALPHA_SAT;
		case BlendFactor::Blendfactor: return D3D12_BLEND_BLEND_FACTOR;
		case BlendFactor::InvBlendfactor: return D3D12_BLEND_INV_BLEND_FACTOR;
		default: return D3D12_BLEND_ONE;
		}
	}

	D3D12_BLEND_OP Dx12BlendState::ToD3D12BlendOp(BlendOp op)
	{
		switch (op)
		{
		case BlendOp::Add: return D3D12_BLEND_OP_ADD;
		case BlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
		case BlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
		case BlendOp::Min: return D3D12_BLEND_OP_MIN;
		case BlendOp::Max: return D3D12_BLEND_OP_MAX;
		default: return D3D12_BLEND_OP_ADD;
		}
	}

	D3D12_LOGIC_OP Dx12BlendState::ToD3D12LogicOp(LogicOp op)
	{
		switch (op)
		{
		case LogicOp::Noop: return D3D12_LOGIC_OP_NOOP;
		case LogicOp::Clear: return D3D12_LOGIC_OP_CLEAR;
		case LogicOp::Set: return D3D12_LOGIC_OP_SET;
		case LogicOp::Copy: return D3D12_LOGIC_OP_COPY;
		case LogicOp::CopyInverted: return D3D12_LOGIC_OP_COPY_INVERTED;
		case LogicOp::Invert: return D3D12_LOGIC_OP_INVERT;
		case LogicOp::And: return D3D12_LOGIC_OP_AND;
		case LogicOp::Nand: return D3D12_LOGIC_OP_NAND;
		case LogicOp::Or: return D3D12_LOGIC_OP_OR;
		case LogicOp::Nor: return D3D12_LOGIC_OP_NOR;
		case LogicOp::Xor: return D3D12_LOGIC_OP_XOR;
		case LogicOp::Equiv: return D3D12_LOGIC_OP_EQUIV;
		case LogicOp::AndReverse: return D3D12_LOGIC_OP_AND_REVERSE;
		case LogicOp::AndInverted: return D3D12_LOGIC_OP_AND_INVERTED;
		case LogicOp::OrReverse: return D3D12_LOGIC_OP_OR_REVERSE;
		case LogicOp::OrInverted: return D3D12_LOGIC_OP_OR_INVERTED;
		default: return D3D12_LOGIC_OP_NOOP;
		}
	}
}
