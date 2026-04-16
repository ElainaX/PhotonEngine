#pragma once
#include <array>

namespace photon
{
	class DX12RHI;

	// 每次BeginFrame调用之后，FrameSyncSystem保证之后的帧资源不会有冲突
	class FrameSyncSystem
	{
	public:
		static constexpr uint32_t kMaxFramesInFlight = 3;

	public:
		// 这里Initialize只是存个DX12RHI指针，不会去使用他，因为DX12RHI的初始化需要用到FrameSyncSystem
		bool Initialize(DX12RHI* rhi);
		void Shutdown();

		void BeginFrame();
		// 传入的是最后一次RHI::SignalQueue得到的结果
		void EndFrame(uint64_t submittedGraphicsFenceValue);

		uint32_t GetCurrentFrameIndex() const { return m_currentFrameIndex; }
		uint64_t GetCompletedGraphicsFenceValue() const;


		uint64_t GetSubmittedFenceValue(uint32_t frameIndex) const;
		bool CanReuseFrame(uint32_t frameIndex) const;

	private:
		DX12RHI* m_rhi = nullptr;
		uint32_t m_currentFrameIndex = kMaxFramesInFlight - 1;
		std::array<uint64_t, kMaxFramesInFlight> m_submittedFenceValues{};
	};
}


