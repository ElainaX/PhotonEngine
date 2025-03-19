#pragma once 

#include "DXPipeline.h"


#include <wrl.h>
#include <d3d12.h>
#include <d3dcompiler.h>

namespace photon 
{
	class DXGraphicsPipeline : public DXPipeline
	{
	public:
		DXGraphicsPipeline() = default;



		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
	};


}