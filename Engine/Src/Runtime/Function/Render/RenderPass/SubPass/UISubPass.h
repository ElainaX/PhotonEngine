#pragma once
#include "Function/Render/RenderObject/RenderItem.h"
#include "Function/Render/RHI.h"
#include "Function/Render/Shader/TestShader.h"
#include "Function/Render/DX12RHI/DescriptorHeap/RtvDescriptorHeap.h"
#include "Function/Render/DX12RHI/DescriptorHeap/CbvSrvUavDescriptorHeap.h"
#include "Function/Render/DX12RHI/DXPipeline/DXGraphicsPipeline.h"
#include "Function/Render/RenderResourceData.h"

namespace photon 
{
	class UISubPass
	{
	public:
		void Initialize(RHI* _rhi);
		void PrepareForData(RenderResourceData* data);
		void Draw(EG_FrameContext* frame, PassBlackboard* bb);

		RenderTargetView* renderTargetView;
		DepthStencilView* depthStencilView;

	private:
		RHI* m_Rhi = nullptr;
	};
}