#pragma once

#include "Function/Render/DX12RHI/GpuResourceManager.h"
#include "Function/Render/DX12RHI/FrameResource/FrameAllocatorSystem.h"
#include "Function/Render/DX12RHI/CommandContextManager.h"
#include "Function/Render/DX12RHI/RenderResource/VertexBuffer.h"
#include "Function/Render/DX12RHI/RenderResource/IndexBuffer.h"
#include "Function/Render/DX12RHI/RenderResource/ConstantBuffer.h"
#include "Function/Render/DX12RHI/RenderResource/MaterialRenderResource.h"
#include "Function/Render/DX12RHI/RenderResource/MeshRenderResource.h"
#include "Function/Render/DX12RHI/RenderResource/ResourceResolver.h"
#include "Function/Render/DX12RHI/RenderResource/TextureRenderResource.h"
#include "Function/Render/RenderObject/MaterialAsset.h"
#include "Function/Render/RenderObject/TextureAsset.h"

namespace photon
{
	/*
	 * RenderResourceFactory 的职责只保留在“把 CPU 描述翻译成 GPU RenderResource”。
	 *
		创建底层 GPU 大 Buffer / Texture
		往大 Buffer 的某个 offset 上传数据
		基于“已分配好的子区间”组装 MeshRenderResource / MaterialRenderResource
	 *
	 * 因此，外部在调用本工厂前，必须保证：
	 * - 已经存在一个有效的 GraphicsContext（初始化阶段可以是 InitContext，运行阶段可以是当前帧 GraphicsContext）。
	 * - 当前 FrameAllocatorSystem 也已经切换到了可分配的状态。
	 */
	class RenderResourceFactory
	{
	public:
		RenderResourceFactory() = default;

		bool Initialize(
			DX12RHI* rhi,
			GpuResourceManager* gpuResMgr,
			FrameAllocatorSystem* frameAlloc,
			CommandContextManager* commandMgr);

		void Shutdown();

	public:
		// ---------------------------------------------------------------------
		// Buffer helpers
		// ---------------------------------------------------------------------
		// initData == nullptr 时，仅创建资源，不记录初始化上传命令。
		VertexBuffer CreateVertexBuffer(
			const void* initData,
			uint64_t sizeInBytes,
			uint32_t vertStrideInBytes,
			const std::string& debugName = "");

		// 从一个“大 buffer” 中裁出一个逻辑 VertexBuffer 视图，同时把数据上传到指定 offset。
		VertexBuffer UpdateVertexBuffer(
			VertexBuffer bigBuffer,
			uint64_t offsetInBigBufferBytes,
			const void* initData,
			uint32_t sizeInBytes,
			uint32_t viewStrideInBytes,
			const std::string& debugName = "");

		IndexBuffer CreateIndexBuffer(
			const void* initData,
			uint32_t sizeInBytes,
			DXGI_FORMAT format,
			const std::string& debugName = "");

		IndexBuffer UpdateIndexBuffer(
			IndexBuffer bigBuffer,
			uint32_t offsetInBigBufferBytes,
			const void* initData,
			uint32_t sizeInBytes,
			DXGI_FORMAT viewFormat,
			const std::string& debugName = "");

		// ConstantBuffer 的 cbvSize 始终按 256B 对齐后的大小保存。
		ConstantBuffer CreateConstantBuffer(
			const void* initData,
			uint32_t sizeInBytes,
			ResourceUsage usage,
			const std::string& debugName = "");

		ConstantBuffer UpdateConstantBuffer(
			ConstantBuffer bigBuffer,
			uint32_t offsetInBigBufferBytes,
			const void* initData,
			uint32_t sizeInBytes,
			const std::string& debugName = "");

	public:
		TextureRenderResource BuildTextureRenderResource(const TextureAsset& asset, TextureHandle handle);
		MaterialRenderResource BuildMaterialRenderResource(
			const MaterialAsset& asset,
			MaterialHandle handle,
			const PersistentConstantBufferAllocation* matConstantAlloc,
			const IResourceView& resources);

	private:
		TextureRenderResource BuildTexture2DRenderResource(const TextureAsset& asset, TextureHandle handle);
		TextureRenderResource BuildTexture2DArrayRenderResource(const TextureAsset& asset, TextureHandle handle);
		TextureRenderResource BuildTextureCubeRenderResource(const TextureAsset& asset, TextureHandle handle);
		TextureRenderResource BuildTexture3DRenderResource(const TextureAsset& asset, TextureHandle handle);

		TextureHandle ResolveTextureOrFallback(Guid textureGuid, const IResourceView& resources) const;
		ShaderHandle ResolveShaderProgramOrFallback(Guid shaderGuid, const IResourceView& resources) const;

	private:
		// ---------------------------------------------------------------------
		// Internal helpers
		// ---------------------------------------------------------------------
		// 取得“当前已经由外部 Begin 过”的 GraphicsContext。
		// 工厂本身不负责 Begin/Submit，所以这里取不到就直接 assert。
		DX12CommandContext& RequireGraphicsContext() const;

		void UploadBufferToDefaultResource(
			GpuResourceHandle dstBuffer,
			const void* initData,
			uint32_t sizeInBytes,
			D3D12_RESOURCE_STATES stateAfterCopy) const;

		void UploadTextureSubresources(
			GpuResourceHandle dstTexture,
			const std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
			D3D12_RESOURCE_STATES stateAfterCopy) const;

		static uint32_t GetTexturePixelStride(TextureFormat format);
		static bool IsDepthTextureFormat(TextureFormat format);
		static ViewDimension GetSrvViewDimension(TextureDimension dimension, uint32_t arraySize);
		static std::vector<D3D12_SUBRESOURCE_DATA> BuildTextureSubresources(const TextureCpuData& cpuData);

	private:
		GpuResourceManager* m_gpuResMgr = nullptr;
		FrameAllocatorSystem* m_frameAllocSystem = nullptr;
		CommandContextManager* m_commandManager = nullptr;
		DX12RHI* m_rhi = nullptr;
	};
}
