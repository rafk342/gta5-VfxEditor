#pragma once
#include <unordered_map>

#include "Preload.h"
#include "game/timecycle/tcData.h"
#include "game/timecycle/tcdef.h"



class CategoriesIntegrated
{
protected:
	//					category name -> vec ( pair (param_label , varId))
	static std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> CategoriesMapIngr;
	static std::vector<std::string> CategoryNames;

public:

	static std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>& getCategoriesMap();
	static std::vector<std::string>& getNamesVec();

};

class Preload_Integration : CategoriesIntegrated
{
	static void prepare_Categories_For_Usage();
	static void findNamesWhichAreNotInCategories();
	static void fillNewCategoriesMap();
	static int FindTcIndexByName(std::string& name);
	static void load_Tooltips_As_Param_Names();
	static void handlePreload();

public:

	static void Preload();

};


