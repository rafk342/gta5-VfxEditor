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
	std::vector<std::pair<std::string, std::filesystem::path>> files_vec;
	bool invalid_path = false;

public:

	std::vector<std::pair<std::string, std::filesystem::path>>& get_files_vec();
	void fill_files_vec(std::filesystem::path wpath);
	bool invalid_path_check();
};
