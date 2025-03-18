#include "RenderUtil.h"

#include <map>

namespace photon 
{

	Microsoft::WRL::ComPtr<ID3DBlob> RenderUtil::CreateD3DBlob(const void* data, UINT64 sizeInBytes)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		D3DCreateBlob(sizeInBytes, &blob);
		CopyMemory(blob->GetBufferPointer(), data, sizeInBytes);
		return blob;
	}

}