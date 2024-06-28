#pragma once
#include <string>
#include <iostream>
#include "Windows.h"
#include <iostream>

#include "ImGui/imgui.h"
#include "app/fileSystem/FileHandler.h"
#include "app/helpers/helpers.h"
#include "Preload/Preload.h"

class FileListUI
{
private:

	FileHandler						m_fhandler;
	char							buff[256];
	std::filesystem::path			m_path;
	bool							m_check_if_check_files_btn_was_pressed = false;
	s32								m_selected_radio_btn = 0;
	void							check_files_btn_foo(char* buff);
	void							fill_file_sel_window();

protected:
	
	FileListUI();

	void							file_section();
	std::filesystem::path			GetCurrentSelectedFileName();
	std::filesystem::path			getPathOfSelectedFile();
	std::filesystem::path			selectPathForSaveAsBtn();

	virtual void					LoadSelectedBtn() = 0;
	virtual void					SaveAsBtn() = 0;
	virtual void					SaveBtn() = 0;

public:
};
