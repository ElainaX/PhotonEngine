#include "FileSystem.h"
#include "Macro.h"

namespace photon 
{
	// FileSystem
	
	std::vector<std::filesystem::path> FileSystem::GetFiles(const std::filesystem::path& directory, const std::wstring& extension /*= ""*/)
	{
		// extension
		std::vector<std::filesystem::path> fileLists;
		for (const auto& directoryEntry : std::filesystem::directory_iterator{directory})
		{
			if(directoryEntry.is_regular_file())
			{
				if (extension.empty() || directoryEntry.path().extension() == extension)
					fileLists.emplace_back(directoryEntry.path());
			}
		}
		return fileLists;
	}

	std::vector<std::filesystem::path> FileSystem::GetFilesRecursive(const std::filesystem::path& directory, const std::wstring& extension /*= ""*/)
	{
		std::vector<std::filesystem::path> fileLists;
		for (const auto& directoryEntry : std::filesystem::recursive_directory_iterator{ directory })
		{
			if (directoryEntry.is_regular_file())
			{
				if (extension.empty() || directoryEntry.path().extension() == extension)
					fileLists.emplace_back(directoryEntry.path());
			}
		}
		return fileLists;
	}


	// Path
	
	const std::filesystem::path Path::GetRelativePath(const std::filesystem::path& directory, const std::filesystem::path& filePath)
	{
		if(directory.empty())
		{
			auto dir = std::filesystem::current_path();
			LOG_WARN("Not Specify Correct Directory, Using Source File Directory: {}", WString2String(dir.generic_wstring()));
			return filePath.lexically_relative(dir);
		}
		else 
		{
			return filePath.lexically_relative(directory);
		}
	}

	const std::vector<std::wstring> Path::GetPathSegments(const std::filesystem::path& filePath)
	{
		std::vector<std::wstring> segments;
		for(auto& iter : filePath)
		{
			if (iter.generic_wstring() == L"/" || iter.generic_wstring().empty())
				continue;
			segments.emplace_back(iter.generic_wstring());
		}
		return segments;
	}

	const std::tuple<std::wstring, std::wstring, std::wstring> Path::GetFileExtensions(const std::filesystem::path& filePath)
	{
		return std::make_tuple<std::wstring, std::wstring, std::wstring>(
			filePath.extension().generic_wstring(),
			filePath.stem().extension().generic_wstring(),
			filePath.stem().stem().extension().generic_wstring()); // 防止多个extension后缀，比如archive.tar.gz
	}

	// path without extension
	const std::wstring Path::GetFilePureName(const std::wstring& fileFullName)
	{
		std::wstring filePureName = fileFullName;
		auto lastPos = fileFullName.find_first_of('.');
		if(lastPos != std::wstring::npos)
		{
			filePureName = fileFullName.substr(0, lastPos);
		}
		return filePureName;
	}

}