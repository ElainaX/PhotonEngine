#pragma once
#include <vector>

#include "ConstantBufferCollection.h"
#include "Function/Render/ResourceHandle.h"
#include "Function/Render/ResourceTypes.h"
#include "Function/Render/DX12RHI/DescriptorHeap/Descriptor.h"
#include "Function/Render/DX12RHI/RenderResource/ConstantBuffer.h"

namespace photon
{
	struct MaterialRenderResource
	{
		MaterialHandle handle;
		Guid assetGuid;

		ShaderHandle shaderProgram;

		// 持久化 constant buffer 在 collection 里的句柄
		PersistentConstantBufferHandle matConstantHandle = {};

		// 运行时视图
		ConstantBuffer matConstant = {};
		DescriptorHandle matConstantCbv = {};

		// 与 MaterialAsset.textureBindings 对齐后的解析结果
		std::vector<TextureHandle> resolvedTextures;

		// 暂时未使用
		DescriptorHandle materialTable;
	};
}
