#pragma once
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <format>
#include <set>
#include <iomanip>


using std::vector;

bool                        check_str_ending                (const std::string& fullstr, const std::string& ending);
bool                        check_str_ending                (const char* fullstr, const char* ending);
std::string                 remove_str_last_2_symb          (std::string string);
vector<float>               convert_str_to_float_arr        (const std::string& str, int size);
std::string                 convert_float_arr_to_str        (vector<float>& arr);
void                        replace_symb                    (std::string& str, char symb1, char symb2);
std::string                 strip_str                       (const std::string& str);
std::vector<const char*>    convert_str_to_char_vec         (std::vector<std::string>& vec);
std::vector<std::string>    split_string                    (const std::string& input, const std::string& delimiters);


template <class T>
void RemoveDuplicatesInVector(std::vector<T>& vec)
{
    std::set<T> values;
    vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const T& value) { return !values.insert(value).second; }), vec.end());
}
