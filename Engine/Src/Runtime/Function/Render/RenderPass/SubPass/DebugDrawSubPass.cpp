#include "DebugDrawSubPass.h"

namespace photon 
{



	void DebugDrawSubPass::Initialize(RHI* _rhi)
	{
		m_Rhi = _rhi;
	}

	void DebugDrawSubPass::PrepareForData(RenderResourceData* data)
	{
		auto resourceData = dynamic_cast<DebugDrawLightPassData*>(data);
		lightRenderItems = resourceData->lightRenderItems;
		renderTargetView = resourceData->renderTargetView;
		depthStencilView = resourceData->depthStencilView;

		if (lightRenderItems.empty())
			return;

		m_Shader = lightRenderItems[0]->shader;

		if (pipeline == nullptr)
		{
			pipeline = std::make_shared<DXGraphicsPipeline>();
			m_RootSignature = m_Rhi->CreateRootSignature(m_Shader, 0, nullptr);
			pipeline->SetShaderMust(m_Shader, {}, m_RootSignature.Get());
			pipeline->SetRenderTargetMust({ renderTargetView->resource->dxDesc.Format });
			pipeline->FinishOffRenderSet(m_Rhi);
		}

		m_PassConstantIdx = resourceData->passConstantIdx;
	}

	void DebugDrawSubPass::Draw(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport)
	{
		if (lightRenderItems.empty())
			return;

		auto RenderTex = (Texture2D*)renderTargetView->resource;
		auto DepthTex = (Texture2D*)depthStencilView->resource;

		m_Rhi->ResourceStateTransform(RenderTex, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_Rhi->ResourceStateTransform(DepthTex, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		m_Rhi->CmdSetViewportsAndScissorRects(scissorRect, viewport);
		//rhi->SetPipelineState(m_PipelineState.Get());
		m_Rhi->CmdSetPipelineState(pipeline->GetDXPipelineState());
		m_Rhi->CmdSetGraphicsRootSignature(m_RootSignature.Get());

		m_Rhi->CmdSetRenderTargets(1, &renderTargetView->cpuHandleInHeap, true, &depthStencilView->cpuHandleInHeap);

		auto currFrameResource = (StaticModelFrameResource*)m_Rhi->GetCurrFrameResource(frameResourceType);
		
		auto passView = currFrameResource->GetPassConstantBufferView(m_PassConstantIdx);
	
		auto passConstantsSlot = m_Shader->GetPhotonRootSignature()->GetTableParameterIndex(passConstantInTable);
		auto objectConstantsSlot = m_Shader->GetPhotonRootSignature()->GetTableParameterIndex(objectConstantInTable);

		for(auto& ritem : lightRenderItems)
		{
			auto objectView = currFrameResource->GetObjectConstantBufferView(ritem->frameResourceInfo.objConstantIdx);

			m_Rhi->CmdSetGraphicsRootDescriptorTable(objectConstantsSlot, objectView->gpuHandleInHeap);
			m_Rhi->CmdSetGraphicsRootDescriptorTable(passConstantsSlot, passView->gpuHandleInHeap);

			auto meshCollection = ritem->meshCollection;
			m_Rhi->CmdSetVertexBuffers(0, 1, &meshCollection->VertexBufferView());
			m_Rhi->CmdSetIndexBuffer(&meshCollection->IndexBufferView());

			auto mesh = meshCollection->GetMesh(ritem->meshGuid);
			m_Rhi->CmdSetPrimitiveTopology(ritem->primitiveType);
			m_Rhi->CmdDrawIndexedInstanced(mesh->indexCount, 1, mesh->startIndexLocation, mesh->baseVertexLocation, 0);
		}
	}

}