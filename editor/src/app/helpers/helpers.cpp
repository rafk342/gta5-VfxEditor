#include "helpers.h"



std::vector<float> convert_str_to_float_arr(const std::string& str , int size)
{
    std::vector<float> arr;
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
            arr.push_back(0);
        }
    }
    
    return arr;
}


std::string strip_str(const std::string& str)
{
	if (str.empty())
		return str;

	size_t start_pos = 0;
	size_t end_pos = str.size() - 1;

	while (start_pos < str.size() && std::isspace(str[start_pos]))
		++start_pos;

	while (end_pos > start_pos && std::isspace(str[end_pos]))
		--end_pos;

	return str.substr(start_pos, end_pos - start_pos + 1);
}


std::vector<std::string> split_string(const std::string& input, const std::string& delimiters, u16 expected_vec_size) {
    std::vector<std::string> elements;
    std::size_t start = 0, end = 0;
    elements.reserve(expected_vec_size);

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

