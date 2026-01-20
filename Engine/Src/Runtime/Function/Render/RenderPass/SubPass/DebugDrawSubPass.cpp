#include "DebugDrawSubPass.h"
#include <algorithm>

namespace photon 
{



	void DebugDrawSubPass::Initialize(RHI* _rhi)
	{
		m_Rhi = _rhi;
	}

	void DebugDrawSubPass::PrepareForData(RenderResourceData* _data)
	{
		auto data = dynamic_cast<DebugDrawLightPassData*>(_data);
		auto frame = data->frame;
		lightRenderItems = data->lightRenderItems;
		wireframeRenderItems.clear();
		auto innerRenderItems = data->frame->renderlists
						.innerRitems[frame->services.rhi->GetCurrFrameResource(FrameResourceType::StaticModelFrameResource)];
		std::for_each(innerRenderItems.begin(), innerRenderItems.end(), [this](std::shared_ptr<CommonRenderItem> ri) {
			if (ri->wireframeOn)
				wireframeRenderItems.push_back(ri.get());
			});
		renderTargetView = data->frame->services.rhi->CreateRenderTargetView(data->bb->Get<Texture2D>("back_buffer").get(), nullptr, renderTargetView);
		depthStencilView = data->frame->services.rhi->CreateDepthStencilView(data->bb->Get<Texture2D>("depth_stencil_buffer").get(), nullptr, depthStencilView);

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
			wireframePipeline = std::make_shared<DXGraphicsPipeline>(*pipeline);
			wireframePipeline->SetCullMode(D3D12_CULL_MODE_NONE);
			wireframePipeline->SetWireFrameMode(true);
			wireframePipeline->FinishOffRenderSet(m_Rhi);
		}

		m_PassConstantIdx = data->passConstantIdx;
	}

	void DebugDrawSubPass::Draw(EG_FrameContext* frame, PassBlackboard* bb)
	{
		if (lightRenderItems.empty())
			return;


		D3D12_RECT scissorRect = { 0, 0, frame->uniforms.viewportSize.x, frame->uniforms.viewportSize.y };
		D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)frame->uniforms.viewportSize.x, (float)frame->uniforms.viewportSize.y, 0.0f, 1.0f };

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
			auto objectView = currFrameResource->GetObjectConstantBufferView(ritem->objConstantIdx);

			m_Rhi->CmdSetGraphicsRootDescriptorTable(objectConstantsSlot, objectView->gpuHandleInHeap);
			m_Rhi->CmdSetGraphicsRootDescriptorTable(passConstantsSlot, passView->gpuHandleInHeap);

			auto meshCollection = ritem->meshCollection;
			m_Rhi->CmdSetVertexBuffers(0, 1, &meshCollection->VertexBufferView());
			m_Rhi->CmdSetIndexBuffer(&meshCollection->IndexBufferView());

			auto mesh = meshCollection->GetMesh(ritem->meshGuid);
			m_Rhi->CmdSetPrimitiveTopology(ritem->primitiveType);
			m_Rhi->CmdDrawIndexedInstanced(mesh->indexCount, 1, mesh->startIndexLocation, mesh->baseVertexLocation, 0);
		}




		// wireFrame Render

		m_Rhi->ResourceStateTransform(RenderTex, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_Rhi->ResourceStateTransform(DepthTex, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		m_Rhi->CmdSetViewportsAndScissorRects(scissorRect, viewport);
		//rhi->SetPipelineState(m_PipelineState.Get());
		m_Rhi->CmdSetPipelineState(wireframePipeline->GetDXPipelineState());
		m_Rhi->CmdSetGraphicsRootSignature(m_RootSignature.Get());

		m_Rhi->CmdSetRenderTargets(1, &renderTargetView->cpuHandleInHeap, true, &depthStencilView->cpuHandleInHeap);

		for (auto& ritem : wireframeRenderItems)
		{
			auto objectView = currFrameResource->GetObjectConstantBufferView(ritem->objConstantIdx);

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