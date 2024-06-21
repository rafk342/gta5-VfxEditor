#pragma once

#include "helpers/helpers.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

inline spdlog::logger* getLogger()
{
	static std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("log", "__TceLog.log", std::ios_base::trunc);
	return logger.get();
}

template<typename T>
inline void LogInfo(const T& str)
{
	auto* logger = getLogger();
	logger->info(str);
	logger->flush();
}

template<typename... T>
inline void LogInfo(std::format_string<T...> fmt, T&&... args)
{
	auto* logger = getLogger();
	logger->info(vfmt(fmt, std::forward<T>(args)...));
	logger->flush();
}

inline void logPtrBytes(u8* address, size_t len)
{
	std::string res;
	res.reserve(len * 3);
	for (size_t i = 0; i < len; ++i)
	{
		res += vfmt("{:02X} ", address[i]);
	}
	LogInfo(res);
}

