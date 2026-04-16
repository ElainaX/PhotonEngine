#pragma once
#include "FrameResource.h"
#include "FrameBufferSystem.h"
#include "FrameDescriptorSystem.h"
#include "Function/Render/DX12RHI/DescriptorHeap/DescriptorSystem.h"
#include "Function/Util/RenderUtil.h"

namespace photon
{
	class FrameAllocatorSystem
	{
	public:
		FrameAllocatorSystem() = default;

		bool Initialize(DescriptorSystem* descriptorSystem, GpuResourceManager* gpuResMgr, DX12RHI* rhi);

		void BeginFrame(UINT frameIndex);

		void BeginBootstrapFrame();
		void EndBootsstrapFrame();

		template<typename T>
		FrameResourceRange AllocateFrameUploadBuffer256(uint32_t elementCount)
		{
			auto frameResRange = m_frameBufferSystem->AllocateAlignment256<T>(
				elementCount, FrameResourceType::UploadBuffer);
			return frameResRange;
		}

		FrameResourceRange AllocateFrameUploadBuffer(uint32_t sizeInBytes)
		{
			return m_frameBufferSystem->Allocate(sizeInBytes, FrameResourceType::UploadBuffer);
		}

		FrameResourceRange AllocateFrameDefaultBuffer(uint32_t sizeInBytes)
		{
			return m_frameBufferSystem->Allocate(sizeInBytes, FrameResourceType::DefaultBuffer);
		}

		// -------- Prepare phase only --------
		FrameDescriptorHandle AllocateFrameCbvDescriptor();
		FrameDescriptorRange AllocateFrameCbvDescriptors(uint32_t count);

		FrameDescriptorHandle AttachToFrameHeap(const DescriptorHandle& handle);
		std::vector<FrameDescriptorHandle> AttachToFrameHeap(const std::vector<DescriptorHandle>& handles);

		FrameDescriptorRange BatchAttachToFrameHeap(const std::vector<DescriptorHandle>& handles);

		// -------- Freeze before Execute --------
		void FinalizeFrameDescriptors();
		bool IsDescriptorHeapFrozen() const;

		// -------- Execute phase read-only usage --------
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(FrameDescriptorHandle handle);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(FrameDescriptorHandle handle);

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(FrameDescriptorRange range);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(FrameDescriptorRange range);

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(FrameDescriptorRange range, uint32_t offset);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(FrameDescriptorRange range, uint32_t offset);

		ID3D12DescriptorHeap* GetCurrentCbvSrvUavHeap();

	private:
		FrameDescriptorHandle* TryGetFrameHandle(const DescriptorHandle& handle);
		FrameDescriptorHandle HashDescriptorHandle(const DescriptorHandle& handle);

	private:
		std::shared_ptr<FrameBufferSystem> m_frameBufferSystem = nullptr;
		std::shared_ptr<FrameDescriptorSystem> m_frameDescriptorSystem = nullptr;

		std::map<DescriptorHandle, FrameDescriptorHandle> m_hashedHandles;

		DescriptorSystem* m_descriptorSystem = nullptr;
		GpuResourceManager* m_gpuResMgr = nullptr;
		DX12RHI* m_rhi = nullptr;

		bool canBootStrap = true;
		bool bootStrapOn = false;
	};
}