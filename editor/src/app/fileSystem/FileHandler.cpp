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
            files_vec.push_back({ entry.path().filename(), entry.path()});
        }
    }
    new_vec_requested = true;
    invalid_path = false;
}

bool FileHandler::invalid_path_check()
{
    return invalid_path;
}

std::vector<std::pair<std::wstring, std::filesystem::path>>& FileHandler::get_files_vec()
{
    return files_vec;
}

std::vector<std::string>& FileHandler::to_string_file_names_vec()
{
    static std::vector<std::string> tmp;
    if (!new_vec_requested)
        return tmp;
   
    new_vec_requested = false;

    tmp.clear();
    tmp.reserve(files_vec.size());

    for (size_t i = 0; i < files_vec.size(); i++)
    {
        tmp.push_back({ std::string(files_vec[i].first.begin(),files_vec[i].first.end())});
    }

    return tmp;
}
