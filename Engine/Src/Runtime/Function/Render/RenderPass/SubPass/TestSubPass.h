#pragma once

#include <vector>
#include "Function/Render/RenderObject/RenderItem.h"
#include "Function/Render/RHI.h"
#include "Function/Render/Shader/TestShader.h"
#include "Function/Render/DX12RHI/DescriptorHeap/RtvDescriptorHeap.h"
#include "Function/Render/DX12RHI/DescriptorHeap/CbvSrvUavDescriptorHeap.h"
#include "Function/Render/DX12RHI/DXPipeline/DXGraphicsPipeline.h"
#include "Function/Render/RenderResourceData.h"

namespace photon 
{


	class TestSubPass
	{
	public:
		void Initialize(RHI* _rhi);
		void PrepareForData(RenderResourceData* data);
		void Draw(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport);


		std::vector<CommonRenderItem*> commonRenderItems;
		std::map<UINT64, ShaderResourceView*> texGuidToShaderResourceViews;
		RenderTargetView* renderTargetView;
		DepthStencilView* depthStencilView;
		std::shared_ptr<DXGraphicsPipeline> pipeline;

		ConstantBufferParameter objectConstantInTable = ConstantBufferParameter(0);
		ConstantBufferParameter passConstantInTable = ConstantBufferParameter(1);
		ConstantBufferParameter matDataConstantInTable = ConstantBufferParameter(2);
		TextureParameter textureInTable = TextureParameter(0);
		FrameResourceType frameResourceType = FrameResourceType::StaticModelFrameResource;

	private:
		Shader* m_Shader;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		RHI* m_Rhi;
		UINT m_PassConstantIdx = 0;
	};
}