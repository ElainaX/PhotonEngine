#include "VertexLayoutRegistry.h"

namespace photon
{
	bool VertexLayoutRegistry::IsSameLayout(const VertexLayoutDesc& lhs, const VertexLayoutDesc& rhs)
	{
		if (lhs.stride != rhs.stride || lhs.elements.size() != rhs.elements.size())
			return false;

		for (size_t i = 0; i < lhs.elements.size(); ++i)
		{
			const auto& a = lhs.elements[i];
			const auto& b = rhs.elements[i];
			if (a.semantic != b.semantic ||
				a.semanticIndex != b.semanticIndex ||
				a.format != b.format ||
				a.offset != b.offset ||
				a.inputSlot != b.inputSlot ||
				a.inputClass != b.inputClass ||
				a.instanceDataStepRate != b.instanceDataStepRate)
			{
				return false;
			}
		}
		return true;
	}

	uint64_t VertexLayoutRegistry::ComputeLayoutHash(const VertexLayoutDesc& layout, uint64_t salt)
	{
		uint64_t seed = 1469598103934665603ull;
		HashCombine(seed, static_cast<uint64_t>(layout.stride));
		HashCombine(seed, static_cast<uint64_t>(layout.elements.size()));
		HashCombine(seed, salt);

		for (const auto& e : layout.elements)
		{
			HashCombine(seed, static_cast<uint64_t>(e.semantic));
			HashCombine(seed, static_cast<uint64_t>(e.semanticIndex));
			HashCombine(seed, static_cast<uint64_t>(e.format));
			HashCombine(seed, static_cast<uint64_t>(e.offset));
			HashCombine(seed, static_cast<uint64_t>(e.inputSlot));
			HashCombine(seed, static_cast<uint64_t>(e.inputClass));
			HashCombine(seed, static_cast<uint64_t>(e.instanceDataStepRate));
		}
		
		return seed == 0 ? 1 : seed;
	}

	uint64_t VertexLayoutRegistry::Register(const VertexLayoutDesc& layout)
	{
		if (layout.stride == 0 || layout.elements.empty())
			return 0;

		uint64_t salt = 0;
		for (;;)
		{
			const uint64_t hash = ComputeLayoutHash(layout, salt);
			auto iter = m_layouts.find(hash);
			if (iter == m_layouts.end())
			{
				m_layouts.emplace(hash, layout);
				return hash;
			}

			if (IsSameLayout(iter->second, layout))
				return hash;

			++salt;
		}
	}

	const VertexLayoutDesc* VertexLayoutRegistry::TryGet(uint64_t hash) const
	{
		auto iter = m_layouts.find(hash);
		return iter != m_layouts.end() ? &iter->second : nullptr;
	}

	const Dx12InputLayout* VertexLayoutRegistry::GetOrCreateDx12InputLayout(uint64_t hash)
	{
		auto layoutIter = m_layouts.find(hash);
		if (layoutIter == m_layouts.end())
			return nullptr;

		auto dxIter = m_dx12InputLayouts.find(hash);
		if (dxIter != m_dx12InputLayouts.end())
			return &dxIter->second;

		Dx12InputLayout inputLayout(layoutIter->second);
		auto [iter, inserted] = m_dx12InputLayouts.emplace(hash, std::move(inputLayout));
		return &iter->second;
	}

	void VertexLayoutRegistry::Reset()
	{
		m_layouts.clear();
		m_dx12InputLayouts.clear();
	}
}