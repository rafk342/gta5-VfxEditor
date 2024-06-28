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

namespace details
{
	template<size_t buff_sz>
	struct vfmt_helper
	{
		template<typename... T>
		static const char* _vfmt(std::format_string<T...> fmt, T&&... args)
		{
			static std::recursive_mutex mtx;
			thread_local static char tls_buff[buff_sz];
			std::unique_lock lock(mtx);
			std::memset(tls_buff, 0, std::size(tls_buff));
			std::format_to(tls_buff, fmt, std::forward<T>(args)...);
			tls_buff[buff_sz - 1] = '\0';
			return tls_buff;
		}
	};
}

template<typename... T>
const char* vfmt(std::format_string<T...> fmt, T&&... args) {
	return details::vfmt_helper<size_t(0x100)>::_vfmt(fmt, std::forward<T>(args)...);
}

template<typename... T>
const char* vfmt512(std::format_string<T...> fmt, T&&... args) {
	return details::vfmt_helper<size_t(0x200)>::_vfmt(fmt, std::forward<T>(args)...);
}

template<typename... T>
const char* vfmt1024(std::format_string<T...> fmt, T&&... args) {
	return details::vfmt_helper<size_t(0x400)>::_vfmt(fmt, std::forward<T>(args)...);
}
