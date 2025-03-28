﻿#include "MainCameraPass.h"

namespace photon 
{
	void MainCameraPass::Initialize(RHI* rhi, WindowSystem* windowSystem)
	{
		m_Rhi = rhi;
		m_WindowSystem = windowSystem;

		m_PassConstantsIdx = StaticModelPassConstants::s_CurrPassIndex++;
		//m_PassConstants.color = Vector4{ 0.2, 0.5, 0.0, 1.0 };

		m_TestSubpass = std::make_shared<TestSubPass>();
		m_TestSubpass->Initialize(rhi);
	}

	void MainCameraPass::PrepareContext(RenderResourceData* data)
	{


		MainPassRenderResourceData* renderResource = dynamic_cast<MainPassRenderResourceData*>(data);

		auto renderCamera = renderResource->mainCamera;
		auto viewMat = renderCamera->GetViewMatrix();
		auto viewDet = XMMatrixDeterminant(viewMat);
		auto invViewMat = XMMatrixInverse(&viewDet, viewMat);
		auto projMat = renderCamera->GetProjMatrix();
		auto projDet = XMMatrixDeterminant(projMat);
		auto invProjMat = XMMatrixInverse(&projDet, projMat);
		auto viewProj = XMMatrixMultiply(viewMat, projMat);
		auto invViewProj = XMMatrixMultiply(invProjMat, invViewMat);
		Vector3 eyePos = renderCamera->pos;
		Vector2 renderTargetSize = m_WindowSystem->GetClientWidthAndHeight();
		Vector2 invRenderTargetSize = 1.0f / renderTargetSize;
		float znear = renderCamera->znear;
		float zfar = renderCamera->zfar;
		float totalTime = renderResource->gameTimer->TotalTime();
		float deltaTime = renderResource->gameTimer->DeltaTime();

		XMStoreFloat4x4(&m_PassConstants.view, viewMat);
		XMStoreFloat4x4(&m_PassConstants.invView, invViewMat);
		XMStoreFloat4x4(&m_PassConstants.proj, projMat);
		XMStoreFloat4x4(&m_PassConstants.invProj, invProjMat);
		XMStoreFloat4x4(&m_PassConstants.viewProj, viewProj);
		XMStoreFloat4x4(&m_PassConstants.invViewProj, invViewProj);
		m_PassConstants.eyePos = eyePos;
		m_PassConstants.renderTargetSize = renderTargetSize;
		m_PassConstants.invRenderTargetSize = invRenderTargetSize;
		m_PassConstants.znear = znear; 
		m_PassConstants.zfar = zfar;
		m_PassConstants.totalTime = totalTime;
		m_PassConstants.deltaTime = deltaTime;
		m_PassConstants.ambientLight = { 0.1, 0.1, 0.1, 1.0 };

		m_TestRenderTargetView = m_Rhi->CreateRenderTargetView(renderResource->renderTarget.get(), nullptr, m_TestRenderTargetView);
		m_TestDepthStencilView = m_Rhi->CreateDepthStencilView(renderResource->depthStencil.get(), nullptr, m_TestDepthStencilView);

		//auto resourceTex = renderResource->resourceTex;
		//D3D12_SHADER_RESOURCE_VIEW_DESC resourceViewDesc = {};
		//resourceViewDesc.Texture2D.MipLevels = resourceTex->dxDesc.MipLevels;
		//resourceViewDesc.Format = resourceTex->dxDesc.Format;
		//resourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		//resourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		//m_TestShaderResourceView = m_Rhi->CreateShaderResourceView(resourceTex.get(), &resourceViewDesc, m_TestShaderResourceView);

		TestSubPassData TestSubpassData;
		//TestSubpassData.texView = m_TestShaderResourceView;
		TestSubpassData.renderTargetView = m_TestRenderTargetView;
		TestSubpassData.depthStencilView = m_TestDepthStencilView;
		TestSubpassData.renderItems = renderResource->allRenderItems;
		TestSubpassData.shader = TestSubpassData.renderItems[0]->shader;
		TestSubpassData.macros = {};
		TestSubpassData.passConstantIdx = m_PassConstantsIdx;

		// Update FramePassConstants

		StaticModelFrameResource* frameResource = (StaticModelFrameResource*)m_Rhi->GetCurrFrameResource(FrameResourceType::StaticModelFrameResource);
		frameResource->UpdatePassConstantBuffer(m_PassConstantsIdx, &m_PassConstants);

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