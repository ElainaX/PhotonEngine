#pragma once
#include "RHI.h"
#include "RenderType.h"
#include "RenderPipeline.h"
#include "RenderResourceData.h"
#include "ResourceManager.h"
#include "RenderResourceData/ForwardPipelineRenderResourceData.h"
#include "Shader/ShaderFactory.h"
#include "RenderObject/RenderItem.h"
#include "Platform/FileSystem/FileSystem.h"
#include "RenderScene.h"
#include "GeometryGenerator.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>


namespace photon 
{
	const std::filesystem::path g_AssetTextureFolder = L"E:/Code/PhotonEngine/Engine/Assets/Texture";
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
		void Tick(GameTimer& gt);

		void ReCreateRenderTargetTexAndDepthStencilTex(Vector2i size);
		std::shared_ptr<RHI> GetRHI();

		void SetRenderPipelineType(RenderPipelineType renderType); 
		RenderCamera* GetRenderCamera();


	private:


		std::shared_ptr<RHI> m_Rhi;
		std::unordered_map<RenderPipelineType, std::shared_ptr<RenderPipeline>> m_RenderPipelines;
		//std::shared_ptr<RenderResourceData> m_ResourceData;
		std::shared_ptr<ResourceManager> m_ResourceManager;
		std::unique_ptr<GeometryGenerator> m_GeometryGenerator;
		//std::shared_ptr<RenderMeshCollection> m_RenderMeshCollection;
		//std::shared_ptr<TestShader> m_TestShader;
		//std::shared_ptr<CommonRenderItem> m_CommomRenderItem;


		std::shared_ptr<WindowSystem> m_WindowSystem;
		std::shared_ptr<ShaderFactory> m_ShaderFactory;

		//std::vector<RenderItem*> m_RenderItems;
		std::shared_ptr<Texture2D> m_RenderTarget;
		std::shared_ptr<Texture2D> m_DepthStencil;
		std::shared_ptr<Texture2D> m_ResourceTex;
		//std::shared_ptr<RenderCamera> m_MainRenderCamera;
		std::vector<std::shared_ptr<RenderScene>> m_RenderScene;

	};

}