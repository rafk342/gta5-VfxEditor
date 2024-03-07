#pragma once

#include <string>
#pragma warning(disable : 4996)

namespace mPaths 
{

	std::string getExecutablePath();
	std::string getExecutableDir();
	std::string mergePaths(std::string pathA, std::string pathB);
	bool checkIfFileExists(const std::string& filePath);

}