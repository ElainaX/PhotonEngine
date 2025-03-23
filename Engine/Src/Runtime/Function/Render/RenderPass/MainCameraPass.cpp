#include "MainCameraPass.h"

namespace photon 
{
	void MainCameraPass::Initialize(RHI* rhi, WindowSystem* windowSystem)
	{
		m_Rhi = rhi;
		m_WindowSystem = windowSystem;

		m_PassConstantsIdx = StaticModelPassConstants::s_CurrPassIndex;
		m_PassConstants.color = Vector4{ 0.2, 0.5, 0.0, 1.0 };

		m_TestSubpass = std::make_shared<TestSubPass>();
		m_TestSubpass->Initialize(rhi);
	}

	void MainCameraPass::PrepareContext(RenderResourceData* data)
	{
		// Update FramePassConstants
		StaticModelFrameResource* frameResource = (StaticModelFrameResource*)m_Rhi->GetCurrFrameResource(FrameResourceType::StaticModelFrameResource);
		frameResource->UpdatePassConstantBuffer(m_PassConstantsIdx, &m_PassConstants);

		MainPassRenderResourceData* renderResource = dynamic_cast<MainPassRenderResourceData*>(data);
 
		m_TestRenderTargetView = m_Rhi->CreateRenderTargetView(renderResource->renderTarget.get(), nullptr, m_TestRenderTargetView);
		m_TestDepthStencilView = m_Rhi->CreateDepthStencilView(renderResource->depthStencil.get(), nullptr, m_TestDepthStencilView);

		TestSubPassData TestSubpassData;
		TestSubpassData.renderTargetView = m_TestRenderTargetView;
		TestSubpassData.depthStencilView = m_TestDepthStencilView;
		TestSubpassData.renderItems = renderResource->allRenderItems;
		TestSubpassData.shader = TestSubpassData.renderItems[0]->shader;
		TestSubpassData.macros = {};
		TestSubpassData.passConstantIdx = m_PassConstantsIdx;

		m_TestSubpass->PrepareForData(TestSubpassData);

	}

	void MainCameraPass::Draw()
	{
		auto [width, height] = m_WindowSystem->GetClientWidthAndHeight();
		D3D12_RECT scissorRect = { 0, 0, width, height };
		D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f};
		
		
		m_TestSubpass->Draw(scissorRect, viewport);



		m_Rhi->CopyTextureToSwapChain(dynamic_cast<Texture2D*>(m_TestRenderTargetView->resource));
		m_Rhi->PrepareForPresent();
		m_Rhi->Present();
	}

}