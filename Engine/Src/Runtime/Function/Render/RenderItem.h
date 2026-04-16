#pragma once
#include <vector>

#include "ResourceHandle.h"
#include "Core/Math/MathFunction.h"
#include "DX12RHI/DescriptorHeap/Descriptor.h"
#include "RenderObject/Bounds.h"
#include "Shader/ShaderParameter/ShaderConstantBuffer.h"

namespace photon 
{
	enum class RenderLayer : uint32_t
	{
		Default =		1 << 0,
		Opaque =		1 << 1,
		Transparent =	1 << 2,
		ShadowCaster =	1 << 3,
		UI =			1 << 4,
		Debug =			1 << 5,
	};

	inline constexpr RenderLayer operator|(RenderLayer a, RenderLayer b)
	{
		return static_cast<RenderLayer>(
			static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline constexpr bool HasLayer(RenderLayer value, RenderLayer bit)
	{
		return (static_cast<uint32_t>(value) & static_cast<uint32_t>(bit)) != 0;
	}


	struct RenderItemFlags
	{
		bool visible = true;
		bool castShadow = true;
		bool receiveShadow = true;
		bool transparent = false;
		bool doubleSided = false;
		bool staticObject = true;
	};

	// 场景里的“一个可渲染对象”
	// 一个对象可以引用一个 mesh，mesh 内部再有多个 submesh
	struct RenderItem
	{
		uint64_t id = 0;

		MeshHandle mesh = {};
		std::vector<MaterialHandle> overrideMaterials = {};

		// 世界变换。科研阶段直接存矩阵最省事
		ObjectData objConstant = {};
		FrameDescriptorHandle objCbvHandle = {};

		DirectX::BoundingBox worldBounds = {};

		RenderLayer layers = RenderLayer::Default;
		RenderItemFlags flags = {};

		// 未来做实例化可以用
		uint32_t instanceGroupHint = 0;

		// 若想只渲染部分 submesh，可加 mask。这里先简单处理：0 表示全部启用
		uint64_t submeshMask = 0;

		bool IsVisible() const { return flags.visible; }
		bool IsTransparent() const { return flags.transparent; }
	};
}
