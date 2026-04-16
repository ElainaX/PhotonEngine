#pragma once

#include "Function/Render/DX12RHI/DX12RHI.h"
#include "Resource/DXResourceHeader.h"
#include "ViewDesc.h"
#include "DescriptorHeap/Descriptor.h"
#include "GpuResource.h"

namespace photon
{

	class DescriptorSystem;



	struct DefaultViews
	{
		// 一个资源的默认DescriptorHandle
		DescriptorHandle srv = {};
		DescriptorHandle uav = {};
		DescriptorHandle rtv = {};
		DescriptorHandle dsv = {};
		DescriptorHandle cbv = {};

		void Reset()
		{
			srv = {};
			uav = {};
			rtv = {};
			dsv = {};
			cbv = {};
		}
	};

	struct ManagedResourceRecord
	{
		std::shared_ptr<DXResource> resource = nullptr;

		// 一个资源的默认DescriptorHandle
		DefaultViews defaults = {};
		

		// persistent类的资源满足的特点：
		// 1. 生命周期和resource接近
		// 2. 会被反复使用
		// 3. 创建成本/查找成本值得缓存
		// 4. 不是每帧临时生成
		// 持久化view缓存，ViewKey存Srv，Uav，Dsv，Rtv
		std::unordered_map<ViewKey, DescriptorHandle, ViewKeyHasher> persistentViews;
		// CbvKey单独缓存，因为其不需要Format等和其他资源类似的多余字段
		std::unordered_map<CbvKey, DescriptorHandle, CbvKeyHasher> persistentCbvs;

		void ResetViewCaches()
		{
			defaults.Reset();
			persistentViews.clear();
			persistentCbvs.clear();
		}

		void ResetAll()
		{
			resource.reset();
			ResetViewCaches();
		}
	};


	class GpuResourceManager
	{
	public:
		bool Initialize(DX12RHI* rhi, DescriptorSystem* descriptorSystem);
		void Shutdown();

	public:
		// 创建资源，创建时需要指定资源是否是持久化资源
		// 暂时只分为持久化资源和非持久化资源，后面会把bool字段改成资源生命周期类型的enum class
		GpuResourceHandle CreateBuffer(const DXBufferDesc& desc);
		GpuResourceHandle CreateTexture2D(const DXTexture2DDesc& desc);
		GpuResourceHandle CreateTexture2DArray(const DXTexture2DArrayDesc& desc);
		GpuResourceHandle CreateTexture3D(const DXTexture3DDesc& desc);

		// 查询，资源必须有handle对应的slot，并且资源没有被标志为废弃pendingDestory
		DXResource* GetResource(GpuResourceHandle handle);
		const DXResource* GetResource(GpuResourceHandle handle) const;

		ID3D12Resource* GetNativeResource(GpuResourceHandle handle);
		const ID3D12Resource* GetNativeResource(GpuResourceHandle handle) const;


		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(GpuResourceHandle handle);


		// Srv，Uav，Rtv，Dsv的创建

		// 持久化资源Descriptor的创建
		// 所以我们提供此接口，并且使用Bindless table技术，于是返回值会是一个DescriptorHandle
		DescriptorHandle GetOrCreatePersistentView(GpuResourceHandle handle, const ViewDesc& desc);
		void FreeDescriptorHandle(DescriptorHandle handle);
		D3D12_CPU_DESCRIPTOR_HANDLE GetDXCpuViewHandle(DescriptorHandle handle);
		D3D12_GPU_DESCRIPTOR_HANDLE GetDXGpuViewHandle(DescriptorHandle handle);

		// 销毁
		void DestoryResource(GpuResourceHandle handle, uint64_t currentFenceValue);
		void DestoryResourceImmediate(GpuResourceHandle handle);

		void ProcessDeferredRelease();

		// 调试
		uint64_t GetTotalAllocatedBytes() const;
		bool isAlive(GpuResourceHandle handle) const;

	private:
		// 资源有handle就能得到，不管是否是pendingDestory的
		DXResource* GetResourceUnsafe(GpuResourceHandle handle);
		const DXResource* GetResourceUnsafe(GpuResourceHandle handle) const;
		GpuResourceHandle AllocateHandle();
		GpuResourceHandle InsertResource(std::shared_ptr<DXResource> res);
		void FreeResHandle(GpuResourceHandle handle);
		bool IsHandleValid(GpuResourceHandle handle) const;
		void FreeResourceViews(GpuResourceHandle handle);

		ViewKey MakeViewKey(const ViewDesc& desc);
		CbvKey MakeCbvKey(const ViewDesc& desc);

		ManagedResourceRecord* GetResourceRecord(GpuResourceHandle handle);
		const ManagedResourceRecord* GetResourceRecord(GpuResourceHandle handle) const;

	private:
		DX12RHI* m_rhi = nullptr;
		DescriptorSystem* m_descriptorSystem = nullptr;

		std::vector<ManagedResourceRecord> m_resources;
		std::vector<GpuResourceSlot> m_slots;
		std::vector<uint32_t> m_freeIndices;
		const int raw_capacity = 1024;

		struct PendingRelease
		{
			GpuResourceHandle handle;
			uint64_t retireFenceValue = 0;
		};

		std::vector<PendingRelease> m_pendingReleases;
		uint64_t m_totalAllocatedBytes = 0;
		uint32_t m_aliveResourceCount = 0;
	};

}
