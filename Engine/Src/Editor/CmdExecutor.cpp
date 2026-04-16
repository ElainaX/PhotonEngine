#include "CmdExecutor.h"

namespace photon 
{
	//inline std::string WString2String(const std::wstring& wstr)
	//{
	//	int length = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	//	if (length == 0) {
	//		return "";
	//	}

	//	std::string str(length, 0);
	//	WideCharToMultiByte(CP_ACP, 0, wstr.data(), -1, (char*)str.data(), length, NULL, NULL);
	//	return str;
	//}

	//std::future<bool> CmdExecutor::Execute(const char* cmd)
	//{
	//	std::string cmdStr = cmd;
	//	std::future<bool> execOver = std::async(std::launch::async, [this, cmdStr]() {
	//		return this->exec(cmdStr);
	//		});
	//	return execOver;
	//}

	//std::future<bool> CmdExecutor::Execute(const wchar_t* cmd)
	//{
	//	auto str = WString2String(cmd);
	//	return Execute(str.c_str());
	//}

	//int CmdExecutor::Output2Progress(const std::string& line)
	//{
	//	if (line == "")
	//		return progressBar.load();
	//	if (line == "End Process")
	//		return 100;

	//	auto percentPos = line.find_first_of('%');
	//	if (percentPos == std::string::npos)
	//		return ++startPoint;
	//	else
	//	{
	//		try
	//		{
	//			int num = std::stoi(line.substr(0, line.find_first_of('%')));
	//			int progress = startPoint + ((num + 1) * (endPoint - startPoint) / 101);
	//			return progress;
	//		}
	//		catch (const std::invalid_argument&) {
	//			startPoint++;
	//		}
	//	}
	//	return startPoint;
	//}

	//int CmdExecutor::GetProgress() const
	//{
	//	{
	//		return progressBar.load();
	//	}
	//}

	//void CmdExecutor::SetProgress(int progress)
	//{
	//	{
	//		progressBar.store(progress);
	//	}
	//}


	//bool CmdExecutor::exec(std::string cmd)
	//{
	//	std::string result;
	//	std::array<char, 128> buffer;
	//	std::string line;
	//	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
	//	if (!pipe) {
	//		throw std::runtime_error("popen() failed!");
	//	}
	//	setvbuf(pipe.get(), NULL, _IONBF, 0);
	//	while (fgets(buffer.data(), (int)buffer.size(), pipe.get()) != nullptr) {
	//		line = buffer.data();
	//		int newProgress = Output2Progress(line);
	//		SetProgress(newProgress);
	//	}
	//	return true;
	//}

}


