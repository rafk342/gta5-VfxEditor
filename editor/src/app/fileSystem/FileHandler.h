#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <map>


class FileHandler
{
	//map filename -> path
	std::vector<std::pair<std::string, std::string>> files_vec;
	std::unordered_map<std::string, std::string> files;
	bool invalid_path;

public:
	std::vector<std::pair<std::string, std::string>>& get_files_vec();

	void set_files_map(std::filesystem::path wpath, std::string extension);
	std::unordered_map<std::string, std::string>& get_files_map();
	bool invalid_path_check();
};

