#pragma once
#include <wrl/event.h>
#include <d3d12.h>
#include <cstdint>
#include <vector>

#include "Descriptor.h"

namespace photon
{
	struct DescriptorSlot
	{
		uint32_t generation = 0;
		bool occupied = false;
	};

	struct DescriptorPage
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;

		uint32_t descriptorCount = 0;
		uint32_t descriptorSize = 0;
		uint32_t pageIndex = 0;

		bool shaderVisible = false;
		DescriptorHeapKind heapKind = DescriptorHeapKind::Unknown;

		std::vector<uint32_t> freeList;
		std::vector<DescriptorSlot> slots;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t slot) const
		{
			auto base = heap->GetCPUDescriptorHandleForHeapStart();
			base.ptr += static_cast<SIZE_T>(slot) * descriptorSize;
			return base;
		}

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t slot) const
		{
			D3D12_GPU_DESCRIPTOR_HANDLE base{};
			// 只有ShaderVisible的Heap才有独立的Gpu内存
			if (!shaderVisible)
				return base;

			base = heap->GetGPUDescriptorHandleForHeapStart();
			base.ptr += static_cast<UINT64>(slot) * descriptorSize;
			return base;
		}

		bool IsSlotValid(uint32_t slot, uint32_t generation) const
		{
			if (slot >= slots.size())
				return false;

			return slots[slot].occupied && slots[slot].generation == generation;
		}
	};
}
