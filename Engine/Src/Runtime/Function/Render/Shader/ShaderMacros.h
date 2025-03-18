#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <d3d12.h>

namespace photon
{
	struct MacroInfo
	{
		std::string macroDefine;
		std::string value;

		bool operator==(const MacroInfo& rhs) const
		{
			return (macroDefine + value) == (rhs.macroDefine + rhs.value);
		}

		size_t GetHashValue() const
		{
			size_t h1 = std::hash<std::string>{}(macroDefine);
			size_t h2 = std::hash<std::string>{}(value);
			return h1 ^ (h2 << 1);
		}

		std::string GetCombine() const
		{
			return macroDefine + value;
		}
	};

	class ShaderMacros
	{
	public:
		ShaderMacros() = default;
		
		bool IsVariantLoaded(const std::vector<MacroInfo>& macros)
		{
			std::string variant = VectorToVariantString(macros);
			return m_UsedShaderVariants.find(variant) != m_UsedShaderVariants.end();
		}

		std::string SetVariantLoaded(const std::vector<MacroInfo>& macros)
		{
			std::string variant = VectorToVariantString(macros);
			m_UsedShaderVariants.insert({ variant, macros });
			return variant;
		}

		std::vector<D3D_SHADER_MACRO> GetD3DMacroInfo(const std::string& variant)
		{
			auto find_it = m_UsedShaderVariants.find(variant);
			if(find_it == m_UsedShaderVariants.end())
			{
				throw;
			}

			auto& rawMacroInfo = m_UsedShaderVariants[variant];
			std::vector<D3D_SHADER_MACRO> ret(rawMacroInfo.size()+1);
			for(int i = 0; i < rawMacroInfo.size(); ++i)
			{
				ret[i].Name = rawMacroInfo[i].macroDefine.data();
				ret[i].Definition = rawMacroInfo[i].value.empty() ? nullptr : rawMacroInfo[i].value.data();
			}
			ret[ret.size() - 1].Name = NULL;
			ret[ret.size() - 1].Definition = NULL;
			return ret;
		}

		std::string VectorToVariantString(const std::vector<MacroInfo>& macros)
		{
			if (macros.empty())
				return "";

			std::string ret = macros[0].GetCombine();
			for(int i = 1; i < macros.size(); ++i)
			{
				ret += (s_SplitChar + macros[i].GetCombine());
			}
			return ret;
		}


	private:
		static std::string s_SplitChar;
		std::unordered_map<std::string, std::vector<MacroInfo>> m_UsedShaderVariants;
	};
}

template<>
struct std::hash<photon::MacroInfo>
{
	size_t operator()(const photon::MacroInfo& rhs) const noexcept { return rhs.GetHashValue(); }
};