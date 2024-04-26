#include "FileHandler.h"


void FileHandler::fill_files_vec(std::filesystem::path path)
{
    files_vec.clear();

    namespace stdfs = std::filesystem;

    if (!stdfs::exists(path) || !stdfs::is_directory(path))
    {
        invalid_path = true;
        return;
    }

    const stdfs::directory_iterator end{};

    for (const auto& entry : stdfs::directory_iterator{ path })
    {
        if (stdfs::is_regular_file(entry) && (entry.path().extension() == ".xml" || entry.path().extension() == ".dat"))
        {
            files_vec.push_back({ entry.path().filename().string(), entry.path()});
        }
    }
    invalid_path = false;
}

bool FileHandler::invalid_path_check()
{
    return invalid_path;
}

std::vector<std::pair<std::string, std::filesystem::path>>& FileHandler::get_files_vec()
{
    return files_vec;
}
