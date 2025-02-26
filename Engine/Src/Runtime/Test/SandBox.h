#pragma once


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
			//LOG_INFO("Hello, {}!", "World");
			//LOG_ERROR("Some error message with arg: {}", 1);
			//LOG_WARN("Easy padding in numbers like {:08d}", 12);
		}

		void TestLogSystem();

		void TestFileSystem();
	};

}