#include "CommandContextManager.h"


namespace photon
{
	bool CommandContextManager::Initialize(DX12RHI* rhi)
	{
		m_rhi = rhi;
		auto device = m_rhi->GetDevice();
		for (auto& frameContext : m_frames)
		{
			DX_LogIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameContext.graphicsAllocator)));
			DX_LogIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&frameContext.computeAllocator)));
			DX_LogIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&frameContext.copyAllocator)));
		}
		DX_LogIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_initGraphicsAllocator)));

		bool suc = m_graphicsContext.Initialize(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		suc &= m_computeContext.Initialize(device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		suc &= m_copyContext.Initialize(device, D3D12_COMMAND_LIST_TYPE_COPY);

		suc &= m_initGraphicsContext.Initialize(device, D3D12_COMMAND_LIST_TYPE_DIRECT);

		return suc;
	}

	void CommandContextManager::Shutdown()
	{
		m_graphicsContext.Shutdown();
		m_computeContext.Shutdown();
		m_copyContext.Shutdown();
		m_initGraphicsContext.Shutdown();

		m_initGraphicsAllocator.Reset();
		for (auto& frame : m_frames)
		{
			frame.graphicsAllocator.Reset();
			frame.computeAllocator.Reset();
			frame.copyAllocator.Reset();
		}
		m_rhi = nullptr;
	}

	DX12CommandContext& CommandContextManager::BeginInitContext()
	{
		DX_LogIfFailed(m_initGraphicsAllocator->Reset());
		m_initGraphicsContext.Reset(m_initGraphicsAllocator.Get(), nullptr);
		m_currValidGraphicsContext = &m_initGraphicsContext;
		return m_initGraphicsContext;
	}

	uint64_t CommandContextManager::SubmitInitContextAndWait(DX12CommandContext& context)
	{
		context.Close();
		ID3D12CommandList* cmdLists[] =
		{
			context.GetNativeCommandList()
		};

		m_rhi->GetGraphicsQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		uint64_t fenceValue = m_rhi->SignalQueue(QueueType::Graphics);
		m_rhi->WaitForFenceValue(QueueType::Graphics, fenceValue);

		m_currValidGraphicsContext = nullptr;

		return fenceValue;
	}

	void CommandContextManager::BeginFrame(uint32_t frameIndex)
	{
		PHOTON_ASSERT(frameIndex < FrameSyncSystem::kMaxFramesInFlight, "Invalid frame Index!");

		auto& frame = m_frames[frameIndex];
		// 这里默认 FrameSyncSystem 已经保证该 frame slot 可复用
		// 所以只需要 reset allocator
		DX_LogIfFailed(frame.graphicsAllocator->Reset());
		DX_LogIfFailed(frame.computeAllocator->Reset());
		DX_LogIfFailed(frame.copyAllocator->Reset());
	}

	void CommandContextManager::EndFrame(uint32_t frameIndex)
	{
		// 暂时不做事情，后续可以检查allocator或者list状态是否合法

	}

	DX12CommandContext& CommandContextManager::BeginGraphicsContext(uint32_t frameIndex)
	{
		auto& frame = m_frames[frameIndex];
		m_graphicsContext.Reset(frame.graphicsAllocator.Get(), nullptr);
		m_currValidGraphicsContext = &m_graphicsContext;
		return m_graphicsContext;
	}

	DX12CommandContext& CommandContextManager::BeginComputeContext(uint32_t frameIndex)
	{
		auto& frame = m_frames[frameIndex];
		m_computeContext.Reset(frame.computeAllocator.Get(), nullptr);
		return m_computeContext;
	}

	DX12CommandContext& CommandContextManager::BeginCopyContext(uint32_t frameIndex)
	{
		auto& frame = m_frames[frameIndex];
		m_copyContext.Reset(frame.copyAllocator.Get(), nullptr);
		return m_copyContext;
	}

	DX12CommandContext& CommandContextManager::GetCurrentFrameGraphicsContext()
	{
		return m_graphicsContext;
	}

	DX12CommandContext& CommandContextManager::GetCurrentFrameComputeContext()
	{
		return m_computeContext;
	}

	DX12CommandContext& CommandContextManager::GetCurrentFrameCopyContext()
	{
		return m_copyContext;
	}

	DX12CommandContext* CommandContextManager::GetCurrentValidGraphicsContext()
	{
		return m_currValidGraphicsContext;
	}

	uint64_t CommandContextManager::SubmitGraphicsContext(DX12CommandContext& context)
	{
		context.Close();
		ID3D12CommandList* cmdLists[] =
		{
			context.GetNativeCommandList()
		};

		m_rhi->GetGraphicsQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		uint64_t fenceValue = m_rhi->SignalQueue(QueueType::Graphics);

		m_currValidGraphicsContext = nullptr;

		return fenceValue;
	}

	uint64_t CommandContextManager::SubmitComputeContext(DX12CommandContext& context)
	{
		context.Close();
		ID3D12CommandList* cmdLists[] =
		{
			context.GetNativeCommandList()
		};

		m_rhi->GetComputeQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		uint64_t fenceValue = m_rhi->SignalQueue(QueueType::Compute);

		return fenceValue;
	}

	uint64_t CommandContextManager::SubmitCopyContext(DX12CommandContext& context)
	{
		context.Close();
		ID3D12CommandList* cmdLists[] =
		{
			context.GetNativeCommandList()
		};

		m_rhi->GetCopyQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		uint64_t fenceValue = m_rhi->SignalQueue(QueueType::Copy);

		return fenceValue;
	}
}
