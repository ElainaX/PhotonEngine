#include "BlendState.h"

namespace photon 
{

	BlendState::BlendState(BlendEquation equation, UINT8 writeMask /*= D3D12_COLOR_WRITE_ENABLE_ALL*/)
	{
		m_RenderTarget0BlendDesc.BlendEnable = true;
		m_RenderTarget0BlendDesc.LogicOpEnable = false;
		m_RenderTarget0BlendDesc.SrcBlend = equation.colorEquation.srcBlend;
		m_RenderTarget0BlendDesc.DestBlend = equation.colorEquation.destBlend;
		m_RenderTarget0BlendDesc.BlendOp = equation.colorEquation.blendOp;
		m_RenderTarget0BlendDesc.SrcBlendAlpha = equation.alphaEquation.srcBlend;
		m_RenderTarget0BlendDesc.DestBlendAlpha = equation.alphaEquation.destBlend;
		m_RenderTarget0BlendDesc.BlendOpAlpha = equation.alphaEquation.blendOp;

		m_RenderTarget0BlendDesc.RenderTargetWriteMask = writeMask;
	}

	BlendState::BlendState(LogicOp op)
	{
		m_RenderTarget0BlendDesc.BlendEnable = false;
		m_RenderTarget0BlendDesc.LogicOpEnable = true;
		m_RenderTarget0BlendDesc.LogicOp = (D3D12_LOGIC_OP)op;
	}

	D3D12_BLEND_DESC BlendState::GetBlendDesc() const
	{
		D3D12_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = m_IsAlphaToConverageEnable;
		desc.IndependentBlendEnable = m_IsIndependentBlendEnable;
		desc.RenderTarget[0] = m_RenderTarget0BlendDesc;
		return desc;
	}

}