#pragma once 

#include <d3d12.h>
#include "Macro.h"

namespace photon 
{
	enum class BlendFactorValueType
	{
		zero = D3D12_BLEND_ZERO,
		one = D3D12_BLEND_ONE,
		srcColor = D3D12_BLEND_SRC_COLOR,
		oneMinusSrcColor = D3D12_BLEND_INV_SRC_COLOR,
		srcAlpha = D3D12_BLEND_SRC_ALPHA,
		oneMinusSrcAlpha = D3D12_BLEND_INV_SRC_ALPHA,
		destAlpha = D3D12_BLEND_DEST_ALPHA,
		oneMinusDestAlpha = D3D12_BLEND_INV_DEST_ALPHA,
		destColor = D3D12_BLEND_DEST_COLOR,
		oneMinusDestColor = D3D12_BLEND_INV_DEST_COLOR,
		unUsed0 = D3D12_BLEND_SRC_ALPHA_SAT,
		onRenderBind = D3D12_BLEND_BLEND_FACTOR,
		oneMinusOnRenderBind = D3D12_BLEND_INV_BLEND_FACTOR,
		unUsed1 = D3D12_BLEND_SRC1_COLOR,
		unUsed2 = D3D12_BLEND_INV_SRC1_COLOR,
		unUsed3 = D3D12_BLEND_SRC1_ALPHA,
		unUsed4 = D3D12_BLEND_INV_SRC1_ALPHA,
		onRenderBindAlpha = D3D12_BLEND_ALPHA_FACTOR,
		oneMinusonRenderBindAlpha = D3D12_BLEND_INV_ALPHA_FACTOR
	};

	enum class BlendOp
	{
		add = D3D12_BLEND_OP_ADD,
		subtract = D3D12_BLEND_OP_SUBTRACT,
		min = D3D12_BLEND_OP_MIN,
		max = D3D12_BLEND_OP_MAX
	};
	
	enum class LogicOp
	{
		ClearRenderTargetTo0 = D3D12_LOGIC_OP_CLEAR,
		ClearRenderTargetTo1 = D3D12_LOGIC_OP_SET,
		CopySrcToDest = D3D12_LOGIC_OP_COPY,
		InvertSrc = D3D12_LOGIC_OP_COPY_INVERTED, // ~s
		DoNothing = D3D12_LOGIC_OP_NOOP,    
		InvertDest = D3D12_LOGIC_OP_INVERT, // ~d
		And = D3D12_LOGIC_OP_AND, // s & d
		NotAnd = D3D12_LOGIC_OP_NAND, //~ (s & d)
		Or = D3D12_LOGIC_OP_OR, // s | d
		NotOr = D3D12_LOGIC_OP_NOR, // ~ (s | d)
		XOR = D3D12_LOGIC_OP_XOR, // s ^ d
		InvertXOR = D3D12_LOGIC_OP_EQUIV, // ~(s ^ d)
		SrcAndInvertDest = D3D12_LOGIC_OP_AND_REVERSE, // s & ~d
		InvertSrcAndDest = D3D12_LOGIC_OP_AND_INVERTED, // ~s & d
		SrcOrInvertDest = D3D12_LOGIC_OP_OR_REVERSE, // s | ~d
		InvertSrcOrDest = D3D12_LOGIC_OP_OR_INVERTED, // ~s | d
	};

	struct BlendSrc{};
	struct BlendSrcAlpha {};
	struct BlendDst{};
	struct BlendDstAlpha{};


	struct BlendSrcF 
	{
		BlendSrcF(BlendFactorValueType f):factor(f){}
		BlendFactorValueType factor;
	};
	struct BlendDstF 
	{
		BlendDstF(BlendFactorValueType f) :factor(f) {}
		BlendFactorValueType factor;
	};

	struct BlendColorEquation
	{
		BlendColorEquation(D3D12_BLEND src, D3D12_BLEND dest, D3D12_BLEND_OP op)
		{
			SetBlendValue(src, dest, op);
		}
		BlendColorEquation(BlendSrc src, BlendSrcF fsrc, BlendOp op, BlendDst dst, BlendDstF fdst)
		{
			SetBlendValue((D3D12_BLEND)fsrc.factor, (D3D12_BLEND)fdst.factor,(D3D12_BLEND_OP)op);
		}
		BlendColorEquation(BlendDst dst, BlendDstF fdst, BlendOp op, BlendSrc src, BlendSrcF fsrc)
		{
			PHOTON_ASSERT(op == BlendOp::subtract, "BlendOp only support dest minus src");
			SetBlendValue((D3D12_BLEND)fsrc.factor, (D3D12_BLEND)fdst.factor, D3D12_BLEND_OP_REV_SUBTRACT);
		}

		void SetBlendValue(D3D12_BLEND src, D3D12_BLEND dest, D3D12_BLEND_OP op)
		{
			srcBlend = src;
			destBlend = dest;
			blendOp = op;
		}
		D3D12_BLEND srcBlend;
		D3D12_BLEND destBlend;
		D3D12_BLEND_OP blendOp;
	};

	struct BlendAlphaEquation
	{
		BlendAlphaEquation(D3D12_BLEND src, D3D12_BLEND dest, D3D12_BLEND_OP op)
		{
			SetBlendValue(src, dest, op);
		}
		BlendAlphaEquation(BlendSrcAlpha src, BlendSrcF fsrc, BlendOp op, BlendDstAlpha dst, BlendDstF fdst)
		{
			SetBlendValue((D3D12_BLEND)fsrc.factor, (D3D12_BLEND)fdst.factor, (D3D12_BLEND_OP)op);
		}
		BlendAlphaEquation(BlendDstAlpha dst, BlendDstF fdst, BlendOp op, BlendSrcAlpha src, BlendSrcF fsrc)
		{
			PHOTON_ASSERT(op == BlendOp::subtract, "BlendOp only support dest minus src");
			SetBlendValue((D3D12_BLEND)fsrc.factor, (D3D12_BLEND)fdst.factor, D3D12_BLEND_OP_REV_SUBTRACT);
		}

		void SetBlendValue(D3D12_BLEND src, D3D12_BLEND dest, D3D12_BLEND_OP op)
		{
			srcBlend = src;
			destBlend = dest;
			blendOp = op;
		}
		D3D12_BLEND srcBlend;
		D3D12_BLEND destBlend;
		D3D12_BLEND_OP blendOp;
	};

	struct BlendEquation
	{
		BlendEquation()
			: colorEquation(GetDefaultColorEquation()), alphaEquation(GetDefaultAlphaEquation())
		{
		}
		BlendEquation(BlendColorEquation _colorEquation, BlendAlphaEquation _alphaEquation)
			: colorEquation(_colorEquation), alphaEquation(_alphaEquation){}
		BlendEquation(BlendColorEquation _colorEquation, bool alphaUseSame = false)
			: colorEquation(_colorEquation), alphaEquation(colorEquation.srcBlend, colorEquation.destBlend, colorEquation.blendOp)
		{
			if (!alphaUseSame)
				alphaEquation = GetDefaultAlphaEquation();
		}
		BlendEquation(BlendAlphaEquation _alphaEquation, bool colorUseSame = false)
			: alphaEquation(_alphaEquation), colorEquation(alphaEquation.srcBlend, alphaEquation.destBlend, alphaEquation.blendOp) 
		{
			if (!colorUseSame)
				colorEquation = GetDefaultColorEquation();
		}

		static BlendColorEquation GetDefaultColorEquation()
		{
			return BlendColorEquation(BlendSrc(), BlendSrcF(BlendFactorValueType::one), BlendOp::add, BlendDst(), BlendDstF(BlendFactorValueType::zero));
		}
		static BlendAlphaEquation GetDefaultAlphaEquation()
		{
			return BlendAlphaEquation(BlendSrcAlpha(), BlendSrcF(BlendFactorValueType::one), BlendOp::add, BlendDstAlpha(), BlendDstF(BlendFactorValueType::zero));
		}

		BlendColorEquation colorEquation;
		BlendAlphaEquation alphaEquation;
	};

	// typedef enum D3D12_COLOR_WRITE_ENABLE {
//	D3D12_COLOR_WRITE_ENABLE_RED = 1,
//		D3D12_COLOR_WRITE_ENABLE_GREEN = 2,
//		D3D12_COLOR_WRITE_ENABLE_BLUE = 4,
//		D3D12_COLOR_WRITE_ENABLE_ALPHA = 8,
//		D3D12_COLOR_WRITE_ENABLE_ALL
//};

	struct BlendState
	{
		BlendState(BlendEquation equation, UINT8 writeMask = D3D12_COLOR_WRITE_ENABLE_ALL);
		BlendState(LogicOp op);

		void SetAlphaToConvergeEnbale(bool enable = true)
		{
			m_IsAlphaToConverageEnable = enable;
		}
		void SetIndependentBlendEnable(bool enable = true)
		{
			m_IsIndependentBlendEnable = enable;
		}
		D3D12_BLEND_DESC GetBlendDesc() const;



		// 暂时不想支持多个RenderTarget独立渲染
	private:
		bool m_IsAlphaToConverageEnable = false;
		bool m_IsIndependentBlendEnable = false;
		D3D12_RENDER_TARGET_BLEND_DESC m_RenderTarget0BlendDesc;
	};

}