#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "Function/Render/DX12RHI/RenderResource/ConstantBuffer.h"
#include "Function/Render/DX12RHI/DescriptorHeap/DescriptorSystem.h"
#include "Function/Render/DX12RHI/GpuResourceManager.h"
#include "Function/Util/RenderUtil.h"

namespace photon
{
	class RenderResourceFactory;

	struct PersistentConstantBufferHandle
	{
		uint32_t stride = 0;      // bucket key
		uint32_t chunkIndex = 0;
		uint32_t slotIndex = 0;
		uint32_t generation = 0;  // 0 = invalid

		bool IsValid() const { return generation != 0; }
		auto operator<=>(const PersistentConstantBufferHandle&) const = default;
	};

	struct PersistentConstantBufferAllocation
	{
		PersistentConstantBufferHandle handle = {};

		ConstantBuffer bufferView = {};
		DescriptorHandle cbv = {};

		uint32_t logicalSizeInBytes = 0;
		uint32_t alignedStrideInBytes = 0;

		bool IsValid() const
		{
			return handle.IsValid() && bufferView.buffer.IsValid() && cbv.IsValid();
		}
	};

	class ConstantBufferCollection
	{
	public:
		ConstantBufferCollection() = default;
		~ConstantBufferCollection() = default;

		bool Initialize(
			RenderResourceFactory* factory,
			DescriptorSystem* descriptorSystem,
			GpuResourceManager* gpuResMgr);

		void Shutdown();

		template<typename T>
		PersistentConstantBufferAllocation Allocate(
			const T& value,
			const std::string& debugName = "")
		{
			return AllocateRaw(&value, static_cast<uint32_t>(sizeof(T)), debugName);
		}

		PersistentConstantBufferAllocation AllocateRaw(
			const void* data,
			uint32_t logicalSizeInBytes,
			const std::string& debugName = "");

		bool Free(PersistentConstantBufferHandle handle);

		template<typename T>
		bool Update(PersistentConstantBufferHandle handle, const T& value)
		{
			return UpdateRaw(handle, &value, static_cast<uint32_t>(sizeof(T)));
		}

		bool UpdateRaw(
			PersistentConstantBufferHandle handle,
			const void* data,
			uint32_t logicalSizeInBytes);

		const PersistentConstantBufferAllocation* GetAllocation(
			PersistentConstantBufferHandle handle) const;

	private:
		struct SlotRecord
		{
			uint32_t generation = 1;
			bool allocated = false;
			uint32_t logicalSizeInBytes = 0;

			PersistentConstantBufferAllocation allocation = {};
		};

		struct Chunk
		{
			ConstantBuffer bigBuffer = {};

			uint32_t alignedStrideInBytes = 0;
			uint32_t capacity = 0;

			std::vector<DescriptorHandle> slotCbvs;
			std::vector<SlotRecord> slots;
			std::vector<uint32_t> freeList; // stack
		};

		struct Bucket
		{
			uint32_t alignedStrideInBytes = 0;
			std::vector<Chunk> chunks;
		};

	private:
		static constexpr uint32_t kTargetChunkBytes = 256u * 1024u;
		static constexpr uint32_t kMaxElementsPerChunk = 1024u;

	private:
		static uint32_t AlignStride(uint32_t logicalSizeInBytes);
		static uint32_t ComputeChunkCapacity(uint32_t alignedStrideInBytes);

		Bucket& GetOrCreateBucket(uint32_t alignedStrideInBytes);
		Chunk& GetOrCreateChunkWithFreeSlot(Bucket& bucket, const std::string& debugName);
		Chunk CreateChunk(uint32_t alignedStrideInBytes, uint32_t capacity, const std::string& debugName);

		SlotRecord* TryGetSlotRecord(PersistentConstantBufferHandle handle);
		const SlotRecord* TryGetSlotRecord(PersistentConstantBufferHandle handle) const;

		void ReleaseChunk(Chunk& chunk);

	private:
		std::unordered_map<uint32_t, Bucket> m_buckets;

		RenderResourceFactory* m_factory = nullptr;
		DescriptorSystem* m_descriptorSystem = nullptr;
		GpuResourceManager* m_gpuResMgr = nullptr;
	};
}