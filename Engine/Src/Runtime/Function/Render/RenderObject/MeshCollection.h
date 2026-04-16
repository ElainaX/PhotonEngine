#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "Function/Util/ResourceSlotCollection.h"
#include "Function/Render/RenderObject/MeshAsset.h"
#include "Function/Render/DX12RHI/RenderResource/MeshRenderResource.h"
#include "Function/Render/DX12RHI/RenderResource/RenderResourceFactory.h"
#include "Function/Render/DX12RHI/RenderResource/VertexLayoutRegistry.h"

namespace photon
{
	class MeshCollection : public ResourceSlotCollection<MeshHandle, MeshRenderResource>
	{
	public:
		bool Initialize(
			RenderResourceFactory* factory,
			GpuResourceManager* gpuResMgr,
			VertexLayoutRegistry* vertexLayoutRegistry);

		void Shutdown();

		MeshHandle CreateFromAsset(const MeshAsset& asset);

		MeshRenderResource* TryGet(MeshHandle h)
		{
			auto* e = TryGetEntry(h);
			return e ? &e->resource : nullptr;
		}

		const MeshRenderResource* TryGet(MeshHandle h) const
		{
			auto* e = TryGetEntry(h);
			return e ? &e->resource : nullptr;
		}

		void Destroy(MeshHandle h);
		bool IsValid(MeshHandle h) const;

	private:
		struct FreeRange
		{
			uint32_t offset = 0;
			uint32_t size = 0;
		};

		struct VertexChunk
		{
			uint64_t layoutHash = 0;
			uint32_t strideInBytes = 0;

			VertexBuffer bigVb = {};
			uint32_t sizeInBytes = 0;

			bool dedicated = false;
			uint32_t liveAllocations = 0;

			std::vector<FreeRange> freeRanges;
		};

		struct IndexChunk
		{
			DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;

			IndexBuffer bigIb = {};
			uint32_t sizeInBytes = 0;

			bool dedicated = false;
			uint32_t liveAllocations = 0;

			std::vector<FreeRange> freeRanges;
		};

		struct BufferSubAlloc
		{
			uint32_t chunkIndex = 0;

			uint32_t offsetInBytes = 0;
			uint32_t sizeInBytes = 0;

			uint32_t firstElement = 0;
			uint32_t elementCount = 0;

			bool dedicated = false;
		};

		struct MeshPoolAllocation
		{
			uint64_t vbLayoutHash = 0;
			DXGI_FORMAT ibFormat = DXGI_FORMAT_R32_UINT;

			BufferSubAlloc vbAlloc = {};
			BufferSubAlloc ibAlloc = {};
		};

	private:
		static constexpr uint32_t kVertexChunkBytes = 16u * 1024u * 1024u; // 16 MB
		static constexpr uint32_t kIndexChunkBytes = 8u * 1024u * 1024u; // 8 MB
		static constexpr uint32_t kVertexAlignment = 16u;
		static constexpr uint32_t kIndexAlignment = 16u;

	private:
		static uint32_t AlignUp(uint32_t x, uint32_t a);

		static bool AllocateFromRanges(
			std::vector<FreeRange>& ranges,
			uint32_t sizeInBytes,
			uint32_t alignment,
			uint32_t& outOffset);

		static void FreeToRangesAndMerge(
			std::vector<FreeRange>& ranges,
			uint32_t offset,
			uint32_t size);

		BufferSubAlloc AllocateVertexBytes(
			uint64_t layoutHash,
			uint32_t strideInBytes,
			uint32_t sizeInBytes,
			const std::string& debugName);

		BufferSubAlloc AllocateIndexBytes(
			DXGI_FORMAT format,
			uint32_t indexStrideInBytes,
			uint32_t sizeInBytes,
			const std::string& debugName);

		void FreeVertexBytes(uint64_t layoutHash, const BufferSubAlloc& alloc);
		void FreeIndexBytes(DXGI_FORMAT format, const BufferSubAlloc& alloc);

		VertexChunk CreateVertexChunk(
			uint64_t layoutHash,
			uint32_t strideInBytes,
			uint32_t sizeInBytes,
			bool dedicated,
			const std::string& debugName);

		IndexChunk CreateIndexChunk(
			DXGI_FORMAT format,
			uint32_t sizeInBytes,
			bool dedicated,
			const std::string& debugName);

	private:
		std::map<ResourceHandle, MeshPoolAllocation> m_allocations;

		// 一个 vertexLayout 对应一个 chunk list
		std::unordered_map<uint64_t, std::vector<VertexChunk>> m_vertexBuckets;

		// 一个 index format 对应一个 chunk list
		std::unordered_map<DXGI_FORMAT, std::vector<IndexChunk>> m_indexBuckets;

		RenderResourceFactory* m_factory = nullptr;
		GpuResourceManager* m_gpuResMgr = nullptr;
		VertexLayoutRegistry* m_vertexLayoutRegistry = nullptr;
	};
}