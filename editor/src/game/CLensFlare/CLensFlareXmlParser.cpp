#include "CLensFlare.h"
#include "CLensFlareXmlParser.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				Import
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CLensFlareXmlParser::ImportData(std::filesystem::path path, CLensFlareSettings& settings)
{
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.c_str());
	if (!res)
		return;

	pugi::xml_node root = doc.first_child();
	if (std::string_view(root.name()) != "CLensFlareSettings")
		return;

	auto arrFlareTypeFX_node = root.child("arrFlareTypeFX");
	auto arrFlareFX_node = arrFlareTypeFX_node.child("Item").child("arrFlareFX");

	atArray<CFlareFX>& arr = settings.m_arrFlareFX;
	arr.clear();
	for (auto Item_node : arrFlareFX_node.children())
	{
		CFlareFX& item = arr.push_back(CFlareFX());
		item.m_fDistFromLight		= Item_node.child("fDistFromLight").attribute("value").as_float();
		item.m_fSize				= Item_node.child("fSize").attribute("value").as_float();
		item.m_fWidthRotate			= Item_node.child("fWidthRotate").attribute("value").as_float();
		item.m_fScrollSpeed			= Item_node.child("fScrollSpeed").attribute("value").as_float();
		item.m_fDistanceInnerOffset = Item_node.child("fDistanceInnerOffset").attribute("value").as_float();
		item.m_fIntensityScale		= Item_node.child("fIntensityScale").attribute("value").as_float();
		item.m_fIntensityFade		= Item_node.child("fIntensityFade").attribute("value").as_float();
		item.m_fAnimorphicScaleFactorU = Item_node.child("fAnimorphicScaleFactorU").attribute("value").as_float();
		item.m_fAnimorphicScaleFactorV = Item_node.child("fAnimorphicScaleFactorV").attribute("value").as_float();
		item.m_bAnimorphicBehavesLikeArtefact = Item_node.child("bAnimorphicBehavesLikeArtefact").attribute("value").as_bool();
		item.m_bAlignRotationToSun	= Item_node.child("bAlignRotationToSun").attribute("value").as_bool();
		item.m_color				= Item_node.child("color").attribute("value").as_uint();
		item.m_fGradientMultiplier	= Item_node.child("fGradientMultiplier").attribute("value").as_float();
		item.m_nTexture				= Item_node.child("nTexture").attribute("value").as_uint();
		item.m_nSubGroup			= Item_node.child("nSubGroup").attribute("value").as_uint();
		item.m_fPositionOffsetU		= Item_node.child("fPositionOffsetU").attribute("value").as_float();
		item.m_fTextureColorDesaturate = Item_node.child("fTextureColorDesaturate").attribute("value").as_float();
	}

	settings.m_fSunVisibilityFactor			= root.child("fSunVisibilityFactor").attribute("value").as_float();
	settings.m_iSunVisibilityAlphaClip		= root.child("iSunVisibilityAlphaClip").attribute("value").as_int();
	settings.m_fSunFogFactor				= root.child("fSunFogFactor").attribute("value").as_float();
	settings.m_fChromaticTexUSize			= root.child("fChromaticTexUSize").attribute("value").as_float();
	settings.m_fExposureScaleFactor			= root.child("fExposureScaleFactor").attribute("value").as_float();
	settings.m_fExposureIntensityFactor		= root.child("fExposureIntensityFactor").attribute("value").as_float();
	settings.m_fExposureRotationFactor		= root.child("fExposureRotationFactor").attribute("value").as_float();
	settings.m_fChromaticFadeFactor			= root.child("fChromaticFadeFactor").attribute("value").as_float();
	settings.m_fArtefactDistanceFadeFactor	= root.child("fArtefactDistanceFadeFactor")	.attribute("value").as_float();
	settings.m_fArtefactRotationFactor		= root.child("fArtefactRotationFactor").attribute("value").as_float();
	settings.m_fArtefactScaleFactor			= root.child("fArtefactScaleFactor").attribute("value").as_float();
	settings.m_fArtefactGradientFactor		= root.child("fArtefactGradientFactor")	.attribute("value").as_float();
	settings.m_fCoronaDistanceAdditionalSize = root.child("fCoronaDistanceAdditionalSize").attribute("value").as_float();
	settings.m_fMinExposureScale			= root.child("fMinExposureScale").attribute("value").as_float();
	settings.m_fMaxExposureScale			= root.child("fMaxExposureScale").attribute("value").as_float();
	settings.m_fMinExposureIntensity		= root.child("fMinExposureIntensity").attribute("value").as_float();
	settings.m_fMaxExposureIntensity		= root.child("fMaxExposureIntensity").attribute("value").as_float();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				Export
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CLensFlareXmlParser::ExportData(std::filesystem::path path,CLensFlareSettings& settings)
{
	pugi::xml_document doc;
	pugi::xml_node decl = doc.append_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "UTF-8";

	pugi::xml_node CLensFlareSettings_node = doc.append_child("CLensFlareSettings");
	pugi::xml_node arrFlareTypeFX_node = CLensFlareSettings_node.append_child("arrFlareTypeFX");
	pugi::xml_node Item_node = arrFlareTypeFX_node.append_child("Item");
	pugi::xml_node arrFlareFX_node = Item_node.append_child("arrFlareFX");

	for (CFlareFX& item : settings.m_arrFlareFX)
	{
		pugi::xml_node EmptyItem_node = AppendItemNode(arrFlareFX_node);
		InitEmptyItemNode(EmptyItem_node,item);
	}

	AppendCommonSettings(CLensFlareSettings_node, settings);

	doc.save_file(path.c_str());
}



void CLensFlareXmlParser::AppendCommonSettings(pugi::xml_node root, CLensFlareSettings& settings)
{
	root.append_child("fSunVisibilityFactor").append_attribute("value").set_value(settings.m_fSunVisibilityFactor);
	root.append_child("iSunVisibilityAlphaClip").append_attribute("value").set_value(settings.m_iSunVisibilityAlphaClip);
	root.append_child("fSunFogFactor").append_attribute("value").set_value(settings.m_fSunFogFactor);
	root.append_child("fChromaticTexUSize").append_attribute("value").set_value(settings.m_fChromaticTexUSize);
	root.append_child("fExposureScaleFactor").append_attribute("value").set_value(settings.m_fExposureScaleFactor);
	root.append_child("fExposureIntensityFactor").append_attribute("value").set_value(settings.m_fExposureIntensityFactor);
	root.append_child("fExposureRotationFactor").append_attribute("value").set_value(settings.m_fExposureRotationFactor);
	root.append_child("fChromaticFadeFactor").append_attribute("value").set_value(settings.m_fChromaticFadeFactor);
	root.append_child("fArtefactDistanceFadeFactor").append_attribute("value").set_value(settings.m_fArtefactDistanceFadeFactor);
	root.append_child("fArtefactRotationFactor").append_attribute("value").set_value(settings.m_fArtefactRotationFactor);
	root.append_child("fArtefactScaleFactor").append_attribute("value").set_value(settings.m_fArtefactScaleFactor);
	root.append_child("fArtefactGradientFactor").append_attribute("value").set_value(settings.m_fArtefactGradientFactor);
	root.append_child("fCoronaDistanceAdditionalSize").append_attribute("value").set_value(settings.m_fCoronaDistanceAdditionalSize);
	root.append_child("fMinExposureScale").append_attribute("value").set_value(settings.m_fMinExposureScale);
	root.append_child("fMaxExposureScale").append_attribute("value").set_value(settings.m_fMaxExposureScale);
	root.append_child("fMinExposureIntensity").append_attribute("value").set_value(settings.m_fMinExposureIntensity);
	root.append_child("fMaxExposureIntensity").append_attribute("value").set_value(settings.m_fMaxExposureIntensity);
}

pugi::xml_node CLensFlareXmlParser::AppendItemNode(pugi::xml_node Array_node)
{
	pugi::xml_node Item = Array_node.append_child("Item");

	Item.append_child("fDistFromLight").append_attribute("value").set_value(0.0f);
	Item.append_child("fSize").append_attribute("value").set_value(0.0f);
	Item.append_child("fWidthRotate").append_attribute("value").set_value(0.0f);
	Item.append_child("fScrollSpeed").append_attribute("value").set_value(0.0f);
	Item.append_child("fDistanceInnerOffset").append_attribute("value").set_value(0.0f);
	Item.append_child("fIntensityScale").append_attribute("value").set_value(0.0f);
	Item.append_child("fIntensityFade").append_attribute("value").set_value(0.0f);
	Item.append_child("fAnimorphicScaleFactorU").append_attribute("value").set_value(0.0f);
	Item.append_child("fAnimorphicScaleFactorV").append_attribute("value").set_value(0.0f);
	Item.append_child("bAnimorphicBehavesLikeArtefact").append_attribute("value").set_value(false);
	Item.append_child("bAlignRotationToSun").append_attribute("value").set_value(false);
	Item.append_child("color").append_attribute("value").set_value("0xFFFFFFFF");
	Item.append_child("fGradientMultiplier").append_attribute("value").set_value(0.0f);
	Item.append_child("nTexture").append_attribute("value").set_value(0.0f);
	Item.append_child("nSubGroup").append_attribute("value").set_value(0.0f);
	Item.append_child("fPositionOffsetU").append_attribute("value").set_value(0.0f);
	Item.append_child("fTextureColorDesaturate").append_attribute("value").set_value(0.0f);
	return Item;

}

void CLensFlareXmlParser::InitEmptyItemNode(pugi::xml_node Node, CFlareFX& item)
{
	FlareFxTextureType_e currTexture = static_cast<FlareFxTextureType_e>(item.m_nTexture);
	Node.child("nTexture").attribute("value").set_value(currTexture);

	switch (currTexture)
	{
	case AnimorphicFx:
		InitAnimorphicFxNode(Node, item);
		break;

	case ArtefactFx:
		InitArtefactFxNode(Node, item);
		break;

	case ChromaticFx:
		InitChromaticFxNode(Node, item);
		break;

	case CoronaFx:
		InitCoronaFxNode(Node, item);
		break;

	default:
		break;
	}
}

void CLensFlareXmlParser::InitAnimorphicFxNode(pugi::xml_node Item_node, CFlareFX& item)
{
	Item_node.child("bAnimorphicBehavesLikeArtefact").attribute("value").set_value(item.m_bAnimorphicBehavesLikeArtefact);
	Item_node.child("fDistFromLight").attribute("value").set_value(item.m_fDistFromLight);
	Item_node.child("fSize").attribute("value").set_value(item.m_fSize);
	Item_node.child("fAnimorphicScaleFactorU").attribute("value").set_value(item.m_fAnimorphicScaleFactorU);
	Item_node.child("fAnimorphicScaleFactorV").attribute("value").set_value(item.m_fAnimorphicScaleFactorV);
	Item_node.child("fWidthRotate").attribute("value").set_value(item.m_fWidthRotate);
	Item_node.child("fIntensityScale").attribute("value").set_value(item.m_fIntensityScale);
	Item_node.child("color").attribute("value").set_value(vfmt("0x{:08X}", item.m_color.GetRawInt()));
}

void CLensFlareXmlParser::InitArtefactFxNode(pugi::xml_node Item_node, CFlareFX& item)
{
	Item_node.child("fDistFromLight").attribute("value").set_value(item.m_fDistFromLight);
	Item_node.child("fSize").attribute("value").set_value(item.m_fSize);
	Item_node.child("fAnimorphicScaleFactorU").attribute("value").set_value(item.m_fAnimorphicScaleFactorU);
	Item_node.child("fAnimorphicScaleFactorV").attribute("value").set_value(item.m_fAnimorphicScaleFactorV);
	Item_node.child("fIntensityScale").attribute("value").set_value(item.m_fIntensityScale);
	Item_node.child("bAlignRotationToSun").attribute("value").set_value(item.m_bAlignRotationToSun);
	Item_node.child("color").attribute("value").set_value(vfmt("0x{:08X}", item.m_color.GetRawInt()));
	Item_node.child("fGradientMultiplier").attribute("value").set_value(item.m_fGradientMultiplier);
	Item_node.child("nSubGroup").attribute("value").set_value(item.m_nSubGroup);
	Item_node.child("fPositionOffsetU").attribute("value").set_value(item.m_fPositionOffsetU);

}

void CLensFlareXmlParser::InitChromaticFxNode(pugi::xml_node Item_node, CFlareFX& item)
{
	Item_node.child("fDistFromLight").attribute("value").set_value(item.m_fDistFromLight);
	Item_node.child("fSize").attribute("value").set_value(item.m_fSize);
	Item_node.child("fWidthRotate").attribute("value").set_value(item.m_fWidthRotate);
	Item_node.child("fAnimorphicScaleFactorU").attribute("value").set_value(item.m_fAnimorphicScaleFactorU);
	Item_node.child("fAnimorphicScaleFactorV").attribute("value").set_value(item.m_fAnimorphicScaleFactorV);
	Item_node.child("fScrollSpeed").attribute("value").set_value(item.m_fScrollSpeed);
	Item_node.child("fDistanceInnerOffset").attribute("value").set_value(item.m_fDistanceInnerOffset);
	Item_node.child("fIntensityScale").attribute("value").set_value(item.m_fIntensityScale);
	Item_node.child("fIntensityFade").attribute("value").set_value(item.m_fIntensityFade);
	Item_node.child("color").attribute("value").set_value(vfmt("0x{:08X}", item.m_color.GetRawInt()));
	Item_node.child("fTextureColorDesaturate").attribute("value").set_value(item.m_fTextureColorDesaturate);
}

void CLensFlareXmlParser::InitCoronaFxNode(pugi::xml_node Item_node, CFlareFX& item)
{
	Item_node.child("fDistFromLight").attribute("value").set_value(item.m_fDistFromLight);
	Item_node.child("fSize").attribute("value").set_value(item.m_fSize);
	Item_node.child("fAnimorphicScaleFactorU").attribute("value").set_value(item.m_fAnimorphicScaleFactorU);
	Item_node.child("fAnimorphicScaleFactorV").attribute("value").set_value(item.m_fAnimorphicScaleFactorV);
	Item_node.child("fWidthRotate").attribute("value").set_value(item.m_fWidthRotate);
	Item_node.child("fIntensityScale").attribute("value").set_value(item.m_fIntensityScale);
	Item_node.child("color").attribute("value").set_value(vfmt("0x{:08X}", item.m_color.GetRawInt()));
}
