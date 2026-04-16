#include "MaterialCollection.h"

namespace photon
{
	bool MaterialCollection::Initialize(
		RenderResourceFactory* factory,
		ConstantBufferCollection* constantBuffers,
		const IResourceView* resView)
	{
		if (!factory || !constantBuffers || !resView)
			return false;

		m_factory = factory;
		m_constantBuffers = constantBuffers;
		m_resView = resView;

		return true;
	}

	void MaterialCollection::Shutdown()
	{
		ResetAll();
		m_factory = nullptr;
		m_constantBuffers = nullptr;
		m_resView = nullptr;
	}

	MaterialHandle MaterialCollection::CreateFromAsset(const MaterialAsset& asset)
	{
		if (!m_factory || !m_constantBuffers || !m_resView)
			return {};

		MaterialHandle handle = AllocateHandle();
		auto* entry = TryGetEntry(handle);
		if (!entry)
			return {};

		PersistentConstantBufferAllocation matAlloc = {};

		// 如果这个材质有 matData，就从 ConstantBufferCollection 里申请一块持久化空间
		if (!asset.matConstant.bytes.empty())
		{
			matAlloc = m_constantBuffers->AllocateRaw(
				asset.matConstant.bytes.data(),
				static_cast<uint32_t>(asset.matConstant.bytes.size()),
				asset.meta.debugName.empty() ? "MaterialCB" : asset.meta.debugName + "_MaterialCB");
		}

		MaterialRenderResource rr = m_factory->BuildMaterialRenderResource(
			asset,
			handle,
			matAlloc.IsValid() ? &matAlloc : nullptr,
			*m_resView);

		entry->resource = std::move(rr);
		return handle;
	}

	void MaterialCollection::Destroy(MaterialHandle h)
	{
		auto* rr = TryGet(h);
		if (rr && rr->matConstantHandle.IsValid() && m_constantBuffers)
		{
			m_constantBuffers->Free(rr->matConstantHandle);
		}

		FreeHandle(h);
	}

	bool MaterialCollection::IsValid(MaterialHandle h) const
	{
		return IsHandleValid(h);
	}
}