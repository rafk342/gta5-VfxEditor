#include "Preload_Integration.h"

 std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> CategoriesIntegrated::CategoriesMapIngr;
 std::vector<std::string> CategoriesIntegrated::CategoryNames;



void Preload_Integration::Preload()
{
	configHandler::readCfg();
	CategoriesHandler::LoadCategories();
	TooltipsHandler::load_tooltips();

	handlePreload();
}

void Preload_Integration::handlePreload()
{
	if (config_params::categories)
	{
		prepare_Categories_For_Usage();
	}
	if (config_params::replace_item_names_with_tooltips_definition)
	{
		load_Tooltips_As_Param_Names();
	}
}

////					category name -> pair (param_label , varId)
//std::unordered_map<std::string, std::pair<std::string, int>> CategoriesMapIngr;

void Preload_Integration::prepare_Categories_For_Usage()
{
	fillNewCategoriesMap();
	findNamesWhichAreNotInCategories();
}

void Preload_Integration::findNamesWhichAreNotInCategories()
{
	bool found;

	for (size_t i = 0; i < TIMECYCLE_VAR_COUNT; i++)
	{
		if (g_varInfos[i].labelName == "None")
		{
			continue;
		}
		found = false;

		for (auto& category : CategoryNames)
		{
			for (auto& [name, id] : CategoriesMapIngr.at(category))
			{
				if (g_varInfos[i].labelName == name)
				{
					found = true;
				}
			}
			if (found) 
			{
				break;
			}
		}
		if (!found)
		{
			CategoriesMapIngr["None"].push_back({ g_varInfos[i].labelName, i });
		}
	}
	if (CategoriesMapIngr.contains("None"))
	{
		if (CategoryNames.back() != "None")
		{
			CategoryNames.push_back("None");
		}
	}
}


void Preload_Integration::fillNewCategoriesMap()
{
	CategoryNames = CategoriesHandler::get_categories_names();
	auto& PrevMap = CategoriesHandler::get_categories_map();

	for (auto& category : CategoryNames)
	{
		for (auto& prev_name : PrevMap.at(category))
		{
			int index = FindTcIndexByName(prev_name);
			if (index != -1)
			{
				CategoriesMapIngr[category].push_back({ prev_name, index });
			}
		}
	}
}


int Preload_Integration::FindTcIndexByName(std::string& name)
{
	for (size_t i = 0; i < TIMECYCLE_VAR_COUNT; i++)
	{
		if (g_varInfos[i].labelName == "None")
		{
			continue;
		}
		if (name == g_varInfos[i].labelName)
		{
			return g_varInfos[i].varId;
		}
	}
	return -1;
}


void Preload_Integration::load_Tooltips_As_Param_Names()
{
	auto& tooltips_map = TooltipsHandler::get_tooltip_map();

	for (size_t i = 0; i < TIMECYCLE_VAR_COUNT; i++)
	{
		if (g_varInfos[i].varType == VARTYPE_NONE)
		{
			continue;
		}
		if (tooltips_map.contains(g_varInfos[i].labelName))
		{
			g_varInfos[i].labelName = tooltips_map.at(g_varInfos[i].labelName).c_str();
		}
	}
}

#if debug0
void CategoriesIntegrated::print_debug_data()
{
	for (auto& c_name : CategoryNames)
	{
		std::cout << "\n\n\n" << c_name << "\n\n\n";

		for (auto& [var_name, id] : CategoriesMapIngr.at(c_name))
		{
			std::cout << var_name << std::endl;
		}
	}
}
#endif

std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>& CategoriesIntegrated::getCategoriesMap()
{
	return CategoriesMapIngr;
}

std::vector<std::string>& CategoriesIntegrated::getNamesVec()
{
	return CategoryNames;
}
