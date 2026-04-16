#pragma once

#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <wrl.h>
#include <vector>
#include <utility>

#include "Core/Math/Vector4.h"
#include "Function/Render/CascadedShadowManager.h"
#include "Function/Render/RenderObject/MaterialAsset.h"
#include "Function/Render/Resource/MeshCpuData.h"
#include "Resource/DXResourceHeader.h"

namespace photon 
{
	enum class TextureFormat : uint16_t;

	class RenderUtil
	{
	public:
		static Microsoft::WRL::ComPtr<ID3DBlob> CreateD3DBlob(const void* data, UINT64 sizeInBytes, INT64 maxCopySize = -1);

		// constant buffer必须256B对齐
		static UINT GetConstantBufferByteSize(UINT byteSize);

		static UINT FloatRoundToUINT(float a);

		// first = 相机空间的distance的spliters，元素的数值在cam_near ~ shadow_distance之间
		// second = 根据frustum比例的分割线，元素的数值在0~1之间
		static std::pair<std::vector<float>, std::vector<float>> LogLinearCascadedSplit(float cam_near, float cam_far, float shadow_distance, int cascadedCount, float lambda = 0.5f);

		static std::vector<Vector3> ProjViewToCorners(DirectX::XMMATRIX proj, DirectX::XMMATRIX view, bool reversedZ = false);
	
		static uint32_t GetFormatSizeInBytes(DXGI_FORMAT format);

		static DXGI_FORMAT ResolveViewFormat(const DXResource* resource, DXGI_FORMAT viewFormat);

		static uint32_t Align256(uint32_t value);

		static D3D12_DESCRIPTOR_HEAP_TYPE HeapKindToDXType(DescriptorHeapKind heapkind);


		static DXGI_FORMAT ToDxgiFormat(VertexElementFormat fmt);
		static DXGI_FORMAT ToDxgiFormat(TextureFormat format);
		static DXGI_FORMAT ToDxgiFormat(IndexFormat format);

		static uint32_t GetVertexElementFormatSize(VertexElementFormat fmt);


		template<typename TVertex, typename TIndex>
		static MeshCpuData BuildMeshCpuData(
			const std::vector<TVertex>& vertices,
			const std::vector<TIndex>& indices)
		{
			ValidateVertexType<TVertex>();

			MeshCpuData mesh;
			mesh.vertexLayout = VertexTraits<TVertex>::GetLayout();

			mesh.vertexCount = static_cast<uint32_t>(vertices.size());
			mesh.indexCount = static_cast<uint32_t>(indices.size());

			mesh.vertexData.resize(sizeof(TVertex) * vertices.size());
			std::memcpy(mesh.vertexData.data(), vertices.data(), mesh.vertexData.size());

			mesh.indexData.resize(sizeof(TIndex) * indices.size());
			std::memcpy(mesh.indexData.data(), indices.data(), mesh.indexData.size());

			if constexpr (sizeof(TIndex) == 2)
				mesh.indexFormat = IndexFormat::UInt16;
			else
				mesh.indexFormat = IndexFormat::UInt32;
		
			return mesh;
		}

		static int FindRootParameterIndex(
			const photon::ShaderProgramRenderResource& shaderRR,
			std::string_view name);

		static int FindMaterialTextureBindingIndex(
			const photon::MaterialAsset& matAsset,
			std::string_view name);


		static D3D12_VERTEX_BUFFER_VIEW GetDX12VertexBufferView(const VertexBuffer& vb, GpuResourceManager* gpuResMgr);
		static D3D12_INDEX_BUFFER_VIEW GetDX12IndexBufferView(const IndexBuffer& vb, GpuResourceManager* gpuResMgr);

		static std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
	};


}
