#pragma once
#include <d3d12.h>
#include "Macro.h"

namespace photon
{
	struct StencilRef
	{
		UINT8 ref;
	};

	struct StencilValue {};
	
	enum class DepthStencilOp
	{
		AlwaysFailed = D3D12_COMPARISON_FUNC_NEVER,
		LessThan = D3D12_COMPARISON_FUNC_LESS,
		EqualTo = D3D12_COMPARISON_FUNC_EQUAL,
		LessEqualThan = D3D12_COMPARISON_FUNC_LESS_EQUAL,
		GreaterThan = D3D12_COMPARISON_FUNC_GREATER,
		NotEqualTo = D3D12_COMPARISON_FUNC_NOT_EQUAL,
		GreaterEqualThan = D3D12_COMPARISON_FUNC_GREATER_EQUAL,
		AlwaysSuccess = D3D12_COMPARISON_FUNC_ALWAYS
	};

	enum class StencilUpdateStrategy
	{
		Keep = D3D12_STENCIL_OP_KEEP,
		SetZero = D3D12_STENCIL_OP_ZERO,
		SetToOnRenderBindRefValue = D3D12_STENCIL_OP_REPLACE,
		Plus1Clamp = D3D12_STENCIL_OP_INCR_SAT,
		Minus1Clamp = D3D12_STENCIL_OP_DECR_SAT,
		Invert = D3D12_STENCIL_OP_INVERT,
		Plus1Loop = D3D12_STENCIL_OP_INCR,
		Minus1Loop = D3D12_STENCIL_OP_DECR
	};

	struct DepthState 
	{
		bool isDepthTestEnbale = true;
		bool isDepthWriteEnable = true;
		DepthStencilOp depthCompareOp = DepthStencilOp::LessThan;
	};

	struct StencilState
	{
		void SetStencilTestEquation(StencilRef _refValue, UINT refValueBitAndReadMask, DepthStencilOp op, StencilValue place, UINT stencilValueBitAndReadMask = 0)
		{
			refValue = _refValue;
			PHOTON_ASSERT(refValueBitAndReadMask == stencilValueBitAndReadMask || stencilValueBitAndReadMask == 0, "StencilReadMask Must Be Same!");
			stencilReadMask = refValueBitAndReadMask;
			frontStencilCompareOp = op;
			backStencilCompareOp = op;
		}

		void SetReadAndWriteMask(UINT readMask, UINT writeMask = D3D12_DEFAULT_STENCIL_WRITE_MASK)
		{
			stencilReadMask = readMask;
			stencilWriteMask = writeMask;
		}

		void SetBackRefAndOp(DepthStencilOp op)
		{
			backStencilCompareOp = op;
		}

		void SetFrontRefAndOp(DepthStencilOp op)
		{
			frontStencilCompareOp = op;
		}

		void SetFrontStencilUpdateStrategies(StencilUpdateStrategy _onStencilTestFailed, StencilUpdateStrategy _onStencilTestPassButDepthTestFailed, StencilUpdateStrategy _onDepthStencilTestPass)
		{
			onFrontStencilTestFailed = _onStencilTestFailed;
			onFrontStencilTestPassButDepthTestFailed = _onStencilTestPassButDepthTestFailed;
			onFrontStencilDepthTestPass = _onDepthStencilTestPass;
		}

		void SetBackStencilUpdateStrategies(StencilUpdateStrategy _onStencilTestFailed, StencilUpdateStrategy _onStencilTestPassButDepthTestFailed, StencilUpdateStrategy _onDepthStencilTestPass)
		{
			onBackStencilTestFailed = _onStencilTestFailed;
			onBackStencilTestPassButDepthTestFailed = _onStencilTestPassButDepthTestFailed;
			onBackStencilDepthTestPass = _onDepthStencilTestPass;
		}

		bool isStencilTestEnable = false;
		UINT8 stencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		UINT8 stencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		StencilUpdateStrategy onFrontStencilTestFailed = StencilUpdateStrategy::Keep;
		StencilUpdateStrategy onFrontStencilTestPassButDepthTestFailed = StencilUpdateStrategy::Keep;
		StencilUpdateStrategy onFrontStencilDepthTestPass = StencilUpdateStrategy::Keep;
		StencilUpdateStrategy onBackStencilTestFailed = StencilUpdateStrategy::Keep;
		StencilUpdateStrategy onBackStencilTestPassButDepthTestFailed = StencilUpdateStrategy::Keep;
		StencilUpdateStrategy onBackStencilDepthTestPass = StencilUpdateStrategy::Keep;
		DepthStencilOp frontStencilCompareOp = DepthStencilOp::AlwaysSuccess;
		DepthStencilOp backStencilCompareOp = DepthStencilOp::AlwaysSuccess;
		StencilRef refValue;
	};

	struct DepthStencilState
	{
	public:
		DepthStencilState() = default;
		
		D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc() const;


		DepthState depthState;
		StencilState stencilState;
	};
}