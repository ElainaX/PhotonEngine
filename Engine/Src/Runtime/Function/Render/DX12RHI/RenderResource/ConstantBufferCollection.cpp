#include "ConstantBufferCollection.h"
#include "Function/Render/DX12RHI/RenderResource/RenderResourceFactory.h"
#include <algorithm>
#include <cstring>

namespace photon
{
	bool ConstantBufferCollection::Initialize(
		RenderResourceFactory* factory,
		DescriptorSystem* descriptorSystem,
		GpuResourceManager* gpuResMgr)
	{
		m_factory = factory;
		m_descriptorSystem = descriptorSystem;
		m_gpuResMgr = gpuResMgr;

		return m_factory && m_descriptorSystem && m_gpuResMgr;
	}

	void ConstantBufferCollection::Shutdown()
	{
		for (auto& [stride, bucket] : m_buckets)
		{
			for (auto& chunk : bucket.chunks)
			{
				ReleaseChunk(chunk);
			}
		}
		m_buckets.clear();

		m_factory = nullptr;
		m_descriptorSystem = nullptr;
		m_gpuResMgr = nullptr;
	}

	PersistentConstantBufferAllocation ConstantBufferCollection::AllocateRaw(
		const void* data,
		uint32_t logicalSizeInBytes,
		const std::string& debugName)
	{
		if (!m_factory || !m_descriptorSystem || !m_gpuResMgr)
			return {};

		if (logicalSizeInBytes == 0)
			return {};

		const uint32_t alignedStride = AlignStride(logicalSizeInBytes);
		Bucket& bucket = GetOrCreateBucket(alignedStride);
		Chunk& chunk = GetOrCreateChunkWithFreeSlot(bucket, debugName);

		if (chunk.freeList.empty())
			return {};

		const uint32_t slotIndex = chunk.freeList.back();
		chunk.freeList.pop_back();

		SlotRecord& slot = chunk.slots[slotIndex];
		slot.allocated = true;
		slot.logicalSizeInBytes = logicalSizeInBytes;

		const uint64_t offset = static_cast<uint64_t>(slotIndex) * alignedStride;

		std::vector<std::byte> zeroBytes;
		const void* srcData = data;
		if (!srcData)
		{
			zeroBytes.resize(logicalSizeInBytes, std::byte{ 0 });
			srcData = zeroBytes.data();
		}

		ConstantBuffer view = m_factory->UpdateConstantBuffer(
			chunk.bigBuffer,
			static_cast<uint32_t>(offset),
			srcData,
			logicalSizeInBytes,
			debugName.empty() ? "PersistentCB" : debugName);

		PersistentConstantBufferHandle handle = {};
		handle.stride = alignedStride;
		handle.chunkIndex = 0; // 后面补
		handle.slotIndex = slotIndex;
		handle.generation = slot.generation;

		// 找 chunkIndex
		Bucket& bucketRef = m_buckets[alignedStride];
		for (uint32_t i = 0; i < bucketRef.chunks.size(); ++i)
		{
			if (&bucketRef.chunks[i] == &chunk)
			{
				handle.chunkIndex = i;
				break;
			}
		}

		PersistentConstantBufferAllocation alloc = {};
		alloc.handle = handle;
		alloc.bufferView = view;
		alloc.cbv = chunk.slotCbvs[slotIndex];
		alloc.logicalSizeInBytes = logicalSizeInBytes;
		alloc.alignedStrideInBytes = alignedStride;

		slot.allocation = alloc;
		return alloc;
	}

	bool ConstantBufferCollection::Free(PersistentConstantBufferHandle handle)
	{
		SlotRecord* slot = TryGetSlotRecord(handle);
		if (!slot || !slot->allocated)
			return false;

		auto bucketIt = m_buckets.find(handle.stride);
		if (bucketIt == m_buckets.end())
			return false;

		Bucket& bucket = bucketIt->second;
		if (handle.chunkIndex >= bucket.chunks.size())
			return false;

		Chunk& chunk = bucket.chunks[handle.chunkIndex];
		if (handle.slotIndex >= chunk.slots.size())
			return false;

		slot->allocated = false;
		slot->logicalSizeInBytes = 0;
		slot->allocation = {};
		++slot->generation;
		if (slot->generation == 0)
			slot->generation = 1;

		chunk.freeList.push_back(handle.slotIndex);
		return true;
	}

	bool ConstantBufferCollection::UpdateRaw(
		PersistentConstantBufferHandle handle,
		const void* data,
		uint32_t logicalSizeInBytes)
	{
		SlotRecord* slot = TryGetSlotRecord(handle);
		if (!slot || !slot->allocated)
			return false;

		if (!data || logicalSizeInBytes == 0)
			return false;

		const uint32_t alignedStride = handle.stride;
		if (logicalSizeInBytes > alignedStride)
			return false;

		auto bucketIt = m_buckets.find(alignedStride);
		if (bucketIt == m_buckets.end())
			return false;

		Bucket& bucket = bucketIt->second;
		if (handle.chunkIndex >= bucket.chunks.size())
			return false;

		Chunk& chunk = bucket.chunks[handle.chunkIndex];
		if (handle.slotIndex >= chunk.slots.size())
			return false;

		const uint64_t offset = static_cast<uint64_t>(handle.slotIndex) * alignedStride;

		ConstantBuffer view = m_factory->UpdateConstantBuffer(
			chunk.bigBuffer,
			static_cast<uint32_t>(offset),
			data,
			logicalSizeInBytes,
			"PersistentCB_Update");

		slot->logicalSizeInBytes = logicalSizeInBytes;
		slot->allocation.bufferView = view;
		slot->allocation.logicalSizeInBytes = logicalSizeInBytes;
		return true;
	}

	const PersistentConstantBufferAllocation* ConstantBufferCollection::GetAllocation(
		PersistentConstantBufferHandle handle) const
	{
		const SlotRecord* slot = TryGetSlotRecord(handle);
		if (!slot || !slot->allocated)
			return nullptr;
		return &slot->allocation;
	}

	uint32_t ConstantBufferCollection::AlignStride(uint32_t logicalSizeInBytes)
	{
		return RenderUtil::GetConstantBufferByteSize(logicalSizeInBytes);
	}

	uint32_t ConstantBufferCollection::ComputeChunkCapacity(uint32_t alignedStrideInBytes)
	{
		if (alignedStrideInBytes == 0)
			return 1;

		uint32_t capacity = std::max(1u, kTargetChunkBytes / alignedStrideInBytes);
		capacity = std::min(capacity, kMaxElementsPerChunk);
		return std::max(1u, capacity);
	}

	ConstantBufferCollection::Bucket& ConstantBufferCollection::GetOrCreateBucket(uint32_t alignedStrideInBytes)
	{
		auto it = m_buckets.find(alignedStrideInBytes);
		if (it != m_buckets.end())
			return it->second;

		Bucket bucket = {};
		bucket.alignedStrideInBytes = alignedStrideInBytes;
		auto [newIt, inserted] = m_buckets.emplace(alignedStrideInBytes, std::move(bucket));
		return newIt->second;
	}

	ConstantBufferCollection::Chunk& ConstantBufferCollection::GetOrCreateChunkWithFreeSlot(
		Bucket& bucket,
		const std::string& debugName)
	{
		for (auto& chunk : bucket.chunks)
		{
			if (!chunk.freeList.empty())
				return chunk;
		}

		const uint32_t capacity = ComputeChunkCapacity(bucket.alignedStrideInBytes);
		bucket.chunks.push_back(CreateChunk(bucket.alignedStrideInBytes, capacity, debugName));
		return bucket.chunks.back();
	}

	ConstantBufferCollection::Chunk ConstantBufferCollection::CreateChunk(
		uint32_t alignedStrideInBytes,
		uint32_t capacity,
		const std::string& debugName)
	{
		Chunk chunk = {};
		chunk.alignedStrideInBytes = alignedStrideInBytes;
		chunk.capacity = std::max(1u, capacity);

		const uint32_t totalBytes = alignedStrideInBytes * chunk.capacity;
		const std::string chunkName = debugName.empty()
			? ("PersistentCBChunk_" + std::to_string(alignedStrideInBytes))
			: (debugName + "_PersistentCBChunk_" + std::to_string(alignedStrideInBytes));

		chunk.bigBuffer = m_factory->CreateConstantBuffer(
			nullptr,
			totalBytes,
			ResourceUsage::Dynamic,
			chunkName);

		DXResource* dxRes = m_gpuResMgr->GetResource(chunk.bigBuffer.buffer);
		if (!dxRes)
			return chunk;

		chunk.slotCbvs.resize(chunk.capacity);
		chunk.slots.resize(chunk.capacity);
		chunk.freeList.reserve(chunk.capacity);

		for (uint32_t i = 0; i < chunk.capacity; ++i)
		{
			ViewDesc cbvDesc = {};
			cbvDesc.type = ViewType::CBV;
			cbvDesc.dimension = ViewDimension::Buffer;
			cbvDesc.cbvOffsetInBytes =
				chunk.bigBuffer.range.cbvOffsetInBytes +
				static_cast<uint64_t>(i) * alignedStrideInBytes;
			cbvDesc.cbvSizeInBytes = alignedStrideInBytes;

			chunk.slotCbvs[i] = m_descriptorSystem->CreateDescriptor(dxRes, cbvDesc);
			chunk.freeList.push_back(chunk.capacity - 1 - i); // stack，先分配低位槽
		}

		return chunk;
	}

	ConstantBufferCollection::SlotRecord* ConstantBufferCollection::TryGetSlotRecord(
		PersistentConstantBufferHandle handle)
	{
		auto bucketIt = m_buckets.find(handle.stride);
		if (bucketIt == m_buckets.end())
			return nullptr;

		Bucket& bucket = bucketIt->second;
		if (handle.chunkIndex >= bucket.chunks.size())
			return nullptr;

		Chunk& chunk = bucket.chunks[handle.chunkIndex];
		if (handle.slotIndex >= chunk.slots.size())
			return nullptr;

		SlotRecord& slot = chunk.slots[handle.slotIndex];
		if (slot.generation != handle.generation)
			return nullptr;

		return &slot;
	}

	const ConstantBufferCollection::SlotRecord* ConstantBufferCollection::TryGetSlotRecord(
		PersistentConstantBufferHandle handle) const
	{
		auto bucketIt = m_buckets.find(handle.stride);
		if (bucketIt == m_buckets.end())
			return nullptr;

		const Bucket& bucket = bucketIt->second;
		if (handle.chunkIndex >= bucket.chunks.size())
			return nullptr;

		const Chunk& chunk = bucket.chunks[handle.chunkIndex];
		if (handle.slotIndex >= chunk.slots.size())
			return nullptr;

		const SlotRecord& slot = chunk.slots[handle.slotIndex];
		if (slot.generation != handle.generation)
			return nullptr;

		return &slot;
	}

	void ConstantBufferCollection::ReleaseChunk(Chunk& chunk)
	{
		for (auto h : chunk.slotCbvs)
		{
			if (h.IsValid())
				m_descriptorSystem->FreeDescriptor(h);
		}
		chunk.slotCbvs.clear();
		chunk.slots.clear();
		chunk.freeList.clear();

		if (chunk.bigBuffer.buffer.IsValid())
		{
			m_gpuResMgr->DestoryResourceImmediate(chunk.bigBuffer.buffer);
		}
		chunk.bigBuffer = {};
	}
}