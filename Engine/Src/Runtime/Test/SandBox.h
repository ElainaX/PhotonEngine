#pragma once

#include <functional>
#include <string>
#include <windows.h>


namespace photon
{
	class SandBox 
	{
	public:
		SandBox() = default;

		void Run()
		{
			TestLogSystem();
			TestFileSystem();
			//TestMd5Code();
			//TestGuidAllocator();
			//LOG_INFO("Hello, {}!", "World");
			//LOG_ERROR("Some error message with arg: {}", 1);
			//LOG_WARN("Easy padding in numbers like {:08d}", 12);
		}

		void TestLogSystem();

		void TestFileSystem();

		void TestGuidAllocator();

		void TestMd5Code();
	};



	struct AllocTestor
	{
		std::string name;
		int age;

		bool operator==(const AllocTestor& rhs) const
		{
			return name == rhs.name && age == rhs.age;
		}

		size_t GetHashValue() const
		{
			size_t h1 = std::hash<std::string>{}(name);
			size_t h2 = std::hash<int>{}(age);
			return h1 ^ (h2 << 1);
		}
	};


}