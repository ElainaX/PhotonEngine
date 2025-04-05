#pragma once
#include "Core/Log/LogManager.h"
#include "Function/Global/RuntimeGlobalContext.h"

#include <string>
#include <format>
#include <cassert>
#include <iostream>
#include <d3d12.h>
#include <windows.h>

#define LOG_DEBUG(...) \
		g_RuntimeGlobalContext.logManager->Log(__FUNCTION__ " (line " + std::to_string(__LINE__) + "): " + std::format(__VA_ARGS__))
#define LOG_INFO(...) \
		g_RuntimeGlobalContext.logManager->LogInfo(__FUNCTION__ " (line " + std::to_string(__LINE__) + "): " + std::format(__VA_ARGS__))
#define LOG_WARN(...) \
		g_RuntimeGlobalContext.logManager->LogWarn(__FUNCTION__ " (line " + std::to_string(__LINE__) + "): " + std::format(__VA_ARGS__))
#define LOG_ERROR(...) \
		g_RuntimeGlobalContext.logManager->LogError(__FUNCTION__ " (line " + std::to_string(__LINE__) + "): " + std::format(__VA_ARGS__))
#define LOG_FATAL(...) \
		g_RuntimeGlobalContext.logManager->LogFatal(__FUNCTION__ " (line " + std::to_string(__LINE__) + "): " + std::format(__VA_ARGS__))

#define PHOTON_ASSERT(condition, message) \
    if (!(condition)) { \
        std::cerr << "自定义断言失败: " << message << std::endl; \
        assert(condition); \
    }

#define DX_LogIfFailed(result) \
	    if (FAILED((result))) { \
        std::cerr << "DX FAILED" << std::endl; \
        assert(0); \
		}

inline std::string WString2String(const std::wstring & wstr)
{
	int length = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	if (length == 0) {
		return "";
	}

	std::string str(length, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.data(), -1, str.data(), length, NULL, NULL);
	return str;
}

inline std::wstring String2WString(const std::string& str)
{
	std::wstring wstr;
	wstr.resize(str.size());
	MultiByteToWideChar(CP_ACP, 0, str.data(), str.size(), wstr.data(), str.size());
	return wstr;
}