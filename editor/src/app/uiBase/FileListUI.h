#pragma once
#include <string>
#include <iostream>
#include "Windows.h"
#include <iostream>

#include "ImGui/imgui.h"
#include "app/fileSystem/FileHandler.h"
#include "app/helpers/helpers.h"


class FileListUI
{
private:

	FileHandler						fhandler;
	static char						pre_buff[255];
	char							buff[255];
	std::filesystem::path			m_path;
	bool							check_if_check_files_btn_was_pressed = false;
	s32								selected_radio_btn = 0;
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

	static void						setPreBuff(const std::string& str);

};
