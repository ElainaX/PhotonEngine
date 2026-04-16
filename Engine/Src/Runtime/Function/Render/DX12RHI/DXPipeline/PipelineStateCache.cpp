#include "PipelineStateCache.h"

#include "PipelineStateResource.h"
#include "BlendState/Dx12BlendState.h"
#include "DepthStencilState/Dx12DepthStencilState.h"
#include "RasterState/Dx12RasterState.h"
#include "Function/Render/DX12RHI/RenderResource/Dx12InputLayout.h"
#include "Function/Util/RenderUtil.h"

namespace photon
{
	static D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3D12PrimitiveTopologyType(PrimitiveTopologyType topologyType)
	{
		switch (topologyType)
		{
		case PrimitiveTopologyType::Point: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case PrimitiveTopologyType::Line: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case PrimitiveTopologyType::Triangle: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case PrimitiveTopologyType::Patch: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		case PrimitiveTopologyType::Undefined:
		default:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		}
	}

	BlendStateHandle BlendStateCache::GetOrCreate(const BlendStateDesc& inDesc, uint32_t renderTargetCount)
	{
		BlendStateDesc desc = inDesc;
		desc.Canonicalize();

		auto iter = m_descToHandle.find(desc);
		if (iter != m_descToHandle.end())
			return iter->second;

		BlendStateHandle handle = AllocateHandle();
		auto* entry = TryGetEntry(handle);
		if (!entry)
			return {};

		entry->resource.handle = handle;
		entry->resource.desc = desc;
		(void)renderTargetCount;
		entry->resource.nativeDesc = Dx12BlendState::ToNative(desc, 8);
		m_descToHandle.emplace(entry->resource.desc, handle);
		return handle;
	}

	const BlendStateResource* BlendStateCache::TryGet(BlendStateHandle h) const
	{
		auto* e = TryGetEntry(h);
		return e ? &e->resource : nullptr;
	}

	void BlendStateCache::Shutdown()
	{
		m_descToHandle.clear();
		ResetAll();
	}

	RasterStateHandle RasterStateCache::GetOrCreate(const RasterStateDesc& inDesc)
	{
		RasterStateDesc desc = inDesc;
		desc.Canonicalize();

		auto iter = m_descToHandle.find(desc);
		if (iter != m_descToHandle.end())
			return iter->second;

		RasterStateHandle handle = AllocateHandle();
		auto* entry = TryGetEntry(handle);
		if (!entry)
			return {};

		entry->resource.handle = handle;
		entry->resource.desc = desc;
		entry->resource.nativeDesc = Dx12RasterState::ToNative(desc);
		m_descToHandle.emplace(entry->resource.desc, handle);
		return handle;
	}

	const RasterStateResource* RasterStateCache::TryGet(RasterStateHandle h) const
	{
		auto* e = TryGetEntry(h);
		return e ? &e->resource : nullptr;
	}

	void RasterStateCache::Shutdown()
	{
		m_descToHandle.clear();
		ResetAll();
	}

	DepthStencilStateHandle DepthStencilStateCache::GetOrCreate(const DepthStencilStateDesc& inDesc)
	{
		DepthStencilStateDesc desc = inDesc;
		desc.Canonicalize();

		auto iter = m_descToHandle.find(desc);
		if (iter != m_descToHandle.end())
			return iter->second;

		DepthStencilStateHandle handle = AllocateHandle();
		auto* entry = TryGetEntry(handle);
		if (!entry)
			return {};

		entry->resource.handle = handle;
		entry->resource.desc = desc;
		entry->resource.nativeDesc = Dx12DepthStencilState::ToNative(desc);
		m_descToHandle.emplace(entry->resource.desc, handle);
		return handle;
	}

	const DepthStencilStateResource* DepthStencilStateCache::TryGet(DepthStencilStateHandle h) const
	{
		auto* e = TryGetEntry(h);
		return e ? &e->resource : nullptr;
	}

	void DepthStencilStateCache::Shutdown()
	{
		m_descToHandle.clear();
		ResetAll();
	}

	bool PipelineStateCache::Initialize(DX12RHI* rhi, RootSignatureCache* rootSigCache,
		const IResourceView* resourceView, VertexLayoutRegistry* registry)
	{
		if (!rhi || !rootSigCache || !resourceView || !registry)
			return false;

		m_rhi = rhi;
		m_rootSigCache = rootSigCache;
		m_resView = resourceView;
		m_vertLayoutRegistry = registry;
		return true;
	}

	void PipelineStateCache::Shutdown()
	{
		m_graphicsKeyToHandle.clear();
		m_computeKeyToHandle.clear();
		m_blendStateCache.Shutdown();
		m_rasterStateCache.Shutdown();
		m_depthStencilStateCache.Shutdown();
		ResetAll();
		m_rhi = nullptr;
		m_resView = nullptr;
		m_rootSigCache = nullptr;
		m_vertLayoutRegistry = nullptr;
	}

	PipelineStateHandle PipelineStateCache::GetOrCreateGraphics(const GraphicsPipelineDesc& inDesc)
	{
		GraphicsPipelineDesc desc = inDesc;
		desc.Canonicalize();
		return GetOrCreateGraphics(BuildGraphicsKey(desc));
	}

	PipelineStateHandle PipelineStateCache::GetOrCreateGraphics(const GraphicsPipelineKey& key)
	{
		auto iter = m_graphicsKeyToHandle.find(key);
		if (iter != m_graphicsKeyToHandle.end())
		{
			return iter->second;
		}
		return BuildGraphicsPipeline(key);
	}

	PipelineStateHandle PipelineStateCache::GetOrCreateCompute(const ComputePipelineKey& key)
	{
		auto iter = m_computeKeyToHandle.find(key);
		if (iter != m_computeKeyToHandle.end())
		{
			return iter->second;
		}
		return BuildComputePipeline(key);
	}

	bool PipelineStateCache::IsValid(PipelineStateHandle h) const
	{
		return IsHandleValid(h);
	}

	GraphicsPipelineKey PipelineStateCache::BuildGraphicsKey(const GraphicsPipelineDesc& desc)
	{
		GraphicsPipelineKey key = {};
		key.shaderProgram = desc.shaderProgram;
		key.rootSignature = desc.rootSignature;
		key.vertexLayoutHash = desc.vertexLayoutHash;
		key.rasterState = m_rasterStateCache.GetOrCreate(desc.rasterState);
		key.depthStencilState = m_depthStencilStateCache.GetOrCreate(desc.depthStencilState);
		key.blendState = m_blendStateCache.GetOrCreate(desc.blendState, desc.rtLayout.rtvCount > 0 ? desc.rtLayout.rtvCount : 1);
		key.rtLayout = desc.rtLayout;
		key.topologyType = desc.topologyType;
		key.sampleDesc = desc.sampleDesc;
		key.sampleMask = desc.sampleMask;
		return key;
	}

	PipelineStateHandle PipelineStateCache::BuildGraphicsPipeline(const GraphicsPipelineKey& key)
	{
		const ShaderProgramRenderResource* shader = m_resView->GetShaderProgramRenderResource(key.shaderProgram);
		if (!shader)
			return {};

		ID3D12RootSignature* rs = m_rootSigCache->GetNative(key.rootSignature);
		if (!rs)
			return {};

		const auto* rasterState = m_rasterStateCache.TryGet(key.rasterState);
		const auto* depthStencilState = m_depthStencilStateCache.TryGet(key.depthStencilState);
		const auto* blendState = m_blendStateCache.TryGet(key.blendState);
		if (!rasterState || !depthStencilState || !blendState)
			return {};

		const CompiledShaderBytecode* vs = shader->TryGetStageBytecode(ShaderStage::VS);
		const CompiledShaderBytecode* ps = shader->TryGetStageBytecode(ShaderStage::PS);
		const CompiledShaderBytecode* gs = shader->TryGetStageBytecode(ShaderStage::GS);
		const CompiledShaderBytecode* hs = shader->TryGetStageBytecode(ShaderStage::HS);
		const CompiledShaderBytecode* ds = shader->TryGetStageBytecode(ShaderStage::DS);

		if (!vs)
			return {};

		PipelineStateHandle handle = AllocateHandle();
		auto* entry = TryGetEntry(handle);
		if (!entry)
			return {};

		PipelineStateResource rr = {};
		rr.handle = handle;
		rr.kind = PipelineStateKind::Graphics;
		rr.shaderProgram = key.shaderProgram;
		rr.rootSignature = key.rootSignature;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		if (key.vertexLayoutHash != 0)
		{
			const Dx12InputLayout* inputLayout = m_vertLayoutRegistry->GetOrCreateDx12InputLayout(key.vertexLayoutHash);
			if (inputLayout)
				inputLayoutDesc = inputLayout->GetDesc();
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = rs;
		desc.VS = vs ? D3D12_SHADER_BYTECODE{ reinterpret_cast<const void*>(vs->data.data()), vs->data.size() } : D3D12_SHADER_BYTECODE{};
		desc.PS = ps ? D3D12_SHADER_BYTECODE{ reinterpret_cast<const void*>(ps->data.data()), ps->data.size() } : D3D12_SHADER_BYTECODE{};
		desc.GS = gs ? D3D12_SHADER_BYTECODE{ reinterpret_cast<const void*>(gs->data.data()), gs->data.size() } : D3D12_SHADER_BYTECODE{};
		desc.HS = hs ? D3D12_SHADER_BYTECODE{ reinterpret_cast<const void*>(hs->data.data()), hs->data.size() } : D3D12_SHADER_BYTECODE{};
		desc.DS = ds ? D3D12_SHADER_BYTECODE{ reinterpret_cast<const void*>(ds->data.data()), ds->data.size() } : D3D12_SHADER_BYTECODE{};
		desc.InputLayout = inputLayoutDesc;
		desc.BlendState = blendState->nativeDesc;
		desc.RasterizerState = rasterState->nativeDesc;
		desc.DepthStencilState = depthStencilState->nativeDesc;
		desc.SampleMask = key.sampleMask;
		desc.PrimitiveTopologyType = ToD3D12PrimitiveTopologyType(key.topologyType);
		desc.NumRenderTargets = static_cast<UINT>(key.rtLayout.rtvCount);
		for (uint32_t i = 0; i < key.rtLayout.rtvCount && i < 8; ++i)
		{
			desc.RTVFormats[i] = RenderUtil::ToDxgiFormat(key.rtLayout.rtvFormats[i]);
		}
		desc.DSVFormat = RenderUtil::ToDxgiFormat(key.rtLayout.dsvFormat);
		desc.SampleDesc.Count = key.sampleDesc.count;
		desc.SampleDesc.Quality = key.sampleDesc.quality;
		desc.NodeMask = 0;
		desc.CachedPSO = {};
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRESULT hr = m_rhi->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(rr.native.GetAddressOf()));
		if (FAILED(hr))
		{
			FreeHandle(handle);
			return {};
		}

		entry->resource = std::move(rr);
		m_graphicsKeyToHandle.emplace(key, handle);
		return handle;
	}

	PipelineStateHandle PipelineStateCache::BuildComputePipeline(const ComputePipelineKey& key)
	{
		const ShaderProgramRenderResource* shader = m_resView->GetShaderProgramRenderResource(key.shaderProgram);
		if (!shader)
			return {};

		ID3D12RootSignature* rs = m_rootSigCache->GetNative(key.rootSignature);
		if (!rs)
			return {};

		const CompiledShaderBytecode* cs = shader->TryGetStageBytecode(ShaderStage::CS);
		if (!cs)
			return {};

		PipelineStateHandle handle = AllocateHandle();
		auto* entry = TryGetEntry(handle);
		if (!entry)
			return {};

		PipelineStateResource rr = {};
		rr.handle = handle;
		rr.kind = PipelineStateKind::Compute;
		rr.shaderProgram = key.shaderProgram;
		rr.rootSignature = key.rootSignature;

		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = rs;
		desc.CS = D3D12_SHADER_BYTECODE{ reinterpret_cast<const void*>(cs->data.data()), cs->data.size() };
		desc.NodeMask = 0;
		desc.CachedPSO = {};
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRESULT hr = m_rhi->GetDevice()->CreateComputePipelineState(&desc, IID_PPV_ARGS(rr.native.GetAddressOf()));
		if (FAILED(hr))
		{
			FreeHandle(handle);
			return {};
		}

		entry->resource = std::move(rr);
		m_computeKeyToHandle.emplace(key, handle);
		return handle;
	}
}
