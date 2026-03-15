#include "DrawShadowSubPass.h"

namespace photon 
{

	void DrawShadowSubPass::Initialize(RHI* _rhi)
	{
		m_Rhi = _rhi;
	}

	void DrawShadowSubPass::PrepareForData(RenderResourceData* data)
	{
		auto shadowData = static_cast<DrawShadowSubPassData*>(data);
		m_PassConstantsIndices = shadowData->passConstantses;
		m_Shader = shadowData->shadowShader;
		shadowRItems = shadowData->frame->renderlists.shadowCasters;

		if(pipeline == nullptr)
		{
			pipeline = std::make_shared<DXGraphicsPipeline>();
			pipeline->SetRenderTargetMust({});
			m_RootSignature = m_Rhi->CreateRootSignature(m_Shader, 6, m_Rhi->GetStaticSamplers().data());
			pipeline->SetShaderMust(m_Shader, {}, m_RootSignature.Get());
			pipeline->SetDepthBias(1000, 1.0f, 0.0f);
			pipeline->FinishOffRenderSet(m_Rhi);
		}
	}

	void DrawShadowSubPass::Draw(EG_FrameContext* frame, PassBlackboard* bb)
	{
		//if (shadowRItems.empty())
		//	return;
		auto cascadedShadowManager = bb->Get<CascadedShadowManager>("csm_mgr");
		auto DepthTexViewes = cascadedShadowManager->GetAllDepthStencilViews();
		auto Resolution = cascadedShadowManager->GetResolution();
		D3D12_VIEWPORT viewPort{0.0f, 0.0f, (float)Resolution.x, (float)Resolution.y, 0.0f, 1.0f};
		D3D12_RECT rect = { 0, 0, Resolution.x, Resolution.y };
		for(int i = 0; i < m_PassConstantsIndices.size(); ++i)
		{
			auto depthTexView = DepthTexViewes[i];
			m_Rhi->ResourceStateTransform(depthTexView->resource, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			m_Rhi->CmdClearDepthStencil(depthTexView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0, 0);
			m_Rhi->CmdSetViewportsAndScissorRects(rect, viewPort);
			m_Rhi->CmdSetPipelineState(pipeline->GetDXPipelineState());
			m_Rhi->CmdSetGraphicsRootSignature(m_RootSignature.Get());
			m_Rhi->CmdSetRenderTargets(0, nullptr, true, &depthTexView->cpuHandleInHeap);

			auto currFrameResource = (StaticModelFrameResource*)m_Rhi->GetCurrFrameResource(frameResourceType);
			auto passView = currFrameResource->GetPassConstantBufferView(m_PassConstantsIndices[i]);
			auto passConstantTableIndex = m_Shader->GetPhotonRootSignature()->GetTableParameterIndex(passConstantInTable);
			auto objectConstantTableIndex = m_Shader->GetPhotonRootSignature()->GetTableParameterIndex(objectConstantInTable);
			m_Rhi->CmdSetGraphicsRootDescriptorTable(passConstantTableIndex, passView->gpuHandleInHeap);

			for (int i = 0; i < shadowRItems.size(); ++i)
			{
				auto ritem = shadowRItems[i];
				auto objectView = currFrameResource->GetObjectConstantBufferView(ritem->objConstantIdx);
				m_Rhi->CmdSetGraphicsRootDescriptorTable(objectConstantTableIndex, objectView->gpuHandleInHeap);

				auto meshCollection = ritem->meshCollection;
				m_Rhi->CmdSetVertexBuffers(0, 1, &meshCollection->VertexBufferView());
				m_Rhi->CmdSetIndexBuffer(&meshCollection->IndexBufferView());

				auto mesh = meshCollection->GetMesh(ritem->meshGuid);
				m_Rhi->CmdSetPrimitiveTopology(ritem->primitiveType);
				m_Rhi->CmdDrawIndexedInstanced(mesh->indexCount, 1, mesh->startIndexLocation, mesh->baseVertexLocation, 0);

			}
		}
	}

}