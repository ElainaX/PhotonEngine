#pragma once
#include "MaterialAsset.h"
#include "Function/Render/ResourceHandle.h"
#include "Function/Render/DX12RHI/RenderResource/MaterialRenderResource.h"
#include "Function/Render/DX12RHI/RenderResource/RenderResourceFactory.h"
#include "Function/Render/DX12RHI/RenderResource/ResourceResolver.h"
#include "Function/Render/DX12RHI/RenderResource/ConstantBufferCollection.h"
#include "Function/Util/ResourceSlotCollection.h"

namespace photon
{
	class MaterialCollection : public ResourceSlotCollection<MaterialHandle, MaterialRenderResource>
	{
	public:
		bool Initialize(
			RenderResourceFactory* factory,
			ConstantBufferCollection* constantBuffers,
			const IResourceView* resView);

		void Shutdown();

		MaterialHandle CreateFromAsset(const MaterialAsset& asset);

		MaterialRenderResource* TryGet(MaterialHandle h)
		{
			auto* e = TryGetEntry(h);
			return e ? &e->resource : nullptr;
		}

		const MaterialRenderResource* TryGet(MaterialHandle h) const
		{
			auto* e = TryGetEntry(h);
			return e ? &e->resource : nullptr;
		}

		void Destroy(MaterialHandle h);
		bool IsValid(MaterialHandle h) const;

	private:
		RenderResourceFactory* m_factory = nullptr;
		ConstantBufferCollection* m_constantBuffers = nullptr;
		const IResourceView* m_resView = nullptr;
	};
}