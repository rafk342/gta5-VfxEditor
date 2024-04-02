#pragma once

#include <vector>
#include <filesystem>
#include <unordered_map>
#include <fstream>

#include "memory/address.h"
#include "memory/hook.h"

#include "atl/atArray.h"
#include "crypto/joaat.h"
#include "logger.h"
#include "helpers/SimpleTimer.h"
#include "helpers/helpers.h"

enum Vs_VarType_e
{
	V_FLOAT,
	V_COL3,
	V_NONE
};

struct gSettingsItem
{
	u32 hash;
	float value;
};

struct gVisualSettings
{
	bool isLoaded;
	atArray<gSettingsItem> data;
};

class VisualSettingsHandler;
struct VScontainer
{	
	struct VSitem
	{
		const char*		 labelName;
		const char*		 paramName;
		u32				 hash;
		Vs_VarType_e	 vType;
		gSettingsItem*   gPtrItem = nullptr;
		
		bool			 found = false;
		bool			 is_used = false;
	};

	//				 category   ->	vec (VSitem)
	std::unordered_map<const char*, std::vector<VSitem>> paramsMap;
	std::vector<const char*> categoriesOrder;

	void initContainer(VisualSettingsHandler* handler);
	void updateContainer(VisualSettingsHandler* handler);
	void clearContainer();
};


class VisualSettingsHandler
{	
	friend struct VScontainer;
	gVisualSettings* p_Vsettings = nullptr;

	void RestoreFuncBytes();
	void SetFuncN_Bytes();
	void getUsedParamNames();
public:
	VScontainer	mContainer;

	VisualSettingsHandler();
	~VisualSettingsHandler();
	
	void importData(std::string srcPath);
	void exportData(std::string path);
	bool updateData();

};


struct VsItemTmp
{
	std::string name;
	u32 hash;
	bool already_exists_in_container = false;

	VsItemTmp(std::string name, u32 hash) : name(name), hash(hash) {};
	VsItemTmp(std::string name) : name(name), hash(rage::joaat(name.c_str())) {};
};