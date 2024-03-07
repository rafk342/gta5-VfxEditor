#include <thread>
#include <shobjidl.h>
#include <mutex>

#include "FileListUI.h"

char FileListUI::pre_buff[255];

void FileListUI::setPreBuff(const std::string& str)
{
    strcat_s(pre_buff, str.c_str());
}


FileListUI::FileListUI()
{
    strcat_s(this->buff, FileListUI::pre_buff);
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
        if (!files_vec.empty())
        {
            LoadSelectedBtn();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Save to selected"))
    {
        if (!files_vec.empty())
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


void FileListUI::check_files_btn_foo(char* buf)
{
    path.assign(buf);

    fhandler.set_files_map(path, ".xml");
    files_vec = fhandler.get_files_vec();
    
    convert_str_pair_to_char();

    check_if_check_files_btn_was_pressed = true;
}


void FileListUI::fill_file_sel_window()
{
    if (fhandler.invalid_path_check())
    {
        ImGui::Text("Invalid directory path");
        return;
    }

    if (files_vec.empty())
    {
        ImGui::Text("No files found");
        return;
    }

    static size_t i;
    for (i = 0; i < files_vec.size(); i++)
    {
        ImGui::RadioButton(char_files_vec[i].first, &selected_radio_btn, i);
    }
}


std::string FileListUI::getPathOfSelectedFile()
{
    return files_vec[selected_radio_btn].second;
}

std::string FileListUI::GetCurrentSelectedFileName()
{
    return files_vec[selected_radio_btn].first;
}


std::string FileListUI::selectPathForSaveAsBtn()   
{
    //idk what's going on here

    std::string strFilePath = ""; 

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    IFileSaveDialog* pFileSave;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, (void**)&pFileSave);

    if (SUCCEEDED(hr)) 
    {
        DWORD dwFlags;
        pFileSave->GetOptions(&dwFlags);
        pFileSave->SetOptions(dwFlags | FOS_OVERWRITEPROMPT);
        
        COMDLG_FILTERSPEC filters[] = { { L"XML", L"*.xml" } };
        pFileSave->SetFileTypes(1, filters);

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
                    std::wstring wstrFilePath(pszFilePath);
                    std::string str(wstrFilePath.begin(), wstrFilePath.end());
                    strFilePath = str;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileSave->Release();
    }
    CoUninitialize();
    
    if (!check_str_end(strFilePath, ".xml"))
    {
        strFilePath += ".xml";
    }
    return strFilePath;
}


void FileListUI::convert_str_pair_to_char()
{
    char_files_vec.clear();
    for (size_t i = 0; i < files_vec.size(); i++)
    {
        char_files_vec.push_back({ files_vec[i].first.c_str(), files_vec[i].second.c_str() });
    }
}
