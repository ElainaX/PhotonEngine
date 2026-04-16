#pragma once

#include <vector>
#include <filesystem>
#include <tuple>

namespace photon 
{
	const std::filesystem::path g_AssetTextureFolder = "E:/Code/PhotonEngine/Engine/Assets/Texture";
	const std::filesystem::path g_AssetTextureCubemapFolder = "E:/Code/PhotonEngine/Engine/Assets/Texture/Cubemap";
	const std::filesystem::path g_AssetModelFolder = "E:/Code/PhotonEngine/Engine/Assets/Model";
	const std::filesystem::path g_AssetShaderFolder = "E:/Code/PhotonEngine/Engine/Src/Runtime/Function/Render/Shaders";
	const std::filesystem::path g_AssetFolder = "E:/Code/PhotonEngine/Engine/Assets";
	const std::filesystem::path g_EditorFolder = "E:/Code/PhotonEngine/Engine/Src/Editor";

	class FileSystem
	{
	public:

		static std::vector<std::filesystem::path> GetFiles(const std::filesystem::path& directory,
		                                                   const std::string& extension = "");

		static std::vector<std::filesystem::path> GetFilesRecursive(const std::filesystem::path& directory,
		                                                            const std::string& extension = "");


	};

	class Path 
	{
	public:
		static const std::filesystem::path GetRelativePath(const std::filesystem::path& directory, 
			const std::filesystem::path& filePath);

		static const std::vector<std::string> GetPathSegments(const std::filesystem::path& filePath);

		static const std::tuple<std::string, std::string, std::string>
		GetFileExtensions(const std::filesystem::path& filePath);

		static const std::string GetFilePureName(const std::string& filePath);

		static const std::filesystem::path GetFileFolder(const std::filesystem::path& path);
	};

}