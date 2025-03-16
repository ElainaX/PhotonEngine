#pragma once

#include <vector>
#include <filesystem>
#include <tuple>

namespace photon 
{
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
	};

}