#pragma once
#include <cstdint>

#include "Dx12InputLayout.h"
#include "VertexType.h"

namespace photon
{
	class VertexLayoutRegistry
	{
	public:
		uint64_t Register(const VertexLayoutDesc& layout);
		const VertexLayoutDesc* TryGet(uint64_t hash) const;
		// 首先需要保证VertexLayoutDesc已被缓存，然后会根据缓存的Desc创建对应Dx12InputLayout对象
		const Dx12InputLayout* GetOrCreateDx12InputLayout(uint64_t hash);
		void Reset();

	private:
		static bool IsSameLayout(const VertexLayoutDesc& lhs, const VertexLayoutDesc& rhs);
		static uint64_t ComputeLayoutHash(const VertexLayoutDesc& layout, uint64_t salt = 0);

	private:
		std::unordered_map<uint64_t, VertexLayoutDesc> m_layouts;
		std::unordered_map<uint64_t, Dx12InputLayout> m_dx12InputLayouts;
	};
}

