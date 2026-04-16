#pragma once
#include "RHI.h"
#include "RenderTypes.h"
#include "RenderPipeline.h"
#include "ResourceManager.h"
#include "Platform/FileSystem/FileSystem.h"
#include "RenderScene.h"
#include "Function/UI/WindowUI.h"
#include "Function/UI/ImGuiSystem.h"
#include "EGFrameContext.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>

#include "DX12RHI/CommandContextManager.h"

namespace photon
{
	class WindowSystem;
	class GameTimer;

	struct RenderSystemInitInfo
	{
		WindowSystem* windowSystem = nullptr;
	};

	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem();

		void Initialize(RenderSystemInitInfo initInfo);
		void InitializeEditorUI(WindowUI* windowUI);
		void Tick(GameTimer& gt);
		void Stop();
		void ReStart();

		void BuildEGFrameContext(EG_FrameContext& frameCtx, GameTimer* timer, DX12CommandContext* graphicsCmd);
		void PrepareFrameObjectBindings(EG_FrameContext& frameCtx);

		void SetRenderPipelineType(RenderPipelineType renderType);
		RenderCamera* GetRenderCamera();
		ResourceManager* GetResourceManager();
		RenderScene* GetRenderScene();

	public:
		TextureHandle GetSceneColorHandle() const { return m_sceneColor; }
		Vector2i GetMainViewportSize() const { return m_mainViewportSize; }
		bool HasEditorUI() const { return m_imguiSystem != nullptr; }
		ImGuiSystem* GetImGuiSystem() const { return m_imguiSystem.get(); }
		DescriptorSystem* GetDescriptorSystem() const { return m_descriptorSystem.get(); }

	private:
		void CreateOrRefreshBackBufferRtvs();
		void CreateOrResizeMainViewportTargets(Vector2i size);
		void ExecutePresentPass(EG_FrameContext& frameCtx, DX12CommandContext& gfxCtx);
		void ExecuteEditorUIPass(EG_FrameContext& frameCtx, DX12CommandContext& gfxCtx);
		bool IsEditorMode() const;

	private:
		RenderPipeline* m_CurrRenderPipeline = nullptr;
		std::unordered_map<RenderPipelineType, std::shared_ptr<RenderPipeline>> m_RenderPipelines;
		RenderPipelineType m_CurrPipelineType = RenderPipelineType::ForwardPipeline;

		std::shared_ptr<DX12RHI> m_rhi;
		std::shared_ptr<FrameSyncSystem> m_frameSyncSystem;
		std::shared_ptr<ResourceManager> m_resourceManager;
		std::shared_ptr<GpuResourceManager> m_gpuResMgr;
		std::shared_ptr<DescriptorSystem> m_descriptorSystem;
		std::shared_ptr<CommandContextManager> m_CmdCtxMgr;
		std::shared_ptr<FrameAllocatorSystem> m_frameAllocSystem;
		std::shared_ptr<ImGuiSystem> m_imguiSystem;

		WindowSystem* m_WindowSystem = nullptr;
		std::vector<std::shared_ptr<RenderScene>> m_RenderScene;

		TextureHandle m_sceneColor = {};
		TextureHandle m_sceneDepth = {};
		std::array<DescriptorHandle, FrameSyncSystem::kMaxFramesInFlight> m_backbufferRtvs = {};

		Vector2i m_mainViewportSize = {};

		bool m_bStopRenderContent = false;
	};
}