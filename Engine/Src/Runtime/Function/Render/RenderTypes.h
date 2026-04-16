#pragma once

#include <cstdint>
#include <d3d12.h>

namespace photon 
{
	enum class RenderPipelineType : uint8_t
	{
		ForwardPipeline = 0,
		DifferedPipeline,
		RenderPipelineTypeCount

	};

	enum class PrimitiveTopologyType : uint8_t
	{
		Undefined,
		Point,
		Line,
		Triangle,
		Patch
	};

	enum class FillMode : uint8_t
	{
		Solid = D3D12_FILL_MODE_SOLID,
		Wireframe = D3D12_FILL_MODE_WIREFRAME
	};

	enum class CullMode :uint8_t
	{
		None = D3D12_CULL_MODE_NONE,
		Front = D3D12_CULL_MODE_FRONT,
		Back = D3D12_CULL_MODE_BACK
	};

	enum class CompareOp : uint8_t
	{
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always,
	};

	enum class DepthWriteMask : uint8_t
	{
		Zero,
		All,
	};

	enum class StencilOp : uint8_t
	{
		Keep,
		Zero,
		Replace,
		IncrSat,
		DecrSat,
		Invert,
		Incr,
		Decr,
	};

	enum class BlendFactor : uint8_t
	{
		Zero,
		One,
		SrcColor,
		InvSrcColor,
		SrcAlpha,
		InvSrcAlpha,
		DstAlpha,
		InvDstAlpha,
		DstColor,
		InvDstColor,
		SrcAlphaSat,
		Blendfactor,
		InvBlendfactor,
	};

	enum class BlendOp : uint8_t
	{
		Add,
		Subtract,
		RevSubtract,
		Min,
		Max,
	};

	enum class LogicOp : uint8_t
	{
		Noop,
		Clear,
		Set,
		Copy,
		CopyInverted,
		Invert,
		And,
		Nand,
		Or,
		Nor,
		Xor,
		Equiv,
		AndReverse,
		AndInverted,
		OrReverse,
		OrInverted,
	};

	enum ColorWriteMask : uint8_t
	{
		ColorWrite_None = 0,
		ColorWrite_R = 1 << 0,
		ColorWrite_G = 1 << 1,
		ColorWrite_B = 1 << 2,
		ColorWrite_A = 1 << 3,
		ColorWrite_All = ColorWrite_R | ColorWrite_G | ColorWrite_B | ColorWrite_A,
	};



}