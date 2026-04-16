#pragma once
#include "FrameResource.h"
#include "Function/Render/DX12RHI/FrameSyncSystem.h"
#include "Function/Render/DX12RHI/DescriptorHeap/Descriptor.h"


namespace photon
{
	class FrameDescriptorSystem
	{
	public:
		FrameDescriptorSystem() = default;

		bool Initialize(ID3D12Device* device);

		void BeginFrame(uint32_t frameIndex);
		void Clear();

		FrameDescriptorHandle AllocateDescriptor(DescriptorHeapKind heapkind);
		FrameDescriptorRange AllocateDescriptors(uint32_t count, DescriptorHeapKind heapkind);

		// Execute 前调用，冻结本帧 descriptor heap identity
		void FinalizeFrame();
		bool IsFrozen() const { return m_isFrozen; }



		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(FrameDescriptorHandle handle);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(FrameDescriptorHandle handle);

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(FrameDescriptorRange range);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(FrameDescriptorRange range);

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(FrameDescriptorRange range, uint32_t offset);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(FrameDescriptorRange range, uint32_t offset);

		ID3D12DescriptorHeap* GetCurrentCbvSrvUavHeap();

	private:
		// 传一个count，判断当前heap是否还拥有这么多空间，如果没有则扩容
		bool HasEnoughSpace(uint32_t count);
		bool CheckHeapKind(DescriptorHeapKind heapkind) const;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateHeap(uint32_t size);
		void CopyHeap(ID3D12DescriptorHeap* src, ID3D12DescriptorHeap* dst);
		void EnsureSpace(uint32_t count, DescriptorHeapKind heapkind);

	private:
		// 只存cbv，srv，uav
		std::array<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>, FrameSyncSystem::kMaxFramesInFlight> m_heaps;
		ID3D12Device* m_device = nullptr;

		uint32_t m_currFrameIndex = 0;
		uint32_t m_startPosIndex = 0;
		uint32_t m_descriptorPerSize = 0;

		uint32_t m_heapSize = 50000;
		uint32_t m_frameHeapSize = 0;

		bool m_isFrozen = false;
	};

}
