#pragma once 

#include "DXPipeline.h"
#include "Function/Render/Shader/Shader.h"
#include "Function/Render/DX12RHI/d3dx12.h"
#include "Function/Render/RHI.h"
#include "BlendState/BlendState.h"
#include "DepthStencilState/DepthStencilState.h"

#include <wrl.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <vector>
#include <array>
#include <vector>

namespace photon 
{
	class DXGraphicsPipeline : public DXPipeline
	{
	public:
		DXGraphicsPipeline();
		DXGraphicsPipeline(const DXGraphicsPipeline& rhs)
		{
			m_UsedShaderBlob = rhs.m_UsedShaderBlob;
			m_UsedShader = rhs.m_UsedShader;
			m_PipelineDesc = rhs.m_PipelineDesc;
			m_PipelineState = nullptr;
		}
		DXGraphicsPipeline(DXGraphicsPipeline&& rhs)
		{
			m_UsedShaderBlob = rhs.m_UsedShaderBlob;
			m_UsedShader = rhs.m_UsedShader;
			m_PipelineDesc = rhs.m_PipelineDesc;
			m_PipelineState = rhs.m_PipelineState;

			rhs.m_UsedShaderBlob = nullptr;
			rhs.m_UsedShader = nullptr;
			rhs.m_PipelineState = nullptr;
		}

		void SetShaderMust(Shader* shader, const std::vector<MacroInfo>& macros, ID3D12RootSignature* rootSignature);
		void SetBlendState(const BlendState& state);
		void SetDepthStencilState(const DepthStencilState& state, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);
		void SetPrimitiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType);
		void SetRenderTargetMust(const std::vector<DXGI_FORMAT>& renderTargets);
		void SetMultiSampleState(DXGI_SAMPLE_DESC sampleDesc, bool enableMultiSample = true);
		void SetPipelineStateFlag(D3D12_PIPELINE_STATE_FLAGS flag);


		void SetBlendFactorOnRender(const std::vector<float>& _blendFactor);
		void SetDepthStencilValueOnRender(UINT8 _stencilRef);

		void SetWireFrameMode(bool useWareFrame = true);
		void SetCullMode(D3D12_CULL_MODE cullMode);
		void SetCounterClockWise(bool useCounterClockWise = true);
		void SetMultiSampleEnable(bool useMultiSample = false);
		void SetAntialiasedLine(bool AntialiasedLineEnable = false);
		void SetDepthBias(int depthBias, float slopeScaledDepthBias, float maxDepthBias);

		void FinishOffRenderSet(RHI* rhi);
		ID3D12PipelineState* GetDXPipelineState(RHI* rhi = nullptr);

		std::array<float, 4> blendFactor;
		UINT8 stencilRef;

	protected:
		virtual void Initialize();

	private:
		
		ShaderBlob* m_UsedShaderBlob = nullptr;
		Shader* m_UsedShader = nullptr;	
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_PipelineDesc;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
	};


}