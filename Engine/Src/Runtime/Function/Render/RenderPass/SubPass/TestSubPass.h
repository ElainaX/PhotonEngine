#pragma once

#include <vector>
#include "Function/Render/RenderObject/RenderItem.h"
#include "Function/Render/RHI.h"
#include "Function/Render/Shader/TestShader.h"
#include "Function/Render/DX12RHI/DescriptorHeap/RtvDescriptorHeap.h"
#include "Function/Render/DX12RHI/DXPipeline/DXGraphicsPipeline.h"

namespace photon 
{
	struct TestSubPassData
	{
		Shader* shader;
		std::vector<MacroInfo> macros;
		RenderTargetView* renderTargetView = nullptr;
		DepthStencilView* depthStencilView = nullptr;
		std::vector<CommonRenderItem*> renderItems;
		UINT passConstantIdx = 0;
		// 用staticModel FrameResource
	};

	class TestSubPass
	{
	public:
		void Initialize(RHI* _rhi);
		void PrepareForData(const TestSubPassData& data);
		void Draw(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport);


		std::vector<CommonRenderItem*> commonRenderItems;
		RenderTargetView* renderTargetView;
		DepthStencilView* depthStencilView;
		std::shared_ptr<DXGraphicsPipeline> pipeline;

		ConstantBufferParameter objectConstantInTable = ConstantBufferParameter(0);
		ConstantBufferParameter passConstantInTable = ConstantBufferParameter(1);
		FrameResourceType frameResourceType = FrameResourceType::StaticModelFrameResource;

	private:
		Shader* shader;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		RHI* rhi;
		UINT passConstantIdx = 0;
	};
}