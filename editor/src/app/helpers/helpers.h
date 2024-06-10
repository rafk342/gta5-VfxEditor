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


std::vector<float>          convert_str_to_float_arr        (const std::string& str, int size);
std::string                 strip_str                       (const std::string& str);
std::vector<std::string>    split_string                    (const std::string& input, const std::string& delimiters, u16 expected_vec_size = 16);


template <class T>
void RemoveDuplicatesInVector(std::vector<T>& vec)
{
    std::set<T> values;
    vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const T& value) { return !values.insert(value).second; }), vec.end());
}
