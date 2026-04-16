#pragma once 

#include <array>
#include <filesystem>
#include <map>
#include <memory>
#include "ResourceHandle.h"
#include "DX12RHI/CommandContextManager.h"
#include "DX12RHI/DXPipeline/PipelineStateCache.h"
#include "DX12RHI/FrameResource/FrameAllocatorSystem.h"
#include "DX12RHI/RenderResource/MaterialRenderResource.h"
#include "DX12RHI/RenderResource/MeshRenderResource.h"
#include "DX12RHI/RenderResource/ResourceResolver.h"
#include "DX12RHI/RenderResource/TextureRenderResource.h"
#include "RenderObject/MaterialAsset.h"
#include "RenderObject/MaterialCollection.h"
#include "RenderObject/MeshAsset.h"
#include "RenderObject/MeshCollection.h"
#include "RenderObject/ModelAsset.h"
#include "RenderObject/TextureAsset.h"
#include "RenderObject/TextureCollection.h"
#include "ResourceLoader/ResourceLoader.h"
#include "Shader/ShaderProgramCollection.h"


namespace photon
{


	class ResourceManager : public IResourceView
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;
		void Initialize(DX12RHI* rhi, GpuResourceManager* gpuResMgr,
			DescriptorSystem* descriptorSystem,
			FrameAllocatorSystem* frameAllocatorSystem,
			CommandContextManager* cmdCtxMgr);
		void Shutdown();

		// 引擎可支持的导入，这些都是职责明确的函数
		std::shared_ptr<ModelAsset> LoadModel(const std::filesystem::path& path);
		MeshHandle LoadMesh(const std::filesystem::path& path);
		TextureHandle LoadTexture(const std::filesystem::path& path);
		MaterialHandle LoadMaterial(const std::filesystem::path& path);
		MaterialHandle CreatePBRMaterial(ShaderHandle shaderHandle, const std::string& debugName = "PBRMaterial");
		ShaderHandle LoadShaderProgram(const std::filesystem::path& path, const ShaderProgramLoadDesc& loadDesc);
		bool SetMaterialTextureBinding(MaterialHandle mat, std::string_view name, TextureHandle tex);

		TextureHandle CreateRuntimeTexture2D(
			const DXTexture2DDesc& desc,
			TextureDimension dimension = TextureDimension::Tex2D,
			const std::string& debugName = "RuntimeTex2D");

		TextureHandle CreateRuntimeTexture2DArray(
			const DXTexture2DArrayDesc& desc,
			TextureDimension dimension = TextureDimension::Tex2DArray,
			const std::string& debugName = "RuntimeTex2DArray");

		TextureHandle CreateRuntimeTextureCube(
			const DXTexture2DArrayDesc& desc,
			const std::string& debugName = "RuntimeCubemap");


		// --------- Destroy / Unload ----------
		bool DestroyTexture(TextureHandle h, bool eraseAsset = true);
		bool DestroyMesh(MeshHandle h, bool eraseAsset = true);
		bool DestroyMaterial(MaterialHandle h, bool eraseAsset = true);
		bool DestroyShaderProgram(ShaderHandle h, bool eraseAsset = true);

		// model 目前还不是 handle 资源，先只支持删 asset 记录，不默认级联删子资源
		bool UnloadModel(Guid modelGuid);

		const ShaderStageAsset* GetShaderStageAsset(Guid guid) const;

		// --------- IResourceView ----------------
		const MeshAsset* GetMeshAsset(Guid guid) const override;
		const MeshRenderResource* GetMeshRenderResource(MeshHandle h) const override;

		const TextureAsset* GetTextureAsset(Guid guid) const override;
		const TextureRenderResource* GetTextureRenderResource(TextureHandle h) const override;

		const MaterialAsset* GetMaterialAsset(Guid guid) const override;
		const MaterialRenderResource* GetMaterialRenderResource(MaterialHandle h) const override;

		const ShaderProgramAsset* GetShaderProgramAsset(Guid guid) const override;
		const ShaderProgramRenderResource* GetShaderProgramRenderResource(ShaderHandle h) const override;

		bool SetMeshShader(MeshHandle mesh, ShaderHandle shader);

		MeshHandle GetMeshHandleByGuid(Guid guid) const override;
		TextureHandle GetTextureHandleByGuid(Guid guid) const override;
		MaterialHandle GetMaterialHandleByGuid(Guid guid) const override;
		ShaderHandle GetShaderHandleByGuid(Guid guid) const override;

		MaterialHandle GetFallbackMaterial() const override;
		TextureHandle GetFallbackTexture() const override;
		ShaderHandle GetFallbackShader() const override;

		PipelineStateCache* GetPipelineStateCache() const { return m_pipelineStateCache.get(); }
		RootSignatureCache* GetRootSignatureCache() const { return m_rootSignatureCache.get(); }
		VertexLayoutRegistry* GetVertexLayoutRegistry() const { return m_vertexLayoutRegistry.get(); }

	private:
		ShaderStageAsset* GetShaderStageAssetMutable(Guid guid);
		// --------- IResourceView ----------------
		MeshAsset* GetMeshAssetMutable(Guid guid);
		MeshRenderResource* GetMeshRenderResourceMutable(MeshHandle h);
		TextureAsset* GetTextureAssetMutable(Guid guid);
		TextureRenderResource* GetTextureRenderResourceMutable(TextureHandle h);
		MaterialAsset* GetMaterialAssetMutable(Guid guid);
		MaterialRenderResource* GetMaterialRenderResourceMutable(MaterialHandle h);
		ShaderProgramAsset* GetShaderProgramAssetMutable(Guid guid);
		ShaderProgramRenderResource* GetShaderProgramRenderResourceMutable(ShaderHandle h);

		ShaderHandle LoadShaderProgram(const std::filesystem::path& path);

		ShaderProgramAsset ImportShaderProgramAsset(const std::filesystem::path& path, const ShaderProgramLoadDesc* loadDesc);
		MeshAsset ImportStaticMeshAsset(const std::filesystem::path& path);
		TextureAsset ImportTextureAsset(const std::filesystem::path& path);
		MaterialAsset ImportMaterialAsset(const std::filesystem::path& path, ShaderHandle shaderHandle);
		void BuildDefaultAssets();

	private:
		// assets
		std::map<Guid, std::shared_ptr<ModelAsset>> m_modelAssets;
		std::map<Guid, MeshAsset> m_meshAssets;
		std::map<Guid, TextureAsset> m_textureAssets;
		std::map<Guid, MaterialAsset> m_materialAssets;
		std::map<Guid, ShaderProgramAsset> m_shaderProgramAssets;
		std::map<Guid, ShaderStageAsset> m_shaderStageAssets;

		// guid -> handle
		std::map<Guid, MeshHandle> m_meshGuidToHandle;
		std::map<Guid, TextureHandle> m_textureGuidToHandle;
		std::map<Guid, MaterialHandle> m_materialGuidToHandle;
		std::map<Guid, ShaderHandle> m_shaderGuidToHandle;

		// collections
		std::shared_ptr<VertexLayoutRegistry> m_vertexLayoutRegistry;
		std::shared_ptr<MeshCollection> m_meshCollection;
		std::shared_ptr<TextureCollection> m_textureCollection;
		std::shared_ptr<MaterialCollection> m_materialCollection;
		std::shared_ptr<ShaderProgramCollection> m_shaderProgramCollection;
		std::shared_ptr<RootSignatureCache> m_rootSignatureCache;
		std::shared_ptr<PipelineStateCache> m_pipelineStateCache;
		std::shared_ptr<RenderResourceFactory> m_renderResourceFactory;
		std::shared_ptr<ConstantBufferCollection> m_constantBufferCollection;

		DX12RHI* m_rhi = nullptr;
		GpuResourceManager* m_gpuResMgr = nullptr;
		DescriptorSystem* m_descriptorSystem = nullptr;
		FrameAllocatorSystem* m_frameAllocatorSystem = nullptr;
		CommandContextManager* m_cmdCtxMgr = nullptr;

		MaterialHandle m_fallbackMaterial = {};
		TextureHandle  m_fallbackTexture = {};
		ShaderHandle   m_fallbackShader = {};
	};


}
