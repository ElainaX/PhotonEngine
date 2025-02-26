#pragma once

#include "Macro.h"
#include "spdlog/spdlog.h"

namespace photon
{
	class SandBox 
	{
	public:
		SandBox() = default;

		void Run()
		{
			std::string a = "Hello, {}!";
			std::string b = "World";
			LOG_INFO("Hello, {}!", b);
			//LOG_INFO("Hello, {}!", "World");
			//LOG_ERROR("Some error message with arg: {}", 1);
			//LOG_WARN("Easy padding in numbers like {:08d}", 12);
		}
	};

}