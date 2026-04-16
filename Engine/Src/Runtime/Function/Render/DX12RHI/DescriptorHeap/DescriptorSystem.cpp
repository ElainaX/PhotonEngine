#include "DescriptorSystem.h"

#include "Function/Util/RenderUtil.h"

namespace photon
{
	bool DescriptorSystem::Initialize(ID3D12Device* device, RHI* rhi)
	{
		m_device = device;
		m_rhi = rhi;

		if (!m_device || !m_rhi)
			return false;

		if (!m_cbvPool.Initialize(m_device, DescriptorHeapKind::Cbv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			false, m_descriptorsPerPage))
		{
			return false;
		}

		if (!m_srvPool.Initialize(m_device, DescriptorHeapKind::Srv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			false, m_descriptorsPerPage))
		{
			return false;
		}

		if (!m_uavPool.Initialize(m_device, DescriptorHeapKind::Uav, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			false, m_descriptorsPerPage))
		{
			return false;
		}

		if (!m_rtvPool.Initialize(m_device, DescriptorHeapKind::Rtv, D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			false, m_descriptorsPerPage))
		{
			return false;
		}

		if (!m_dsvPool.Initialize(m_device, DescriptorHeapKind::Dsv, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			false, m_descriptorsPerPage))
		{
			return false;
		}

		if (!m_samplerPool.Initialize(m_device, DescriptorHeapKind::Sampler, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
			false, m_descriptorsPerPage))
		{
			return false;
		}

		GetDescriptorPagePoolFn cbvfn = [this]()
			{
				return &GetCbvPool();
			};
		GetDescriptorPagePoolFn srvfn = [this]()
			{
				return &GetSrvPool();
			};
		GetDescriptorPagePoolFn uavfn = [this]()
			{
				return &GetUavPool();
			};
		GetDescriptorPagePoolFn rtvfn = [this]()
			{
				return &GetRtvPool();
			};
		GetDescriptorPagePoolFn dsvfn = [this]()
			{
				return &GetDsvPool();
			};
		GetDescriptorPagePoolFn samplerfn = [this]()
			{
				return &GetSamplerPool();
			};

		m_viewTypeToPoolFnMap[ViewType::CBV] = cbvfn;
		m_viewTypeToPoolFnMap[ViewType::SRV] = srvfn;
		m_viewTypeToPoolFnMap[ViewType::UAV] = uavfn;
		m_viewTypeToPoolFnMap[ViewType::RTV] = rtvfn;
		m_viewTypeToPoolFnMap[ViewType::DSV] = dsvfn;
		m_viewTypeToPoolFnMap[ViewType::Sampler] = samplerfn;

		m_heapkindToPoolFreeFnMap[DescriptorHeapKind::Cbv] = cbvfn;
		m_heapkindToPoolFreeFnMap[DescriptorHeapKind::Srv] = srvfn;
		m_heapkindToPoolFreeFnMap[DescriptorHeapKind::Uav] = uavfn;
		m_heapkindToPoolFreeFnMap[DescriptorHeapKind::Rtv] = rtvfn;
		m_heapkindToPoolFreeFnMap[DescriptorHeapKind::Dsv] = dsvfn;
		m_heapkindToPoolFreeFnMap[DescriptorHeapKind::Sampler] = samplerfn;

		return true;
	}

	void DescriptorSystem::Shutdown()
	{
		m_cbvPool.Shutdown();
		m_srvPool.Shutdown();
		m_uavPool.Shutdown();
		m_rtvPool.Shutdown();
		m_dsvPool.Shutdown();
		m_samplerPool.Shutdown();
	}

	DescriptorHandle DescriptorSystem::CreateDescriptor(const DXResource* res, const ViewDesc& desc)
	{
		auto& pool = SelectPool(desc.type);
		auto handle = pool.Allocate();
		auto cpuhandle = pool.GetCpuHandle(handle);

		if (!pool.IsHandleValid(handle))
		{
			return {};
		}

		switch (desc.type)
		{
		case ViewType::CBV:
			auto cbv = BuildCbvDesc(res, desc);
			m_device->CreateConstantBufferView(&cbv, cpuhandle);
			break;
		case ViewType::SRV:
			auto srv = BuildSrvDesc(res, desc);
			m_device->CreateShaderResourceView(res->GetNative(), &srv, cpuhandle);
			break;
		case ViewType::UAV:
			auto uav = BuildUavDesc(res, desc);
			m_device->CreateUnorderedAccessView(res->GetNative(), nullptr, &uav, cpuhandle);
			break;
		case ViewType::RTV:
			auto rtv = BuildRtvDesc(res, desc);
			m_device->CreateRenderTargetView(res->GetNative(), &rtv, cpuhandle);
			break;
		case ViewType::DSV:
			auto dsv = BuildDsvDesc(res, desc);
			m_device->CreateDepthStencilView(res->GetNative(), &dsv, cpuhandle);
			break;
		case ViewType::Sampler:
			auto sampler = desc.sampler;
			m_device->CreateSampler(&sampler, cpuhandle);
			break;
		}


		return handle;
	}

	void DescriptorSystem::FreeDescriptor(DescriptorHandle handle)
	{
		auto& pool = SelectPool(handle.heapKind);
		pool.Free(handle);
	}

	bool DescriptorSystem::IsHandleValid(DescriptorHandle handle) const
	{
		auto& pool = SelectPool(handle.heapKind);
		return pool.IsHandleValid(handle);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorSystem::GetCpuHandle(DescriptorHandle handle) const
	{
		auto& pool = SelectPool(handle.heapKind);
		if (pool.IsHandleValid(handle))
		{
			return pool.GetCpuHandle(handle);
		}
		PHOTON_ASSERT(false, "handle invalid");
		return {};
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorSystem::GetGpuHandle(DescriptorHandle handle) const
	{
		auto& pool = SelectPool(handle.heapKind);
		if (pool.IsHandleValid(handle))
		{
			return pool.GetGpuHandle(handle);
		}
		PHOTON_ASSERT(false, "handle invalid");
		return {};
	}

	bool DescriptorSystem::CopyToCpuHandle(DescriptorHandle src, D3D12_CPU_DESCRIPTOR_HANDLE dstHandle)
	{
		auto dxheapType = RenderUtil::HeapKindToDXType(src.heapKind);
		m_device->CopyDescriptorsSimple(1, dstHandle, GetCpuHandle(src), dxheapType);
		return true;
	}

	bool DescriptorSystem::CopyBatchToCpuHandle(const std::vector<DescriptorHandle>& srcRangeStarts,
	                                            const std::vector<uint32_t>& srcRangeCounts, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& dstRangeStarts,
	                                            const std::vector<uint32_t>& dstRangeCounts)
	{
		if (srcRangeStarts.empty() || srcRangeCounts.empty() || dstRangeCounts.empty() || dstRangeStarts.empty())
			return false;
		auto dxheapType = RenderUtil::HeapKindToDXType(srcRangeStarts[0].heapKind);
		uint32_t srcRangeNum = srcRangeStarts.size();
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescRanges(srcRangeNum);
		std::transform(srcRangeStarts.begin(), srcRangeStarts.end(), srcDescRanges.begin(), 
			[this](const DescriptorHandle& handle) {return GetCpuHandle(handle); });
		m_device->CopyDescriptors(dstRangeStarts.size(), dstRangeStarts.data(), dstRangeCounts.data(),
			srcRangeNum, srcDescRanges.data(), srcRangeCounts.data(), dxheapType);
		return true;
	}

	bool DescriptorSystem::CopyBatchToCpuHandle(const std::vector<DescriptorHandle>& srcRangeStarts,
		const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& dstRangeStarts)
	{
		if (srcRangeStarts.empty() || dstRangeStarts.empty())
			return false;
		auto dxheapType = RenderUtil::HeapKindToDXType(srcRangeStarts[0].heapKind);
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescRanges(srcRangeStarts.size());
		std::transform(srcRangeStarts.begin(), srcRangeStarts.end(), srcDescRanges.begin(),
			[this](const DescriptorHandle& handle) {return GetCpuHandle(handle); });
		m_device->CopyDescriptors(dstRangeStarts.size(), dstRangeStarts.data(), nullptr,
			srcRangeStarts.size(), srcDescRanges.data(), nullptr, dxheapType);
		return true;
	}

	bool DescriptorSystem::CreateDescriptorAtCpuHandle(const DXResource* res, const ViewDesc& desc,
		D3D12_CPU_DESCRIPTOR_HANDLE dstHandle)
	{
		if (!res || dstHandle.ptr == 0)
			return false;

		switch (desc.type)
		{
		case ViewType::CBV:
		{
			auto cbv = BuildCbvDesc(res, desc);
			m_device->CreateConstantBufferView(&cbv, dstHandle);
			return true;
		}
		case ViewType::SRV:
		{
			auto srv = BuildSrvDesc(res, desc);
			m_device->CreateShaderResourceView(res->GetNative(), &srv, dstHandle);
			return true;
		}
		case ViewType::UAV:
		{
			auto uav = BuildUavDesc(res, desc);
			m_device->CreateUnorderedAccessView(res->GetNative(), nullptr, &uav, dstHandle);
			return true;
		}
		case ViewType::RTV:
		{
			auto rtv = BuildRtvDesc(res, desc);
			m_device->CreateRenderTargetView(res->GetNative(), &rtv, dstHandle);
			return true;
		}
		case ViewType::DSV:
		{
			auto dsv = BuildDsvDesc(res, desc);
			m_device->CreateDepthStencilView(res->GetNative(), &dsv, dstHandle);
			return true;
		}
		case ViewType::Sampler:
		{
			m_device->CreateSampler(&desc.sampler, dstHandle);
			return true;
		}
		default:
			return false;
		}
	}


	DescriptorPagePool& DescriptorSystem::SelectPool(ViewType type)
	{
		return *m_viewTypeToPoolFnMap[type]();
	}

	const DescriptorPagePool& DescriptorSystem::SelectPool(ViewType type) const
	{
		auto it = m_viewTypeToPoolFnMap.find(type);
		auto pool = it->second();
		return *pool;
	}

	DescriptorPagePool& DescriptorSystem::SelectPool(DescriptorHeapKind kind)
	{
		auto pool = m_heapkindToPoolFreeFnMap[kind]();
		return *pool;
	}

	const DescriptorPagePool& DescriptorSystem::SelectPool(DescriptorHeapKind kind) const
	{
		auto it = m_heapkindToPoolFreeFnMap.find(kind);
		auto pool = it->second();
		return *pool;
	}








	D3D12_SHADER_RESOURCE_VIEW_DESC DescriptorSystem::BuildSrvDesc(const DXResource* resource,
		const ViewDesc& desc) const
	{
		PHOTON_ASSERT(resource, "BuildSrvDesc: resource is null!");

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = RenderUtil::ResolveViewFormat(resource, desc.format);
		srvDesc.Shader4ComponentMapping = desc.shader4ComponentMapping;

		if (resource->dimension == DXResourceDimension::Buffer)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = desc.firstElement;

			if (desc.flags & ViewKeyFlag_RawBuffer)
			{
				srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
				srvDesc.Buffer.StructureByteStride = 0;

				auto formatSize = RenderUtil::GetFormatSizeInBytes(srvDesc.Format);

				const uint64_t byteOffset = desc.firstElement;
				const uint64_t byteSize = (desc.numElements > 0)
					? static_cast<uint64_t>(desc.numElements)
					: (resource->dxDesc.Width > byteOffset ? resource->dxDesc.Width - byteOffset : 0ull);

				srvDesc.Buffer.FirstElement = static_cast<UINT>(byteOffset / formatSize);
				srvDesc.Buffer.NumElements = static_cast<UINT>(byteSize / formatSize);
			}
			else if (desc.structureByteStride > 0)
			{
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
				srvDesc.Buffer.StructureByteStride = desc.structureByteStride;
				srvDesc.Format = DXGI_FORMAT_UNKNOWN;

				if (desc.numElements > 0)
				{
					srvDesc.Buffer.NumElements = desc.numElements;
				}
				else
				{
					const uint64_t remainingBytes =
						(resource->dxDesc.Width > desc.firstElement) ? (resource->dxDesc.Width - desc.firstElement) : 0ull;
					srvDesc.Buffer.NumElements = static_cast<UINT>(remainingBytes / desc.structureByteStride);
				}
			}
			else
			{
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
				srvDesc.Buffer.StructureByteStride = 0;

				if (desc.numElements > 0)
				{
					srvDesc.Buffer.NumElements = desc.numElements;
				}
				else
				{
					PHOTON_ASSERT(srvDesc.Format != DXGI_FORMAT_UNKNOWN, "Typed buffer SRV requires valid format!");
					const uint32_t elementSize = RenderUtil::GetFormatSizeInBytes(srvDesc.Format); // 先简化，typed buffer 默认按4字节元素处理
					const uint64_t remainingBytes =
						(resource->dxDesc.Width > desc.firstElement) ? (resource->dxDesc.Width - desc.firstElement) : 0ull;
					srvDesc.Buffer.NumElements = static_cast<UINT>(remainingBytes / elementSize);
				}
			}

			return srvDesc;
		}

		switch (desc.dimension)
		{
		case ViewDimension::Texture1D:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			srvDesc.Texture1D.MostDetailedMip = desc.mostDetailedMip;
			srvDesc.Texture1D.MipLevels =
				(desc.mipLevels == UINT32_MAX)
				? (resource->dxDesc.MipLevels - static_cast<UINT16>(desc.mostDetailedMip))
				: desc.mipLevels;
			srvDesc.Texture1D.ResourceMinLODClamp = 0.0f;
			break;

		case ViewDimension::Texture1DArray:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			srvDesc.Texture1DArray.MostDetailedMip = desc.mostDetailedMip;
			srvDesc.Texture1DArray.MipLevels =
				(desc.mipLevels == UINT32_MAX)
				? (resource->dxDesc.MipLevels - static_cast<UINT16>(desc.mostDetailedMip))
				: desc.mipLevels;
			srvDesc.Texture1DArray.FirstArraySlice = desc.firstArraySlice;
			srvDesc.Texture1DArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			srvDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
			break;

		case ViewDimension::Texture2D:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = desc.mostDetailedMip;
			srvDesc.Texture2D.MipLevels =
				(desc.mipLevels == UINT32_MAX)
				? (resource->dxDesc.MipLevels - static_cast<UINT16>(desc.mostDetailedMip))
				: desc.mipLevels;
			srvDesc.Texture2D.PlaneSlice = desc.planeSlice;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			break;

		case ViewDimension::Texture2DArray:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MostDetailedMip = desc.mostDetailedMip;
			srvDesc.Texture2DArray.MipLevels =
				(desc.mipLevels == UINT32_MAX)
				? (resource->dxDesc.MipLevels - static_cast<UINT16>(desc.mostDetailedMip))
				: desc.mipLevels;
			srvDesc.Texture2DArray.FirstArraySlice = desc.firstArraySlice;
			srvDesc.Texture2DArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			srvDesc.Texture2DArray.PlaneSlice = desc.planeSlice;
			srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
			break;

		case ViewDimension::Texture2DMS:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
			break;

		case ViewDimension::Texture2DMSArray:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
			srvDesc.Texture2DMSArray.FirstArraySlice = desc.firstArraySlice;
			srvDesc.Texture2DMSArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			break;

		case ViewDimension::Texture3D:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			srvDesc.Texture3D.MostDetailedMip = desc.mostDetailedMip;
			srvDesc.Texture3D.MipLevels =
				(desc.mipLevels == UINT32_MAX)
				? (resource->dxDesc.MipLevels - static_cast<UINT16>(desc.mostDetailedMip))
				: desc.mipLevels;
			srvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
			break;

		case ViewDimension::TextureCube:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = desc.mostDetailedMip;
			srvDesc.TextureCube.MipLevels =
				(desc.mipLevels == UINT32_MAX)
				? (resource->dxDesc.MipLevels - static_cast<UINT16>(desc.mostDetailedMip))
				: desc.mipLevels;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			break;

		case ViewDimension::TextureCubeArray:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
			srvDesc.TextureCubeArray.MostDetailedMip = desc.mostDetailedMip;
			srvDesc.TextureCubeArray.MipLevels =
				(desc.mipLevels == UINT32_MAX)
				? (resource->dxDesc.MipLevels - static_cast<UINT16>(desc.mostDetailedMip))
				: desc.mipLevels;
			srvDesc.TextureCubeArray.First2DArrayFace = desc.firstArraySlice;
			srvDesc.TextureCubeArray.NumCubes =
				(desc.arraySize == UINT32_MAX)
				? ((resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice)) / 6u)
				: (desc.arraySize / 6u);
			srvDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
			break;

		default:
			PHOTON_ASSERT(false, "Unsupported SRV ViewDimension!");
			break;
		}

		return srvDesc;
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC DescriptorSystem::BuildUavDesc(const DXResource* resource,
		const ViewDesc& desc) const
	{
		PHOTON_ASSERT(resource, "BuildUavDesc: resource is null!");

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = RenderUtil::ResolveViewFormat(resource, desc.format);

		if (resource->dimension == DXResourceDimension::Buffer)
		{
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = static_cast<UINT>(desc.firstElement);

			if (desc.flags & ViewKeyFlag_RawBuffer)
			{
				uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
				uavDesc.Buffer.StructureByteStride = 0;

				auto formatSize = RenderUtil::GetFormatSizeInBytes(uavDesc.Format);

				const uint64_t byteOffset = desc.firstElement;
				const uint64_t byteSize = (desc.numElements > 0)
					? static_cast<uint64_t>(desc.numElements)
					: (resource->dxDesc.Width > byteOffset ? resource->dxDesc.Width - byteOffset : 0ull);

				uavDesc.Buffer.FirstElement = static_cast<UINT>(byteOffset / formatSize);
				uavDesc.Buffer.NumElements = static_cast<UINT>(byteSize / formatSize);
			}
			else if (desc.structureByteStride > 0)
			{
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
				uavDesc.Buffer.StructureByteStride = desc.structureByteStride;

				if (desc.numElements > 0)
				{
					uavDesc.Buffer.NumElements = desc.numElements;
				}
				else
				{
					const uint64_t remainingBytes =
						(resource->dxDesc.Width > desc.firstElement) ? (resource->dxDesc.Width - desc.firstElement) : 0ull;
					uavDesc.Buffer.NumElements = static_cast<UINT>(remainingBytes / desc.structureByteStride);
				}
			}
			else
			{
				uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
				uavDesc.Buffer.StructureByteStride = 0;

				if (desc.numElements > 0)
				{
					uavDesc.Buffer.NumElements = desc.numElements;
				}
				else
				{
					PHOTON_ASSERT(uavDesc.Format != DXGI_FORMAT_UNKNOWN, "Typed buffer UAV requires valid format!");
					const uint32_t elementSize = RenderUtil::GetFormatSizeInBytes(uavDesc.Format);
					const uint64_t remainingBytes =
						(resource->dxDesc.Width > desc.firstElement) ? (resource->dxDesc.Width - desc.firstElement) : 0ull;
					uavDesc.Buffer.NumElements = static_cast<UINT>(remainingBytes / elementSize);
				}
			}

			uavDesc.Buffer.CounterOffsetInBytes = desc.counterOffsetInBytes;
			return uavDesc;
		}

		switch (desc.dimension)
		{
		case ViewDimension::Texture1D:
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
			uavDesc.Texture1D.MipSlice = desc.mipSlice;
			break;

		case ViewDimension::Texture1DArray:
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
			uavDesc.Texture1DArray.MipSlice = desc.mipSlice;
			uavDesc.Texture1DArray.FirstArraySlice = desc.firstArraySlice;
			uavDesc.Texture1DArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			break;

		case ViewDimension::Texture2D:
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = desc.mipSlice;
			uavDesc.Texture2D.PlaneSlice = desc.planeSlice;
			break;

		case ViewDimension::Texture2DArray:
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.Texture2DArray.MipSlice = desc.mipSlice;
			uavDesc.Texture2DArray.FirstArraySlice = desc.firstArraySlice;
			uavDesc.Texture2DArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			uavDesc.Texture2DArray.PlaneSlice = desc.planeSlice;
			break;

		case ViewDimension::Texture3D:
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			uavDesc.Texture3D.MipSlice = desc.mipSlice;
			uavDesc.Texture3D.FirstWSlice = desc.firstArraySlice;
			uavDesc.Texture3D.WSize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			break;

		default:
			PHOTON_ASSERT(false, "Unsupported UAV ViewDimension!");
			break;
		}

		return uavDesc;
	}

	D3D12_RENDER_TARGET_VIEW_DESC DescriptorSystem::BuildRtvDesc(const DXResource* resource,
		const ViewDesc& desc) const
	{
		PHOTON_ASSERT(resource, "BuildRtvDesc: resource is null!");

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = RenderUtil::ResolveViewFormat(resource, desc.format);

		if (resource->dimension == DXResourceDimension::Buffer)
		{
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_BUFFER;
			rtvDesc.Buffer.FirstElement = static_cast<UINT>(desc.firstElement);
			rtvDesc.Buffer.NumElements = desc.numElements;
			return rtvDesc;
		}

		switch (desc.dimension)
		{
		case ViewDimension::Texture1D:
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
			rtvDesc.Texture1D.MipSlice = desc.mipSlice;
			break;

		case ViewDimension::Texture1DArray:
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
			rtvDesc.Texture1DArray.MipSlice = desc.mipSlice;
			rtvDesc.Texture1DArray.FirstArraySlice = desc.firstArraySlice;
			rtvDesc.Texture1DArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			break;

		case ViewDimension::Texture2D:
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = desc.mipSlice;
			rtvDesc.Texture2D.PlaneSlice = desc.planeSlice;
			break;

		case ViewDimension::Texture2DArray:
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.MipSlice = desc.mipSlice;
			rtvDesc.Texture2DArray.FirstArraySlice = desc.firstArraySlice;
			rtvDesc.Texture2DArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			rtvDesc.Texture2DArray.PlaneSlice = desc.planeSlice;
			break;

		case ViewDimension::Texture2DMS:
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			break;

		case ViewDimension::Texture2DMSArray:
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
			rtvDesc.Texture2DMSArray.FirstArraySlice = desc.firstArraySlice;
			rtvDesc.Texture2DMSArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			break;

		case ViewDimension::Texture3D:
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
			rtvDesc.Texture3D.MipSlice = desc.mipSlice;
			rtvDesc.Texture3D.FirstWSlice = desc.firstArraySlice;
			rtvDesc.Texture3D.WSize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			break;

		default:
			PHOTON_ASSERT(false, "Unsupported RTV ViewDimension!");
			break;
		}

		return rtvDesc;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC DescriptorSystem::BuildDsvDesc(const DXResource* resource,
		const ViewDesc& desc) const
	{
		PHOTON_ASSERT(resource, "BuildDsvDesc: resource is null!");

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = RenderUtil::ResolveViewFormat(resource, desc.format);
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		if (desc.flags & ViewKeyFlag_ReadOnlyDepth)
			dsvDesc.Flags |= D3D12_DSV_FLAG_READ_ONLY_DEPTH;
		if (desc.flags & ViewKeyFlag_ReadOnlyStencil)
			dsvDesc.Flags |= D3D12_DSV_FLAG_READ_ONLY_STENCIL;

		switch (desc.dimension)
		{
		case ViewDimension::Texture1D:
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
			dsvDesc.Texture1D.MipSlice = desc.mipSlice;
			break;

		case ViewDimension::Texture1DArray:
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
			dsvDesc.Texture1DArray.MipSlice = desc.mipSlice;
			dsvDesc.Texture1DArray.FirstArraySlice = desc.firstArraySlice;
			dsvDesc.Texture1DArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			break;

		case ViewDimension::Texture2D:
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = desc.mipSlice;
			break;

		case ViewDimension::Texture2DArray:
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.MipSlice = desc.mipSlice;
			dsvDesc.Texture2DArray.FirstArraySlice = desc.firstArraySlice;
			dsvDesc.Texture2DArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			break;

		case ViewDimension::Texture2DMS:
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
			break;

		case ViewDimension::Texture2DMSArray:
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
			dsvDesc.Texture2DMSArray.FirstArraySlice = desc.firstArraySlice;
			dsvDesc.Texture2DMSArray.ArraySize =
				(desc.arraySize == UINT32_MAX)
				? (resource->dxDesc.DepthOrArraySize - static_cast<UINT16>(desc.firstArraySlice))
				: desc.arraySize;
			break;

		default:
			PHOTON_ASSERT(false, "Unsupported DSV ViewDimension!");
			break;
		}

		return dsvDesc;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC DescriptorSystem::BuildCbvDesc(const DXResource* resource,
		const ViewDesc& desc) const
	{
		PHOTON_ASSERT(resource, "BuildCbvDesc: resource is null!");
		PHOTON_ASSERT(resource->dimension == DXResourceDimension::Buffer, "CBV must be created from buffer resource!");

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};

		const uint64_t offset = desc.cbvOffsetInBytes;
		uint32_t sizeInBytes = desc.cbvSizeInBytes;

		if (sizeInBytes == 0)
		{
			PHOTON_ASSERT(resource->dxDesc.Width >= offset, "CBV offset out of range!");
			sizeInBytes = static_cast<uint32_t>(resource->dxDesc.Width - offset);
		}

		cbvDesc.BufferLocation = resource->GetNative()->GetGPUVirtualAddress() + offset;
		cbvDesc.SizeInBytes = RenderUtil::Align256(sizeInBytes);

		return cbvDesc;
	}

}
