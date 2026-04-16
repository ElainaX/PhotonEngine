#pragma once
#include "DX12CommandContext.h"
#include "FrameSyncSystem.h"
#include <array>
#include <wrl/client.h>
#include <d3d12.h>

namespace photon
{
	class DX12RHI;

	class CommandContextManager
	{
	public:
		bool Initialize(DX12RHI* rhi);
		void Shutdown();

		DX12CommandContext& BeginInitContext();
		uint64_t SubmitInitContextAndWait(DX12CommandContext& context);

		void BeginFrame(uint32_t frameIndex);
		void EndFrame(uint32_t frameIndex);

		DX12CommandContext& BeginGraphicsContext(uint32_t frameIndex);
		DX12CommandContext& BeginComputeContext(uint32_t frameIndex);
		DX12CommandContext& BeginCopyContext(uint32_t frameIndex);


		DX12CommandContext& GetCurrentFrameGraphicsContext();
		DX12CommandContext& GetCurrentFrameComputeContext();
		DX12CommandContext& GetCurrentFrameCopyContext();
		DX12CommandContext* GetCurrentValidGraphicsContext();

		// 这里的submit将会调用signal，返回一个合理的fence值
		uint64_t SubmitGraphicsContext(DX12CommandContext& context);
		uint64_t SubmitComputeContext(DX12CommandContext& context);
		uint64_t SubmitCopyContext(DX12CommandContext& context);

	private:
		struct PerFrameContext
		{
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> graphicsAllocator;
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> computeAllocator;
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> copyAllocator;
		};

	private:
		DX12RHI* m_rhi = nullptr;

		DX12CommandContext* m_currValidGraphicsContext = nullptr;

		std::array<PerFrameContext, FrameSyncSystem::kMaxFramesInFlight> m_frames;

		DX12CommandContext m_graphicsContext;
		DX12CommandContext m_computeContext;
		DX12CommandContext m_copyContext;

		DX12CommandContext m_initGraphicsContext;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_initGraphicsAllocator;
	};
}

