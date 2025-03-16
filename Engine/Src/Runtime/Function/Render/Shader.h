#pragma once
#include "Function/Util/Md5.h"
#include <vector>
#include <d3d12.h>
#include <wrl.h>

namespace photon 
{
	class Shader 
	{
	public:



		Microsoft::WRL::ComPtr<ID3DBlob> vs;
		Microsoft::WRL::ComPtr<ID3DBlob> ps;
		Microsoft::WRL::ComPtr<ID3DBlob> gs;
		std::string name;
	};
}