#pragma once
#include "ComputePipelineState.h"
#include "GraphicsPipelineState.h"
#include "PipelineStateResource.h"
#include "Function/Render/DX12RHI/DX12RHI.h"
#include "Function/Render/DX12RHI/RenderResource/ResourceResolver.h"
#include "Function/Render/DX12RHI/RenderResource/VertexLayoutRegistry.h"
#include "Function/Util/ResourceSlotCollection.h"

#include <unordered_map>

#include "Function/Render/DX12RHI/RootSignature/RootSignatureCache.h"

namespace photon
{
	struct BlendStateResource
	{
		BlendStateHandle handle;
		BlendStateDesc desc = {};
		D3D12_BLEND_DESC nativeDesc = {};
	};

	struct RasterStateResource
	{
		RasterStateHandle handle;
		RasterStateDesc desc = {};
		D3D12_RASTERIZER_DESC nativeDesc = {};
	};

	struct DepthStencilStateResource
	{
		DepthStencilStateHandle handle;
		DepthStencilStateDesc desc = {};
		D3D12_DEPTH_STENCIL_DESC nativeDesc = {};
	};

	class BlendStateCache final : public ResourceSlotCollection<BlendStateHandle, BlendStateResource>
	{
	public:
		BlendStateHandle GetOrCreate(const BlendStateDesc& desc, uint32_t renderTargetCount);
		const BlendStateResource* TryGet(BlendStateHandle h) const;
		void Shutdown();

	private:
		std::unordered_map<BlendStateDesc, BlendStateHandle, BlendStateDescHasher> m_descToHandle;
	};

	class RasterStateCache final : public ResourceSlotCollection<RasterStateHandle, RasterStateResource>
	{
	public:
		RasterStateHandle GetOrCreate(const RasterStateDesc& desc);
		const RasterStateResource* TryGet(RasterStateHandle h) const;
		void Shutdown();

	private:
		std::unordered_map<RasterStateDesc, RasterStateHandle, RasterStateDescHasher> m_descToHandle;
	};

	class DepthStencilStateCache final : public ResourceSlotCollection<DepthStencilStateHandle, DepthStencilStateResource>
	{
	public:
		DepthStencilStateHandle GetOrCreate(const DepthStencilStateDesc& desc);
		const DepthStencilStateResource* TryGet(DepthStencilStateHandle h) const;
		void Shutdown();

	private:
		std::unordered_map<DepthStencilStateDesc, DepthStencilStateHandle, DepthStencilStateDescHasher> m_descToHandle;
	};

	class PipelineStateCache : public ResourceSlotCollection<PipelineStateHandle, PipelineStateResource>
	{
	public:
		bool Initialize(DX12RHI* rhi, RootSignatureCache* rootSigCache, const IResourceView* resourceView, VertexLayoutRegistry* registry);
		void Shutdown();

		PipelineStateHandle GetOrCreateGraphics(const GraphicsPipelineDesc& desc);
		PipelineStateHandle GetOrCreateGraphics(const GraphicsPipelineKey& key);
		PipelineStateHandle GetOrCreateCompute(const ComputePipelineKey& key);

		PipelineStateResource* TryGet(PipelineStateHandle h)
		{
			auto* e = TryGetEntry(h);
			return e ? &e->resource : nullptr;
		}
		const PipelineStateResource* TryGet(PipelineStateHandle h) const
		{
			auto* e = TryGetEntry(h);
			return e ? &e->resource : nullptr;
		}

		ID3D12PipelineState* GetNative(PipelineStateHandle h) const
		{
			auto* rr = TryGet(h);
			return rr ? rr->native.Get() : nullptr;
		}

		bool IsValid(PipelineStateHandle h) const;

	private:
		GraphicsPipelineKey BuildGraphicsKey(const GraphicsPipelineDesc& desc);
		PipelineStateHandle BuildGraphicsPipeline(const GraphicsPipelineKey& key);
		PipelineStateHandle BuildComputePipeline(const ComputePipelineKey& key);

	private:
		DX12RHI* m_rhi = nullptr;
		RootSignatureCache* m_rootSigCache = nullptr;
		VertexLayoutRegistry* m_vertLayoutRegistry = nullptr;
		const IResourceView* m_resView = nullptr;

		BlendStateCache m_blendStateCache;
		RasterStateCache m_rasterStateCache;
		DepthStencilStateCache m_depthStencilStateCache;

		std::unordered_map<GraphicsPipelineKey, PipelineStateHandle, GraphicsPipelineKeyHasher> m_graphicsKeyToHandle;
		std::unordered_map<ComputePipelineKey, PipelineStateHandle, ComputePipelineKeyHasher> m_computeKeyToHandle;
	};
}
