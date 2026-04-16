#pragma once
#include "TextureAsset.h"
#include "Function/Render/ResourceHandle.h"
#include "Function/Render/DX12RHI/RenderResource/RenderResourceFactory.h"
#include "Function/Render/DX12RHI/RenderResource/TextureRenderResource.h"
#include "Function/Util/ResourceSlotCollection.h"

namespace photon
{
	class TextureCollection : public ResourceSlotCollection<TextureHandle, TextureRenderResource>
	{
	public:
		bool Initialize(RenderResourceFactory* factory, GpuResourceManager* gpuResMgr);
		void Shutdown();

		TextureHandle CreateFromAsset(const TextureAsset& asset);
		
		TextureRenderResource* TryGet(TextureHandle h)
		{
			auto* e = TryGetEntry(h);
			return e ? &e->resource : nullptr;
		}
		
		const TextureRenderResource* TryGet(TextureHandle h) const
		{
			auto* e = TryGetEntry(h);
			return e ? &e->resource : nullptr;
		}
		void Destroy(TextureHandle h);
		bool IsValid(TextureHandle h);

	private:
		RenderResourceFactory* m_factory = nullptr;
		GpuResourceManager* m_gpuResMgr = nullptr;
	};

}
