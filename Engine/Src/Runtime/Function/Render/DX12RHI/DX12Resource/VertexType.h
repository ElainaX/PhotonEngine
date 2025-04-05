#pragma once 

#include "Core/Math/Vector4.h"
#include "Core/Math/Vector3.h"
#include "Core/Math/Vector2.h"
#include "VertexLayout.h"

#include <vector>
namespace photon 
{
	enum class VertexType 
	{
		VertexSimple, // VertexBase: 3 Position 
	};


	struct VertexSimple
	{
		VertexSimple() = default;
		VertexSimple(Vector3 _pos, Vector3 _normal, Vector3 _tangent, Vector2 _texcoord)
			: position(_pos), normal(_normal), tangent(_tangent), texCoord(_texcoord){}

		Vector3 position;
		Vector3 normal;
		Vector3 tangent;
		Vector2 texCoord;

		static VertexType vertexType;
		static VertexLayout layout;

		static VertexLayout&& BuildAndReturnLayout()
		{
			static VertexLayout layout;
			layout.PushLayout("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
			layout.PushLayout("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
			layout.PushLayout("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
			layout.PushLayout("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
			return std::move(layout);
		}
		static uint32_t GetInputLayoutSizeInBytes()
		{
			return layout.GetSizeInBytes();
		}

		static std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout()
		{
			return layout.GetInputLayouts();
		}
		

	};



	inline uint32_t VertexTypeToSizeInBytes(VertexType type)
	{
		switch (type)
		{
		case VertexType::VertexSimple:
			return VertexSimple::GetInputLayoutSizeInBytes();
		default:
			return 0;
		}
	}
}