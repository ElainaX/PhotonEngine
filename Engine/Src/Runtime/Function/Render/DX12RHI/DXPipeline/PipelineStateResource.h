#pragma once 
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <d3d12.h>

#include "Function/Render/ResourceHandle.h"
#include "Function/Render/Shader/ShaderEnum.h"


namespace photon 
{
	using PipelineStateKind = ShaderProgramKind;
	struct PipelineStateResource
	{
		PipelineStateHandle handle;
		PipelineStateKind kind = PipelineStateKind::Graphics;

		ShaderHandle shaderProgram;
		RootSignatureHandle rootSignature;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> native;
	};


}
