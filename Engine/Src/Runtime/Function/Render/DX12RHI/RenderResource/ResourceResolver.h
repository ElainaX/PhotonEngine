#pragma once
#include "MaterialRenderResource.h"
#include "MeshRenderResource.h"
#include "TextureRenderResource.h"
#include "Function/Render/RendererComponent/MeshRendererComponent.h"
#include "Function/Render/RenderObject/MaterialAsset.h"
#include "Function/Render/RenderObject/MeshAsset.h"
#include "Function/Render/RenderObject/TextureAsset.h"

namespace photon
{
	class IResourceView
	{
	public:
		virtual const MeshAsset* GetMeshAsset(Guid guid) const = 0;
		virtual const MeshRenderResource* GetMeshRenderResource(MeshHandle h) const = 0;

		virtual const TextureAsset* GetTextureAsset(Guid guid) const = 0;
		virtual const TextureRenderResource* GetTextureRenderResource(TextureHandle h) const = 0;

		virtual const MaterialAsset* GetMaterialAsset(Guid guid) const = 0;
		virtual const MaterialRenderResource* GetMaterialRenderResource(MaterialHandle h) const = 0;

		virtual const ShaderProgramAsset* GetShaderProgramAsset(Guid guid) const = 0;
		virtual const ShaderProgramRenderResource* GetShaderProgramRenderResource(ShaderHandle h) const = 0;

		virtual MeshHandle GetMeshHandleByGuid(Guid guid) const = 0;
		virtual TextureHandle GetTextureHandleByGuid(Guid guid) const = 0;
		virtual MaterialHandle GetMaterialHandleByGuid(Guid guid) const = 0;
		virtual ShaderHandle GetShaderHandleByGuid(Guid guid) const = 0;

		virtual MaterialHandle GetFallbackMaterial() const = 0;
		virtual TextureHandle GetFallbackTexture() const = 0;
		virtual ShaderHandle GetFallbackShader() const = 0;

		virtual ~IResourceView() = default;
	};

	inline MaterialHandle ResolveMaterialForSubmesh(
		const IResourceView& resources,
		const MeshRendererComponent& renderer,
		const MeshRenderResource& meshRR,
		uint32_t submeshIndex)
	{
		if (submeshIndex >= meshRR.submeshes.size())
			return resources.GetFallbackMaterial();

		const auto& sub = meshRR.submeshes[submeshIndex];
		const uint32_t slot = sub.materialSlot;

		if (slot < renderer.overrideMaterials.size() 
			&& renderer.overrideMaterials[slot].handle.IsValid())
		{
			return renderer.overrideMaterials[slot];
		}

		const MeshAsset* meshAsset = resources.GetMeshAsset(meshRR.assetGuid);
		if (!meshAsset)
			return resources.GetFallbackMaterial();

		if (slot < meshAsset->defaultMaterialGuids.size())
		{
			MaterialHandle h = resources.GetMaterialHandleByGuid(meshAsset->defaultMaterialGuids[slot]);
			if (h.handle.IsValid())
				return h;
		}

		return resources.GetFallbackMaterial();
	}


}
