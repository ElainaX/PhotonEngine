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
			rootSignature = rhi->CreateRootSignature(shader, 6, rhi->GetStaticSamplers().data());
			pipeline->SetShaderMust(shader, data.macros, rootSignature.Get());
			pipeline->SetRenderTargetMust({ data.renderTargetView->resource->dxDesc.Format });
			pipeline->SetWireFrameMode();
			pipeline->FinishOffRenderSet(rhi);
		}
		if (data.renderItems.empty())
			return;
		commonRenderItems = std::move(data.renderItems);
		for(auto& ritem : commonRenderItems)
		{
			if(texGuidToShaderResourceViews.contains(ritem->material->diffuseMap->guid))
			{
				continue;
			}
			D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
			auto resourceTex = ritem->material->diffuseMap;
			desc.Texture2D.MipLevels = resourceTex->dxDesc.MipLevels;
			desc.Format = resourceTex->dxDesc.Format;
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			texGuidToShaderResourceViews[resourceTex->guid] = rhi->CreateShaderResourceView(resourceTex, &desc, texGuidToShaderResourceViews[resourceTex->guid]);
			
		}
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
		auto matDataConstantTableIndex = shader->GetPhotonRootSignature()->GetTableParameterIndex(matDataConstantInTable);
		auto tex0TableIndex = shader->GetPhotonRootSignature()->GetTableParameterIndex(textureInTable);
		rhi->CmdSetGraphicsRootDescriptorTable(passConstantTableIndex, passView->gpuHandleInHeap);

		for(int i = 0; i < commonRenderItems.size(); ++i)
		{
			auto ritem = commonRenderItems[i];
			auto objectView = currFrameResource->GetObjectConstantBufferView(ritem->frameResourceInfo.objConstantIdx);
			auto matView = currFrameResource->GetMatDataConstantBufferView(ritem->material->matCBufferIdx);
			rhi->CmdSetGraphicsRootDescriptorTable(objectConstantTableIndex, objectView->gpuHandleInHeap);
			rhi->CmdSetGraphicsRootDescriptorTable(matDataConstantTableIndex, matView->gpuHandleInHeap);
			rhi->CmdSetGraphicsRootDescriptorTable(tex0TableIndex, texGuidToShaderResourceViews[ritem->material->diffuseMap->guid]->gpuHandleInHeap);
			
			auto meshCollection = ritem->meshCollection;
			rhi->CmdSetVertexBuffers(0, 1, &meshCollection->VertexBufferView());
			rhi->CmdSetIndexBuffer(&meshCollection->IndexBufferView());

			auto mesh = meshCollection->GetMesh(ritem->meshGuid);
			rhi->CmdSetPrimitiveTopology(ritem->primitiveType);
			rhi->CmdDrawIndexedInstanced(mesh->indexCount, 1, mesh->startIndexLocation, mesh->baseVertexLocation, 0);

		}
	}

}