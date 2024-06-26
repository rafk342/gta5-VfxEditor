#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <array>
#include <regex>
#include <chrono>

#include "inih/INIReader.h"
#include "app/helpers/helpers.h"
#include "timecycle/tcData.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           TcParameter Names replacement
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class TcNamesReplacemantHandler
{
	// param -> new_name
	std::unordered_map<std::string, std::string> params_map;
	bool loaded = false;

	bool load_from_file(const char* filename);
	void save_hardcoded_params_to_file(const char* filename);
public:
	inline bool isLoaded();
	bool load_names_replacement(const char* filename);
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           TcCategories
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class TcCategoriesHandler
{
	std::vector<std::string> CategoriesOrder;
    //                  category name -> vec ( pair (param_label , varId))
	std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> CategoriesMap;
	bool loaded = false;

	bool load_from_file(const char* filename);
	void save_hardcoded_categories_to_file(const char* filename);

public:

	inline bool isLoaded();
	bool load_categories(const char* filename);
	
	auto& getCategoriesOrder()	{ return CategoriesOrder; }
	auto& getCategoriesMap()	{ return CategoriesMap; }
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           Config
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class ConfigWrapper
{
	INIReader reader;

public:
	ConfigWrapper() = default;
	void readCfg(const char* filename);
	void writeCfg(const char* filename);
	INIReader* getCfgReader();
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           Preload
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class Preload
{
	static Preload* self;
	Preload() = default;
	~Preload() = default;

	struct fileNames
	{
		static constexpr const char* tcNames_repl_file = "__TcEditorNamesReplacement.txt";
		static constexpr const char* tcCategories_file = "__TcEditorCategories.txt";
		static constexpr const char* config_file = "__TcEditorConfig.ini";
	};

	TcNamesReplacemantHandler	TcNames;
	TcCategoriesHandler			TcCategories;
	ConfigWrapper				CfgWrapper;

	bool preload_done = false;

public:

	static Preload& Create();
	static Preload*	Get();
	static void		Destroy();

	void Init();

	TcCategoriesHandler* getTcCategoriesHandler();
	INIReader* getConfigParser();
};