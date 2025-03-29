#include "ResourceLoader.h"

namespace photon 
{



	std::pair<std::shared_ptr<Buffer>, std::shared_ptr<Texture2D>> ResourceLoader::LoadTexture(const std::filesystem::path& filepath)
	{
		std::unique_ptr<uint8_t[]> decodedData;
		D3D12_SUBRESOURCE_DATA subresource;
		auto Tex = m_Rhi->LoadTextureFromFile(filepath, decodedData, subresource);
		Tex->name = filepath.generic_wstring();
		UINT64 uploadBufferSize = GetRequiredIntermediateSize(Tex->gpuResource.Get(), 0, 1);
		BufferDesc desc;
		desc.bufferSizeInBytes = uploadBufferSize;
		desc.heapProp = ResourceHeapProperties::Upload;
		auto Upload = m_Rhi->CreateBuffer(desc);
		m_Rhi->CopySubResourceDataCpuToGpu(Tex.get(), Upload.get(), 0, &subresource);
		m_Rhi->ResourceStateTransform(Tex.get(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
		return std::make_pair(Upload, Tex);
	}

}