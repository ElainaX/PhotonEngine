#include "PreprocessPass.h"
#include "Function/Global/RuntimeGlobalContext.h"
#include "Function/Render/RenderSystem.h"

namespace photon 
{


	void PreprocessPass::Initialize(RHI* rhi)
	{
		m_Rhi = rhi;

		for(int i = 0; i < MaxCascadedNum; ++i)
		{
			m_PassConstantsIndices.push_back(StaticModelPassConstants::s_CurrPassIndex++);
		}
		m_PassConstantses.resize(MaxCascadedNum);
		m_CascadedShadowManager = std::make_shared<CascadedShadowManager>();
		m_CascadedShadowManager->Initialize({ 1024, 1024 }, 3);

		m_ShadowSubPass = std::make_shared<DrawShadowSubPass>();
		m_ShadowSubPass->Initialize(m_Rhi);
		m_ShadowShader = g_RuntimeGlobalContext.renderSystem->GetShaderFactory()->Create(L"DrawShadowMap");
	}

	void PreprocessPass::PrepareContext(RenderResourceData* data)
	{
		auto preprocessData = static_cast<PreprocessPassRenderResourceData*>(data);
		m_CascadedShadowManager->Track(preprocessData->mainLight, preprocessData->mainCamera);
		preprocessData->cascadedShadowManager = m_CascadedShadowManager;
		auto viewProjMatrices = m_CascadedShadowManager->GetViewAndProjMatrices(preprocessData->spliters);

		DrawShadowSubPassData shadowData;


		// Do Shadow Render
		for(int i = 0; i < viewProjMatrices.size(); ++i)
		{
			auto[viewMat, projMat] = viewProjMatrices[i];
			auto viewProj = XMMatrixMultiply(viewMat, projMat);

			XMStoreFloat4x4(&m_PassConstantses[i].view, viewMat);
			XMStoreFloat4x4(&m_PassConstantses[i].proj, projMat);
			XMStoreFloat4x4(&m_PassConstantses[i].viewProj, viewProj);


			StaticModelFrameResource* frameResource = (StaticModelFrameResource*)m_Rhi->GetCurrFrameResource(FrameResourceType::StaticModelFrameResource);
			frameResource->UpdatePassConstantBuffer(m_PassConstantsIndices[i], &m_PassConstantses[i]);

			shadowData.passConstantses.push_back(m_PassConstantsIndices[i]);

		}

		
		shadowData.cascadedShadowManager = m_CascadedShadowManager;
		shadowData.renderItems = preprocessData->allRenderItems;
		shadowData.shadowShader = m_ShadowShader;
		m_ShadowSubPass->PrepareForData(&shadowData);
	}

	void PreprocessPass::Draw()
	{
		m_ShadowSubPass->Draw(D3D12_RECT{}, D3D12_VIEWPORT{});
	}

}