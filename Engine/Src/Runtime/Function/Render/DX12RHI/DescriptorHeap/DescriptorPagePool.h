#pragma once

#include "DescriptorPage.h"

namespace photon
{
	class DescriptorPagePool
	{
	public:
		bool Initialize(ID3D12Device* device,
			DescriptorHeapKind heapKind,
			D3D12_DESCRIPTOR_HEAP_TYPE dxType,
			bool shaderVisible,
			uint32_t descriptorsPerPage);
		void Shutdown();

		DescriptorHandle Allocate();
		void Free(DescriptorHandle handle);

		bool IsHandleValid(DescriptorHandle handle) const;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(DescriptorHandle handle) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(DescriptorHandle handle) const;

		const DescriptorPage* GetPage(uint32_t pageIndex) const;
		DescriptorPage* GetPage(uint32_t pageIndex);
		int GetPageSize() const;

		DescriptorHeapKind GetHeapKind() const { return m_heapKind; }
		bool IsShaderVisible() const { return m_shaderVisible; }
		uint32_t GetDescriptorSize() const { return m_descriptorSize; }

	private:
		DescriptorHandle AllocateFromPage(DescriptorPage& page);
		bool HasFreeSlot(const DescriptorPage& page) const;
		uint32_t CreatePage();
		D3D12_DESCRIPTOR_HEAP_DESC GetDXHeapDesc() const;

	private:
		ID3D12Device* m_device = nullptr;

		DescriptorHeapKind m_heapKind = DescriptorHeapKind::Unknown;
		D3D12_DESCRIPTOR_HEAP_TYPE m_dxType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		bool m_shaderVisible = false;

		uint32_t m_descriptorSize = 0;
		uint32_t m_descriptorsPerPage = 0;

		std::vector<DescriptorPage> m_pages;
	};
}


