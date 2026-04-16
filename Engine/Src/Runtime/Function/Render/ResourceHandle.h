#pragma once
#include <cstdint>
#include <compare>

namespace photon
{
	struct ResourceHandle
	{
		uint64_t index = 0;
		uint32_t generation = 0;
		// Handle = 0 表示非有效handle
		bool IsValid() const { return generation != 0; }
		auto operator<=>(const ResourceHandle&) const = default;
	};

	struct TextureHandle
	{
		ResourceHandle handle;
	};

	struct MeshHandle
	{
		ResourceHandle handle;
	};

	struct ShaderHandle
	{
		ResourceHandle handle;
	};

	struct MaterialHandle
	{
		ResourceHandle handle;
	};

	struct ModelHandle
	{
		ResourceHandle handle;
	};


	// 接下来是引擎内部的RuntimeAsset
	struct BufferHandle
	{
		ResourceHandle handle;
	};

	struct RootSignatureHandle
	{
		ResourceHandle handle;
	};

	struct PipelineStateHandle
	{
		ResourceHandle handle;
	};

	struct BlendStateHandle
	{
		ResourceHandle handle;
	};

	struct RasterStateHandle
	{
		ResourceHandle handle;
	};

	struct DepthStencilStateHandle
	{
		ResourceHandle handle;
	};

	inline bool operator==(const TextureHandle& a, const TextureHandle& b) { return a.handle == b.handle; }
	inline bool operator==(const MeshHandle& a, const MeshHandle& b) { return a.handle == b.handle; }
	inline bool operator==(const ShaderHandle& a, const ShaderHandle& b) { return a.handle == b.handle; }
	inline bool operator==(const MaterialHandle& a, const MaterialHandle& b) { return a.handle == b.handle; }
	inline bool operator==(const ModelHandle& a, const ModelHandle& b) { return a.handle == b.handle; }
	inline bool operator==(const BufferHandle& a, const BufferHandle& b) { return a.handle == b.handle; }
	inline bool operator==(const RootSignatureHandle& a, const RootSignatureHandle& b) { return a.handle == b.handle; }
	inline bool operator==(const PipelineStateHandle& a, const PipelineStateHandle& b) { return a.handle == b.handle; }
	inline bool operator==(const BlendStateHandle& a, const BlendStateHandle& b) { return a.handle == b.handle; }
	inline bool operator==(const RasterStateHandle& a, const RasterStateHandle& b) { return a.handle == b.handle; }
	inline bool operator==(const DepthStencilStateHandle& a, const DepthStencilStateHandle& b) { return a.handle == b.handle; }

	inline void HashCombine(size_t& seed, size_t value)
	{
		seed ^= value + 0x9e3779b97f4a7c15ull + (seed << 6) + (seed >> 2);
	}

	inline size_t HashResourceHandle(const ResourceHandle& h)
	{
		size_t seed = 0;
		HashCombine(seed, std::hash<uint64_t>{}(h.index));
		HashCombine(seed, std::hash<uint32_t>{}(h.generation));
		return seed;
	}
}