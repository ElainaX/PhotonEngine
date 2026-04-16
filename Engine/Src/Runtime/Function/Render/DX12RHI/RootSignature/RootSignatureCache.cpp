#include "RootSignatureCache.h"

#include <algorithm>

#include "Function/Render/DX12RHI/d3dx12.h"
#include "Macro.h"
#include "Function/Util/RenderUtil.h"

namespace photon
{
	namespace
	{
		D3D12_DESCRIPTOR_RANGE_TYPE ToDescriptorRangeType(ShaderParameterType type)
		{
			switch (type)
			{
			case ShaderParameterType::Cbv: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			case ShaderParameterType::Srv: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case ShaderParameterType::Uav: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			case ShaderParameterType::Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			default: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			}
		}
	}

	bool RootSignatureCache::Initialize(DX12RHI* rhi)
	{
		m_rhi = rhi;
		m_cache.clear();
		return m_rhi != nullptr;
	}

	void RootSignatureCache::Shutdown()
	{
		m_cache.clear();
		ResetAll();
		m_rhi = nullptr;
	}

	RootSignatureHandle RootSignatureCache::GetOrCreate(const RootSignatureDesc& desc)
	{
		PHOTON_ASSERT(m_rhi, "RootSignatureCache 尚未初始化");

		const size_t hash = HashRootSignatureDesc(desc);
		auto it = m_cache.find(hash);
		if (it != m_cache.end())
		{
			for (RootSignatureHandle h : it->second)
			{
				const RootSignatureResource* rr = TryGet(h);
				if (rr && rr->desc == desc)
					return h;
			}
		}

		std::vector<CD3DX12_DESCRIPTOR_RANGE> ranges;
		std::vector<CD3DX12_ROOT_PARAMETER> params;
		ranges.reserve(desc.parameters.size());
		params.reserve(desc.parameters.size());

		for (const RootParameterDesc& p : desc.parameters)
		{
			ranges.emplace_back();
			ranges.back().Init(
				ToDescriptorRangeType(p.type),
				p.numDescriptors,
				p.bindPoint,
				p.registerSpace);

			params.emplace_back();
			params.back().InitAsDescriptorTable(1, &ranges.back(), D3D12_SHADER_VISIBILITY_ALL);
		}

		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
		if (desc.allowInputAssembler)
			flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> staticSamplers = RenderUtil::GetStaticSamplers();
		rsDesc.Init(static_cast<UINT>(params.size()), params.empty() ? nullptr : params.data(), staticSamplers.size(), staticSamplers.data(), flags);

		Microsoft::WRL::ComPtr<ID3DBlob> serialized;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &errorBlob);
		PHOTON_ASSERT(SUCCEEDED(hr), "D3D12SerializeRootSignature failed");

		Microsoft::WRL::ComPtr<ID3D12RootSignature> native;
		hr = m_rhi->GetDevice()->CreateRootSignature(
			0, serialized->GetBufferPointer(), serialized->GetBufferSize(), IID_PPV_ARGS(native.GetAddressOf()));
		PHOTON_ASSERT(SUCCEEDED(hr), "CreateRootSignature failed");

		RootSignatureHandle handle = AllocateHandle();
		Entry* e = TryGetEntry(handle);
		if (!e)
			return {};

		e->resource.desc = desc;
		e->resource.native = std::move(native);
		m_cache[hash].push_back(handle);
		return handle;
	}

	void RootSignatureCache::Destroy(RootSignatureHandle h)
	{
		const RootSignatureResource* rr = TryGet(h);
		if (rr)
		{
			const size_t hash = HashRootSignatureDesc(rr->desc);
			auto it = m_cache.find(hash);
			if (it != m_cache.end())
			{
				auto& vec = it->second;
				vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const RootSignatureHandle& x) { return x == h; }), vec.end());
				if (vec.empty())
					m_cache.erase(it);
			}
		}
		FreeHandle(h);
	}

	ID3D12RootSignature* RootSignatureCache::GetNative(RootSignatureHandle h) const
	{
		const RootSignatureResource* rr = TryGet(h);
		return rr ? rr->native.Get() : nullptr;
	}

	const RootSignatureResource* RootSignatureCache::TryGet(RootSignatureHandle h) const
	{
		auto* e = TryGetEntry(h);
		return e ? &e->resource : nullptr;
	}

	RootSignatureResource* RootSignatureCache::TryGet(RootSignatureHandle h)
	{
		auto* e = TryGetEntry(h);
		return e ? &e->resource : nullptr;
	}

	bool RootSignatureCache::IsValid(RootSignatureHandle h) const
	{
		return IsHandleValid(h);
	}
}
