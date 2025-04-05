#pragma once

#include "TestShader.h"
#include "DebugDrawLight.h"
#include "Platform/FileSystem/FileSystem.h"

#include <string>
#include <memory>
#include <unordered_map>

namespace photon 
{
	class ShaderFactory
	{
	public:


		Shader* Create(const std::wstring& shaderName)
		{
			auto find_it = m_LoadedShaders.find(shaderName);
			if(find_it != m_LoadedShaders.end())
			{
				return find_it->second.get();
			}

			std::wstring shaderfilepath = g_ShaderFileFolder + shaderName + L".hlsl";
			if(shaderName == L"TestShader")
			{
				m_LoadedShaders[shaderName] = std::make_shared<TestShader>(shaderfilepath);
				return m_LoadedShaders[shaderName].get();
			}
			else if(shaderName == L"DebugDrawLight")
			{
				m_LoadedShaders[shaderName] = std::make_shared<DebugDrawLightShader>(shaderfilepath);
				return m_LoadedShaders[shaderName].get();
			}
			return nullptr;
		}

		std::unordered_map<std::wstring, std::shared_ptr<Shader>> m_LoadedShaders;
	};


}