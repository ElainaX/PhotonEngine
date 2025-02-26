#include "SandBox.h"
#include "Macro.h"
#include "Platform/FileSystem/FileSystem.h"


namespace photon 
{
	void SandBox::TestLogSystem()
	{
		std::string a = "Hello, {}!";
		std::string b = "World";
		LOG_INFO("Hello, {}!", b);
	}

	void SandBox::TestFileSystem()
	{
		FileSystem fileSystem;
		// Test Path
		std::filesystem::path path("./");
		std::filesystem::path absolutePath = std::filesystem::absolute(path);
		std::string currentPath = Path::GetRelativePath("E:/", absolutePath).generic_string();
		auto&& pathSegments = Path::GetPathSegments(absolutePath);
		auto&& currFolderfiles = fileSystem.GetFiles(path);
		auto&& currFolderCPPfiles = fileSystem.GetFiles(path, ".cpp");
		auto&& allfiles = fileSystem.GetFilesRecursive(path);
		auto&& allCPPfiles = fileSystem.GetFilesRecursive(path, ".cpp");

		if(!allfiles.empty())
		{
			std::filesystem::path p = allfiles[0];
			auto&& extensions = Path::GetFileExtensions(p);
			auto&& pSegments = Path::GetPathSegments(p);
			auto&& pureName = Path::GetFilePureName(p.lexically_normal().generic_string());
		}
	}
}

