#pragma once
#include "RHI.h"
#include "RenderType.h"
#include "RenderPipeline.h"
#include "RenderResourceData.h"
#include "ResourceManager.h"
#include "RenderResourceData.h"
#include "Shader/ShaderFactory.h"
#include "RenderObject/RenderItem.h"
#include "Platform/FileSystem/FileSystem.h"
#include "RenderScene.h"
#include "GeometryGenerator.h"
#include "Function/UI/WindowUI.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>


namespace photon 
{

	class WindowSystem;
	class GameTimer;

	struct RenderSystemInitInfo
	{
		std::shared_ptr<WindowSystem> windowSystem;
	};


	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem();

		void Initialize(RenderSystemInitInfo initInfo);
		void InitializeEditorUI(WindowUI* windowUI);
		void Tick(GameTimer& gt);

		void ReCreateRenderTargetTexAndDepthStencilTex(Vector2i size);
		std::shared_ptr<RHI> GetRHI();
		ShaderResourceView* GetFinalOutputShaderResourceView();

		void SetRenderPipelineType(RenderPipelineType renderType); 
		RenderCamera* GetRenderCamera();
		ResourceManager* GetResourceManager();
		GeometryGenerator* GetGeometryGenerator();
		RenderScene* GetRenderScene();
		ShaderFactory* GetShaderFactory();


	private:


		std::shared_ptr<RHI> m_Rhi;
		std::unordered_map<RenderPipelineType, std::shared_ptr<RenderPipeline>> m_RenderPipelines;
		RenderPipeline* m_CurrRenderPipeline = nullptr;
		std::shared_ptr<ResourceManager> m_ResourceManager;
		std::unique_ptr<GeometryGenerator> m_GeometryGenerator;


		std::shared_ptr<WindowSystem> m_WindowSystem;
		std::shared_ptr<ShaderFactory> m_ShaderFactory;

		std::shared_ptr<Texture2D> m_RenderTarget;
		std::shared_ptr<Texture2D> m_DepthStencil;
		//std::shared_ptr<Texture2D> m_ResourceTex;

		std::vector<std::shared_ptr<RenderScene>> m_RenderScene;

		ShaderResourceView* m_RenderTargetSRV = nullptr;

	};

}