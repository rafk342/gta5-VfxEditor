#pragma once

#include <vector>
#include <filesystem>
#include <unordered_map>
#include <fstream>

#include "memory/address.h"
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


struct gVisualSettings
{
	struct gSettingsItem
	{
		u32 hash;
		float value;
	};

	bool v1;
	atArray<gSettingsItem> data;
};


class VisualSettingsHandler;

struct VScontainer
{	
	struct VSitem
	{
		const char*		labelName;
		const char*		paramName;
		u32				hash;
		Vs_VarType_e	vType;
		bool			found = false;

		gVisualSettings::gSettingsItem* gPtrItem = nullptr;
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
	gVisualSettings*	p_Vsettings = nullptr;

	void RestoreFuncBytes();
	void SetFuncN_Bytes();
public:
	VScontainer			mContainer;

	VisualSettingsHandler();
	~VisualSettingsHandler();
	
	void importData(std::string srcPath);
	void exportData(std::string path);
	bool updateData();

};

