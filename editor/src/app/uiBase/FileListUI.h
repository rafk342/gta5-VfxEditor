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

	FileHandler		fhandler;
	
	static char		pre_buff[255];
	char			buff[255];
	std::string		path;
	bool			check_if_check_files_btn_was_pressed = false;
	int				selected_radio_btn = 0;
	wchar_t			filename[MAX_PATH]{};
	
	//filename -> path  
	std::vector<std::pair<std::string, std::string>>&	files_vec = fhandler.get_files_vec();
	std::vector<std::pair<const char*, const char*>>	char_files_vec;

	void			check_files_btn_foo(char* buf);
	void			fill_file_sel_window();
	void			convert_str_pair_to_char();

protected:
	
	FileListUI();

	void			file_section();
	std::string		GetCurrentSelectedFileName();
	std::string		getPathOfSelectedFile();
	std::string		selectPathForSaveAsBtn();

	virtual void	LoadSelectedBtn() = 0;
	virtual void	SaveAsBtn() = 0;
	virtual void	SaveBtn() = 0;

public:

	static void		setPreBuff(const std::string& str);

};
