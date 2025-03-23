#pragma once
#include "Function/Render/RenderPass.h"
#include "Function/Render/RenderResourceData.h"
#include "Function/Render/RHI.h"
#include "../Shader/TestShader.h"
#include "../RenderObject/RenderMeshCollection.h"
#include "../DX12RHI/DescriptorHeap/Descriptor.h"
#include "../DX12RHI/DXPipeline/DXGraphicsPipeline.h"
#include "Function/Render/WindowSystem.h"
#include "SubPass/TestSubPass.h"
#include "../RenderResourceData/MainPassRenderResourceData.h"
#include "../DX12RHI/FrameResource/StaticModelFrameResource.h"

#include <map>
#include <memory>

namespace photon 
{
	class MainCameraPass : public RenderPass
	{
	public:
		void Initialize(RHI* rhi, WindowSystem* windowSystem);

		void PrepareContext(RenderResourceData*) override;

		void Draw() override;
		
	private:
		std::shared_ptr<TestSubPass> m_TestSubpass;
		RenderTargetView* m_TestRenderTargetView = nullptr;
		DepthStencilView* m_TestDepthStencilView = nullptr;

		StaticModelPassConstants m_PassConstants;
		UINT m_PassConstantsIdx;

	private:
		//std::map<Resource*, ViewBase*> m_ResourceToViews;
		//ViewBase* m_ColorAView;
		//ViewBase* m_ColorBView;
		//Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		//std::shared_ptr<DXGraphicsPipeline> m_GraphicsPipeline;
		//std::shared_ptr<DXGraphicsPipeline> m_GraphicsPipeline2;
		////Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
		////std::shared_ptr<Texture2D> m_RenderTex;
		////std::shared_ptr<Texture2D> m_DepthStencilTex;
		//std::shared_ptr<RenderMeshCollection> m_RenderMeshCollection;
		//CommonRenderItem m_RenderItem;
		//CommonRenderItem m_RenderItem2;
		//std::shared_ptr<TestShader> m_TestShader;
		//std::shared_ptr<Buffer> m_ConstantBuffer;

		WindowSystem* m_WindowSystem;
	};
}