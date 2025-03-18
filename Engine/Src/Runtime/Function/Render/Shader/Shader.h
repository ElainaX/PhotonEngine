#pragma once

#include <string>
#include <d3dcompiler.h>
#include <windows.h>

namespace photon 
{
	class Shader
	{
	public:
		Shader(const std::wstring& filepath)
			: sourceFilepath(filepath){}

		std::wstring sourceFilepath;
		
	protected:
		UINT GetFlag1()
		{
#if defined(DEBUG) || defined(_DEBUG)
			return D3DCOMPILE_DEBUG;
#else
			return D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
		}
	};
}