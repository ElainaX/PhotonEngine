#include "FrameSyncSystem.h"

#include "DX12RHI.h"

namespace photon
{
	bool FrameSyncSystem::Initialize(DX12RHI* rhi)
	{
		if (!rhi)
			return false;

		m_rhi = rhi;
		return true;
	}

	void FrameSyncSystem::Shutdown()
	{
		// Not need to do anything
	}

	void FrameSyncSystem::BeginFrame()
	{
		m_currentFrameIndex = (m_currentFrameIndex+1) % kMaxFramesInFlight;
		// 查看当前frame的任务是否已被完成
		if (!CanReuseFrame(m_currentFrameIndex))
			m_rhi->WaitForFenceValue(QueueType::Graphics, m_submittedFenceValues[m_currentFrameIndex]);
	}

	void FrameSyncSystem::EndFrame(uint64_t submittedGraphicsFenceValue)
	{
		if (submittedGraphicsFenceValue > m_submittedFenceValues[m_currentFrameIndex])
		{
			m_submittedFenceValues[m_currentFrameIndex] = submittedGraphicsFenceValue;
		}


	}

	uint64_t FrameSyncSystem::GetCompletedGraphicsFenceValue() const
	{
		return m_rhi->GetCompletedFenceValue(QueueType::Graphics);
	}

	uint64_t FrameSyncSystem::GetSubmittedFenceValue(uint32_t frameIndex) const
	{
		return m_submittedFenceValues[frameIndex];
	}

	bool FrameSyncSystem::CanReuseFrame(uint32_t frameIndex) const
	{
		return GetCompletedGraphicsFenceValue() >= m_submittedFenceValues[frameIndex];

	}
}
