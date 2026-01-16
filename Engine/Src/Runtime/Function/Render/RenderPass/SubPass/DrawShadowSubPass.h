#pragma once

#include <vector>
#include "Function/Render/RenderObject/RenderItem.h"
#include "Function/Render/RHI.h"
#include "Function/Render/Shader/CascadedShadowShader.h"
#include "Function/Render/DX12RHI/DescriptorHeap/RtvDescriptorHeap.h"
#include "Function/Render/DX12RHI/DescriptorHeap/CbvSrvUavDescriptorHeap.h"
#include "Function/Render/DX12RHI/DXPipeline/DXGraphicsPipeline.h"
#include "Function/Render/RenderResourceData.h"
#include "Function/Render/CascadedShadowManager.h"

namespace photon
{


	class DrawShadowSubPass
	{
	public:
		void Initialize(RHI* _rhi);
		void PrepareForData(RenderResourceData* data);
		void Draw(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport);


		std::vector<CommonRenderItem*> commonRenderItems;
		std::shared_ptr<CascadedShadowManager> cascadedShadowManager;
		std::shared_ptr<DXGraphicsPipeline> pipeline;

		ConstantBufferParameter objectConstantInTable = ConstantBufferParameter(0);
		ConstantBufferParameter passConstantInTable = ConstantBufferParameter(1);
		FrameResourceType frameResourceType = FrameResourceType::StaticModelFrameResource;

	private:
		Shader* m_Shader;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		RHI* m_Rhi;
		std::vector<int> m_PassConstantsIndices;
	};
}