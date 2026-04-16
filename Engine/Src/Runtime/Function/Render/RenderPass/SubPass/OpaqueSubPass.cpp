#include "OpaqueSubPass.h"

#include "Function/Render/EGFrameContext.h"
#include "Function/Render/RenderPass/PassSort.h"
#include "Function/Render/RenderPass/SubPassHelpers.h"
#include "Function/Render/ResourceManager.h"
#include "Function/Util/RenderUtil.h"

namespace photon
{
	void OpaqueSubPass::Initialize()
	{
		BaseSubPass::Initialize();
		m_pipelineOverrides.rtLayout.rtvFormats[0] = m_rtvFormat;
		m_pipelineOverrides.rtLayout.rtvCount = 1;
		m_pipelineOverrides.rtLayout.dsvFormat = m_dsvFormat;
	}

	void OpaqueSubPass::Shutdown()
	{
		BaseSubPass::Shutdown();
	}

	void OpaqueSubPass::Prepare(const PassPrepareContext& ctx)
	{
		ClearDrawList();
		BuildDrawList(ctx);
		SortDrawList();
	}

	void OpaqueSubPass::Execute(const PassExecuteContext& ctx)
	{
		if (!ctx.cmd || !ctx.services || !ctx.services->resourceManager)
			return;

		auto mainPassCbv = ctx.blackboard->Get<FrameDescriptorHandle>("main_pass_cbv");


		ResourceManager& rm = *ctx.services->resourceManager;
		DX12CommandContext& cmd = *ctx.cmd;
		FrameAllocatorSystem& fa = *ctx.services->frameAllocator;

		PipelineStateHandle currentPipeline = {};
		RootSignatureHandle currentRootSig = {};
		MeshHandle currentMesh = {};
		MaterialHandle currentMaterial = {};


		for (const PassDrawItem& draw : m_drawList)
		{
			if (!draw.IsValid())
				continue;

			const MeshRenderResource* meshRR = rm.GetMeshRenderResource(draw.mesh);
			const MaterialRenderResource* matRR = rm.GetMaterialRenderResource(draw.material);
			const ShaderProgramRenderResource* shaderRR = rm.GetShaderProgramRenderResource(draw.shader);
			if (!meshRR || !matRR || !shaderRR)
				continue;
			if (draw.submeshIndex >= meshRR->submeshes.size())
				continue;

			if (currentPipeline != draw.pipeline)
			{
				ID3D12PipelineState* nativePso =
					rm.GetPipelineStateCache()->GetNative(draw.pipeline);
				if (!nativePso)
					continue;

				cmd.SetPipelineState(nativePso);
				currentPipeline = draw.pipeline;
			}

			if (currentRootSig != draw.rootSignature)
			{
				ID3D12RootSignature* nativeRS =
					rm.GetRootSignatureCache()->GetNative(draw.rootSignature);
				if (!nativeRS)
					continue;

				cmd.SetGraphicsRootSignature(nativeRS);
				currentRootSig = draw.rootSignature;

				if (mainPassCbv && mainPassCbv->IsValid())
				{
					int passCbSlot = RenderUtil::FindRootParameterIndex(*shaderRR, "PassCB");
					if (passCbSlot >= 0)
					{
						cmd.SetGraphicsRootDescriptorTable(
							passCbSlot,
							fa.GetGpuHandle(*mainPassCbv));
					}
				}
			}

			if (currentMaterial != draw.material)
			{
				currentMaterial = draw.material;

				int baseSlot = RenderUtil::FindRootParameterIndex(*shaderRR, "BaseColorTex");
				int normalSlot = RenderUtil::FindRootParameterIndex(*shaderRR, "NormalTex");
				int roughSlot = RenderUtil::FindRootParameterIndex(*shaderRR, "RoughnessTex");
				int matCbSlot = RenderUtil::FindRootParameterIndex(*shaderRR, "MaterialCB");

				if (baseSlot >= 0 && draw.baseColorSrv.IsValid())
				{
					cmd.SetGraphicsRootDescriptorTable(baseSlot, fa.GetGpuHandle(draw.baseColorSrv));
				}
				if (normalSlot >= 0 && draw.normalSrv.IsValid())
				{
					cmd.SetGraphicsRootDescriptorTable(normalSlot, fa.GetGpuHandle(draw.normalSrv));
				}
				if (roughSlot >= 0 && draw.roughnessSrv.IsValid())
				{
					cmd.SetGraphicsRootDescriptorTable(roughSlot, fa.GetGpuHandle(draw.roughnessSrv));
				}
				if (matCbSlot >= 0 && draw.materialCbv.IsValid())
				{
					cmd.SetGraphicsRootDescriptorTable(matCbSlot, fa.GetGpuHandle(draw.materialCbv));
				}
			}

			const MeshRenderResource* currMeshRR =
				currentMesh.handle.IsValid()
				? rm.GetMeshRenderResource(currentMesh)
				: nullptr;

			if (!currMeshRR ||
				currMeshRR->bigVb.buffer != meshRR->bigVb.buffer ||
				currMeshRR->bigIb.buffer != meshRR->bigIb.buffer)
			{
				auto vbv = meshRR->bigVb.GetView(ctx.services->gpuResManager);
				auto ibv = meshRR->bigIb.GetView(ctx.services->gpuResManager);
				cmd.SetVertexBuffers(0, 1, &vbv);
				cmd.SetIndexBuffer(&ibv);
				currentMesh = draw.mesh;
			}

			if (draw.objectCbv.IsValid())
			{
				int objSlot = RenderUtil::FindRootParameterIndex(*shaderRR, "ObjectCB");
				if (objSlot >= 0)
				{
					cmd.SetGraphicsRootDescriptorTable(objSlot, fa.GetGpuHandle(draw.objectCbv));
				}
			}


			const auto& sub = meshRR->submeshes[draw.submeshIndex];
			cmd.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			cmd.DrawIndexedInstanced(
				sub.indexCount,
				1,
				sub.indexStartInMesh,
				sub.vertexStartInMesh,
				0);
		}
	}

	void OpaqueSubPass::BuildDrawList(const PassPrepareContext& ctx)
	{
		if (!ctx.visibleItems || !ctx.services ||
			!ctx.services->resourceManager || !ctx.services->pipelineCache)
			return;

		ResourceManager& rm = *ctx.services->resourceManager;
		FrameAllocatorSystem& fa = *ctx.services->frameAllocator;

		for (const RenderItem* item : *ctx.visibleItems)
		{
			if (!item || !item->IsVisible())
				continue;

			if (item->IsTransparent())
				continue;

			if (!HasLayer(item->layers, RenderLayer::Opaque)
				&& !HasLayer(item->layers, RenderLayer::Default))
				continue;

			const MeshRenderResource* meshRR = rm.GetMeshRenderResource(item->mesh);
			if (!meshRR)
				continue;

			const uint64_t vertexLayoutHash =
				rm.GetVertexLayoutRegistry()->Register(meshRR->vertexLayout);

			for (uint32_t submeshIndex = 0; submeshIndex < meshRR->submeshes.size(); ++submeshIndex)
			{
				if (!IsSubmeshEnabled(*item, submeshIndex))
					continue;

				MaterialHandle material =
					ResolveRenderItemMaterial(rm, *item, *meshRR, submeshIndex);
				if (!material.handle.IsValid())
					continue;

				ShaderHandle shader = ResolveShaderForMaterial(rm, material);
				if (!shader.handle.IsValid())
					continue;

				const ShaderProgramRenderResource* shaderRR =
					rm.GetShaderProgramRenderResource(shader);
				if (!shaderRR)
					continue;

				PipelineStateHandle pipeline =
					ResolvePipelineForDrawItem(ctx, material, vertexLayoutHash, m_pipelineOverrides);
				if (!pipeline.handle.IsValid())
					continue;

				PassDrawItem draw = {};
				draw.item = item;
				draw.mesh = item->mesh;
				draw.material = material;
				draw.shader = shader;
				draw.submeshIndex = submeshIndex;
				draw.pipeline = pipeline;
				draw.rootSignature = shaderRR->rootSignature;
				draw.sortKey = BuildSortKey(draw, ctx);

				// ---- Prepare 阶段预先 attach descriptor ----
				const MaterialRenderResource* matRR = rm.GetMaterialRenderResource(material);
				const MaterialAsset* matAsset = matRR ? rm.GetMaterialAsset(matRR->assetGuid) : nullptr;

				if (matAsset)
				{
					int baseColorMapIdx = RenderUtil::FindMaterialTextureBindingIndex(*matAsset, "baseColorMap");
					int normalMapIdx = RenderUtil::FindMaterialTextureBindingIndex(*matAsset, "normalMap");
					int roughnessMapIdx = RenderUtil::FindMaterialTextureBindingIndex(*matAsset, "roughnessMap");

					if (baseColorMapIdx >= 0)
					{
						TextureHandle h = rm.GetTextureHandleByGuid(
							matAsset->textureBindings[baseColorMapIdx].textureGuid);
						if (const auto* rr = rm.GetTextureRenderResource(h); rr && rr->srv.IsValid())
						{
							draw.baseColorSrv = fa.AttachToFrameHeap(rr->srv);
						}
					}

					if (normalMapIdx >= 0)
					{
						TextureHandle h = rm.GetTextureHandleByGuid(
							matAsset->textureBindings[normalMapIdx].textureGuid);
						if (const auto* rr = rm.GetTextureRenderResource(h); rr && rr->srv.IsValid())
						{
							draw.normalSrv = fa.AttachToFrameHeap(rr->srv);
						}
					}

					if (roughnessMapIdx >= 0)
					{
						TextureHandle h = rm.GetTextureHandleByGuid(
							matAsset->textureBindings[roughnessMapIdx].textureGuid);
						if (const auto* rr = rm.GetTextureRenderResource(h); rr && rr->srv.IsValid())
						{
							draw.roughnessSrv = fa.AttachToFrameHeap(rr->srv);
						}
					}
				}

				if (matRR && matRR->matConstantCbv.IsValid())
				{
					draw.materialCbv = fa.AttachToFrameHeap(matRR->matConstantCbv);
				}

				if (item->objCbvHandle.IsValid())
				{
					draw.objectCbv = item->objCbvHandle;
				}

				m_drawList.push_back(std::move(draw));
			}
		}
	}

	uint64_t OpaqueSubPass::BuildSortKey(const PassDrawItem& item, const PassPrepareContext& ctx) const
	{
		const uint32_t pipelineKey = static_cast<uint32_t>(item.pipeline.handle.index & 0xFFFFFFFFull);
		const uint32_t materialKey = static_cast<uint32_t>(item.material.handle.index & 0xFFFFFFFFull);
		const uint16_t meshKey = static_cast<uint16_t>(item.mesh.handle.index & 0xFFFFull);
		const uint16_t depthKey = 0;
		return BuildOpaqueSortKey(pipelineKey, materialKey, meshKey, depthKey);
	}
}