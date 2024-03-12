#pragma once
#include <vector>
#include "memory/address.h"
#include <unordered_map>
#include "atl/atArray.h"
#include "crypto/joaat.h"

#include "logger.h"

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
	bool v1;
	atArray<gSettingsItem> data;
};


class VisualSettingsHandler;

struct VScontainer
{	
	struct VSitem
	{
		const char*		name;
		u32				hash;
		float			value;
		Vs_VarType_e	vType;
		bool			found = false;
		gSettingsItem*	gPtrItem = nullptr;
	};

	//				 category   ->   vec : param_name / value
	std::unordered_map<const char*, std::vector<VSitem>> paramsMap;
	std::vector<const char*> categoriesOrder;

	void initContainer(VisualSettingsHandler* handler);
};


class VisualSettingsHandler
{	
	friend class VScontainer;
	gVisualSettings*	p_Vsettings = nullptr;

public:
	VScontainer			mContainer;

	VisualSettingsHandler();
	~VisualSettingsHandler();
	bool updateData();

};

