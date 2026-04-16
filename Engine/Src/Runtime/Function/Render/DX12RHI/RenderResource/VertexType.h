#pragma once 

#include <d3d12.h>
#include <dxgiformat.h>
#include <map>
#include <string>

#include "Core/Math/Vector4.h"
#include "Core/Math/Vector3.h"
#include "Core/Math/Vector2.h"


#include <vector>

#include "Core/Math/Vector4i.h"

namespace photon 
{
	enum class IndexFormat : uint8_t
	{
		UInt16 = DXGI_FORMAT_R16_UINT,
		UInt32 = DXGI_FORMAT_R32_UINT
	};

	enum class VertexSemantic : uint8_t
	{
		Position,
		Normal,
		Tangent,
		Color,
		TexCoord,
		BlendIndices,
		BlendWeights

	};

	enum class VertexElementFormat : uint8_t
	{
		Float1 = DXGI_FORMAT_R32_FLOAT,
		Float2 = DXGI_FORMAT_R32G32_FLOAT,
		Float3 = DXGI_FORMAT_R32G32B32_FLOAT,
		Float4 = DXGI_FORMAT_R32G32B32A32_FLOAT,
		UInt4 = DXGI_FORMAT_R32G32B32A32_UINT,
		UByte4N = DXGI_FORMAT_R8G8B8A8_UNORM
	};

	enum class VertexInputClassification : uint8_t
	{
		PerVertex,
		PerInstance
	};

	struct VertexElementDesc
	{
		VertexSemantic semantic = VertexSemantic::Position;
		uint32_t semanticIndex = 0; // TexCoord0 / TexCoord1
		VertexElementFormat format = VertexElementFormat::Float3;
		uint32_t offset = 0;

		// 先默认都走 slot 0 / PerVertex
		uint32_t inputSlot = 0;
		VertexInputClassification inputClass = VertexInputClassification::PerVertex;
		uint32_t instanceDataStepRate = 0;
	};

	struct VertexLayoutDesc
	{
		uint32_t stride = 0;
		std::vector<VertexElementDesc> elements;

		const VertexElementDesc* Find(VertexSemantic semantic, uint32_t semanticIndex = 0) const
		{
			for (const auto& e : elements)
			{
				if (e.semantic == semantic && e.semanticIndex == semanticIndex)
					return &e;
			}
			return nullptr;
		}
	};

	struct NativeInputElementDesc
	{
		std::string semanticName;
		uint32_t semanticIndex = 0;
		VertexElementFormat format = VertexElementFormat::Float3;
		uint32_t inputSlot = 0;
		uint32_t alignedByteOffset = 0;
	};

	struct VertexPNTUV
	{
		Vector3 pos;
		Vector3 normal;
		Vector3 tangent;
		Vector2 uv0;
	};

	struct VertexSkinned
	{
		Vector3 pos;
		Vector3 normal;
		Vector3 tangent;
		Vector2 uv0;
		Vector4i blendIndices;
		Vector4 blendWeights;
	};

	template<typename TVertex>
	struct VertexTraits;

	template<>
	struct VertexTraits<VertexPNTUV>
	{
		static VertexLayoutDesc GetLayout()
		{
			VertexLayoutDesc layout;
			layout.stride = sizeof(VertexPNTUV);
			layout.elements = {
				{VertexSemantic::Position, 0, VertexElementFormat::Float3, offsetof(VertexPNTUV, pos)},
				{VertexSemantic::Normal, 0, VertexElementFormat::Float3, offsetof(VertexPNTUV, normal)},
				{VertexSemantic::Tangent, 0, VertexElementFormat::Float3, offsetof(VertexPNTUV, tangent)},
				{VertexSemantic::TexCoord, 0, VertexElementFormat::Float2, offsetof(VertexPNTUV, uv0)},
			};
			return layout;
		}
	};

	template<>
	struct VertexTraits<VertexSkinned>
	{
		static VertexLayoutDesc GetLayout()
		{
			VertexLayoutDesc layout;
			layout.stride = sizeof(VertexSkinned);
			layout.elements = {
				{VertexSemantic::Position, 0, VertexElementFormat::Float3, offsetof(VertexSkinned, pos)},
				{VertexSemantic::Normal, 0, VertexElementFormat::Float3, offsetof(VertexSkinned, normal)},
				{VertexSemantic::Tangent, 0, VertexElementFormat::Float3, offsetof(VertexSkinned, tangent)},
				{VertexSemantic::TexCoord, 0, VertexElementFormat::Float2, offsetof(VertexSkinned, uv0)},
				{VertexSemantic::BlendIndices, 0, VertexElementFormat::UInt4, offsetof(VertexSkinned, blendIndices)},
				{VertexSemantic::BlendWeights, 0, VertexElementFormat::Float4, offsetof(VertexSkinned, blendWeights)},
			};
			return layout;
		}
	};

	template<typename TVertex>
	constexpr void ValidateVertexType()
	{
		static_assert(std::is_standard_layout_v<TVertex>, "Vertex must be standard layout");
		static_assert(std::is_trivially_copyable_v<TVertex>, "Vertex must be trivially copyable");
	}

	inline const char* ToSemanticName(VertexSemantic semantic)
	{
		switch (semantic)
		{
		case VertexSemantic::Position:     return "POSITION";
		case VertexSemantic::Normal:       return "NORMAL";
		case VertexSemantic::Tangent:      return "TANGENT";
		case VertexSemantic::Color:        return "COLOR";
		case VertexSemantic::TexCoord:     return "TEXCOORD";
		case VertexSemantic::BlendIndices: return "BLENDINDICES";
		case VertexSemantic::BlendWeights: return "BLENDWEIGHTS";
		default:
			return "UNKNOWN";
		}
	}

	inline D3D12_INPUT_CLASSIFICATION ToD3D12InputClassification(VertexInputClassification c)
	{
		switch (c)
		{
		case VertexInputClassification::PerVertex:
			return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		case VertexInputClassification::PerInstance:
			return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		default:
			return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		}
	}
}
