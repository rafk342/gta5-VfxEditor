#include "helpers.h"



bool check_str_ending(const std::string& fullstr, const std::string& ending)
{
    if (fullstr.length() >= ending.length()) {
        return (fullstr.compare(fullstr.length() - ending.length(), ending.length(), ending) == 0);
    } else {
        return false;
    }
}


bool check_str_ending(const char* fullstr, const char* ending)
{
    int fullstr_len = strlen(fullstr);
    int ending_len = strlen(ending);

    if (fullstr_len >= ending_len) {
        return (strcmp(fullstr + fullstr_len - ending_len, ending) == 0);
    } else {
        return false;
    }
}


std::string remove_str_last_2_symb(std::string string) 
{
    if (string.length() < 2) {
        return "";  
    } else {
        string.resize(string.length() - 2);
        return string;  
    }
}



std::string convert_float_arr_to_str(vector<float>& arr) 
{
    std::string temp_str;
    temp_str.reserve(100);
    temp_str += " ";

    for (auto& num : arr) 
        temp_str += std::format("{:.4f} ", num);

    replace_symb(temp_str, ',' , '.');
    return temp_str;
}


vector<float> convert_str_to_float_arr(const std::string& str , int size)
{
    vector<float> arr;
    std::stringstream iss(str);
    float num;

    while (iss >> num)
        arr.push_back(num);

    if (arr.size() > size)
    {
        arr.erase(arr.begin() + size, arr.end());
    }
    if (arr.size() < size)
    {
        for (size_t i = arr.size(); i < size; i++)
        {
            arr.push_back(.0f);
        }
    }
    
    return arr;
}


void replace_symb(std::string& str, char symb1 , char symb2) 
{
    for (char& c : str) 
    {
        if (c == symb1)
        {
            c = symb2;
        }
    }
}

std::string strip_str(const std::string& inpt)
{
    std::string str = inpt;

    if (str.empty())
        return str;

    size_t start_pos = 0;
    size_t end_pos = str.size() - 1;

    while (start_pos < str.size() && std::isspace(str[start_pos])) 
        ++start_pos;

    while (end_pos > start_pos && std::isspace(str[end_pos])) 
        --end_pos;


    str = str.substr(start_pos, end_pos - start_pos + 1);
    return str;
}



std::vector<std::string> split_string(const std::string& input, const std::string& delimiters) {
    std::vector<std::string> elements;
    std::size_t start = 0, end = 0;

    while ((end = input.find_first_of(delimiters, start)) != std::string::npos) {
        if (end != start) {
            elements.push_back(input.substr(start, end - start));
        }
        start = end + 1;
    }

    if (start < input.size()) {
        elements.push_back(input.substr(start));
    }

    return elements;
}


std::vector<const char*> convert_str_to_char_vec(std::vector<std::string>& vec)
{
    std::vector<const char*> temp;

    for (const std::string& str : vec)
        temp.push_back(str.c_str());

    return temp;
}

