#include <thread>
#include <shobjidl.h>
#include <mutex>

#include "FileListUI.h"
#include "logger.h"


FileListUI::FileListUI()
{
    std::memset(buff, 0, 255);
    strcpy_s(buff, Preload::Get()->getConfigParser()->GetString("Settings","Default_path","E:\\GTAV\\timecycles").c_str());
}


void FileListUI::file_section()
{
    ImGui::InputText("path", buff, sizeof(buff));
    
    if (ImGui::Button("Check files"))
    {
        check_files_btn_foo(buff);
    }

    ImGui::BeginChild("FilesWindow", ImVec2(-FLT_MIN, 140), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY);

    if (check_if_check_files_btn_was_pressed)
    {
        fill_file_sel_window();
    }

    ImGui::EndChild();

    if (ImGui::Button("Load selected"))
    {
        if (!fhandler.get_files_vec().empty())
        {
            LoadSelectedBtn();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Save to selected"))
    {
        if (!fhandler.get_files_vec().empty())
        {
            SaveBtn();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Save as"))
    {
        std::thread th([this]()
            {
                SaveAsBtn();
            });            

        th.detach();  
    }
}


void FileListUI::check_files_btn_foo(char* buff)
{
    m_path.assign(buff);
    fhandler.fill_files_vec(m_path);
    check_if_check_files_btn_was_pressed = true;
}


void FileListUI::fill_file_sel_window()
{
    if (fhandler.invalid_path_check())
    {
        ImGui::Text("Invalid directory path");
        return;
    }

    if (fhandler.get_files_vec().empty())
    {
        ImGui::Text("No files found");
        return;
    }

    static size_t i;
    auto& char_files_vec = fhandler.get_files_vec();

    for (i = 0; i < char_files_vec.size(); i++)
    {
        ImGui::RadioButton(char_files_vec[i].first.c_str(), &selected_radio_btn, i);
    }
}


std::filesystem::path FileListUI::getPathOfSelectedFile()
{
    return fhandler.get_files_vec().empty() ? std::filesystem::path() : fhandler.get_files_vec()[selected_radio_btn].second;
}

std::filesystem::path FileListUI::GetCurrentSelectedFileName()
{
    return fhandler.get_files_vec().empty() ? std::filesystem::path() : fhandler.get_files_vec()[selected_radio_btn].first;
}


std::filesystem::path FileListUI::selectPathForSaveAsBtn()   
{
    std::filesystem::path fPath;
   
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    IFileSaveDialog* pFileSave;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, (void**)&pFileSave);

    if (SUCCEEDED(hr)) 
    {
        DWORD dwFlags;
        pFileSave->GetOptions(&dwFlags);
        pFileSave->SetOptions(dwFlags | FOS_OVERWRITEPROMPT);

        COMDLG_FILTERSPEC filters[] = { { L"XML Files", L"*.xml; *.dat"} };
        pFileSave->SetFileTypes(1, filters);
        pFileSave->SetFileName(L"default");

        hr = pFileSave->Show(NULL); 

        if (SUCCEEDED(hr)) 
        {
            IShellItem* pItem;
            hr = pFileSave->GetResult(&pItem);

            if (SUCCEEDED(hr)) 
            {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                if (SUCCEEDED(hr)) 
                {
                    fPath = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileSave->Release();
    }
    CoUninitialize();
   
    if (!fPath.empty())
        fPath.replace_extension("xml");

    return fPath;
}
