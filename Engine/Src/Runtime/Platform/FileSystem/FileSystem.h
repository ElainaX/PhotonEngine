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
	};

}