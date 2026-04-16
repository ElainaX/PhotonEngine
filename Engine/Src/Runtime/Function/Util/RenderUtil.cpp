#include "RenderUtil.h"
#include "Macro.h"

#include <map>
#include <cmath>

#include "Function/Render/CascadedShadowManager.h"
#include "Function/Render/DX12RHI/GpuResourceManager.h"
#include "Function/Render/DX12RHI/RenderResource/VertexBuffer.h"
#include "Function/Render/DX12RHI/RenderResource/IndexBuffer.h"
#include "Function/Render/RenderObject/ImporterSettings.h"
#include "Function/Render/DX12RHI/ViewDesc.h"


namespace photon 
{

	Microsoft::WRL::ComPtr<ID3DBlob> RenderUtil::CreateD3DBlob(const void* data, UINT64 sizeInBytes, INT64 maxCopySize)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		DX_LogIfFailed(D3DCreateBlob(sizeInBytes, &blob));
		if (maxCopySize == -1)
			CopyMemory(blob->GetBufferPointer(), data, sizeInBytes);
		else
			CopyMemory(blob->GetBufferPointer(), data, sizeInBytes > sizeInBytes ? sizeInBytes : maxCopySize);
		return blob;
	}

	UINT RenderUtil::GetConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 256) & ~255;
	}

	UINT RenderUtil::FloatRoundToUINT(float a)
	{
		return (UINT)std::round(a);
	}

	std::pair<std::vector<float>, std::vector<float>> RenderUtil::LogLinearCascadedSplit(float cam_near, float cam_far, float shadow_distance, int cascadedCount, float lambda)
	{
		std::vector<float> spliter_distances;
		std::vector<float> spliter_ratios;
		assert(shadow_distance > cam_near);
		float farDivNear = shadow_distance / cam_near;
		float invCascadedCount = 1.0f / cascadedCount;
		for (int i = 1; i < cascadedCount; ++i)
		{
			float d = lambda * cam_near * std::powf(farDivNear, i * invCascadedCount)
				+ (1 - lambda) * (cam_near + (shadow_distance - cam_near) * (i * invCascadedCount));
			spliter_distances.push_back(d);
			spliter_ratios.push_back((d - cam_near) / (cam_far - cam_near));
		}
		spliter_distances.push_back(shadow_distance);
		spliter_ratios.push_back((shadow_distance - cam_near) / (cam_far - cam_near));

		return std::make_pair(spliter_distances, spliter_ratios);
	}

	std::vector<Vector3> RenderUtil::ProjViewToCorners(DirectX::XMMATRIX proj, DirectX::XMMATRIX view, bool reversedZ)
	{
		using namespace DirectX;

		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invViewProj = XMMatrixInverse(nullptr, viewProj);

		// D3D NDC: x,y in [-1,1], z in [0,1]
		float zNear = reversedZ ? 1.0f : 0.0f;
		float zFar = reversedZ ? 0.0f : 1.0f;

		XMVECTOR ndc[8] = {
			XMVectorSet(-1, -1, zNear, 1), // near
			XMVectorSet(-1, +1, zNear, 1),
			XMVectorSet(+1, +1, zNear, 1),
			XMVectorSet(+1, -1, zNear, 1),

			XMVectorSet(-1, -1, zFar, 1),  // far
			XMVectorSet(-1, +1, zFar, 1),
			XMVectorSet(+1, +1, zFar, 1),
			XMVectorSet(+1, -1, zFar, 1),
		};

		std::vector<Vector3> out(8);
		for (int i = 0; i < 8; ++i) {
			XMVECTOR p = XMVector4Transform(ndc[i], invViewProj);
			p = XMVectorScale(p, 1.0f / XMVectorGetW(p)); // divide by w
			XMStoreFloat3((XMFLOAT3*)&out[i], p);
		}
		return out;
	}

	uint32_t RenderUtil::GetFormatSizeInBytes(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
			return 4;

		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
			return 8;

		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 12;

		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 16;

		default:
			PHOTON_ASSERT(false, "Unsupported DXGI_FORMAT size query!");
			return 0;
		}
	}

	DXGI_FORMAT RenderUtil::ResolveViewFormat(const DXResource* resource, DXGI_FORMAT viewFormat)
	{
		return viewFormat == DXGI_FORMAT_UNKNOWN ? resource->dxDesc.Format : viewFormat;
	}

	uint32_t RenderUtil::Align256(uint32_t value)
	{
		return (value + 255u) & ~255u;
	}

	static std::unordered_map<DescriptorHeapKind, D3D12_DESCRIPTOR_HEAP_TYPE> s_heapKindToDXTypeMap{
		{DescriptorHeapKind::Cbv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV},
		{DescriptorHeapKind::Srv, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV},
		{DescriptorHeapKind::Uav, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV},
		{DescriptorHeapKind::Rtv, D3D12_DESCRIPTOR_HEAP_TYPE_RTV},
		{DescriptorHeapKind::Dsv, D3D12_DESCRIPTOR_HEAP_TYPE_DSV},
		{DescriptorHeapKind::Sampler, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER}
	};

	D3D12_DESCRIPTOR_HEAP_TYPE RenderUtil::HeapKindToDXType(DescriptorHeapKind heapkind)
	{
		D3D12_DESCRIPTOR_HEAP_TYPE type = s_heapKindToDXTypeMap[heapkind];
		return type;
	}

	DXGI_FORMAT RenderUtil::ToDxgiFormat(VertexElementFormat fmt)
	{
		return static_cast<DXGI_FORMAT>(fmt);
	}

	DXGI_FORMAT RenderUtil::ToDxgiFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGBA8_UNorm:      return DXGI_FORMAT_R8G8B8A8_UNORM;
		case TextureFormat::RGBA8_UNorm_sRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case TextureFormat::BGRA8_UNorm:      return DXGI_FORMAT_B8G8R8A8_UNORM;
		case TextureFormat::R8_UNorm:         return DXGI_FORMAT_R8_UNORM;
		case TextureFormat::RG16_Float:       return DXGI_FORMAT_R16G16_FLOAT;
		case TextureFormat::RGBA16_Float:     return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case TextureFormat::D24S8:            return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case TextureFormat::D32_Float:        return DXGI_FORMAT_D32_FLOAT;
		default:                              return DXGI_FORMAT_UNKNOWN;
		}
	}

	DXGI_FORMAT RenderUtil::ToDxgiFormat(IndexFormat format)
	{
		switch (format)
		{
		case IndexFormat::UInt16: return DXGI_FORMAT_R16_UINT;
		case IndexFormat::UInt32: return DXGI_FORMAT_R32_UINT;
		default: return DXGI_FORMAT_UNKNOWN;
		}
	}

	uint32_t RenderUtil::GetVertexElementFormatSize(VertexElementFormat fmt)
	{
		return GetFormatSizeInBytes(static_cast<DXGI_FORMAT>(fmt));
	}

	int RenderUtil::FindRootParameterIndex(
		const photon::ShaderProgramRenderResource& shaderRR,
		std::string_view name)
	{
		for (uint32_t i = 0; i < shaderRR.parameters.size(); ++i)
		{
			if (shaderRR.parameters[i].name == name)
				return static_cast<int>(i);
		}
		return -1;
	}

	int RenderUtil::FindMaterialTextureBindingIndex(const photon::MaterialAsset& matAsset, std::string_view name)
	{
		for (uint32_t i = 0; i < matAsset.textureBindings.size(); ++i)
		{
			if (matAsset.textureBindings[i].name == name)
				return static_cast<int>(i);
		}
		return -1;
	}

	D3D12_VERTEX_BUFFER_VIEW RenderUtil::GetDX12VertexBufferView(const VertexBuffer& vb,
		GpuResourceManager* gpuResMgr)
	{
		auto start = gpuResMgr->GetGpuVirtualAddress(vb.buffer);
		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = start + vb.range.firstVertex * static_cast<uint64_t>(vb.range.strideInBytes);
		vbv.SizeInBytes = vb.range.vertexCount * vb.range.strideInBytes;
		vbv.StrideInBytes = vb.range.strideInBytes;
		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW RenderUtil::GetDX12IndexBufferView(const IndexBuffer& ib,
		GpuResourceManager* gpuResMgr)
	{
		auto start = gpuResMgr->GetGpuVirtualAddress(ib.buffer);
		D3D12_INDEX_BUFFER_VIEW ibv = {};
		uint32_t strideByte = ib.range.format == IndexFormat::UInt16 ? 2u : 4u;
		ibv.BufferLocation = start + ib.range.firstIndex * static_cast<uint64_t>(strideByte);
		ibv.SizeInBytes = ib.range.indexCount * strideByte;
		ibv.Format = ToDxgiFormat(ib.range.format);
		return ibv;
	}

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> RenderUtil::GetStaticSamplers()
	{
		const CD3DX12_STATIC_SAMPLER_DESC pointWrap(0, D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		const CD3DX12_STATIC_SAMPLER_DESC pointClamp(1, D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(2, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(3, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
		const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(4, D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(5, D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

		return { pointWrap, pointClamp,
				linearWrap, linearClamp,
				anisotropicWrap, anisotropicClamp };
	}
}
