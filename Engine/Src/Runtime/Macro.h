#pragma once
#include "Core/Log/LogManager.h"
#include "Function/Global/RuntimeGlobalContext.h"

#include <string>
#include <format>
#include <cassert>
#include <iostream>

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