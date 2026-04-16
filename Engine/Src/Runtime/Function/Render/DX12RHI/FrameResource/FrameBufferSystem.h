#pragma once
#include "FrameResource.h"
#include "Function/Render/DX12RHI/GpuResourceManager.h"
#include "Function/Util/RenderUtil.h"

namespace photon
{
	class FrameBufferSystem
	{
	public:
		FrameBufferSystem() = default;

		bool Initialize(GpuResourceManager* resMgr);

		void BeginFrame(UINT frameIndex);

		void Clear();

		template<typename T>
		FrameResourceRange AllocateAlignment256(uint32_t elementCount, FrameResourceType frametype)
		{
			uint32_t typeSize = sizeof(T);
			uint64_t byteSize = static_cast<uint64_t>(elementCount) * RenderUtil::Align256(typeSize);
			auto range = RequireBuffer(byteSize, frametype);
			return range;
		}

		FrameResourceRange Allocate(uint32_t sizeInBytes, FrameResourceType frametype)
		{
			uint64_t byteSize = sizeInBytes;
			auto range = RequireBuffer(byteSize, frametype);
			return range;
		}

	private:
		struct BufferRecord
		{
			GpuResourceHandle gpuRes = {};
			uint64_t startPos = 0;
			uint32_t byteSize = 0;
		};


		FrameResourceRange RequireBuffer(uint64_t byteSize, FrameResourceType frametype);
		FrameResourceRange RequireUploadBuffer(uint64_t byteSize);

		bool HasEnoughUploadSpace(uint64_t byteSize);
		bool HasEnoughDefaultSpace(uint64_t byteSize);

	private:
		std::array<std::vector<BufferRecord>, FrameSyncSystem::kMaxFramesInFlight> m_uploadBufferPools;
		std::array<std::vector<BufferRecord>, FrameSyncSystem::kMaxFramesInFlight> m_defaultBufferPools;

		uint32_t m_currFrameIndex = 0;


		// 9.76mb
		uint64_t m_poolByteSize = 1024ull * 10000;
		// 可以根据frameTotalByteSize来调整poolByteSize的大小
		uint64_t m_frameTotalByteSize = 0;

		GpuResourceManager* m_resMgr = nullptr;
	};

}
