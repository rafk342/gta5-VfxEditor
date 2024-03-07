#include "FileHandler.h"


void FileHandler::set_files_map(std::filesystem::path wpath, std::string extension)
{
    files_vec.clear();

    namespace stdfs = std::filesystem;

    if (!stdfs::exists(wpath) || !stdfs::is_directory(wpath))
    {
        invalid_path = true;
        return;
    }

    const stdfs::directory_iterator end{};

    for (const auto& entry : stdfs::directory_iterator{ wpath })
    {
        if (stdfs::is_regular_file(entry) && entry.path().extension() == extension)
        {
            files_vec.push_back({ entry.path().filename().string(), entry.path().string()});
        }
    }
    invalid_path = false;
}

bool FileHandler::invalid_path_check()
{
    return invalid_path;
}

std::vector<std::pair<std::string, std::string>>& FileHandler::get_files_vec()
{
    return files_vec;
}
