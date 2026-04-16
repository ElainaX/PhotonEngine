#include "FrameBufferSystem.h"


namespace photon
{
	bool FrameBufferSystem::Initialize(GpuResourceManager* resMgr)
	{
		m_resMgr = resMgr;

		DXBufferDesc uploadDesc;
		uploadDesc.heapProp = HeapProp::Upload;
		uploadDesc.allowUav = false;
		uploadDesc.bufferSizeInBytes = m_poolByteSize;

		for (int i = 0; i < m_uploadBufferPools.size(); ++i)
		{
			GpuResourceHandle uploadBufferHandle = m_resMgr->CreateBuffer(uploadDesc);
			m_uploadBufferPools[i].push_back({ .gpuRes = uploadBufferHandle, .startPos = 0, .byteSize = 0 });
		}


		DXBufferDesc defaultDesc = uploadDesc;
		defaultDesc.heapProp = HeapProp::Default;

		for (int i = 0; i < m_defaultBufferPools.size(); ++i)
		{
			GpuResourceHandle defaultBufferHandle = m_resMgr->CreateBuffer(defaultDesc);
			m_defaultBufferPools[i].push_back({ .gpuRes = defaultBufferHandle, .startPos = 0, .byteSize = 0 });
		}


		return true;
	}

	void FrameBufferSystem::BeginFrame(UINT frameIndex)
	{
		m_currFrameIndex = frameIndex;
	}

	// 调用在BeginFrame之后
	void FrameBufferSystem::Clear()
	{

		auto& uploadBufferPool = m_uploadBufferPools[m_currFrameIndex];
		GpuResourceHandle rawHandle = uploadBufferPool.front().gpuRes;
		for (int i = 1; i < uploadBufferPool.size(); ++i)
		{
			if (rawHandle != uploadBufferPool[i].gpuRes)
			{
				m_resMgr->DestoryResourceImmediate(uploadBufferPool[i].gpuRes);
				rawHandle = uploadBufferPool[i].gpuRes;
			}
		}

		uploadBufferPool.resize(1);

		if (m_frameTotalByteSize > m_poolByteSize)
		{
			m_poolByteSize = m_frameTotalByteSize;

			DXBufferDesc uploadDesc;
			uploadDesc.heapProp = HeapProp::Upload;
			uploadDesc.allowUav = false;
			uploadDesc.bufferSizeInBytes = m_poolByteSize;
			auto newReshandle = m_resMgr->CreateBuffer(uploadDesc);
			m_resMgr->DestoryResourceImmediate(uploadBufferPool.front().gpuRes);
			uploadBufferPool[0] = BufferRecord{ .gpuRes = newReshandle, .startPos = 0, .byteSize = 0 };
		}

		m_frameTotalByteSize = 0;

	}

	FrameResourceRange FrameBufferSystem::RequireBuffer(uint64_t byteSize, FrameResourceType frametype)
	{
		switch (frametype)
		{
		case FrameResourceType::UploadBuffer:
			return RequireUploadBuffer(byteSize);
		case FrameResourceType::DefaultBuffer:
		case FrameResourceType::Unknown:
			break;
		}
		PHOTON_ASSERT(false, "不支持的FrameResourceType类型");
		return {};
	}

	FrameResourceRange FrameBufferSystem::RequireUploadBuffer(uint64_t byteSize)
	{
		auto& uploadBufferPool = m_uploadBufferPools[m_currFrameIndex];

		if (!HasEnoughUploadSpace(byteSize))
		{
			DXBufferDesc uploadDesc;
			uploadDesc.heapProp = HeapProp::Upload;
			uploadDesc.bufferSizeInBytes = byteSize * 2;
			GpuResourceHandle bufferHandle = m_resMgr->CreateBuffer(uploadDesc);
			uploadBufferPool.push_back({ .gpuRes = bufferHandle, .startPos = 0, .byteSize = 0 });
		}

		// 至此应该存在一个可分配的空闲区域
		m_frameTotalByteSize += byteSize;
		auto& record = uploadBufferPool.back();
		FrameResourceRange range;
		range.buffer = record.gpuRes;
		range.frameResType = FrameResourceType::UploadBuffer;
		range.range.cbvOffsetInBytes = record.startPos + record.byteSize;
		range.range.cbvSizeInBytes = byteSize;

		BufferRecord newRecord;
		newRecord.startPos = record.startPos + record.byteSize;
		newRecord.byteSize = byteSize;
		newRecord.gpuRes = record.gpuRes;
		uploadBufferPool.push_back(newRecord);

		return range;
	}

	bool FrameBufferSystem::HasEnoughUploadSpace(uint64_t byteSize)
	{
		auto& uploadBufferPool = m_uploadBufferPools[m_currFrameIndex];
		auto& record = uploadBufferPool.back();
		DXResource* res = m_resMgr->GetResource(record.gpuRes);
		uint64_t maxSize = res->dxDesc.Width;
		uint64_t totalSize = record.startPos + record.byteSize + byteSize;
		return maxSize >= totalSize;
	}

	bool FrameBufferSystem::HasEnoughDefaultSpace(uint64_t byteSize)
	{
		auto& defaultBufferPool = m_defaultBufferPools[m_currFrameIndex];
		auto& record = defaultBufferPool.back();
		DXResource* res = m_resMgr->GetResource(record.gpuRes);
		uint64_t maxSize = res->dxDesc.Width;
		uint64_t totalSize = record.startPos + record.byteSize + byteSize;
		return maxSize >= totalSize;
	}
}
