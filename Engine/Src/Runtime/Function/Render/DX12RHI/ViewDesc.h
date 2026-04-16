#pragma once

#include <cstdint>
#include <d3d12.h>
#include <dxgiformat.h>

#include "Resource/DXResourceHeader.h"

#include "ViewKey.h"
#include "RenderResource/VertexType.h"

namespace photon
{
	// 这个类还需要为其提供一系列辅助的构建struct
	struct ViewDesc
	{
		ViewType type = ViewType::Unknown;
		ViewDimension dimension = ViewDimension::Unknown;
		
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		uint32_t flags = ViewKeyFlag_None;

		uint32_t shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// texture range
		uint32_t mostDetailedMip = 0;
		uint32_t mipLevels = UINT32_MAX; 
		uint32_t mipSlice = 0; // 给RTV用的
		uint32_t firstArraySlice = 0;
		uint32_t arraySize = UINT32_MAX;
		uint32_t planeSlice = 0;


		// buffer range
		uint64_t firstElement = 0;
		uint32_t numElements = 0;
		uint32_t structureByteStride = 0;
		uint64_t counterOffsetInBytes = 0; // UAV counter if needed

		// CBV
		uint64_t cbvOffsetInBytes = 0;
		uint32_t cbvSizeInBytes = 0;

		// Sampler
		D3D12_SAMPLER_DESC sampler = {};
	};

	struct VertexBufferRange
	{
		uint32_t firstVertex = 0;
		uint32_t vertexCount = 0;
		uint32_t strideInBytes = 0;
	};

	struct IndexBufferRange
	{
		uint32_t firstIndex = 0;
		uint32_t indexCount = 0;
		IndexFormat format = IndexFormat::UInt32;
	};

	struct ConstantBufferRange
	{
		// CBV
		uint64_t cbvOffsetInBytes = 0;
		uint32_t cbvSizeInBytes = 0;
	};


	// 这里提供texture，buffer的子资源视图
	struct BufferSubresourceRange
	{
		// buffer range
		uint64_t firstElement = 0;
		uint32_t numElements = 0;
		uint32_t structureByteStride = 0;
		uint64_t counterOffsetInBytes = 0; // UAV counter if needed
	};

	struct TextureSubresourceRange
	{
		// texture range
		uint32_t mostDetailedMip = 0;
		uint32_t mipLevels = UINT32_MAX;
		uint32_t mipSlice = 0; // 给RTV用的
		uint32_t firstArraySlice = 0;
		uint32_t arraySize = UINT32_MAX;
		uint32_t planeSlice = 0;
	};

	// 一系列辅助函数
	inline ViewDesc GetDefaultRenderTargetViewDesc(const DXResource* resource, ViewDimension dimension)
	{
		ViewDesc desc = {};
		desc.format = resource->dxDesc.Format;
		desc.type = ViewType::RTV;
		desc.dimension = dimension;
		desc.mipSlice = 0;
		return desc;
	}

}
