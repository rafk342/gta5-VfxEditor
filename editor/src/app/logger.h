#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "format"

#define mlogger(arg) mlogger_(arg)
inline void mlogger_(std::string str)
{
	static std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("log", "__TceLog.log", std::ios_base::trunc);
	logger->info(str);
	logger->flush();
}
