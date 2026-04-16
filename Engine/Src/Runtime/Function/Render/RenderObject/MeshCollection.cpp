#include "MeshCollection.h"

#include <algorithm>

#include "Function/Util/RenderUtil.h"
#include "Macro.h"

namespace photon
{
	bool MeshCollection::Initialize(
		RenderResourceFactory* factory,
		GpuResourceManager* gpuResMgr,
		VertexLayoutRegistry* vertexLayoutRegistry)
	{
		m_factory = factory;
		m_gpuResMgr = gpuResMgr;
		m_vertexLayoutRegistry = vertexLayoutRegistry;
		return m_factory != nullptr
			&& m_gpuResMgr != nullptr
			&& m_vertexLayoutRegistry != nullptr;
	}

	void MeshCollection::Shutdown()
	{
		for (auto& [layoutHash, chunks] : m_vertexBuckets)
		{
			for (auto& c : chunks)
			{
				if (c.bigVb.buffer.IsValid())
					m_gpuResMgr->DestoryResourceImmediate(c.bigVb.buffer);
			}
		}

		for (auto& [format, chunks] : m_indexBuckets)
		{
			for (auto& c : chunks)
			{
				if (c.bigIb.buffer.IsValid())
					m_gpuResMgr->DestoryResourceImmediate(c.bigIb.buffer);
			}
		}

		m_vertexBuckets.clear();
		m_indexBuckets.clear();
		m_allocations.clear();

		ResetAll();

		m_factory = nullptr;
		m_gpuResMgr = nullptr;
		m_vertexLayoutRegistry = nullptr;
	}

	uint32_t MeshCollection::AlignUp(uint32_t x, uint32_t a)
	{
		return (x + a - 1) / a * a;
	}

	bool MeshCollection::AllocateFromRanges(
		std::vector<FreeRange>& ranges,
		uint32_t sizeInBytes,
		uint32_t alignment,
		uint32_t& outOffset)
	{
		for (size_t i = 0; i < ranges.size(); ++i)
		{
			FreeRange r = ranges[i];
			const uint32_t alignedOffset = AlignUp(r.offset, alignment);
			const uint32_t padding = alignedOffset - r.offset;

			if (r.size < padding || (r.size - padding) < sizeInBytes)
				continue;

			const uint32_t remainBefore = padding;
			const uint32_t remainAfter = r.size - padding - sizeInBytes;
			outOffset = alignedOffset;

			std::vector<FreeRange> replacement;
			if (remainBefore > 0)
				replacement.push_back({ r.offset, remainBefore });
			if (remainAfter > 0)
				replacement.push_back({ alignedOffset + sizeInBytes, remainAfter });

			ranges.erase(ranges.begin() + static_cast<long long>(i));
			ranges.insert(
				ranges.begin() + static_cast<long long>(i),
				replacement.begin(),
				replacement.end());
			return true;
		}

		return false;
	}

	void MeshCollection::FreeToRangesAndMerge(
		std::vector<FreeRange>& ranges,
		uint32_t offset,
		uint32_t size)
	{
		ranges.push_back({ offset, size });

		std::sort(ranges.begin(), ranges.end(),
			[](const FreeRange& a, const FreeRange& b)
			{
				return a.offset < b.offset;
			});

		std::vector<FreeRange> merged;
		merged.reserve(ranges.size());

		for (const auto& r : ranges)
		{
			if (merged.empty())
			{
				merged.push_back(r);
				continue;
			}

			FreeRange& back = merged.back();
			if (back.offset + back.size >= r.offset)
			{
				const uint32_t end = std::max(back.offset + back.size, r.offset + r.size);
				back.size = end - back.offset;
			}
			else
			{
				merged.push_back(r);
			}
		}

		ranges = std::move(merged);
	}

	MeshCollection::VertexChunk MeshCollection::CreateVertexChunk(
		uint64_t layoutHash,
		uint32_t strideInBytes,
		uint32_t sizeInBytes,
		bool dedicated,
		const std::string& debugName)
	{
		VertexChunk c = {};
		c.layoutHash = layoutHash;
		c.strideInBytes = strideInBytes;
		c.sizeInBytes = sizeInBytes;
		c.dedicated = dedicated;
		c.liveAllocations = 0;
		c.freeRanges.push_back({ 0, sizeInBytes });

		// 这里 bigVb 就是整个 chunk 的绑定单元
		c.bigVb = m_factory->CreateVertexBuffer(
			nullptr,
			sizeInBytes,
			strideInBytes,
			debugName.empty() ? "MeshVBChunk" : debugName + "_VBChunk");

		return c;
	}

	MeshCollection::IndexChunk MeshCollection::CreateIndexChunk(
		DXGI_FORMAT format,
		uint32_t sizeInBytes,
		bool dedicated,
		const std::string& debugName)
	{
		IndexChunk c = {};
		c.format = format;
		c.sizeInBytes = sizeInBytes;
		c.dedicated = dedicated;
		c.liveAllocations = 0;
		c.freeRanges.push_back({ 0, sizeInBytes });

		// bigIb 也是整个 chunk 的绑定单元，format 固定
		c.bigIb = m_factory->CreateIndexBuffer(
			nullptr,
			sizeInBytes,
			format,
			debugName.empty() ? "MeshIBChunk" : debugName + "_IBChunk");

		return c;
	}

	MeshCollection::BufferSubAlloc MeshCollection::AllocateVertexBytes(
		uint64_t layoutHash,
		uint32_t strideInBytes,
		uint32_t sizeInBytes,
		const std::string& debugName)
	{
		PHOTON_ASSERT(strideInBytes > 0, "Vertex stride 不能为 0");
		PHOTON_ASSERT(sizeInBytes % strideInBytes == 0, "VertexBuffer size 必须是 stride 的整数倍");

		BufferSubAlloc out = {};
		const bool dedicated = sizeInBytes > (kVertexChunkBytes * 3u / 4u);

		auto& chunks = m_vertexBuckets[layoutHash];

		if (dedicated)
		{
			const uint32_t chunkSize = AlignUp(sizeInBytes, strideInBytes);
			const uint32_t chunkIndex = static_cast<uint32_t>(chunks.size());
			chunks.push_back(CreateVertexChunk(layoutHash, strideInBytes, chunkSize, true, debugName));

			uint32_t offset = 0;
			const bool ok = AllocateFromRanges(
				chunks[chunkIndex].freeRanges,
				sizeInBytes,
				strideInBytes,
				offset);

			PHOTON_ASSERT(ok, "Dedicated VB chunk 分配失败");

			chunks[chunkIndex].liveAllocations++;

			out.chunkIndex = chunkIndex;
			out.offsetInBytes = offset;
			out.sizeInBytes = sizeInBytes;
			out.firstElement = offset / strideInBytes;
			out.elementCount = sizeInBytes / strideInBytes;
			out.dedicated = true;
			return out;
		}

		for (uint32_t i = 0; i < chunks.size(); ++i)
		{
			auto& c = chunks[i];
			if (c.dedicated)
				continue;

			uint32_t offset = 0;
			if (AllocateFromRanges(c.freeRanges, sizeInBytes, strideInBytes, offset))
			{
				c.liveAllocations++;

				out.chunkIndex = i;
				out.offsetInBytes = offset;
				out.sizeInBytes = sizeInBytes;
				out.firstElement = offset / strideInBytes;
				out.elementCount = sizeInBytes / strideInBytes;
				out.dedicated = false;
				return out;
			}
		}

		const uint32_t chunkIndex = static_cast<uint32_t>(chunks.size());
		chunks.push_back(CreateVertexChunk(layoutHash, strideInBytes, kVertexChunkBytes, false, debugName));

		uint32_t offset = 0;
		const bool ok = AllocateFromRanges(
			chunks[chunkIndex].freeRanges,
			sizeInBytes,
			strideInBytes,
			offset);

		PHOTON_ASSERT(ok, "新建 VB chunk 后仍分配失败");

		chunks[chunkIndex].liveAllocations++;

		out.chunkIndex = chunkIndex;
		out.offsetInBytes = offset;
		out.sizeInBytes = sizeInBytes;
		out.firstElement = offset / strideInBytes;
		out.elementCount = sizeInBytes / strideInBytes;
		out.dedicated = false;
		return out;
	}

	MeshCollection::BufferSubAlloc MeshCollection::AllocateIndexBytes(
		DXGI_FORMAT format,
		uint32_t indexStrideInBytes,
		uint32_t sizeInBytes,
		const std::string& debugName)
	{
		PHOTON_ASSERT(indexStrideInBytes == 2u || indexStrideInBytes == 4u,
			"Index stride 只支持 2 / 4");
		PHOTON_ASSERT(sizeInBytes % indexStrideInBytes == 0,
			"IndexBuffer size 必须是 index stride 的整数倍");

		BufferSubAlloc out = {};
		const bool dedicated = sizeInBytes > (kIndexChunkBytes * 3u / 4u);

		auto& chunks = m_indexBuckets[format];

		if (dedicated)
		{
			const uint32_t chunkSize = AlignUp(sizeInBytes, indexStrideInBytes);
			const uint32_t chunkIndex = static_cast<uint32_t>(chunks.size());
			chunks.push_back(CreateIndexChunk(format, chunkSize, true, debugName));

			uint32_t offset = 0;
			const bool ok = AllocateFromRanges(
				chunks[chunkIndex].freeRanges,
				sizeInBytes,
				indexStrideInBytes,
				offset);

			PHOTON_ASSERT(ok, "Dedicated IB chunk 分配失败");

			chunks[chunkIndex].liveAllocations++;

			out.chunkIndex = chunkIndex;
			out.offsetInBytes = offset;
			out.sizeInBytes = sizeInBytes;
			out.firstElement = offset / indexStrideInBytes;
			out.elementCount = sizeInBytes / indexStrideInBytes;
			out.dedicated = true;
			return out;
		}

		for (uint32_t i = 0; i < chunks.size(); ++i)
		{
			auto& c = chunks[i];
			if (c.dedicated)
				continue;

			uint32_t offset = 0;
			if (AllocateFromRanges(c.freeRanges, sizeInBytes, indexStrideInBytes, offset))
			{
				c.liveAllocations++;

				out.chunkIndex = i;
				out.offsetInBytes = offset;
				out.sizeInBytes = sizeInBytes;
				out.firstElement = offset / indexStrideInBytes;
				out.elementCount = sizeInBytes / indexStrideInBytes;
				out.dedicated = false;
				return out;
			}
		}

		const uint32_t chunkIndex = static_cast<uint32_t>(chunks.size());
		chunks.push_back(CreateIndexChunk(format, kIndexChunkBytes, false, debugName));

		uint32_t offset = 0;
		const bool ok = AllocateFromRanges(
			chunks[chunkIndex].freeRanges,
			sizeInBytes,
			indexStrideInBytes,
			offset);

		PHOTON_ASSERT(ok, "新建 IB chunk 后仍分配失败");

		chunks[chunkIndex].liveAllocations++;

		out.chunkIndex = chunkIndex;
		out.offsetInBytes = offset;
		out.sizeInBytes = sizeInBytes;
		out.firstElement = offset / indexStrideInBytes;
		out.elementCount = sizeInBytes / indexStrideInBytes;
		out.dedicated = false;
		return out;
	}

	void MeshCollection::FreeVertexBytes(uint64_t layoutHash, const BufferSubAlloc& alloc)
	{
		auto it = m_vertexBuckets.find(layoutHash);
		if (it == m_vertexBuckets.end())
			return;

		auto& chunks = it->second;
		if (alloc.chunkIndex >= chunks.size())
			return;

		auto& c = chunks[alloc.chunkIndex];
		FreeToRangesAndMerge(c.freeRanges, alloc.offsetInBytes, alloc.sizeInBytes);
		PHOTON_ASSERT(c.liveAllocations > 0, "VB liveAllocations underflow");
		c.liveAllocations--;

		if (c.dedicated && c.liveAllocations == 0)
		{
			if (c.bigVb.buffer.IsValid())
				m_gpuResMgr->DestoryResourceImmediate(c.bigVb.buffer);
			c = {};
		}
	}

	void MeshCollection::FreeIndexBytes(DXGI_FORMAT format, const BufferSubAlloc& alloc)
	{
		auto it = m_indexBuckets.find(format);
		if (it == m_indexBuckets.end())
			return;

		auto& chunks = it->second;
		if (alloc.chunkIndex >= chunks.size())
			return;

		auto& c = chunks[alloc.chunkIndex];
		FreeToRangesAndMerge(c.freeRanges, alloc.offsetInBytes, alloc.sizeInBytes);
		PHOTON_ASSERT(c.liveAllocations > 0, "IB liveAllocations underflow");
		c.liveAllocations--;

		if (c.dedicated && c.liveAllocations == 0)
		{
			if (c.bigIb.buffer.IsValid())
				m_gpuResMgr->DestoryResourceImmediate(c.bigIb.buffer);
			c = {};
		}
	}

	MeshHandle MeshCollection::CreateFromAsset(const MeshAsset& asset)
	{
		if (!m_factory || !m_vertexLayoutRegistry || !asset.HasCpuData())
			return {};

		const uint32_t vbBytes = asset.cpuData.GetVertexBufferSize();
		const uint32_t ibBytes = asset.cpuData.GetIndexBufferSize();
		if (vbBytes == 0 || ibBytes == 0)
			return {};

		const uint32_t vertexStride = asset.cpuData.vertexLayout.stride;
		const DXGI_FORMAT indexFormat = RenderUtil::ToDxgiFormat(asset.cpuData.indexFormat);
		const uint32_t indexStride = (indexFormat == DXGI_FORMAT_R16_UINT) ? 2u : 4u;
		const uint64_t layoutHash = m_vertexLayoutRegistry->Register(asset.cpuData.vertexLayout);

		BufferSubAlloc vbAlloc = AllocateVertexBytes(
			layoutHash,
			vertexStride,
			vbBytes,
			asset.meta.debugName);

		BufferSubAlloc ibAlloc = AllocateIndexBytes(
			indexFormat,
			indexStride,
			ibBytes,
			asset.meta.debugName);

		MeshHandle handle = AllocateHandle();
		auto* entry = TryGetEntry(handle);
		if (!entry)
		{
			FreeVertexBytes(layoutHash, vbAlloc);
			FreeIndexBytes(indexFormat, ibAlloc);
			return {};
		}

		auto& vbChunks = m_vertexBuckets[layoutHash];
		auto& ibChunks = m_indexBuckets[indexFormat];

		VertexBuffer meshVb = m_factory->UpdateVertexBuffer(
			vbChunks[vbAlloc.chunkIndex].bigVb,
			vbAlloc.offsetInBytes,
			asset.cpuData.vertexData.data(),
			vbBytes,
			vertexStride,
			asset.meta.debugName + ":meshVB");

		IndexBuffer meshIb = m_factory->UpdateIndexBuffer(
			ibChunks[ibAlloc.chunkIndex].bigIb,
			ibAlloc.offsetInBytes,
			asset.cpuData.indexData.data(),
			ibBytes,
			indexFormat,
			asset.meta.debugName + ":meshIB");

		MeshRenderResource rr = {};
		rr.handle = handle;
		rr.assetGuid = asset.meta.guid;
		rr.usage = asset.usage;
		rr.vertexLayout = asset.cpuData.vertexLayout;
		rr.localBounds = asset.localBounds;

		// 这里是 chunk 级绑定单元
		rr.bigVb = vbChunks[vbAlloc.chunkIndex].bigVb;
		rr.bigIb = ibChunks[ibAlloc.chunkIndex].bigIb;

		// 这里是 mesh 在 chunk 内的子视图
		rr.meshVb = meshVb;
		rr.meshIb = meshIb;

		if (!asset.submeshes.empty())
		{
			rr.submeshes.reserve(asset.submeshes.size());
			for (const auto& src : asset.submeshes)
			{
				SubmeshRenderDesc dst = {};
				dst.name = src.name;
				dst.indexCount = src.indexCount;

				dst.indexStartInMesh = src.indexStart;
				dst.vertexStartInMesh = src.vertexStart;

				dst.materialSlot = src.materialSlot;
				dst.localBounds = src.localBounds;
				rr.submeshes.push_back(dst);
			}
		}
		else
		{
			SubmeshRenderDesc whole = {};
			whole.name = asset.meta.debugName.empty() ? "Mesh" : asset.meta.debugName;
			whole.indexCount = asset.cpuData.indexCount;
			whole.indexStartInMesh = ibAlloc.firstElement;
			whole.vertexStartInMesh = vbAlloc.firstElement;
			whole.materialSlot = 0;
			whole.localBounds = asset.localBounds;
			rr.submeshes.push_back(whole);
		}

		if (asset.usage != MeshUsage::Static)
		{
			rr.cpuMirror = std::make_unique<MeshCpuData>(asset.cpuData);
			rr.dynamicState = std::make_unique<DynamicMeshState>();
		}

		entry->resource = std::move(rr);

		MeshPoolAllocation alloc = {};
		alloc.vbLayoutHash = layoutHash;
		alloc.ibFormat = indexFormat;
		alloc.vbAlloc = vbAlloc;
		alloc.ibAlloc = ibAlloc;
		m_allocations[handle.handle] = alloc;

		return handle;
	}

	void MeshCollection::Destroy(MeshHandle h)
	{
		auto it = m_allocations.find(h.handle);
		if (it != m_allocations.end())
		{
			FreeVertexBytes(it->second.vbLayoutHash, it->second.vbAlloc);
			FreeIndexBytes(it->second.ibFormat, it->second.ibAlloc);
			m_allocations.erase(it);
		}

		FreeHandle(h);
	}

	bool MeshCollection::IsValid(MeshHandle h) const
	{
		return IsHandleValid(h);
	}
}