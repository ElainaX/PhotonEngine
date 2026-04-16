#pragma once
#include "DescriptorPagePool.h"
#include "Function/Render/RHI.h"
#include "Function/Render/DX12RHI/GpuResource.h"
#include "Function/Render/DX12RHI/ViewDesc.h"
#include <map>
#include <functional>

namespace photon
{
	using GetDescriptorPagePoolFn = std::function<
		DescriptorPagePool*()>;
	using GetPagePoolFromViewTypeFnMap = std::map<ViewType, GetDescriptorPagePoolFn >;
	using GetPagePoolFromHeapkindFnMap = std::map<DescriptorHeapKind, GetDescriptorPagePoolFn >;

	class DescriptorSystem
	{
	public:
		bool Initialize(ID3D12Device* device, RHI* rhi);
		void Shutdown();

		DescriptorHandle CreateDescriptor(const DXResource* res, const ViewDesc& desc);
		void FreeDescriptor(DescriptorHandle handle);

		bool IsHandleValid(DescriptorHandle handle) const;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(DescriptorHandle handle) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(DescriptorHandle handle) const;

		bool CopyToCpuHandle(DescriptorHandle src, D3D12_CPU_DESCRIPTOR_HANDLE dstHandle);
		bool CopyBatchToCpuHandle(const std::vector<DescriptorHandle>& srcRangeStarts, const std::vector<uint32_t>& srcRangeCounts,
		                          const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& dstRangeStarts, const std::vector<uint32_t>& dstRangeCounts);
		bool CopyBatchToCpuHandle(const std::vector<DescriptorHandle>& srcRangeStarts, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& dstRangeStarts);

		bool CreateDescriptorAtCpuHandle(
			const DXResource* res,
			const ViewDesc& desc,
			D3D12_CPU_DESCRIPTOR_HANDLE dstHandle);

	private:
		DescriptorPagePool& SelectPool(ViewType type);
		const DescriptorPagePool& SelectPool(ViewType type) const;
		DescriptorPagePool& SelectPool(DescriptorHeapKind kind);
		const DescriptorPagePool& SelectPool(DescriptorHeapKind kind) const;

		D3D12_SHADER_RESOURCE_VIEW_DESC BuildSrvDesc(const DXResource* resource, const ViewDesc& desc) const;
		D3D12_UNORDERED_ACCESS_VIEW_DESC BuildUavDesc(const DXResource* resource, const ViewDesc& desc) const;
		D3D12_RENDER_TARGET_VIEW_DESC BuildRtvDesc(const DXResource* resource, const ViewDesc& desc) const;
		D3D12_DEPTH_STENCIL_VIEW_DESC BuildDsvDesc(const DXResource* resource, const ViewDesc& desc) const;
		D3D12_CONSTANT_BUFFER_VIEW_DESC BuildCbvDesc(const DXResource* resource, const ViewDesc& desc) const;

		DescriptorPagePool& GetCbvPool() { return m_cbvPool; }
		DescriptorPagePool& GetSrvPool() { return m_srvPool; }
		DescriptorPagePool& GetUavPool() { return m_uavPool; }
		DescriptorPagePool& GetRtvPool() { return m_rtvPool; }
		DescriptorPagePool& GetDsvPool() { return m_dsvPool; }
		DescriptorPagePool& GetSamplerPool() { return m_samplerPool; }

	private:
		ID3D12Device* m_device = nullptr;
		RHI* m_rhi = nullptr;

		DescriptorPagePool m_cbvPool;
		DescriptorPagePool m_srvPool;
		DescriptorPagePool m_uavPool;
		DescriptorPagePool m_rtvPool;
		DescriptorPagePool m_dsvPool;

		uint32_t m_descriptorsPerPage = 2000u;

		// SamplerPool用另外的机制
		DescriptorPagePool m_samplerPool;

		GetPagePoolFromViewTypeFnMap m_viewTypeToPoolFnMap;
		GetPagePoolFromHeapkindFnMap m_heapkindToPoolFreeFnMap;
	};
}

