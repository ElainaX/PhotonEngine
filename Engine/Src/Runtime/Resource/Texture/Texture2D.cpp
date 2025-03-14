#include "Texture2D.h"

namespace photon 
{


	Texture2D::Texture2D(Texture2DDesc desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource)
	{
		dxDesc.Format = desc.format;
		dxDesc.MipLevels = desc.maxMipLevel;
		dxDesc.Width = desc.width;
		dxDesc.Height = desc.height;
		dxDesc.Flags = desc.flag;
		dxDesc.Alignment = 0;
		dxDesc.DepthOrArraySize = 1;
		dxDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		gpuResource = _gpuResource;
	}



	Texture2D::Texture2D(D3D12_RESOURCE_DESC desc, Microsoft::WRL::ComPtr<ID3D12Resource> _gpuResource)
	{
		dxDesc = desc;
		gpuResource = _gpuResource;
	}

}