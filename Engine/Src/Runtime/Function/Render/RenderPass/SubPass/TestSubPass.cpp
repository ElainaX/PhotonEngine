#include "TestSubPass.h"
#include "Function/Util/RenderUtil.h"

namespace photon
{

	void TestSubPass::Initialize(RHI* _rhi)
	{
		rhi = _rhi;
	}

	void TestSubPass::PrepareForData(const TestSubPassData& data)
	{
		shader = data.shader;
		passConstantIdx = data.passConstantIdx;
		renderTargetView = data.renderTargetView;
		depthStencilView = data.depthStencilView;
		if(!shader->GetMacros()->IsVariantLoaded(data.macros) || pipeline == nullptr)
		{
			pipeline = pipeline == nullptr ? std::make_shared<DXGraphicsPipeline>() : std::make_shared<DXGraphicsPipeline>(*pipeline);
			rootSignature = rhi->CreateRootSignature(shader);
			pipeline->SetShaderMust(shader, data.macros, rootSignature.Get());
			pipeline->SetRenderTargetMust({ data.renderTargetView->resource->dxDesc.Format });
			pipeline->FinishOffRenderSet(rhi);
		}
		if (data.renderItems.empty())
			return;
		commonRenderItems = std::move(data.renderItems);
	}

	void TestSubPass::Draw(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport)
	{
		if (commonRenderItems.empty())
			return;
		auto RenderTex = (Texture2D*)renderTargetView->resource;
		auto DepthTex = (Texture2D*)depthStencilView->resource;

		rhi->ResourceStateTransform(RenderTex, D3D12_RESOURCE_STATE_RENDER_TARGET);
		rhi->ResourceStateTransform(DepthTex, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		rhi->CmdClearRenderTarget(renderTargetView, RenderTex->clearValue);
		rhi->CmdClearDepthStencil(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, DepthTex->clearValue.x, RenderUtil::FloatRoundToUINT(DepthTex->clearValue.y));

		rhi->CmdSetViewportsAndScissorRects(scissorRect, viewport);
		//rhi->SetPipelineState(m_PipelineState.Get());
		rhi->CmdSetPipelineState(pipeline->GetDXPipelineState());
		rhi->CmdSetGraphicsRootSignature(rootSignature.Get());

		rhi->CmdSetRenderTargets(1, &renderTargetView->cpuHandleInHeap, true, &depthStencilView->cpuHandleInHeap);

		/*rhi->CmdSetDescriptorHeaps({ m_CbvUavSrvHeap->GetDXHeapPtr() });*/

		auto currFrameResource = (StaticModelFrameResource*)rhi->GetCurrFrameResource(frameResourceType);
		auto passView = currFrameResource->GetPassConstantBufferView(passConstantIdx);
		auto passConstantTableIndex = shader->GetPhotonRootSignature()->GetTableParameterIndex(passConstantInTable);
		auto objectConstantTableIndex = shader->GetPhotonRootSignature()->GetTableParameterIndex(objectConstantInTable);
		rhi->CmdSetGraphicsRootDescriptorTable(passConstantTableIndex, passView->gpuHandleInHeap);

		for(int i = 0; i < commonRenderItems.size(); ++i)
		{
			auto ritem = commonRenderItems[i];
			auto objectView = currFrameResource->GetObjectConstantBufferView(ritem->frameResourceInfo.objConstantIdx);
			rhi->CmdSetGraphicsRootDescriptorTable(objectConstantTableIndex, objectView->gpuHandleInHeap);
			
			auto meshCollection = ritem->meshCollection;
			rhi->CmdSetVertexBuffers(0, 1, &meshCollection->VertexBufferView());
			rhi->CmdSetIndexBuffer(&meshCollection->IndexBufferView());

			auto mesh = meshCollection->GetMesh(ritem->meshGuid);
			rhi->CmdSetPrimitiveTopology(ritem->primitiveType);
			rhi->CmdDrawIndexedInstanced(mesh->indexCount, 1, mesh->startIndexLocation, mesh->baseVertexLocation, 0);

		}

		//auto passConstantTableIndex = shader->GetPhotonRootSignature()->GetTableParameterIndex(passConstantInTable);
		//auto passView = commonRenderItems[0]->frameResourceInfo.passConstantView;
		//


		//auto cbvView = m_ColorAView;
		//rhi->SetGraphicsRootDescriptorTable(tableIndex, cbvView->gpuHandleInHeap);
		////rhi->SetGraphicsRootConstantBufferView(0, m_ConstantBuffer->gpuResource->GetGPUVirtualAddress());

		//rhi->IASetVertexBuffers(0, 1, &m_RenderMeshCollection->VertexBufferView());
		//rhi->IASetIndexBuffer(&m_RenderMeshCollection->IndexBufferView());

		//auto mesh = m_RenderMeshCollection->GetMesh(m_RenderItem.meshGuid);
		//rhi->IASetPrimitiveTopology(m_RenderItem.primitiveType);
		//rhi->DrawIndexedInstanced(mesh->indexCount, 1, mesh->startIndexLocation, mesh->baseVertexLocation, 0);

		//cbvView = m_ColorBView;
		//rhi->SetPipelineState(m_GraphicsPipeline2->GetDXPipelineState());
		//rhi->SetGraphicsRootDescriptorTable(tableIndex, cbvView->gpuHandleInHeap);
		//auto mesh2 = m_RenderMeshCollection->GetMesh(m_RenderItem2.meshGuid);
		//rhi->IASetPrimitiveTopology(m_RenderItem2.primitiveType);
		//rhi->DrawIndexedInstanced(mesh2->indexCount, 1, mesh2->startIndexLocation, mesh2->baseVertexLocation, 0);
	}

}