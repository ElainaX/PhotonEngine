#pragma once

#include <vector>
#include <filesystem>
#include <tuple>

namespace photon 
{
	const std::filesystem::path g_AssetTextureFolder = L"E:/Code/PhotonEngine/Engine/Assets/Texture";
	const std::filesystem::path g_AssetModelFolder = L"E:/Code/PhotonEngine/Engine/Assets/Model";
	const std::wstring g_ShaderFileFolder = L"E:/Code/PhotonEngine/Engine/Src/Runtime/Function/Render/Shaders/";
	const std::filesystem::path g_AssetFolder = L"E:/Code/PhotonEngine/Engine/Assets";

	class FileSystem
	{
	public:

		static std::vector<std::filesystem::path> GetFiles(const std::filesystem::path& directory, 
			const std::wstring& extension = L"");

		static std::vector<std::filesystem::path> GetFilesRecursive(const std::filesystem::path& directory, 
			const std::wstring& extension = L"");


	};

	class Path 
	{
	public:
		static const std::filesystem::path GetRelativePath(const std::filesystem::path& directory, 
			const std::filesystem::path& filePath);

		static const std::vector<std::wstring> GetPathSegments(const std::filesystem::path& filePath);

		static const std::tuple<std::wstring, std::wstring, std::wstring>
			GetFileExtensions(const std::filesystem::path& filePath);

		static const std::wstring GetFilePureName(const std::wstring& filePath);

		static const std::filesystem::path GetFileFolder(const std::filesystem::path& path);
	};

}