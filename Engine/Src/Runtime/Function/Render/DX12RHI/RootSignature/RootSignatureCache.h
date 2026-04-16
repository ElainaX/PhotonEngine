#pragma once
#include <unordered_map>
#include <vector>

#include "RootSignature.h"
#include "Function/Render/ResourceHandle.h"
#include "Function/Render/DX12RHI/DX12RHI.h"
#include "Function/Util/ResourceSlotCollection.h"

namespace photon
{
	struct RootSignatureResource
	{
		RootSignatureDesc desc = {};
		Microsoft::WRL::ComPtr<ID3D12RootSignature> native = nullptr;
	};

	class RootSignatureCache : public ResourceSlotCollection<RootSignatureHandle, RootSignatureResource>
	{
	public:
		RootSignatureCache() = default;

		bool Initialize(DX12RHI* rhi);
		void Shutdown();

		RootSignatureHandle GetOrCreate(const RootSignatureDesc& desc);
		void Destroy(RootSignatureHandle h);

		ID3D12RootSignature* GetNative(RootSignatureHandle h) const;
		const RootSignatureResource* TryGet(RootSignatureHandle h) const;
		RootSignatureResource* TryGet(RootSignatureHandle h);
		bool IsValid(RootSignatureHandle h) const;

	private:
		DX12RHI* m_rhi = nullptr;
		std::unordered_map<size_t, std::vector<RootSignatureHandle>> m_cache;
	};

}
