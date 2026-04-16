#include "FrameDescriptorSystem.h"

#include "Function/Util/RenderUtil.h"


namespace photon
{
	bool FrameDescriptorSystem::Initialize(ID3D12Device* device)
	{
		if (!device)
			return false;
		m_device = device;
		m_descriptorPerSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		for (int i = 0; i < FrameSyncSystem::kMaxFramesInFlight; ++i)
		{
			m_heaps[i] = CreateHeap(m_heapSize);
		}

		return true;
	}

	void FrameDescriptorSystem::BeginFrame(uint32_t frameIndex)
	{
		m_currFrameIndex = frameIndex;
	}

	void FrameDescriptorSystem::Clear()
	{
		// 根据上一帧实际使用量做一个温和扩容，避免下一帧刚开始就频繁 grow
		uint32_t desiredSize = m_heapSize;
		if (m_frameHeapSize > 0)
		{
			uint32_t suggested = m_frameHeapSize * 3 / 2 + 64;
			if (suggested > desiredSize)
				desiredSize = suggested;
		}

		if (m_heaps[m_currFrameIndex]->GetDesc().NumDescriptors < desiredSize)
		{
			m_heaps[m_currFrameIndex] = CreateHeap(desiredSize);
			m_heapSize = desiredSize;
		}
		m_startPosIndex = 0;
		m_frameHeapSize = 0;
		m_isFrozen = false;
	}

	void FrameDescriptorSystem::FinalizeFrame()
	{
		m_isFrozen = true;
	}

	FrameDescriptorHandle FrameDescriptorSystem::AllocateDescriptor(DescriptorHeapKind heapkind)
	{
		PHOTON_ASSERT(!m_isFrozen,
			"FrameDescriptorSystem::AllocateDescriptor called after FinalizeFrame(). "
			"Do all frame descriptor allocations in Prepare phase.");
		PHOTON_ASSERT(CheckHeapKind(heapkind), "Invalid Heapkind");
		EnsureSpace(1, heapkind);

		// 至此一定有空间容纳接下来的descriptor
		FrameDescriptorHandle handle;
		handle.heapKind = heapkind;
		handle.index = m_startPosIndex;
		m_startPosIndex++;
		m_frameHeapSize++;
		return handle;
	}

	FrameDescriptorRange FrameDescriptorSystem::AllocateDescriptors(uint32_t count, DescriptorHeapKind heapkind)
	{
		PHOTON_ASSERT(CheckHeapKind(heapkind), "Invalid Heapkind");
		EnsureSpace(count, heapkind);

		FrameDescriptorHandle handle;
		handle.heapKind = heapkind;
		handle.index = m_startPosIndex;
		m_startPosIndex += count;
		m_frameHeapSize += count;

		FrameDescriptorRange range;
		range.count = count;
		range.startHandle = handle;
		range.descriptorPerSize = m_descriptorPerSize;
		return range;
	}

	void FrameDescriptorSystem::EnsureSpace(uint32_t count, DescriptorHeapKind heapkind)
	{
		PHOTON_ASSERT(CheckHeapKind(heapkind), "Invalid Heapkind");

		if (HasEnoughSpace(count))
			return;

		uint32_t required = m_startPosIndex + count;
		PHOTON_ASSERT(required >= 1000000, "the limit for shader visible view heaps is 1000000");
		uint32_t newSize = std::max(required, m_heapSize * 3 / 2 + 64);

		auto newHeap = CreateHeap(newSize);
		CopyHeap(m_heaps[m_currFrameIndex].Get(), newHeap.Get());
		m_heaps[m_currFrameIndex] = newHeap;
		m_heapSize = newSize;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE FrameDescriptorSystem::GetGpuHandle(FrameDescriptorHandle handle)
	{
		
		auto startHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_heaps[m_currFrameIndex]->GetGPUDescriptorHandleForHeapStart());
		startHandle.Offset(handle.index, m_descriptorPerSize);
		return startHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FrameDescriptorSystem::GetCpuHandle(FrameDescriptorHandle handle)
	{
		auto startHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_heaps[m_currFrameIndex]->GetCPUDescriptorHandleForHeapStart());
		startHandle.Offset(handle.index, m_descriptorPerSize);
		return startHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE FrameDescriptorSystem::GetGpuHandle(FrameDescriptorRange range)
	{
		auto startHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_heaps[m_currFrameIndex]->GetGPUDescriptorHandleForHeapStart());
		startHandle.Offset(range.startHandle.index, m_descriptorPerSize);
		return startHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FrameDescriptorSystem::GetCpuHandle(FrameDescriptorRange range)
	{
		auto startHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_heaps[m_currFrameIndex]->GetCPUDescriptorHandleForHeapStart());
		startHandle.Offset(range.startHandle.index, m_descriptorPerSize);
		return startHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE FrameDescriptorSystem::GetGpuHandle(FrameDescriptorRange range, uint32_t offset)
	{
		auto startHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_heaps[m_currFrameIndex]->GetGPUDescriptorHandleForHeapStart());
		PHOTON_ASSERT(offset < range.count, "提供了超出当前FrameDescriptorRange管理范围的offset！");
		startHandle.Offset(range.startHandle.index + offset, m_descriptorPerSize);
		return startHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FrameDescriptorSystem::GetCpuHandle(FrameDescriptorRange range, uint32_t offset)
	{
		auto startHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_heaps[m_currFrameIndex]->GetCPUDescriptorHandleForHeapStart());
		PHOTON_ASSERT(offset < range.count, "提供了超出当前FrameDescriptorRange管理范围的offset！");
		startHandle.Offset(range.startHandle.index + offset, m_descriptorPerSize);
		return startHandle;
	}

	ID3D12DescriptorHeap* FrameDescriptorSystem::GetCurrentCbvSrvUavHeap()
	{
		return m_heaps[m_currFrameIndex].Get();
	}

	bool FrameDescriptorSystem::HasEnoughSpace(uint32_t count)
	{
		if (m_startPosIndex + count >= m_heapSize)
		{
			return false;
		}
		return true;
	}

	bool FrameDescriptorSystem::CheckHeapKind(DescriptorHeapKind heapkind) const
	{
		if (RenderUtil::HeapKindToDXType(heapkind) != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			return false;

		return true;
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> FrameDescriptorSystem::CreateHeap(uint32_t size)
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> ret;
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		desc.NumDescriptors = size;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&ret));
		return ret;
	}

	void FrameDescriptorSystem::CopyHeap(ID3D12DescriptorHeap* src, ID3D12DescriptorHeap* dst)
	{
		uint32_t descriptorCount = src->GetDesc().NumDescriptors;
		PHOTON_ASSERT(descriptorCount <= dst->GetDesc().NumDescriptors, "dst heap size not enough!");
		auto dstHandle = dst->GetCPUDescriptorHandleForHeapStart();
		auto srcHandle = src->GetCPUDescriptorHandleForHeapStart();

		m_device->CopyDescriptors(1, &dstHandle, &descriptorCount,
			1, &srcHandle, &descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}
