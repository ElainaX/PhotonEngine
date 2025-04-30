#include "DrawSkyboxSubPass.h"

namespace photon
{



	void DrawSkyboxSubPass::Initialize(RHI* _rhi)
	{
		m_Rhi = _rhi;
	}

	void DrawSkyboxSubPass::PrepareForData(RenderResourceData* data)
	{
		auto resourceData = dynamic_cast<DrawSkyboxSubPassData*>(data);
		skyboxCube = resourceData->skyboxRenderItem;
		renderTargetView = resourceData->renderTargetView;
		depthStencilView = resourceData->depthStencilView;
		cubemap = resourceData->cubemap;

		m_Shader = skyboxCube->shader;

		if (pipeline == nullptr)
		{
			pipeline = std::make_shared<DXGraphicsPipeline>();
			m_RootSignature = m_Rhi->CreateRootSignature(m_Shader, 6, m_Rhi->GetStaticSamplers().data());
			pipeline->SetCounterClockWise();
			DepthStencilState state;
			state.depthState.depthCompareOp = DepthStencilOp::LessEqualThan;
			pipeline->SetDepthStencilState(state);
			pipeline->SetShaderMust(m_Shader, {}, m_RootSignature.Get());
			pipeline->SetRenderTargetMust({ renderTargetView->resource->dxDesc.Format });
			pipeline->FinishOffRenderSet(m_Rhi);
		}

		if (!texGuidToShaderResourceViews.contains(cubemap->guid))
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			desc.TextureCube.MostDetailedMip = 0;
			desc.TextureCube.MipLevels = cubemap->dxDesc.MipLevels;
			desc.TextureCube.ResourceMinLODClamp = 0.0f;
			desc.Format = cubemap->dxDesc.Format;
			texGuidToShaderResourceViews[cubemap->guid] = m_Rhi->CreateShaderResourceView(cubemap, &desc, texGuidToShaderResourceViews[cubemap->guid]);
		}

		m_PassConstantIdx = resourceData->passConstantIdx;
	}

	void DrawSkyboxSubPass::Draw(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport)
	{
		assert(skyboxCube);

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
		auto cubemapSlot = m_Shader->GetPhotonRootSignature()->GetTableParameterIndex(cubemapInTable);

		auto cubemapView = texGuidToShaderResourceViews[cubemap->guid];

		m_Rhi->CmdSetGraphicsRootDescriptorTable(cubemapSlot, cubemapView->gpuHandleInHeap);
		m_Rhi->CmdSetGraphicsRootDescriptorTable(passConstantsSlot, passView->gpuHandleInHeap);

		auto meshCollection = skyboxCube->meshCollection;
		m_Rhi->CmdSetVertexBuffers(0, 1, &meshCollection->VertexBufferView());
		m_Rhi->CmdSetIndexBuffer(&meshCollection->IndexBufferView());

		auto mesh = meshCollection->GetMesh(skyboxCube->meshGuid);
		m_Rhi->CmdSetPrimitiveTopology(skyboxCube->primitiveType);
		m_Rhi->CmdDrawIndexedInstanced(mesh->indexCount, 1, mesh->startIndexLocation, mesh->baseVertexLocation, 0);
		
	}

}