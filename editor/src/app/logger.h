#pragma once

#include <format>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

inline std::shared_ptr<spdlog::logger>& getLogger()
{
	static std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("log", "__TceLog.log", std::ios_base::trunc);
	return logger; 
}

template<typename T>
inline void mlogger(const T& str)
{
	auto& logger = getLogger();
	logger->info(str);
	logger->flush();
}

template<typename... T>
inline void mlogger(std::format_string<T...> fmt, T&&... args)
{
	auto& logger = getLogger();
	logger->info(std::format(fmt, std::forward<T>(args)...));
	logger->flush();
}

inline void logPtrBytes(u8* address, size_t len)
{
	std::string res;

	for (int i = 0; i < len; ++i)
	{
		res += std::format("{:02X} ", address[i]);
	}
	mlogger(res);
}

