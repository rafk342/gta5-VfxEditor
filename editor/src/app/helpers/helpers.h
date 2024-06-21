#pragma once
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <format>
#include <set>
#include <iomanip>
#include "common/types.h"
#include <mutex>

std::vector<float>          convert_str_to_float_arr        (const std::string& str, int size);
std::string                 strip_str                       (const std::string& str);
std::vector<std::string>    split_string                    (const std::string& input, const std::string& delimiters, u16 expected_vec_size = 16);


template <typename T>
void RemoveDuplicatesInVector(std::vector<T>& vec)
{
    std::set<T> values;
    vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const T& value) { return !values.insert(value).second; }), vec.end());
}

template<typename... T>
const char* vfmt(std::format_string<T...> fmt, T&&... args)
{
	static std::mutex mtx;
	thread_local static char tls_buff[0x100];
	std::unique_lock lock(mtx);
	std::memset(tls_buff, 0, sizeof(tls_buff));
	std::format_to(tls_buff, fmt, std::forward<T>(args)...);
	return tls_buff;
}

