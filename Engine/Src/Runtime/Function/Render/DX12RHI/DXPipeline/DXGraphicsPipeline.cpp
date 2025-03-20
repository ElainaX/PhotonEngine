#include "DXGraphicsPipeline.h"
#include "Macro.h"

namespace photon 
{


	DXGraphicsPipeline::DXGraphicsPipeline()
	{
		this->Initialize();
	}

	void DXGraphicsPipeline::SetShaderMust(Shader* shader, const std::vector<MacroInfo>& macros, ID3D12RootSignature* rootSignature)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_UsedShader = shader;
		m_UsedShaderBlob = shader->Compile(macros);

		// Wait to initialize
		auto& inputLayout = shader->GetShaderInputLayout();
		m_PipelineDesc.InputLayout = { inputLayout.data(), (unsigned int)inputLayout.size() };
		m_PipelineDesc.VS = m_UsedShaderBlob->GetVSShaderByteCode();
		m_PipelineDesc.PS = m_UsedShaderBlob->GetPSShaderByteCode();
		m_PipelineDesc.DS = m_UsedShaderBlob->GetDSShaderByteCode();
		m_PipelineDesc.HS = m_UsedShaderBlob->GetHSShaderByteCode();
		m_PipelineDesc.GS = m_UsedShaderBlob->GetGSShaderByteCode();

		m_PipelineDesc.pRootSignature = rootSignature;
	}

	void DXGraphicsPipeline::SetBlendState(const BlendState& state)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.BlendState = state.GetBlendDesc();
	}

	void DXGraphicsPipeline::SetDepthStencilState(const DepthStencilState& state, DXGI_FORMAT format)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.DepthStencilState = state.GetDepthStencilDesc();
		m_PipelineDesc.DSVFormat = format;
		stencilRef = state.stencilState.refValue.ref;
	}

	void DXGraphicsPipeline::SetPrimitiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.PrimitiveTopologyType = topologyType;
	}

	void DXGraphicsPipeline::SetRenderTargetMust(const std::vector<DXGI_FORMAT>& renderTargets)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.NumRenderTargets = renderTargets.size();
		for(int i = 0; i < renderTargets.size(); ++i)
		{
			m_PipelineDesc.RTVFormats[i] = renderTargets[i];
		}
	}

	void DXGraphicsPipeline::SetMultiSampleState(DXGI_SAMPLE_DESC sampleDesc, bool enableMultiSample)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.SampleDesc = sampleDesc;
		SetMultiSampleEnable(enableMultiSample);
	}

	void DXGraphicsPipeline::SetPipelineStateFlag(D3D12_PIPELINE_STATE_FLAGS flag)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.Flags = flag;
	}

	void DXGraphicsPipeline::SetBlendFactorOnRender(const std::vector<float>& _blendFactor)
	{
		size_t sz = _blendFactor.size() < blendFactor.size() ? _blendFactor.size() : blendFactor.size();
		for(int i = 0; i < sz; ++i)
		{
			blendFactor[i] = _blendFactor[i];
		}
	}

	void DXGraphicsPipeline::SetDepthStencilValueOnRender(UINT8 _stencilRef)
	{
		stencilRef = _stencilRef;
	}

	void DXGraphicsPipeline::SetWireFrameMode(bool useWareFrame /*= true*/)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.RasterizerState.FillMode = useWareFrame ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
	}

	void DXGraphicsPipeline::SetCullMode(D3D12_CULL_MODE cullMode)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.RasterizerState.CullMode = cullMode;
	}

	void DXGraphicsPipeline::SetCounterClockWise(bool useCounterClockWise /*= true*/)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.RasterizerState.FrontCounterClockwise = useCounterClockWise;
	}

	void DXGraphicsPipeline::SetMultiSampleEnable(bool useMultiSample /*= false*/)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.RasterizerState.MultisampleEnable = useMultiSample;
	}

	void DXGraphicsPipeline::SetAntialiasedLine(bool AntialiasedLineEnable /*= false*/)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.RasterizerState.AntialiasedLineEnable = AntialiasedLineEnable;
	}

	void DXGraphicsPipeline::SetDepthBias(int depthBias, float slopeScaledDepthBias, float maxDepthBias)
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		m_PipelineDesc.RasterizerState.DepthBias = depthBias;
		m_PipelineDesc.RasterizerState.DepthBiasClamp = maxDepthBias;
		m_PipelineDesc.RasterizerState.SlopeScaledDepthBias = slopeScaledDepthBias;
	}

	void DXGraphicsPipeline::FinishOffRenderSet(RHI* rhi)
	{
		m_PipelineState = rhi->CreateGraphicsPipelineState(&m_PipelineDesc);
	}

	ID3D12PipelineState* DXGraphicsPipeline::GetDXPipelineState(RHI* rhi)
	{
		if(m_PipelineState == nullptr)
		{
			PHOTON_ASSERT(rhi != nullptr, "Pipeline Not Manully Finished!");
			FinishOffRenderSet(rhi);
		}
		return m_PipelineState.Get();
	}

	void DXGraphicsPipeline::Initialize()
	{
		PHOTON_ASSERT(m_PipelineState == nullptr, "");
		ZeroMemory(&m_PipelineDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		m_PipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		m_PipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		m_PipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		m_PipelineDesc.SampleMask = UINT_MAX;
		m_PipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_PipelineDesc.SampleDesc.Count = 1;
		m_PipelineDesc.SampleDesc.Quality = 0;
		m_PipelineDesc.NodeMask = 0;
		m_PipelineDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_PipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	}

}