#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <map>


class FileHandler
{
	//vec filename -> path
	std::vector<std::pair<std::wstring, std::filesystem::path>> files_vec;
	bool invalid_path = false;
	bool new_vec_requested = false;
public:
	std::vector<std::pair<std::wstring, std::filesystem::path>>& get_files_vec();
	std::vector<std::string>& to_string_file_names_vec();

	void fill_files_vec(std::filesystem::path wpath);
	bool invalid_path_check();
};
