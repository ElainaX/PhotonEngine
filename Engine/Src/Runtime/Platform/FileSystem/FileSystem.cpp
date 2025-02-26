#include "FileSystem.h"
#include "Macro.h"

namespace photon 
{
	// FileSystem
	
	std::vector<std::filesystem::path> FileSystem::GetFiles(const std::filesystem::path& directory, const std::string& extension /*= ""*/)
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

	std::vector<std::filesystem::path> FileSystem::GetFilesRecursive(const std::filesystem::path& directory, const std::string& extension /*= ""*/)
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
			LOG_WARN("Not Specify Correct Directory, Using Source File Directory: {}", dir.generic_string());
			return filePath.lexically_relative(dir);
		}
		else 
		{
			return filePath.lexically_relative(directory);
		}
	}

	const std::vector<std::string> Path::GetPathSegments(const std::filesystem::path& filePath)
	{
		std::vector<std::string> segments;
		for(auto& iter : filePath)
		{
			if (iter.generic_string() == "/" || iter.generic_string().empty())
				continue;
			segments.emplace_back(iter.generic_string());
		}
		return segments;
	}

	const std::tuple<std::string, std::string, std::string> Path::GetFileExtensions(const std::filesystem::path& filePath)
	{
		return std::make_tuple<std::string, std::string, std::string>(
			filePath.extension().generic_string(),
			filePath.stem().extension().generic_string(),
			filePath.stem().stem().extension().generic_string()); // 防止多个extension后缀，比如archive.tar.gz
	}

	// path without extension
	const std::string Path::GetFilePureName(const std::string& fileFullName)
	{
		std::string filePureName = fileFullName;
		auto lastPos = fileFullName.find_first_of('.');
		if(lastPos != std::string::npos)
		{
			filePureName = fileFullName.substr(0, lastPos);
		}
		return filePureName;
	}

}