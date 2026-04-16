#include "FrameAllocatorSystem.h"

namespace photon
{
	bool FrameAllocatorSystem::Initialize(
		DescriptorSystem* descriptorSystem,
		GpuResourceManager* gpuResMgr,
		DX12RHI* rhi)
	{
		m_descriptorSystem = descriptorSystem;
		m_gpuResMgr = gpuResMgr;
		m_rhi = rhi;

		m_frameBufferSystem = std::make_shared<FrameBufferSystem>();
		m_frameDescriptorSystem = std::make_shared<FrameDescriptorSystem>();

		m_frameBufferSystem->Initialize(gpuResMgr);
		m_frameDescriptorSystem->Initialize(rhi->GetDevice());

		return true;
	}

	void FrameAllocatorSystem::BeginFrame(UINT frameIndex)
	{
		m_frameBufferSystem->BeginFrame(frameIndex);
		m_frameDescriptorSystem->BeginFrame(frameIndex);

		m_frameBufferSystem->Clear();
		m_frameDescriptorSystem->Clear();
		m_hashedHandles.clear();

		canBootStrap = false;
	}

	void FrameAllocatorSystem::BeginBootstrapFrame()
	{
		if (!bootStrapOn && canBootStrap)
		{
			m_frameBufferSystem->BeginFrame(0);
			m_frameBufferSystem->Clear();

			m_frameDescriptorSystem->BeginFrame(0);
			m_frameDescriptorSystem->Clear();
			m_hashedHandles.clear();

			bootStrapOn = true;
		}
	}

	void FrameAllocatorSystem::EndBootsstrapFrame()
	{
		if (bootStrapOn && canBootStrap)
		{
			bootStrapOn = false;
		}
	}

	FrameDescriptorHandle FrameAllocatorSystem::AllocateFrameCbvDescriptor()
	{
		PHOTON_ASSERT(!m_frameDescriptorSystem->IsFrozen(),
			"AllocateFrameCbvDescriptor must be called before FinalizeFrameDescriptors().");
		return m_frameDescriptorSystem->AllocateDescriptor(DescriptorHeapKind::Cbv);
	}

	FrameDescriptorRange FrameAllocatorSystem::AllocateFrameCbvDescriptors(uint32_t count)
	{
		PHOTON_ASSERT(!m_frameDescriptorSystem->IsFrozen(),
			"AllocateFrameCbvDescriptors must be called before FinalizeFrameDescriptors().");
		return m_frameDescriptorSystem->AllocateDescriptors(count, DescriptorHeapKind::Cbv);
	}

	FrameDescriptorHandle FrameAllocatorSystem::AttachToFrameHeap(const DescriptorHandle& handle)
	{
		PHOTON_ASSERT(!m_frameDescriptorSystem->IsFrozen(),
			"AttachToFrameHeap must be called in Prepare phase before FinalizeFrameDescriptors().");
		return HashDescriptorHandle(handle);
	}

	std::vector<FrameDescriptorHandle> FrameAllocatorSystem::AttachToFrameHeap(
		const std::vector<DescriptorHandle>& handles)
	{
		PHOTON_ASSERT(!m_frameDescriptorSystem->IsFrozen(),
			"AttachToFrameHeap(vector) must be called in Prepare phase before FinalizeFrameDescriptors().");

		std::vector<FrameDescriptorHandle> ret;
		ret.reserve(handles.size());

		for (const auto& des : handles)
		{
			ret.emplace_back(AttachToFrameHeap(des));
		}
		return ret;
	}

	FrameDescriptorRange FrameAllocatorSystem::BatchAttachToFrameHeap(const std::vector<DescriptorHandle>& handles)
	{
		PHOTON_ASSERT(!m_frameDescriptorSystem->IsFrozen(),
			"BatchAttachToFrameHeap must be called in Prepare phase before FinalizeFrameDescriptors().");
		PHOTON_ASSERT(!handles.empty(), "Empty Handles!");

		uint32_t rangeSize = static_cast<uint32_t>(handles.size());
		FrameDescriptorRange frameRange =
			m_frameDescriptorSystem->AllocateDescriptors(rangeSize, handles[0].heapKind);

		auto start = m_frameDescriptorSystem->GetCpuHandle(frameRange.startHandle);

		m_descriptorSystem->CopyBatchToCpuHandle(
			handles,
			std::vector<uint32_t>(rangeSize, 1),
			{ start },
			{ rangeSize });

		return frameRange;
	}

	void FrameAllocatorSystem::FinalizeFrameDescriptors()
	{
		m_frameDescriptorSystem->FinalizeFrame();
	}

	bool FrameAllocatorSystem::IsDescriptorHeapFrozen() const
	{
		return m_frameDescriptorSystem->IsFrozen();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE FrameAllocatorSystem::GetGpuHandle(FrameDescriptorHandle handle)
	{
		return m_frameDescriptorSystem->GetGpuHandle(handle);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FrameAllocatorSystem::GetCpuHandle(FrameDescriptorHandle handle)
	{
		return m_frameDescriptorSystem->GetCpuHandle(handle);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE FrameAllocatorSystem::GetGpuHandle(FrameDescriptorRange range)
	{
		return m_frameDescriptorSystem->GetGpuHandle(range);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FrameAllocatorSystem::GetCpuHandle(FrameDescriptorRange range)
	{
		return m_frameDescriptorSystem->GetCpuHandle(range);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE FrameAllocatorSystem::GetGpuHandle(FrameDescriptorRange range, uint32_t offset)
	{
		return m_frameDescriptorSystem->GetGpuHandle(range, offset);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FrameAllocatorSystem::GetCpuHandle(FrameDescriptorRange range, uint32_t offset)
	{
		return m_frameDescriptorSystem->GetCpuHandle(range, offset);
	}

	ID3D12DescriptorHeap* FrameAllocatorSystem::GetCurrentCbvSrvUavHeap()
	{
		return m_frameDescriptorSystem->GetCurrentCbvSrvUavHeap();
	}

	FrameDescriptorHandle* FrameAllocatorSystem::TryGetFrameHandle(const DescriptorHandle& handle)
	{
		auto it = m_hashedHandles.find(handle);
		if (it != m_hashedHandles.end())
			return &it->second;
		return nullptr;
	}

	FrameDescriptorHandle FrameAllocatorSystem::HashDescriptorHandle(const DescriptorHandle& handle)
	{
		FrameDescriptorHandle* hashed = TryGetFrameHandle(handle);
		if (hashed != nullptr)
			return *hashed;

		FrameDescriptorHandle frameHandle =
			m_frameDescriptorSystem->AllocateDescriptor(handle.heapKind);

		auto dst = m_frameDescriptorSystem->GetCpuHandle(frameHandle);
		m_descriptorSystem->CopyToCpuHandle(handle, dst);

		m_hashedHandles.insert({ handle, frameHandle });
		return frameHandle;
	}
}