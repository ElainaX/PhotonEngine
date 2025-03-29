#pragma once

#include "TestShader.h"

#include <string>
#include <memory>
#include <unordered_map>

namespace photon 
{
	class ShaderFactory
	{
	public:
		const std::wstring s_ShaderFileFolder = L"E:/Code/PhotonEngine/Engine/Src/Runtime/Function/Render/Shaders/";

		Shader* Create(const std::wstring& shaderName)
		{
			auto find_it = m_LoadedShaders.find(shaderName);
			if(find_it != m_LoadedShaders.end())
			{
				return find_it->second.get();
			}

			std::wstring shaderfilepath = s_ShaderFileFolder + shaderName + L".hlsl";
			if(shaderName == L"TestShader")
			{
				m_LoadedShaders[shaderName] = std::make_shared<TestShader>(shaderfilepath);
				return m_LoadedShaders[shaderName].get();
			}
			return nullptr;
		}

		std::unordered_map<std::wstring, std::shared_ptr<Shader>> m_LoadedShaders;
	};


}