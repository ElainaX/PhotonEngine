#include "SandBox.h"
#include "Macro.h"
#include "Platform/FileSystem/FileSystem.h"
#include "Resource/GuidAllocator.h"
#include "Function/Util/Md5.h"
#include "Function/Util/GameTimer.h"

template<>
struct std::hash<photon::AllocTestor>
{
	size_t operator()(const photon::AllocTestor& rhs) const noexcept { return rhs.GetHashValue(); }
};

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

	void SandBox::TestGuidAllocator()
	{
		GuidAllocator<std::string> allocA;
		GuidAllocator<AllocTestor> allocB;

		for(int i = 0; i < 100; ++i)
		{
			allocA.AllocGuid("Alloc_" + std::to_string(i));
		}
		allocA.FreeGuid(22);
		allocA.FreeElementGuid("Alloc_21");
		auto ais21 = allocA.IsGuidAllocated(22);

		allocA.AllocGuid("Hello");
		allocA.AllocGuid("World");
		auto ae21 = allocA.GetElement(21);
		auto ae150 = allocA.GetElement(150);
		if(ae21)
		{
			std::cout << ae21->get() << std::endl;
		}
		if(!ae150)
		{
			std::cout << "No Ae150" << std::endl;
		}
		ais21 = allocA.IsGuidAllocated(21);
		auto totalA = allocA.GetAllocatedGuids();

		for (int i = 0; i < 10; ++i)
		{
			allocB.AllocGuid({ "Alloc_" + std::to_string(i), 20});
		}
		allocB.FreeGuid(5);
		allocB.FreeElementGuid(AllocTestor{ "Alloc_0", 0 });
		allocB.IsElementGuidAllocated(AllocTestor{ "Alloc_0", 0 });
		allocB.AllocGuid({ "Hello", 1 });
		allocB.AllocGuid({ "world", 1 });
		auto be0 = allocB.GetElement(0);
		auto be50 = allocB.GetElement(50);
		if(be0)
		{
			auto& [a, b] = be0->get();
		}
		if(!be50) 
		{
			std::cout << "No Be50" << std::endl;
		}
		auto totalB = allocB.GetAllocatedGuids();
	}

	void SandBox::TestMd5Code()
	{
		GameTimer gt;
		std::filesystem::path rataen = L"E:\\Rrcs\\视频\\Bandicam\\拉塔恩.mp4";
		std::filesystem::path rataen2 = L"E:\\Rrcs\\视频\\OBS\\2025-02-22 14-06-17.mkv";
		
		gt.Reset();
		{
			std::string rataenMd5 = Md5::GetMd5FromFile(rataen);
			gt.Tick();
			LOG_INFO("File: {}, Md5: {}, cost: {} s", WString2String(rataen.generic_wstring()), rataenMd5, gt.DeltaTime());

			gt.Reset();
			std::string rataen2Md5 = Md5::GetMd5FromFile(rataen);
			gt.Tick();
			LOG_INFO("File: {}, Md5: {}, cost: {} s", WString2String(rataen2.generic_wstring()), rataen2Md5, gt.DeltaTime());
			LOG_INFO("{} == {}: {}", rataenMd5, rataen2Md5, rataenMd5 == rataen2Md5);
		}
		
	}

}
