#include "GpuResourceManager.h"
#include "DX12Define.h"
#include "DescriptorHeap/DescriptorSystem.h"
#include "Function/Global/RuntimeGlobalContext.h"
#include <algorithm>

namespace photon
{


	bool GpuResourceManager::Initialize(DX12RHI* rhi, DescriptorSystem* descriptorSystem)
	{
		m_rhi = rhi;
		m_descriptorSystem = descriptorSystem;
		m_resources.reserve(raw_capacity);
		m_slots.reserve(raw_capacity);
		m_freeIndices.reserve(raw_capacity);
		m_pendingReleases.reserve(raw_capacity);
		return m_rhi && m_descriptorSystem;
	}

	// 希望外界能保证此时资源已被使用完全可以释放
	void GpuResourceManager::Shutdown()
	{
		for (auto& res : m_resources)
		{
			res.resource.reset();
		}
	}

	GpuResourceHandle GpuResourceManager::CreateBuffer(const DXBufferDesc& desc)
	{

		// 如果是持久化资源，GpuResourceManager自行管理
		auto resPtr = std::make_shared<DXBuffer>();
		DX_LogIfFailed(m_rhi->CreateDXBuffer(desc, *resPtr));

		m_aliveResourceCount++;
		m_totalAllocatedBytes += m_rhi->GetResourceSizeInBytes(resPtr->gpuResource.Get());
		return InsertResource(resPtr);

	}

	GpuResourceHandle GpuResourceManager::CreateTexture2D(const DXTexture2DDesc& desc)
	{

		auto resPtr = std::make_shared<DXTexture2D>();
		DX_LogIfFailed(m_rhi->CreateDXTexture2D(desc, *resPtr));

		m_aliveResourceCount++;
		m_totalAllocatedBytes += m_rhi->GetResourceSizeInBytes(resPtr->gpuResource.Get());
		return InsertResource(resPtr);
	}

	GpuResourceHandle GpuResourceManager::CreateTexture3D(const DXTexture3DDesc& desc)
	{
		auto resPtr = std::make_shared<DXTexture3D>();
		DX_LogIfFailed(m_rhi->CreateDXTexture3D(desc, *resPtr));

		m_aliveResourceCount++;
		m_totalAllocatedBytes += m_rhi->GetResourceSizeInBytes(resPtr->gpuResource.Get());
		return InsertResource(resPtr);
	}

	GpuResourceHandle GpuResourceManager::CreateTexture2DArray(const DXTexture2DArrayDesc& desc)
	{
		auto resPtr = std::make_shared<DXTexture2DArray>();
		DX_LogIfFailed(m_rhi->CreateDXTexture2DArray(desc, *resPtr));

		m_aliveResourceCount++;
		m_totalAllocatedBytes += m_rhi->GetResourceSizeInBytes(resPtr->gpuResource.Get());
		return InsertResource(resPtr);
	}

	DXResource* GpuResourceManager::GetResource(GpuResourceHandle handle)
	{
		auto record = GetResourceRecord(handle);
		return record->resource.get();
	}

	bool GpuResourceManager::isAlive(GpuResourceHandle handle) const
	{

		auto res = GetResourceUnsafe(handle);
		return res && !res->pendingDestory;
	}

	const DXResource* GpuResourceManager::GetResource(GpuResourceHandle handle) const
	{
		auto record = GetResourceRecord(handle);
		return record->resource.get();
	}

	ID3D12Resource* GpuResourceManager::GetNativeResource(GpuResourceHandle handle)
	{
		DXResource* res = GetResource(handle);
		return res->gpuResource.Get();
	}

	const ID3D12Resource* GpuResourceManager::GetNativeResource(GpuResourceHandle handle) const
	{
		const DXResource* res = GetResource(handle);
		return res->gpuResource.Get();
	}

	ManagedResourceRecord* GpuResourceManager::GetResourceRecord(GpuResourceHandle handle)
	{
		if (!isAlive(handle))
			return nullptr;
		return &m_resources[handle.index];
	}

	const ManagedResourceRecord* GpuResourceManager::GetResourceRecord(GpuResourceHandle handle) const
	{
		if (!isAlive(handle))
			return nullptr;
		return &m_resources[handle.index];
	}

	D3D12_GPU_VIRTUAL_ADDRESS GpuResourceManager::GetGpuVirtualAddress(GpuResourceHandle handle)
	{
		ID3D12Resource* res = GetNativeResource(handle);
		return res->GetGPUVirtualAddress();
	}

	DescriptorHandle GpuResourceManager::GetOrCreatePersistentView(GpuResourceHandle handle, const ViewDesc& desc)
	{
		if (desc.type == ViewType::Unknown)
		{
			PHOTON_ASSERT(false, "desc无效或未指定类型");
			return DescriptorHandle{};
		}

		if (desc.type == ViewType::CBV)
		{
			// 走CBVKey缓存渠道
			auto key = MakeCbvKey(desc);
			auto& record_cbvs = GetResourceRecord(handle)->persistentCbvs;
			auto iter = record_cbvs.find(key);
			if (iter != record_cbvs.end())
			{
				// 看一下这个缓存是否还有效
				if (m_descriptorSystem->IsHandleValid(iter->second))
					return iter->second;
				DescriptorHandle desHandle = m_descriptorSystem->CreateDescriptor(GetResource(handle), desc);
				record_cbvs[key] = desHandle;
				return desHandle;
			}

			DescriptorHandle desHandle = m_descriptorSystem->CreateDescriptor(GetResource(handle), desc);
			record_cbvs.insert({ key, desHandle });
			return desHandle;

		}

		// 否则走ViewKey缓存渠道
		auto key = MakeViewKey(desc);
		auto& record_viewkeys = GetResourceRecord(handle)->persistentViews;
		auto iter = record_viewkeys.find(key);
		if (iter != record_viewkeys.end())
		{
			// 看一下这个缓存是否还有效
			if (m_descriptorSystem->IsHandleValid(iter->second))
				return iter->second;
			DescriptorHandle desHandle = m_descriptorSystem->CreateDescriptor(GetResource(handle), desc);
			record_viewkeys[key] = desHandle;
			return desHandle;
		}

		DescriptorHandle desHandle = m_descriptorSystem->CreateDescriptor(GetResource(handle), desc);
		record_viewkeys.insert({ key, desHandle });
		return desHandle;


	}

	void GpuResourceManager::FreeDescriptorHandle(DescriptorHandle handle)
	{
		// 这里只删除descriptorSystem中的，缓存中可以利用handle是否有效判断是否过期
		m_descriptorSystem->FreeDescriptor(handle);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GpuResourceManager::GetDXCpuViewHandle(DescriptorHandle handle)
	{
		return m_descriptorSystem->GetCpuHandle(handle);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GpuResourceManager::GetDXGpuViewHandle(DescriptorHandle handle)
	{
		return m_descriptorSystem->GetGpuHandle(handle);
	}

	// 延迟Destory，放置GPU仍在使用的资源被销毁
	void GpuResourceManager::DestoryResource(GpuResourceHandle handle, uint64_t currentFenceValue)
	{
		if (!IsHandleValid(handle))
			return;

		DXResource* res = GetResource(handle);
		if (!res)
			return;

		if (res->pendingDestory)
			return;

		res->pendingDestory = true;

		PendingRelease pr;
		pr.handle = handle;
		// 多等一帧，直到当前帧渲染结束再Release
		pr.retireFenceValue = currentFenceValue + 1;
		m_pendingReleases.push_back(pr);
	}

	void GpuResourceManager::DestoryResourceImmediate(GpuResourceHandle handle)
	{
		uint64_t fenceval = m_rhi->SignalQueue(QueueType::Graphics);
		DestoryResource(handle, fenceval);
	}

	void GpuResourceManager::ProcessDeferredRelease()
	{
		if (m_pendingReleases.empty())
			return;
		uint64_t completeFenceValue = m_rhi->GetCompletedFenceValue(QueueType::Graphics);
		int p1 = 0, p2 = m_pendingReleases.size() - 1;
		// 双指针删除
		// p1指向当前索引，p2指向数组最后一个有效数据
		while (p1 <= p2)
		{
			// 说明p1该删除了
			if (completeFenceValue >= m_pendingReleases[p1].retireFenceValue)
			{
				// 把p1移动到后面
				m_pendingReleases[p1] = m_pendingReleases[p2];
				// 更新p2
				p2--;
				continue;
			}
			// 否则p1继续前进
			p1++;
		}
		// 至此，p2之后的所有元素都是无效元素，开始逐一删除
		std::for_each(m_pendingReleases.begin() + (p2 + 1), m_pendingReleases.end(), [this](PendingRelease& pr)
			{
				auto res = GetResourceUnsafe(pr.handle);
				if (res)
				{
					m_totalAllocatedBytes -= m_rhi->GetResourceSizeInBytes(res->gpuResource.Get());
					m_aliveResourceCount--;
					res->gpuResource.Reset();
					res->pendingDestory = false;
					FreeResHandle(pr.handle);
				}
			});
		// 处理完毕，收缩pendingReleaseList
		m_pendingReleases.resize(p2 + 1);
	}

	uint64_t GpuResourceManager::GetTotalAllocatedBytes() const
	{
		return m_totalAllocatedBytes;
	}

	// 这个函数是给持久化资源用的
	DXResource* GpuResourceManager::GetResourceUnsafe(GpuResourceHandle handle)
	{
		if (!IsHandleValid(handle))
			return nullptr;

		return m_resources[handle.index].resource.get();
	}

	// 这个函数是给持久化资源用的
	const DXResource* GpuResourceManager::GetResourceUnsafe(GpuResourceHandle handle) const
	{
		if (!IsHandleValid(handle))
			return nullptr;

		return m_resources[handle.index].resource.get();
	}

	GpuResourceHandle GpuResourceManager::AllocateHandle()
	{
		GpuResourceHandle h;
		if (m_freeIndices.empty())
		{
			m_resources.push_back(ManagedResourceRecord{});
			m_slots.push_back({ .generation = 1, .occupied = true });
			h.index = m_resources.size() - 1;
			h.generation = 1;
		}
		else
		{
			uint32_t freeidx = m_freeIndices.back();
			m_freeIndices.pop_back();
			m_resources[freeidx] = ManagedResourceRecord{};
			m_slots[freeidx].generation++;
			m_slots[freeidx].occupied = true;
			h.index = freeidx;
			h.generation = m_slots[freeidx].generation;
		}
		return h;
	}

	void GpuResourceManager::FreeResHandle(GpuResourceHandle handle)
	{
		if (!IsHandleValid(handle))
			return;
		// 只有对有效的handle，我们才会执行释放
		// 先释放资源的Descriptor，然后再释放资源
		FreeResourceViews(handle);
		m_resources[handle.index].resource.reset();
		m_resources[handle.index] = ManagedResourceRecord{};
		m_slots[handle.index].occupied = false;
		m_slots[handle.index].generation++;
		m_freeIndices.push_back(handle.index);

		// 防止generation溢出
		if (m_slots[handle.index].generation == 0)
		{
			m_slots[handle.index].generation = 1;
		}
	}

	bool GpuResourceManager::IsHandleValid(GpuResourceHandle handle) const
	{
		if (!handle.IsValid())
			return false;

		if (handle.index >= m_slots.size())
			return false;

		const GpuResourceSlot& slot = m_slots[handle.index];

		if (!slot.occupied)
			return false;

		if (slot.generation != handle.generation)
			return false;

		return true;
	}

	void GpuResourceManager::FreeResourceViews(GpuResourceHandle handle)
	{
		auto record = GetResourceRecord(handle);
		// 释放persistentViews
		for (auto& viewKey_handle : record->persistentViews)
		{
			// TODO： 使用DescriptorManager去删除对应的view Descriptor
			m_descriptorSystem->FreeDescriptor(viewKey_handle.second);
		}
		for (auto& cbvKey_handle : record->persistentCbvs)
		{
			// TODO： 使用DescriptorManager去删除对应的cbv Descriptor
			m_descriptorSystem->FreeDescriptor(cbvKey_handle.second);
		}
		record->persistentViews.clear();
		record->persistentCbvs.clear();
	}

	ViewKey GpuResourceManager::MakeViewKey(const ViewDesc& desc)
	{
		ViewKey key;
		key.type = desc.type;
		key.dimension = desc.dimension;
		key.format = desc.format;
		key.flags = desc.flags;
		key.shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		key.mostDetailedMip = desc.mostDetailedMip;
		key.mipLevels = desc.mipLevels;
		key.firstArraySlice = desc.firstArraySlice;
		key.arraySize = desc.arraySize;
		return key;
	}

	CbvKey GpuResourceManager::MakeCbvKey(const ViewDesc& desc)
	{
		CbvKey key;
		key.offsetInBytes = desc.cbvOffsetInBytes;
		key.sizeInBytes = desc.cbvSizeInBytes;
		return key;
	}


	GpuResourceHandle GpuResourceManager::InsertResource(std::shared_ptr<DXResource> res)
	{
		GpuResourceHandle h = AllocateHandle();
		m_resources[h.index] = ManagedResourceRecord{ .resource = res };
		return h;
	}
}
