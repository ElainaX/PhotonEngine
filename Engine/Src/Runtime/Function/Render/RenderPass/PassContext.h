#pragma once 

#include <vector>
#include "PassBlackboard.h"
#include "Function/Render/RenderItem.h"

namespace photon 
{


	class RHI;
	class WindowSystem;
	class ResourceManager;
	class GpuResourceManager;
	class DescriptorSystem;
	class FrameAllocatorSystem;
	class CommandContextManager;
	class PipelineStateCache;
	class RootSignatureCache;
	class DX12CommandContext;
	class RenderScene;
	class RenderCamera;
	class PassBlackboard;
	class ImGuiSystem;
	struct EG_FrameContext;

	struct RenderPipelineServices
	{
		RHI* rhi = nullptr;
		WindowSystem* windowSystem = nullptr;

		ResourceManager* resourceManager = nullptr;
		GpuResourceManager* gpuResManager = nullptr;
		DescriptorSystem* descriptorSystem = nullptr;
		FrameAllocatorSystem* frameAllocator = nullptr;
		CommandContextManager* cmdCtxMgr = nullptr;

		PipelineStateCache* pipelineCache = nullptr;
		RootSignatureCache* rootSignatureCache = nullptr;

		ImGuiSystem* imguiSystem = nullptr;
	};

	struct PassPrepareContext
	{
		EG_FrameContext* frame = nullptr;
		PassBlackboard* blackboard = nullptr;
		const RenderPipelineServices* services = nullptr;

		RenderScene* renderScene = nullptr;
		RenderCamera* camera = nullptr;

		// 上游 culling 后的可见对象列表
		const std::vector<const RenderItem*>* visibleItems = nullptr;
	};

	struct PassExecuteContext
	{
		EG_FrameContext* frame = nullptr;
		PassBlackboard* blackboard = nullptr;
		const RenderPipelineServices* services = nullptr;
		DX12CommandContext* cmd = nullptr;
	};
}
