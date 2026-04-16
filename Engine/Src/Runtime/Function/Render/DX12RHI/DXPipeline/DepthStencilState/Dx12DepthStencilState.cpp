#include "Dx12DepthStencilState.h"

namespace photon
{
	void Dx12DepthStencilState::Build(const DepthStencilStateDesc& desc)
	{
		m_desc = ToNative(desc);
	}

	D3D12_DEPTH_STENCIL_DESC Dx12DepthStencilState::ToNative(const DepthStencilStateDesc& inDesc)
	{
		DepthStencilStateDesc desc = inDesc;
		desc.Canonicalize();

		D3D12_DEPTH_STENCIL_DESC nativeDesc = {};
		nativeDesc.DepthEnable = desc.depthEnable ? TRUE : FALSE;
		nativeDesc.DepthWriteMask = ToD3D12WriteMask(desc.depthWriteMask);
		nativeDesc.DepthFunc = ToD3D12Compare(desc.depthFunc);
		nativeDesc.StencilEnable = desc.stencilEnable ? TRUE : FALSE;
		nativeDesc.StencilReadMask = desc.stencilReadMask;
		nativeDesc.StencilWriteMask = desc.stencilWriteMask;
		nativeDesc.FrontFace = ToD3D12StencilFace(desc.frontFace);
		nativeDesc.BackFace = ToD3D12StencilFace(desc.backFace);
		return nativeDesc;
	}

	D3D12_COMPARISON_FUNC Dx12DepthStencilState::ToD3D12Compare(CompareOp op)
	{
		switch (op)
		{
		case CompareOp::Never: return D3D12_COMPARISON_FUNC_NEVER;
		case CompareOp::Less: return D3D12_COMPARISON_FUNC_LESS;
		case CompareOp::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
		case CompareOp::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case CompareOp::Greater: return D3D12_COMPARISON_FUNC_GREATER;
		case CompareOp::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case CompareOp::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case CompareOp::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
		default: return D3D12_COMPARISON_FUNC_LESS;
		}
	}

	D3D12_DEPTH_WRITE_MASK Dx12DepthStencilState::ToD3D12WriteMask(DepthWriteMask mask)
	{
		return mask == DepthWriteMask::All ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	}

	D3D12_STENCIL_OP Dx12DepthStencilState::ToD3D12StencilOp(StencilOp op)
	{
		switch (op)
		{
		case StencilOp::Keep: return D3D12_STENCIL_OP_KEEP;
		case StencilOp::Zero: return D3D12_STENCIL_OP_ZERO;
		case StencilOp::Replace: return D3D12_STENCIL_OP_REPLACE;
		case StencilOp::IncrSat: return D3D12_STENCIL_OP_INCR_SAT;
		case StencilOp::DecrSat: return D3D12_STENCIL_OP_DECR_SAT;
		case StencilOp::Invert: return D3D12_STENCIL_OP_INVERT;
		case StencilOp::Incr: return D3D12_STENCIL_OP_INCR;
		case StencilOp::Decr: return D3D12_STENCIL_OP_DECR;
		default: return D3D12_STENCIL_OP_KEEP;
		}
	}

	D3D12_DEPTH_STENCILOP_DESC Dx12DepthStencilState::ToD3D12StencilFace(const StencilFaceDesc& face)
	{
		D3D12_DEPTH_STENCILOP_DESC nativeFace = {};
		nativeFace.StencilFailOp = ToD3D12StencilOp(face.failOp);
		nativeFace.StencilDepthFailOp = ToD3D12StencilOp(face.depthFailOp);
		nativeFace.StencilPassOp = ToD3D12StencilOp(face.passOp);
		nativeFace.StencilFunc = ToD3D12Compare(face.func);
		return nativeFace;
	}
}
