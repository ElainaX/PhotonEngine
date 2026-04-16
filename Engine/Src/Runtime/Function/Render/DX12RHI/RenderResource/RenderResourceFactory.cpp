#include "RenderResourceFactory.h"

#include <algorithm>
#include <cctype>
#include <vector>

#include "Function/Render/DX12RHI/d3dx12.h"
#include "Function/Util/RenderUtil.h"
#include "Macro.h"
#include "Function/Render/RenderObject/TextureAsset.h"

namespace photon
{
	namespace
	{
		constexpr D3D12_RESOURCE_STATES kShaderReadState =
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

		std::string ToLowerCopy(std::string s)
		{
			std::transform(
				s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return s;
		}

		// ConstantBuffer 上传时会把未使用部分补 0，避免 CBV 的 256B 对齐区间里残留未初始化字节。
		std::vector<std::byte> BuildPaddedConstantBuffer(const void* initData, uint32_t logicalSize, uint32_t alignedSize)
		{
			std::vector<std::byte> bytes(alignedSize, std::byte{ 0 });
			if (initData && logicalSize > 0)
				std::memcpy(bytes.data(), initData, logicalSize);
			return bytes;
		}
	}

	bool RenderResourceFactory::Initialize(
		DX12RHI* rhi,
		GpuResourceManager* gpuResMgr,
		FrameAllocatorSystem* frameAlloc,
		CommandContextManager* commandMgr)
	{
		if (!rhi || !gpuResMgr || !frameAlloc || !commandMgr)
			return false;

		m_rhi = rhi;
		m_gpuResMgr = gpuResMgr;
		m_frameAllocSystem = frameAlloc;
		m_commandManager = commandMgr;
		return true;
	}

	void RenderResourceFactory::Shutdown()
	{
		m_rhi = nullptr;
		m_gpuResMgr = nullptr;
		m_frameAllocSystem = nullptr;
		m_commandManager = nullptr;
	}

	DX12CommandContext& RenderResourceFactory::RequireGraphicsContext() const
	{
		PHOTON_ASSERT(m_commandManager, "RenderResourceFactory 尚未初始化 CommandContextManager");

		DX12CommandContext* context = m_commandManager->GetCurrentValidGraphicsContext();
		PHOTON_ASSERT(
			context,
			"RenderResourceFactory 需要一个已经 Begin 的有效 GraphicsContext。"
			"请在外部先 BeginInitContext 或 BeginGraphicsContext，并在外部统一负责 Submit。");
		return *context;
	}

	void RenderResourceFactory::UploadBufferToDefaultResource(
		GpuResourceHandle dstBuffer,
		const void* initData,
		uint32_t sizeInBytes,
		D3D12_RESOURCE_STATES stateAfterCopy) const
	{
		PHOTON_ASSERT(initData, "UploadBufferToDefaultResource 需要非空 initData");
		PHOTON_ASSERT(sizeInBytes > 0, "UploadBufferToDefaultResource 需要正的 sizeInBytes");

		DXResource* dstResource = m_gpuResMgr->GetResource(dstBuffer);
		PHOTON_ASSERT(dstResource, "目标 Buffer 资源无效");

		FrameResourceRange upload = m_frameAllocSystem->AllocateFrameUploadBuffer(sizeInBytes);
		DXResource* uploadResource = m_gpuResMgr->GetResource(upload.buffer);
		PHOTON_ASSERT(uploadResource, "Frame upload buffer 无效");

		D3D12_SUBRESOURCE_DATA subresource = {};
		subresource.pData = initData;
		subresource.RowPitch = sizeInBytes;
		subresource.SlicePitch = sizeInBytes;

		DX12CommandContext& context = RequireGraphicsContext();
		context.ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_COPY_DEST);
		context.CopyCpuDataToGpuDest(dstResource, uploadResource, upload.range.cbvOffsetInBytes, &subresource);
		context.ResourceStateTransform(dstResource, stateAfterCopy);
	}

	void RenderResourceFactory::UploadTextureSubresources(
		GpuResourceHandle dstTexture,
		const std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
		D3D12_RESOURCE_STATES stateAfterCopy) const
	{
		PHOTON_ASSERT(!subresources.empty(), "UploadTextureSubresources 需要至少一个 subresource");

		DXResource* dstResource = m_gpuResMgr->GetResource(dstTexture);
		PHOTON_ASSERT(dstResource, "目标 Texture 资源无效");

		const UINT64 requiredUploadSize = GetRequiredIntermediateSize(
			dstResource->gpuResource.Get(),
			0,
			static_cast<UINT>(subresources.size()));

		FrameResourceRange upload = m_frameAllocSystem->AllocateFrameUploadBuffer(static_cast<uint32_t>(requiredUploadSize));
		DXResource* uploadResource = m_gpuResMgr->GetResource(upload.buffer);
		PHOTON_ASSERT(uploadResource, "Texture upload buffer 无效");

		DX12CommandContext& context = RequireGraphicsContext();
		context.ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_COPY_DEST);
		context.CopyCpuDataToGpuDest(
			dstResource,
			uploadResource,
			upload.range.cbvOffsetInBytes,
			subresources.data(),
			0,
			static_cast<UINT>(subresources.size()));
		context.ResourceStateTransform(dstResource, stateAfterCopy);
	}

	VertexBuffer RenderResourceFactory::CreateVertexBuffer(
		const void* initData,
		uint64_t sizeInBytes,
		uint32_t vertStrideInBytes,
		const std::string& debugName)
	{
		PHOTON_ASSERT(m_gpuResMgr && m_frameAllocSystem, "RenderResourceFactory 尚未初始化");
		PHOTON_ASSERT(sizeInBytes > 0, "VertexBuffer size 不能为 0");
		PHOTON_ASSERT(vertStrideInBytes > 0, "VertexBuffer stride 不能为 0");

		DXBufferDesc desc = {};
		desc.bufferSizeInBytes = sizeInBytes;
		desc.heapProp = HeapProp::Default;
		desc.allowUav = false;

		VertexBuffer vb = {};
		vb.buffer = m_gpuResMgr->CreateBuffer(desc);
		vb.range.firstVertex = 0;
		vb.range.vertexCount = static_cast<uint32_t>(sizeInBytes / vertStrideInBytes);
		vb.range.strideInBytes = vertStrideInBytes;
		vb.debugName = debugName;

		if (initData)
		{
			UploadBufferToDefaultResource(
				vb.buffer,
				initData,
				sizeInBytes,
				D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}
		else
		{
			DXResource* dst = m_gpuResMgr->GetResource(vb.buffer);
			RequireGraphicsContext().ResourceStateTransform(dst, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}

		return vb;
	}

	VertexBuffer RenderResourceFactory::UpdateVertexBuffer(
		VertexBuffer bigBuffer,
		uint64_t offsetInBigBufferBytes,
		const void* initData,
		uint32_t sizeInBytes,
		uint32_t viewStrideInBytes,
		const std::string& debugName)
	{
		PHOTON_ASSERT(bigBuffer.buffer.IsValid(), "UpdateVertexBuffer 需要有效的大 Buffer");
		PHOTON_ASSERT(initData, "UpdateVertexBuffer 需要非空 initData");
		PHOTON_ASSERT(sizeInBytes > 0, "UpdateVertexBuffer size 不能为 0");
		PHOTON_ASSERT(viewStrideInBytes > 0, "UpdateVertexBuffer stride 不能为 0");
		PHOTON_ASSERT(offsetInBigBufferBytes % viewStrideInBytes == 0,
			"VertexBuffer 子分配 offset 必须按 stride 对齐");
		PHOTON_ASSERT(sizeInBytes % viewStrideInBytes == 0,
			"VertexBuffer 大小必须是 stride 的整数倍");

		DXResource* dstResource = m_gpuResMgr->GetResource(bigBuffer.buffer);
		PHOTON_ASSERT(dstResource, "VertexBuffer 对应的 DXResource 无效");

		FrameResourceRange upload = m_frameAllocSystem->AllocateFrameUploadBuffer(sizeInBytes);
		DXResource* uploadResource = m_gpuResMgr->GetResource(upload.buffer);
		PHOTON_ASSERT(uploadResource, "Frame upload buffer 无效");

		DX12CommandContext& context = RequireGraphicsContext();
		context.CopyBufferCpuToUpload(uploadResource, upload.range.cbvOffsetInBytes, initData, sizeInBytes);
		context.ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_COPY_DEST);
		context.CopyBufferRegion(
			dstResource,
			uploadResource,
			bigBuffer.range.firstVertex * bigBuffer.range.strideInBytes + offsetInBigBufferBytes,
			upload.range.cbvOffsetInBytes,
			sizeInBytes);
		context.ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		VertexBuffer view = bigBuffer;
		view.range.firstVertex = offsetInBigBufferBytes / viewStrideInBytes;
		view.range.vertexCount = sizeInBytes / viewStrideInBytes;
		view.range.strideInBytes = viewStrideInBytes;
		view.debugName = debugName;
		return view;
	}

	IndexBuffer RenderResourceFactory::CreateIndexBuffer(
		const void* initData,
		uint32_t sizeInBytes,
		DXGI_FORMAT format,
		const std::string& debugName)
	{
		const uint32_t indexStride = (format == DXGI_FORMAT_R16_UINT) ? 2u : 4u;

		PHOTON_ASSERT(m_gpuResMgr && m_frameAllocSystem, "RenderResourceFactory 尚未初始化");
		PHOTON_ASSERT(sizeInBytes > 0, "IndexBuffer size 不能为 0");
		PHOTON_ASSERT(format == DXGI_FORMAT_R16_UINT || format == DXGI_FORMAT_R32_UINT, "IndexBuffer 只支持 R16_UINT / R32_UINT");
		PHOTON_ASSERT(sizeInBytes % indexStride == 0, "IndexBuffer size 非法");
		DXBufferDesc desc = {};
		desc.bufferSizeInBytes = sizeInBytes;
		desc.heapProp = HeapProp::Default;
		desc.allowUav = false;

		IndexBuffer ib = {};
		ib.buffer = m_gpuResMgr->CreateBuffer(desc);
		ib.range.firstIndex = 0;
		ib.range.indexCount = sizeInBytes / indexStride;
		ib.range.format = (format == DXGI_FORMAT_R16_UINT) ? IndexFormat::UInt16 : IndexFormat::UInt32;
		ib.debugName = debugName;

		if (initData)
		{
			UploadBufferToDefaultResource(
				ib.buffer,
				initData,
				sizeInBytes,
				D3D12_RESOURCE_STATE_INDEX_BUFFER);
		}
		else
		{
			DXResource* dst = m_gpuResMgr->GetResource(ib.buffer);
			RequireGraphicsContext().ResourceStateTransform(dst, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		}

		return ib;
	}

	IndexBuffer RenderResourceFactory::UpdateIndexBuffer(
		IndexBuffer bigBuffer,
		uint32_t offsetInBigBufferBytes,
		const void* initData,
		uint32_t sizeInBytes,
		DXGI_FORMAT viewFormat,
		const std::string& debugName)
	{
		const uint32_t indexStride = (viewFormat == DXGI_FORMAT_R16_UINT) ? 2u : 4u;

		PHOTON_ASSERT(bigBuffer.buffer.IsValid(), "UpdateIndexBuffer 需要有效的大 Buffer");
		PHOTON_ASSERT(initData, "UpdateIndexBuffer 需要非空 initData");
		PHOTON_ASSERT(sizeInBytes > 0, "UpdateIndexBuffer size 不能为 0");
		PHOTON_ASSERT(viewFormat == DXGI_FORMAT_R16_UINT || viewFormat == DXGI_FORMAT_R32_UINT,
			"IndexBuffer 只支持 R16_UINT / R32_UINT");

		DXResource* dstResource = m_gpuResMgr->GetResource(bigBuffer.buffer);
		PHOTON_ASSERT(dstResource, "IndexBuffer 对应的 DXResource 无效");

		FrameResourceRange upload = m_frameAllocSystem->AllocateFrameUploadBuffer(sizeInBytes);
		DXResource* uploadResource = m_gpuResMgr->GetResource(upload.buffer);
		PHOTON_ASSERT(uploadResource, "Frame upload buffer 无效");

		DX12CommandContext& context = RequireGraphicsContext();
		context.CopyBufferCpuToUpload(uploadResource, upload.range.cbvOffsetInBytes, initData, sizeInBytes);
		context.ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_COPY_DEST);
		context.CopyBufferRegion(
			dstResource,
			uploadResource,
			bigBuffer.range.firstIndex * indexStride + offsetInBigBufferBytes,
			upload.range.cbvOffsetInBytes,
			sizeInBytes);
		context.ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_INDEX_BUFFER);

		IndexBuffer view = bigBuffer;
		view.range.firstIndex = offsetInBigBufferBytes / indexStride;
		view.range.indexCount = sizeInBytes / indexStride;
		view.range.format = (viewFormat == DXGI_FORMAT_R16_UINT) ? IndexFormat::UInt16 : IndexFormat::UInt32;
		view.debugName = debugName;
		return view;
	}

	ConstantBuffer RenderResourceFactory::CreateConstantBuffer(
		const void* initData,
		uint32_t sizeInBytes,
		ResourceUsage usage,
		const std::string& debugName)
	{
		PHOTON_ASSERT(m_gpuResMgr, "RenderResourceFactory 尚未初始化");
		PHOTON_ASSERT(sizeInBytes > 0, "ConstantBuffer size 不能为 0");

		const uint32_t alignedSize = RenderUtil::GetConstantBufferByteSize(sizeInBytes);

		ConstantBuffer cb = {};
		cb.usage = usage;
		cb.range.cbvOffsetInBytes = 0;
		cb.range.cbvSizeInBytes = alignedSize;
		cb.debugName = debugName;

		if (usage == ResourceUsage::Dynamic)
		{
			DXBufferDesc desc = {};
			desc.bufferSizeInBytes = alignedSize;
			desc.heapProp = HeapProp::Upload;
			desc.allowUav = false;

			cb.buffer = m_gpuResMgr->CreateBuffer(desc);

			if (initData)
			{
				auto bytes = BuildPaddedConstantBuffer(initData, sizeInBytes, alignedSize);
				DXResource* uploadResource = m_gpuResMgr->GetResource(cb.buffer);
				RequireGraphicsContext().CopyBufferCpuToUpload(uploadResource, bytes.data(), alignedSize);
			}

			return cb;
		}

		DXBufferDesc desc = {};
		desc.bufferSizeInBytes = alignedSize;
		desc.heapProp = HeapProp::Default;
		desc.allowUav = false;

		cb.buffer = m_gpuResMgr->CreateBuffer(desc);

		if (initData)
		{
			auto bytes = BuildPaddedConstantBuffer(initData, sizeInBytes, alignedSize);
			UploadBufferToDefaultResource(
				cb.buffer,
				bytes.data(),
				alignedSize,
				D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}
		else
		{
			DXResource* dst = m_gpuResMgr->GetResource(cb.buffer);
			RequireGraphicsContext().ResourceStateTransform(dst, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}

		return cb;
	}

	ConstantBuffer RenderResourceFactory::UpdateConstantBuffer(
		ConstantBuffer bigBuffer,
		uint32_t offsetInBigBufferBytes,
		const void* initData,
		uint32_t sizeInBytes,
		const std::string& debugName)
	{
		PHOTON_ASSERT(bigBuffer.buffer.IsValid(), "UpdateConstantBuffer 需要有效的大 Buffer");
		PHOTON_ASSERT(initData, "UpdateConstantBuffer 需要非空 initData");
		PHOTON_ASSERT(sizeInBytes > 0, "UpdateConstantBuffer size 不能为 0");

		const uint32_t alignedSize = RenderUtil::GetConstantBufferByteSize(sizeInBytes);
		auto bytes = BuildPaddedConstantBuffer(initData, sizeInBytes, alignedSize);

		ConstantBuffer view = bigBuffer;
		view.range.cbvOffsetInBytes = bigBuffer.range.cbvOffsetInBytes + offsetInBigBufferBytes;
		view.range.cbvSizeInBytes = alignedSize;
		view.debugName = debugName;

		DXResource* dstResource = m_gpuResMgr->GetResource(bigBuffer.buffer);
		PHOTON_ASSERT(dstResource, "ConstantBuffer 对应的 DXResource 无效");

		DX12CommandContext& context = RequireGraphicsContext();

		if (bigBuffer.usage == ResourceUsage::Dynamic)
		{
			context.CopyBufferCpuToUpload(dstResource, view.range.cbvOffsetInBytes, bytes.data(), alignedSize);
			return view;
		}

		FrameResourceRange upload = m_frameAllocSystem->AllocateFrameUploadBuffer(alignedSize);
		DXResource* uploadResource = m_gpuResMgr->GetResource(upload.buffer);
		PHOTON_ASSERT(uploadResource, "Frame upload buffer 无效");

		context.CopyBufferCpuToUpload(uploadResource, upload.range.cbvOffsetInBytes, bytes.data(), alignedSize);
		context.ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_COPY_DEST);
		context.CopyBufferRegion(
			dstResource,
			uploadResource,
			view.range.cbvOffsetInBytes,
			upload.range.cbvOffsetInBytes,
			alignedSize);
		context.ResourceStateTransform(dstResource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		return view;
	}

	TextureRenderResource RenderResourceFactory::BuildTextureRenderResource(const TextureAsset& asset, TextureHandle handle)
	{
		PHOTON_ASSERT(asset.HasCpuData(), "BuildTextureRenderResource 需要有效的 TextureCpuData");

		switch (asset.cpuData.dimension)
		{
		case TextureDimension::Tex2D:
			return BuildTexture2DRenderResource(asset, handle);
		case TextureDimension::Tex2DArray:
			return BuildTexture2DArrayRenderResource(asset, handle);
		case TextureDimension::TexCube:
			return BuildTextureCubeRenderResource(asset, handle);
		case TextureDimension::Tex3D:
			return BuildTexture3DRenderResource(asset, handle);
		default:
			PHOTON_ASSERT(false, "不支持的 TextureDimension");
			return {};
		}
	}

	TextureRenderResource RenderResourceFactory::BuildTexture2DRenderResource(const TextureAsset& asset, TextureHandle handle)
	{
		const TextureCpuData& cpu = asset.cpuData;
		PHOTON_ASSERT(cpu.arraySize == 1, "Tex2D 的 arraySize 必须为 1");
		PHOTON_ASSERT(cpu.depth == 1, "Tex2D 的 depth 必须为 1");

		TextureRenderResource rr = {};
		rr.handle = handle;
		rr.assetGuid = asset.meta.guid;
		rr.dimension = TextureDimension::Tex2D;
		rr.format = RenderUtil::ToDxgiFormat(cpu.format);
		rr.width = cpu.width;
		rr.height = cpu.height;
		rr.depth = 1;
		rr.arraySize = 1;
		rr.mipCount = cpu.mipCount;

		DXTexture2DDesc desc = {};
		desc.width = cpu.width;
		desc.height = cpu.height;
		desc.maxMipLevels = cpu.mipCount;
		desc.format = rr.format;
		desc.heapProp = HeapProp::Default;
		desc.flag = D3D12_RESOURCE_FLAG_NONE;

		rr.texture = m_gpuResMgr->CreateTexture2D(desc);
		auto subresources = BuildTextureSubresources(cpu);
		UploadTextureSubresources(rr.texture, subresources, kShaderReadState);

		ViewDesc srv = {};
		srv.type = ViewType::SRV;
		srv.dimension = ViewDimension::Texture2D;
		srv.format = rr.format;
		srv.mostDetailedMip = 0;
		srv.mipLevels = cpu.mipCount;
		rr.srv = m_gpuResMgr->GetOrCreatePersistentView(rr.texture, srv);

		return rr;
	}

	TextureRenderResource RenderResourceFactory::BuildTexture2DArrayRenderResource(const TextureAsset& asset, TextureHandle handle)
	{
		const TextureCpuData& cpu = asset.cpuData;
		PHOTON_ASSERT(cpu.arraySize > 1, "Tex2DArray 的 arraySize 必须大于 1");
		PHOTON_ASSERT(cpu.depth == 1, "Tex2DArray 的 depth 必须为 1");

		TextureRenderResource rr = {};
		rr.handle = handle;
		rr.assetGuid = asset.meta.guid;
		rr.dimension = TextureDimension::Tex2DArray;
		rr.format = RenderUtil::ToDxgiFormat(cpu.format);
		rr.width = cpu.width;
		rr.height = cpu.height;
		rr.depth = 1;
		rr.arraySize = cpu.arraySize;
		rr.mipCount = cpu.mipCount;

		DXTexture2DArrayDesc desc = {};
		desc.width = cpu.width;
		desc.height = cpu.height;
		desc.arraySize = cpu.arraySize;
		desc.maxMipLevels = cpu.mipCount;
		desc.format = rr.format;
		desc.heapProp = HeapProp::Default;
		desc.flag = D3D12_RESOURCE_FLAG_NONE;

		rr.texture = m_gpuResMgr->CreateTexture2DArray(desc);
		auto subresources = BuildTextureSubresources(cpu);
		UploadTextureSubresources(rr.texture, subresources, kShaderReadState);

		ViewDesc srv = {};
		srv.type = ViewType::SRV;
		srv.dimension = ViewDimension::Texture2DArray;
		srv.format = rr.format;
		srv.mostDetailedMip = 0;
		srv.mipLevels = cpu.mipCount;
		srv.firstArraySlice = 0;
		srv.arraySize = cpu.arraySize;
		rr.srv = m_gpuResMgr->GetOrCreatePersistentView(rr.texture, srv);

		return rr;
	}

	TextureRenderResource RenderResourceFactory::BuildTextureCubeRenderResource(const TextureAsset& asset, TextureHandle handle)
	{
		const TextureCpuData& cpu = asset.cpuData;
		PHOTON_ASSERT(cpu.arraySize == 6, "Cubemap 的 arraySize 必须为 6");
		PHOTON_ASSERT(cpu.depth == 1, "Cubemap 的 depth 必须为 1");

		TextureRenderResource rr = {};
		rr.handle = handle;
		rr.assetGuid = asset.meta.guid;
		rr.dimension = TextureDimension::TexCube;
		rr.format = RenderUtil::ToDxgiFormat(cpu.format);
		rr.width = cpu.width;
		rr.height = cpu.height;
		rr.depth = 1;
		rr.arraySize = 6;
		rr.mipCount = cpu.mipCount;

		// DX12 中 cubemap 底层仍然是 Texture2DArray，只是 SRV 解释方式是 TextureCube。
		DXTexture2DArrayDesc desc = {};
		desc.width = cpu.width;
		desc.height = cpu.height;
		desc.arraySize = 6;
		desc.maxMipLevels = cpu.mipCount;
		desc.format = rr.format;
		desc.heapProp = HeapProp::Default;
		desc.flag = D3D12_RESOURCE_FLAG_NONE;

		rr.texture = m_gpuResMgr->CreateTexture2DArray(desc);
		auto subresources = BuildTextureSubresources(cpu);
		UploadTextureSubresources(rr.texture, subresources, kShaderReadState);

		ViewDesc srv = {};
		srv.type = ViewType::SRV;
		srv.dimension = ViewDimension::TextureCube;
		srv.format = rr.format;
		srv.mostDetailedMip = 0;
		srv.mipLevels = cpu.mipCount;
		rr.srv = m_gpuResMgr->GetOrCreatePersistentView(rr.texture, srv);

		return rr;
	}

	TextureRenderResource RenderResourceFactory::BuildTexture3DRenderResource(const TextureAsset& asset, TextureHandle handle)
	{
		const TextureCpuData& cpu = asset.cpuData;
		PHOTON_ASSERT(cpu.depth > 1, "Tex3D 的 depth 必须大于 1");
		PHOTON_ASSERT(cpu.arraySize == 1, "Tex3D 的 arraySize 必须为 1");

		TextureRenderResource rr = {};
		rr.handle = handle;
		rr.assetGuid = asset.meta.guid;
		rr.dimension = TextureDimension::Tex3D;
		rr.format = RenderUtil::ToDxgiFormat(cpu.format);
		rr.width = cpu.width;
		rr.height = cpu.height;
		rr.depth = cpu.depth;
		rr.arraySize = 1;
		rr.mipCount = cpu.mipCount;

		DXTexture3DDesc desc = {};
		desc.width = cpu.width;
		desc.height = cpu.height;
		desc.depth = cpu.depth;
		desc.maxMipLevels = cpu.mipCount;
		desc.format = rr.format;
		desc.heapProp = HeapProp::Default;
		desc.flag = D3D12_RESOURCE_FLAG_NONE;

		rr.texture = m_gpuResMgr->CreateTexture3D(desc);
		auto subresources = BuildTextureSubresources(cpu);
		UploadTextureSubresources(rr.texture, subresources, kShaderReadState);

		ViewDesc srv = {};
		srv.type = ViewType::SRV;
		srv.dimension = ViewDimension::Texture3D;
		srv.format = rr.format;
		srv.mostDetailedMip = 0;
		srv.mipLevels = cpu.mipCount;
		rr.srv = m_gpuResMgr->GetOrCreatePersistentView(rr.texture, srv);

		return rr;
	}

	TextureHandle RenderResourceFactory::ResolveTextureOrFallback(Guid textureGuid, const IResourceView& resources) const
	{
		if (textureGuid.IsValid())
		{
			TextureHandle h = resources.GetTextureHandleByGuid(textureGuid);
			if (h.handle.IsValid())
				return h;
		}
		return resources.GetFallbackTexture();
	}

	ShaderHandle RenderResourceFactory::ResolveShaderProgramOrFallback(Guid shaderGuid, const IResourceView& resources) const
	{
		if (shaderGuid.IsValid())
		{
			ShaderHandle h = resources.GetShaderHandleByGuid(shaderGuid);
			if (h.handle.IsValid())
				return h;
		}
		return resources.GetFallbackShader();
	}

	MaterialRenderResource RenderResourceFactory::BuildMaterialRenderResource(
		const MaterialAsset& asset,
		MaterialHandle handle,
		const PersistentConstantBufferAllocation* matConstantAlloc,
		const IResourceView& resources)
	{
		MaterialRenderResource rr = {};
		rr.handle = handle;
		rr.assetGuid = asset.meta.guid;
		rr.shaderProgram = ResolveShaderProgramOrFallback(asset.shaderProgramAssetGuid, resources);

		if (matConstantAlloc && matConstantAlloc->IsValid())
		{
			rr.matConstantHandle = matConstantAlloc->handle;
			rr.matConstant = matConstantAlloc->bufferView;
			rr.matConstantCbv = matConstantAlloc->cbv;
		}

		rr.resolvedTextures.reserve(asset.textureBindings.size());
		for (const MaterialTextureBinding& binding : asset.textureBindings)
		{
			rr.resolvedTextures.push_back(ResolveTextureOrFallback(binding.textureGuid, resources));
		}

		// 暂时不使用
		rr.materialTable = {};
		return rr;
	}

	uint32_t RenderResourceFactory::GetTexturePixelStride(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGBA8_UNorm:
		case TextureFormat::RGBA8_UNorm_sRGB:
		case TextureFormat::BGRA8_UNorm:
			return 4;
		case TextureFormat::R8_UNorm:
			return 1;
		case TextureFormat::RG16_Float:
			return 4;
		case TextureFormat::RGBA16_Float:
			return 8;
		case TextureFormat::D24S8:
		case TextureFormat::D32_Float:
			return 4;
		default:
			PHOTON_ASSERT(false, "暂不支持该 TextureFormat 的字节大小查询");
			return 0;
		}
	}

	bool RenderResourceFactory::IsDepthTextureFormat(TextureFormat format)
	{
		return format == TextureFormat::D24S8 || format == TextureFormat::D32_Float;
	}

	ViewDimension RenderResourceFactory::GetSrvViewDimension(TextureDimension dimension, uint32_t arraySize)
	{
		switch (dimension)
		{
		case TextureDimension::Tex2D:     return ViewDimension::Texture2D;
		case TextureDimension::TexCube:   return ViewDimension::TextureCube;
		case TextureDimension::Tex2DArray:return ViewDimension::Texture2DArray;
		case TextureDimension::Tex3D:     return ViewDimension::Texture3D;
		default:
			PHOTON_ASSERT(false, "不支持的 TextureDimension");
			return arraySize > 1 ? ViewDimension::Texture2DArray : ViewDimension::Texture2D;
		}
	}

	std::vector<D3D12_SUBRESOURCE_DATA> RenderResourceFactory::BuildTextureSubresources(const TextureCpuData& cpuData)
	{
		PHOTON_ASSERT(!cpuData.Empty(), "TextureCpuData 为空");
		PHOTON_ASSERT(cpuData.width > 0 && cpuData.height > 0, "Texture 尺寸非法");
		PHOTON_ASSERT(cpuData.mipCount > 0, "Texture mipCount 不能为 0");

		const uint32_t bytesPerPixel = GetTexturePixelStride(cpuData.format);

		const uint32_t subresourceCount =
			(cpuData.dimension == TextureDimension::Tex3D)
			? cpuData.mipCount
			: (cpuData.arraySize * cpuData.mipCount);

		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		subresources.reserve(subresourceCount);

		size_t byteOffset = 0;

		if (cpuData.dimension == TextureDimension::Tex3D)
		{
			for (uint32_t mip = 0; mip < cpuData.mipCount; ++mip)
			{
				const uint32_t mipWidth = std::max<uint32_t>(1u, cpuData.width >> mip);
				const uint32_t mipHeight = std::max<uint32_t>(1u, cpuData.height >> mip);
				const uint32_t mipDepth = std::max<uint32_t>(1u, cpuData.depth >> mip);

				const size_t rowPitch = static_cast<size_t>(mipWidth) * bytesPerPixel;
				const size_t slicePitch = rowPitch * mipHeight * mipDepth;

				PHOTON_ASSERT(byteOffset + slicePitch <= cpuData.pixels.size(), "TextureCpuData.pixels 不足以覆盖所有 3D subresources");

				D3D12_SUBRESOURCE_DATA sub = {};
				sub.pData = cpuData.pixels.data() + byteOffset;
				sub.RowPitch = static_cast<LONG_PTR>(rowPitch);
				sub.SlicePitch = static_cast<LONG_PTR>(slicePitch);
				subresources.push_back(sub);

				byteOffset += slicePitch;
			}
		}
		else
		{
			for (uint32_t arraySlice = 0; arraySlice < cpuData.arraySize; ++arraySlice)
			{
				for (uint32_t mip = 0; mip < cpuData.mipCount; ++mip)
				{
					const uint32_t mipWidth = std::max<uint32_t>(1u, cpuData.width >> mip);
					const uint32_t mipHeight = std::max<uint32_t>(1u, cpuData.height >> mip);

					const size_t rowPitch = static_cast<size_t>(mipWidth) * bytesPerPixel;
					const size_t slicePitch = rowPitch * mipHeight;

					PHOTON_ASSERT(byteOffset + slicePitch <= cpuData.pixels.size(), "TextureCpuData.pixels 不足以覆盖所有 2D subresources");

					D3D12_SUBRESOURCE_DATA sub = {};
					sub.pData = cpuData.pixels.data() + byteOffset;
					sub.RowPitch = static_cast<LONG_PTR>(rowPitch);
					sub.SlicePitch = static_cast<LONG_PTR>(slicePitch);
					subresources.push_back(sub);

					byteOffset += slicePitch;
				}
			}
		}

		PHOTON_ASSERT(byteOffset == cpuData.pixels.size(),
			"当前 Factory 约定 TextureCpuData.pixels 必须按紧密排列的 subresource 顺序打包，"
			"并且字节总数要与 width/height/depth/arraySize/mipCount/format 完全匹配。");

		return subresources;
	}
}
