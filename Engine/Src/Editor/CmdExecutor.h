#pragma once
#include <stdexcept>
#include <string>
#include <iostream>
#include <array>
#include <Windows.h>
#include <future>
#include <queue>
#include <atomic>
#include <cmath>

namespace photon 
{
	class CmdExecutor
	{
	public:
		std::future<bool> Execute(const char* cmd);
		std::future<bool> Execute(const wchar_t* cmd);

		int Output2Progress(const std::string& line);
		int GetProgress() const;
		void SetProgress(int progress);

	private:
		bool exec(std::string cmd);

		std::queue<std::string> cmdOutputs;
		std::atomic<int> progressBar = 0;

		int startPoint = 0;
		int endPoint = 99;
	};
}

