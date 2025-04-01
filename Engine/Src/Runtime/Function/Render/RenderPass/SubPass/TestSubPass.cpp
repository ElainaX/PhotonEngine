#include "TestSubPass.h"
#include "Function/Util/RenderUtil.h"

namespace photon
{

	void TestSubPass::Initialize(RHI* _rhi)
	{
		m_Rhi = _rhi;
	}

	void TestSubPass::PrepareForData(RenderResourceData* _data)
	{
		auto data = dynamic_cast<TestSubPassData*>(_data);
		m_Shader = data->shader;
		m_PassConstantIdx = data->passConstantIdx;
		renderTargetView = data->renderTargetView;
		depthStencilView = data->depthStencilView;
		if(!m_Shader->GetMacros()->IsVariantLoaded(data->macros) || pipeline == nullptr)
		{
			pipeline = pipeline == nullptr ? std::make_shared<DXGraphicsPipeline>() : std::make_shared<DXGraphicsPipeline>(*pipeline);
			m_RootSignature = m_Rhi->CreateRootSignature(m_Shader, 6, m_Rhi->GetStaticSamplers().data());
			pipeline->SetShaderMust(m_Shader, data->macros, m_RootSignature.Get());
			pipeline->SetRenderTargetMust({ data->renderTargetView->resource->dxDesc.Format });
			//pipeline->SetWireFrameMode();
			pipeline->FinishOffRenderSet(m_Rhi);
		}
		if (data->renderItems.empty())
			return;
		commonRenderItems = data->renderItems;
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
			texGuidToShaderResourceViews[resourceTex->guid] = m_Rhi->CreateShaderResourceView(resourceTex, &desc, texGuidToShaderResourceViews[resourceTex->guid]);
			
		}
	}

	void TestSubPass::Draw(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport)
	{
		if (commonRenderItems.empty())
			return;
		auto RenderTex = (Texture2D*)renderTargetView->resource;
		auto DepthTex = (Texture2D*)depthStencilView->resource;

		m_Rhi->ResourceStateTransform(RenderTex, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_Rhi->ResourceStateTransform(DepthTex, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		m_Rhi->CmdClearRenderTarget(renderTargetView, RenderTex->clearValue);
		m_Rhi->CmdClearDepthStencil(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, DepthTex->clearValue.x, RenderUtil::FloatRoundToUINT(DepthTex->clearValue.y));

		m_Rhi->CmdSetViewportsAndScissorRects(scissorRect, viewport);
		//rhi->SetPipelineState(m_PipelineState.Get());
		m_Rhi->CmdSetPipelineState(pipeline->GetDXPipelineState());
		m_Rhi->CmdSetGraphicsRootSignature(m_RootSignature.Get());

		m_Rhi->CmdSetRenderTargets(1, &renderTargetView->cpuHandleInHeap, true, &depthStencilView->cpuHandleInHeap);

		/*rhi->CmdSetDescriptorHeaps({ m_CbvUavSrvHeap->GetDXHeapPtr() });*/

		auto currFrameResource = (StaticModelFrameResource*)m_Rhi->GetCurrFrameResource(frameResourceType);
		auto passView = currFrameResource->GetPassConstantBufferView(m_PassConstantIdx);
		auto passConstantTableIndex = m_Shader->GetPhotonRootSignature()->GetTableParameterIndex(passConstantInTable);
		auto objectConstantTableIndex = m_Shader->GetPhotonRootSignature()->GetTableParameterIndex(objectConstantInTable);
		auto matDataConstantTableIndex = m_Shader->GetPhotonRootSignature()->GetTableParameterIndex(matDataConstantInTable);
		auto tex0TableIndex = m_Shader->GetPhotonRootSignature()->GetTableParameterIndex(textureInTable);
		m_Rhi->CmdSetGraphicsRootDescriptorTable(passConstantTableIndex, passView->gpuHandleInHeap);

		for(int i = 0; i < commonRenderItems.size(); ++i)
		{
			auto ritem = commonRenderItems[i];
			auto objectView = currFrameResource->GetObjectConstantBufferView(ritem->frameResourceInfo.objConstantIdx);
			auto matView = currFrameResource->GetMatDataConstantBufferView(ritem->material->matCBufferIdx);
			m_Rhi->CmdSetGraphicsRootDescriptorTable(objectConstantTableIndex, objectView->gpuHandleInHeap);
			m_Rhi->CmdSetGraphicsRootDescriptorTable(matDataConstantTableIndex, matView->gpuHandleInHeap);
			m_Rhi->CmdSetGraphicsRootDescriptorTable(tex0TableIndex, texGuidToShaderResourceViews[ritem->material->diffuseMap->guid]->gpuHandleInHeap);
			
			auto meshCollection = ritem->meshCollection;
			m_Rhi->CmdSetVertexBuffers(0, 1, &meshCollection->VertexBufferView());
			m_Rhi->CmdSetIndexBuffer(&meshCollection->IndexBufferView());

			auto mesh = meshCollection->GetMesh(ritem->meshGuid);
			m_Rhi->CmdSetPrimitiveTopology(ritem->primitiveType);
			m_Rhi->CmdDrawIndexedInstanced(mesh->indexCount, 1, mesh->startIndexLocation, mesh->baseVertexLocation, 0);

		}
	}

}