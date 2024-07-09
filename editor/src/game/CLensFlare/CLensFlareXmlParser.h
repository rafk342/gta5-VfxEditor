#pragma once
#include <filesystem>

#include "pugixml/pugixml.hpp"
#include "helpers/helpers.h"

class CLensFlareSettings;
class CFlareFX;
class CLensFlareXmlParser
{

	//enum FlareFxTextureType_e : u8
	//{						 // SubGroups :
	//	AnimorphicFx = 0, // 0
	//	ArtefactFx = 1, // 1-8 
	//	ChromaticFx = 2, // 0
	//	CoronaFx = 3, // 0
	//};

	void AppendCommonSettings(pugi::xml_node root, CLensFlareSettings& settings);
	pugi::xml_node AppendItemNode(pugi::xml_node Array_node);
	void InitEmptyItemNode(pugi::xml_node Node, CFlareFX& item);

	void InitAnimorphicFxNode(pugi::xml_node Item_node, CFlareFX& item);
	void InitArtefactFxNode(pugi::xml_node Item_node, CFlareFX& item);
	void InitChromaticFxNode(pugi::xml_node Item_node, CFlareFX& item);
	void InitCoronaFxNode(pugi::xml_node Item_node, CFlareFX& item);

public:

	void ImportData(std::filesystem::path path, CLensFlareSettings& settings);
	void ExportData(std::filesystem::path path, CLensFlareSettings& settings);

};

