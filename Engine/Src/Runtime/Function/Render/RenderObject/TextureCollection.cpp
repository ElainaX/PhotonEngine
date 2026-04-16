#include "TextureCollection.h"


namespace photon
{
	bool TextureCollection::Initialize(RenderResourceFactory* factory, GpuResourceManager* gpuResMgr)
	{
		m_factory = factory;
		m_gpuResMgr = gpuResMgr;
		return m_factory != nullptr && m_gpuResMgr != nullptr;
	}

	void TextureCollection::Shutdown()
	{
		ResetAll();
		m_factory = nullptr;
		m_gpuResMgr = nullptr;
	}

	TextureHandle TextureCollection::CreateFromAsset(const TextureAsset& asset)
	{
		if (!m_factory || !asset.HasCpuData())
			return {};

		TextureHandle handle = AllocateHandle();
		auto* entry = TryGetEntry(handle);
		if (!entry)
			return {};

		TextureRenderResource rr = m_factory->BuildTextureRenderResource(asset, handle);

		entry->resource = std::move(rr);
		return handle;
	}

	void TextureCollection::Destroy(TextureHandle h)
	{
		auto* entry = TryGetEntry(h);
		if (!entry)
			return;

		TextureRenderResource& rr = entry->resource;

		// 先释放底层 GPU 资源。
		// 你的 GpuResourceManager::FreeResHandle() 会顺带释放 persistent view。
		if (m_gpuResMgr && rr.texture.IsValid())
		{
			m_gpuResMgr->DestoryResourceImmediate(rr.texture);
		}

		// 把本地记录清空，避免误用旧字段
		rr = {};
		FreeHandle(h);
	}

	bool TextureCollection::IsValid(TextureHandle h)
	{
		return IsHandleValid(h);
	}
}
