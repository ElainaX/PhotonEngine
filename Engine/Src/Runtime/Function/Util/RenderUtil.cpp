#include "RenderUtil.h"
#include "Macro.h"

#include <map>
#include <cmath>


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

}