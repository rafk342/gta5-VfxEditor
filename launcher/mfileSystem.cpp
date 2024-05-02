#include "mfileSystem.h"

#include <windows.h>
#include <Shlwapi.h>
#include <io.h> 

#define access _access_s


namespace mPaths 
{
    std::string getExecutablePath() 
    {
        char rawPathName[MAX_PATH];
        GetModuleFileNameA(NULL, rawPathName, MAX_PATH);
        return std::string(rawPathName);
    }

    std::string getExecutableDir()
    {
        std::string executablePath = getExecutablePath();
        size_t lastSlash = executablePath.find_last_of("\\/");
        return executablePath.substr(0, lastSlash);
    }


    std::string mergePaths(std::string pathA, std::string pathB) 
    {
        char combined[MAX_PATH];
        PathCombineA(combined, pathA.c_str(), pathB.c_str());
        std::string mergedPath(combined);
        return mergedPath;
    }
}