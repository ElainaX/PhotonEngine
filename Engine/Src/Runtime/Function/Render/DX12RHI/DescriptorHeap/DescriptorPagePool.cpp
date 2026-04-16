#include "DescriptorPagePool.h"

#include <numeric>

namespace photon
{
	bool DescriptorPagePool::Initialize(ID3D12Device* device, DescriptorHeapKind heapKind,
		D3D12_DESCRIPTOR_HEAP_TYPE dxType, bool shaderVisible, uint32_t descriptorsPerPage)
	{
		if (!device || heapKind == DescriptorHeapKind::Unknown || descriptorsPerPage <= 0)
			return false;
		m_device = device;
		m_heapKind = heapKind;
		m_dxType = dxType;
		m_shaderVisible = shaderVisible;
		m_descriptorsPerPage = descriptorsPerPage;
		m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(dxType);

		CreatePage();

		return true;
	}

	void DescriptorPagePool::Shutdown()
	{
		for (auto& page : m_pages)
		{
			page.heap.Reset();
		}
	}

	DescriptorHandle DescriptorPagePool::Allocate()
	{
		// 分配一个slot给新的handle，如果page满了就新建
		for (int i = 0; i < m_pages.size(); ++i)
		{
			auto& page = m_pages[i];
			if (HasFreeSlot(page))
			{
				return AllocateFromPage(page);
			}
		}

		uint32_t newPageIndex = CreatePage();
		return AllocateFromPage(m_pages[newPageIndex]);
	}

	void DescriptorPagePool::Free(DescriptorHandle handle)
	{
		if (!IsHandleValid(handle))
			return;

		DescriptorPage& page = m_pages[handle.page];
		auto& slot = page.slots[handle.slot];

		slot.occupied = false;
		slot.generation++;
		page.freeList.push_back(handle.slot);
	}

	bool DescriptorPagePool::IsHandleValid(DescriptorHandle handle) const
	{
		if (!handle.IsValid())
			return false;
		const DescriptorPage& page = m_pages[handle.page];
		auto& slot = page.slots[handle.slot];

		if (handle.generation != slot.generation)
			return false;

		return true;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorPagePool::GetCpuHandle(DescriptorHandle handle) const
	{
		if (!IsHandleValid(handle))
			return D3D12_CPU_DESCRIPTOR_HANDLE{};

		auto& page = m_pages[handle.page];
		return page.GetCpuHandle(handle.slot);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorPagePool::GetGpuHandle(DescriptorHandle handle) const
	{
		if (!IsHandleValid(handle))
			return D3D12_GPU_DESCRIPTOR_HANDLE{};

		auto& page = m_pages[handle.page];
		return page.GetGpuHandle(handle.slot);
	}

	const DescriptorPage* DescriptorPagePool::GetPage(uint32_t pageIndex) const
	{
		if (pageIndex >= m_pages.size())
			return nullptr;

		return &m_pages[pageIndex];
	}

	DescriptorPage* DescriptorPagePool::GetPage(uint32_t pageIndex)
	{
		if (pageIndex >= m_pages.size())
			return nullptr;

		return &m_pages[pageIndex];
	}

	int DescriptorPagePool::GetPageSize() const
	{
		return m_pages.size();
	}

	DescriptorHandle DescriptorPagePool::AllocateFromPage(DescriptorPage& page)
	{
		if (page.freeList.empty())
			return DescriptorHandle{};
		uint32_t freeIndex = page.freeList.back();
		page.freeList.pop_back();
		auto& desSlot = page.slots[freeIndex];
		desSlot.generation++;
		desSlot.occupied = true;
		DescriptorHandle handle = {.page = page.pageIndex, .slot = freeIndex,
			.generation = desSlot.generation, .heapKind = page.heapKind};
		return handle;
	}

	bool DescriptorPagePool::HasFreeSlot(const DescriptorPage& page) const
	{
		if (!page.freeList.empty())
			return true;

		return false;
	}

	uint32_t DescriptorPagePool::CreatePage()
	{
		uint32_t pageIndex = m_pages.size();
		m_pages.emplace_back(DescriptorPage{});
		auto& page = m_pages.back();
		page.pageIndex = pageIndex;
		auto dxHeapDesc = GetDXHeapDesc();
		DX_LogIfFailed(m_device->CreateDescriptorHeap(&dxHeapDesc, IID_PPV_ARGS(page.heap.GetAddressOf())));
		page.descriptorCount = m_descriptorsPerPage;
		page.descriptorSize = m_device->GetDescriptorHandleIncrementSize(dxHeapDesc.Type);
		page.shaderVisible = m_shaderVisible;
		page.heapKind = m_heapKind;
		page.freeList.resize( m_descriptorsPerPage);
		std::iota(page.freeList.begin(), page.freeList.end(), 0);
		page.slots.resize(m_descriptorsPerPage);
		return pageIndex;
	}

	D3D12_DESCRIPTOR_HEAP_DESC DescriptorPagePool::GetDXHeapDesc() const
	{
		D3D12_DESCRIPTOR_HEAP_DESC dxDesc;
		dxDesc.Type = m_dxType;
		dxDesc.Flags = m_shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dxDesc.NodeMask = 0;
		dxDesc.NumDescriptors = m_descriptorsPerPage;
		return dxDesc;
	}
}
