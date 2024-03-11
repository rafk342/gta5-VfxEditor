#include "vfxLightningXmlParser.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         Import




void VfxLightningsXmlParser::mImportLightningData(std::string path, gVfxLightningSettings* settings)
{
    if (!settings)
        return;
    if (path.empty())
        return;
    
    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load_file(path.c_str());

    if (!res)
        return;

    pugi::xml_node root = doc.first_child();

    if (!(static_cast<std::string>(root.name()) == "VfxLightningSettings"))
        return;

    settings->lightningOccurranceChance = root.child("lightningOccurranceChance").attribute("value").as_int(0);
    settings->lightningShakeIntensity = root.child("lightningShakeIntensity").attribute("value").as_float(0.0f);
    
    auto DirectionalBurstSettingsNode = root.child("DirectionalBurstSettings");
    {
        settings->m_DirectionalBurstSettings.BurstIntensityMin = DirectionalBurstSettingsNode.child("BurstIntensityMin").attribute("value").as_float(0.0f);
        settings->m_DirectionalBurstSettings.BurstIntensityMax = DirectionalBurstSettingsNode.child("BurstIntensityMax").attribute("value").as_float(0.0f);
        settings->m_DirectionalBurstSettings.BurstDurationMin = DirectionalBurstSettingsNode.child("BurstDurationMin").attribute("value").as_float(0.0f);
        settings->m_DirectionalBurstSettings.BurstDurationMax = DirectionalBurstSettingsNode.child("BurstDurationMax").attribute("value").as_float(0.0f);
        settings->m_DirectionalBurstSettings.BurstSeqCount = DirectionalBurstSettingsNode.child("BurstSeqCount").attribute("value").as_int(0);
        settings->m_DirectionalBurstSettings.RootOrbitXVariance = DirectionalBurstSettingsNode.child("RootOrbitXVariance").attribute("value").as_float(0.0f);
        settings->m_DirectionalBurstSettings.RootOrbitYVarianceMin = DirectionalBurstSettingsNode.child("RootOrbitYVarianceMin").attribute("value").as_float(0.0f);
        settings->m_DirectionalBurstSettings.RootOrbitYVarianceMax = DirectionalBurstSettingsNode.child("RootOrbitYVarianceMax").attribute("value").as_float(0.0f);
        settings->m_DirectionalBurstSettings.BurstSeqOrbitXVariance = DirectionalBurstSettingsNode.child("BurstSeqOrbitXVariance").attribute("value").as_float(0.0f);
        settings->m_DirectionalBurstSettings.BurstSeqOrbitYVariance = DirectionalBurstSettingsNode.child("BurstSeqOrbitYVariance").attribute("value").as_float(0.0f);
    }
    auto CloudBurstSettingsNode = root.child("CloudBurstSettings");
    {
        settings->m_CloudBurstSettings.BurstIntensityMin = CloudBurstSettingsNode.child("BurstIntensityMin").attribute("value").as_float(0.0f);
        settings->m_CloudBurstSettings.BurstIntensityMax = CloudBurstSettingsNode.child("BurstIntensityMax").attribute("value").as_float(0.0f);
        settings->m_CloudBurstSettings.LightSourceExponentFalloff = CloudBurstSettingsNode.child("LightSourceExponentFalloff").attribute("value").as_float(0.0f);
        settings->m_CloudBurstSettings.LightDeltaPos = CloudBurstSettingsNode.child("LightDeltaPos").attribute("value").as_float(0.0f);
        settings->m_CloudBurstSettings.LightDistance = CloudBurstSettingsNode.child("LightDistance").attribute("value").as_float(0.0f);
        settings->m_CloudBurstSettings.LightColor = CloudBurstSettingsNode.child("LightColor").attribute("value").as_uint(0);
       
        LoadCloudBurstCommonSettings(settings->m_CloudBurstSettings.m_CloudBurstCommonSettings, CloudBurstSettingsNode.child("CloudBurstCommonSettings"));
    }
    auto StrikeSettingsNode = root.child("StrikeSettings");
    {
        settings->m_StrikeSettings.IntensityMin = StrikeSettingsNode.child("IntensityMin").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.IntensityMax = StrikeSettingsNode.child("IntensityMax").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.IntensityMinClamp = StrikeSettingsNode.child("IntensityMinClamp").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.WidthMin = StrikeSettingsNode.child("WidthMin").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.WidthMax = StrikeSettingsNode.child("WidthMax").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.WidthMinClamp = StrikeSettingsNode.child("WidthMinClamp").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.IntensityFlickerMin = StrikeSettingsNode.child("IntensityFlickerMin").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.IntensityFlickerMax = StrikeSettingsNode.child("IntensityFlickerMax").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.IntensityFlickerFrequency = StrikeSettingsNode.child("IntensityFlickerFrequency").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.IntensityLevelDecayMin = StrikeSettingsNode.child("IntensityLevelDecayMin").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.IntensityLevelDecayMax = StrikeSettingsNode.child("IntensityLevelDecayMax").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.WidthLevelDecayMin = StrikeSettingsNode.child("WidthLevelDecayMin").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.WidthLevelDecayMax = StrikeSettingsNode.child("WidthLevelDecayMax").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.NoiseTexScale = StrikeSettingsNode.child("NoiseTexScale").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.NoiseAmplitude = StrikeSettingsNode.child("NoiseAmplitude").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.NoiseAmpMinDistLerp = StrikeSettingsNode.child("NoiseAmpMinDistLerp").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.NoiseAmpMaxDistLerp = StrikeSettingsNode.child("NoiseAmpMaxDistLerp").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.SubdivisionCount = StrikeSettingsNode.child("SubdivisionCount").attribute("value").as_uint(0);
        settings->m_StrikeSettings.OrbitDirXVariance = StrikeSettingsNode.child("OrbitDirXVariance").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.OrbitDirYVarianceMin = StrikeSettingsNode.child("OrbitDirYVarianceMin").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.OrbitDirYVarianceMax = StrikeSettingsNode.child("OrbitDirYVarianceMax").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.MaxDistanceForExposureReset = StrikeSettingsNode.child("MaxDistanceForExposureReset").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.AngularWidthFactor = StrikeSettingsNode.child("AngularWidthFactor").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.MaxHeightForStrike = StrikeSettingsNode.child("MaxHeightForStrike").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.CoronaIntensityMult = StrikeSettingsNode.child("CoronaIntensityMult").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.BaseCoronaSize = StrikeSettingsNode.child("BaseCoronaSize").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.BaseColor = StrikeSettingsNode.child("BaseColor").attribute("value").as_uint(0);
        settings->m_StrikeSettings.FogColor = StrikeSettingsNode.child("FogColor").attribute("value").as_uint(0);
        settings->m_StrikeSettings.CloudLightIntensityMult = StrikeSettingsNode.child("CloudLightIntensityMult").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.CloudLightDeltaPos = StrikeSettingsNode.child("CloudLightDeltaPos").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.CloudLightRadius = StrikeSettingsNode.child("CloudLightRadius").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.CloudLightFallOffExponent = StrikeSettingsNode.child("CloudLightFallOffExponent").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.MaxDistanceForBurst = StrikeSettingsNode.child("MaxDistanceForBurst").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.BurstThresholdIntensity = StrikeSettingsNode.child("BurstThresholdIntensity").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.LightningFadeWidth = StrikeSettingsNode.child("LightningFadeWidth").attribute("value").as_float(0.0f);
        settings->m_StrikeSettings.LightningFadeWidthFalloffExp = StrikeSettingsNode.child("LightningFadeWidthFalloffExp").attribute("value").as_float(0.0f);

        auto variationsNode = StrikeSettingsNode.child("variations");
        {
            int idx = 0;
            for (auto Item : variationsNode.children())
            {
                if (static_cast<std::string>(Item.name()) != "Item")
                    continue;

                settings->m_StrikeSettings.m_Variations[idx].DurationMin = Item.child("DurationMin").attribute("value").as_float(0.0f);
                settings->m_StrikeSettings.m_Variations[idx].DurationMax = Item.child("DurationMax").attribute("value").as_float(0.0f);
                settings->m_StrikeSettings.m_Variations[idx].AnimationTimeMin = Item.child("AnimationTimeMin").attribute("value").as_float(0.0f);
                settings->m_StrikeSettings.m_Variations[idx].AnimationTimeMax = Item.child("AnimationTimeMax").attribute("value").as_float(0.0f);
                settings->m_StrikeSettings.m_Variations[idx].EndPointOffsetXVariance = Item.child("EndPointOffsetXVariance").attribute("value").as_float(0.0f);
                settings->m_StrikeSettings.m_Variations[idx].SubdivisionPatternMask = Item.child("SubdivisionPatternMask").attribute("value").as_uint(0);

                auto ZigZagSplitPointNode = Item.child("ZigZagSplitPoint");
                {
                    settings->m_StrikeSettings.m_Variations[idx].mZigZagSplitPoint.m_FractionMin = ZigZagSplitPointNode.child("FractionMin").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mZigZagSplitPoint.m_FractionMax = ZigZagSplitPointNode.child("FractionMax").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mZigZagSplitPoint.m_DeviationDecay = ZigZagSplitPointNode.child("DeviationDecay").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mZigZagSplitPoint.m_DeviationRightVariance = ZigZagSplitPointNode.child("DeviationRightVariance").attribute("value").as_float(0.0f);
                }

                auto ForkZigZagSplitPointNode = Item.child("ForkZigZagSplitPoint");
                {
                    settings->m_StrikeSettings.m_Variations[idx].mForkZigZagSplitPoint.m_FractionMin = ForkZigZagSplitPointNode.child("FractionMin").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mForkZigZagSplitPoint.m_FractionMax = ForkZigZagSplitPointNode.child("FractionMax").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mForkZigZagSplitPoint.m_DeviationDecay = ForkZigZagSplitPointNode.child("DeviationDecay").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mForkZigZagSplitPoint.m_DeviationRightVariance = ForkZigZagSplitPointNode.child("DeviationRightVariance").attribute("value").as_float(0.0f);
                }
                
                auto ForkPointNode = Item.child("ForkPoint");
                {
                    settings->m_StrikeSettings.m_Variations[idx].mForkPoint.m_DeviationRightVariance = ForkPointNode.child("DeviationRightVariance").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mForkPoint.m_DeviationForwardMin = ForkPointNode.child("DeviationForwardMin").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mForkPoint.m_DeviationForwardMax = ForkPointNode.child("DeviationForwardMax").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mForkPoint.m_LengthMin = ForkPointNode.child("LengthMin").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mForkPoint.m_LengthMax = ForkPointNode.child("LengthMax").attribute("value").as_float(0.0f);
                    settings->m_StrikeSettings.m_Variations[idx].mForkPoint.m_LengthDecay = ForkPointNode.child("LengthDecay").attribute("value").as_float(0.0f);
                }

                auto KeyFrameData = Item.child("KeyFrameData");
                auto LightningMainIntensity = KeyFrameData.child("LightningMainIntensity");
                if (LightningMainIntensity) 
                {
                    LoadKeyframeData(LightningMainIntensity.child("keyData"), settings->m_StrikeSettings.m_Variations[idx].mKeyFrameData.LightningMainIntensity);
                }
                auto LightningBranchIntensity = KeyFrameData.child("LightningBranchIntensity");
                if (LightningBranchIntensity)
                {
                    LoadKeyframeData(LightningBranchIntensity.child("keyData"), settings->m_StrikeSettings.m_Variations[idx].mKeyFrameData.LightningBranchIntensity);
                }

                idx++;

                if (idx > 3)
                    break;
            }
        }
        LoadCloudBurstCommonSettings(settings->m_StrikeSettings.m_CloudBurstCommonSettings, StrikeSettingsNode.child("CloudBurstCommonSettings"));
    }
    
}

void VfxLightningsXmlParser::LoadCloudBurstCommonSettings(CloudBurstCommonSettings& settings, const pugi::xml_node& node)
{
    settings.BurstDurationMin = node.child("BurstDurationMin").attribute("value").as_float(0.0f);
    settings.BurstDurationMax = node.child("BurstDurationMax").attribute("value").as_float(0.0f);
    settings.BurstSeqCount = node.child("BurstSeqCount").attribute("value").as_uint(0);
    settings.RootOrbitXVariance = node.child("RootOrbitXVariance").attribute("value").as_float(0.0f);
    settings.RootOrbitYVarianceMin = node.child("RootOrbitYVarianceMin").attribute("value").as_float(0.0f);
    settings.RootOrbitYVarianceMax = node.child("RootOrbitYVarianceMax").attribute("value").as_float(0.0f);
    settings.LocalOrbitXVariance = node.child("LocalOrbitXVariance").attribute("value").as_float(0.0f);
    settings.LocalOrbitYVariance = node.child("LocalOrbitYVariance").attribute("value").as_float(0.0f);
    settings.BurstSeqOrbitXVariance = node.child("BurstSeqOrbitXVariance").attribute("value").as_float(0.0f);
    settings.BurstSeqOrbitYVariance = node.child("BurstSeqOrbitYVariance").attribute("value").as_float(0.0f);
    settings.DelayMin = node.child("DelayMin").attribute("value").as_float(0.0f);
    settings.DelayMax = node.child("DelayMax").attribute("value").as_float(0.0f);
    settings.SizeMin = node.child("SizeMin").attribute("value").as_float(0.0f);
    settings.SizeMax = node.child("SizeMax").attribute("value").as_float(0.0f);
}


void VfxLightningsXmlParser::LoadKeyframeData(const pugi::xml_node& keyData, ptxKeyframe& keyframe)
{
    if (!keyData)
        return;

    std::string	raw_text;

    auto keyEntryData = keyData.child("keyEntryData");
    raw_text = keyEntryData.text().get();

    std::istringstream iss(raw_text);
    std::string line;
    std::vector<float> temp;
    
    int	idx = 0;
    while (std::getline(iss, line, '\n') && idx < keyframe.data.GetSize())  //this is wrong, should be remade with the proper atArray impl
    {
        if (strip_str(line).empty()) 
            continue;

        temp = convert_str_to_float_arr(line, 5);
        keyframe.data[idx].vTime[0] = temp[0];
        std::copy(temp.begin() + 1, temp.end(), keyframe.data[idx].vValue);
        idx++;
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         Export


void VfxLightningsXmlParser::mExportLightningData(std::string path, gVfxLightningSettings* settings)
{

    pugi::xml_document doc;

    pugi::xml_node decl = doc.append_child(pugi::xml_node_type::node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";

    pugi::xml_node root = doc.append_child("VfxLightningSettings");

    root.append_child("lightningOccurranceChance").append_attribute("value").set_value(settings->lightningOccurranceChance);
    root.append_child("lightningShakeIntensity").append_attribute("value").set_value(settings->lightningShakeIntensity);

    pugi::xml_node tcLightningDirectionalBurstNode = root.append_child("lightningTimeCycleMods");
    {
        tcLightningDirectionalBurstNode.append_child("tcLightningDirectionalBurst").text() = "lightning";
        tcLightningDirectionalBurstNode.append_child("tcLightningCloudBurst").text() = "lightning_cloud";
        tcLightningDirectionalBurstNode.append_child("tcLightningStrikeOnly").text() = "lightning_weak";
        tcLightningDirectionalBurstNode.append_child("tcLightningDirectionalBurstWithStrike").text() = "lightning_strong";
    }

    pugi::xml_node DirectionalBurstSettingsNode = root.append_child("DirectionalBurstSettings");
    {
        DirectionalBurstSettingsNode.append_child("BurstIntensityMin").append_attribute("value").set_value(settings->m_DirectionalBurstSettings.BurstIntensityMin);
        DirectionalBurstSettingsNode.append_child("BurstIntensityMax").append_attribute("value").set_value(settings->m_DirectionalBurstSettings.BurstIntensityMax);
        DirectionalBurstSettingsNode.append_child("BurstDurationMin").append_attribute("value").set_value(settings->m_DirectionalBurstSettings.BurstDurationMin);
        DirectionalBurstSettingsNode.append_child("BurstDurationMax").append_attribute("value").set_value(settings->m_DirectionalBurstSettings.BurstDurationMax);
        DirectionalBurstSettingsNode.append_child("BurstSeqCount").append_attribute("value").set_value(settings->m_DirectionalBurstSettings.BurstSeqCount);
        DirectionalBurstSettingsNode.append_child("RootOrbitXVariance").append_attribute("value").set_value(settings->m_DirectionalBurstSettings.RootOrbitXVariance);
        DirectionalBurstSettingsNode.append_child("RootOrbitYVarianceMin").append_attribute("value").set_value(settings->m_DirectionalBurstSettings.RootOrbitYVarianceMin);
        DirectionalBurstSettingsNode.append_child("RootOrbitYVarianceMax").append_attribute("value").set_value(settings->m_DirectionalBurstSettings.RootOrbitYVarianceMax);
        DirectionalBurstSettingsNode.append_child("BurstSeqOrbitXVariance").append_attribute("value").set_value(settings->m_DirectionalBurstSettings.BurstSeqOrbitXVariance);
        DirectionalBurstSettingsNode.append_child("BurstSeqOrbitYVariance").append_attribute("value").set_value(settings->m_DirectionalBurstSettings.BurstSeqOrbitYVariance);
    }

    pugi::xml_node CloudBurstSettingsNode = root.append_child("CloudBurstSettings");
    {
        CloudBurstSettingsNode.append_child("BurstIntensityMin").append_attribute("value").set_value(settings->m_CloudBurstSettings.BurstIntensityMin);
        CloudBurstSettingsNode.append_child("BurstIntensityMax").append_attribute("value").set_value(settings->m_CloudBurstSettings.BurstIntensityMax);
        CloudBurstSettingsNode.append_child("LightSourceExponentFalloff").append_attribute("value").set_value(settings->m_CloudBurstSettings.LightSourceExponentFalloff);
        CloudBurstSettingsNode.append_child("LightDeltaPos").append_attribute("value").set_value(settings->m_CloudBurstSettings.LightDeltaPos);
        CloudBurstSettingsNode.append_child("LightDistance").append_attribute("value").set_value(settings->m_CloudBurstSettings.LightDistance);

        CloudBurstSettingsNode.append_child("LightColor").append_attribute("value").set_value(std::format("0x{:08X}", settings->m_CloudBurstSettings.LightColor.GetRawHexData()).c_str());

        AppendCloudBurstCommonSettingsXmlNodes(CloudBurstSettingsNode, settings->m_CloudBurstSettings.m_CloudBurstCommonSettings);
    }

    pugi::xml_node StrikeSettingsNode = root.append_child("StrikeSettings");
    {
        StrikeSettingsNode.append_child("IntensityMin").append_attribute("value").set_value(settings->m_StrikeSettings.IntensityMin);
        StrikeSettingsNode.append_child("IntensityMax").append_attribute("value").set_value(settings->m_StrikeSettings.IntensityMax);
        StrikeSettingsNode.append_child("IntensityMinClamp").append_attribute("value").set_value(settings->m_StrikeSettings.IntensityMinClamp);
        StrikeSettingsNode.append_child("WidthMin").append_attribute("value").set_value(settings->m_StrikeSettings.WidthMin);
        StrikeSettingsNode.append_child("WidthMax").append_attribute("value").set_value(settings->m_StrikeSettings.WidthMax);
        StrikeSettingsNode.append_child("WidthMinClamp").append_attribute("value").set_value(settings->m_StrikeSettings.WidthMinClamp);
        StrikeSettingsNode.append_child("IntensityFlickerMin").append_attribute("value").set_value(settings->m_StrikeSettings.IntensityFlickerMin);
        StrikeSettingsNode.append_child("IntensityFlickerMax").append_attribute("value").set_value(settings->m_StrikeSettings.IntensityFlickerMax);
        StrikeSettingsNode.append_child("IntensityFlickerFrequency").append_attribute("value").set_value(settings->m_StrikeSettings.IntensityFlickerFrequency);
        StrikeSettingsNode.append_child("IntensityLevelDecayMin").append_attribute("value").set_value(settings->m_StrikeSettings.IntensityLevelDecayMin);
        StrikeSettingsNode.append_child("IntensityLevelDecayMax").append_attribute("value").set_value(settings->m_StrikeSettings.IntensityLevelDecayMax);
        StrikeSettingsNode.append_child("WidthLevelDecayMin").append_attribute("value").set_value(settings->m_StrikeSettings.WidthLevelDecayMin);
        StrikeSettingsNode.append_child("WidthLevelDecayMax").append_attribute("value").set_value(settings->m_StrikeSettings.WidthLevelDecayMax);
        StrikeSettingsNode.append_child("NoiseTexScale").append_attribute("value").set_value(settings->m_StrikeSettings.NoiseTexScale);
        StrikeSettingsNode.append_child("NoiseAmplitude").append_attribute("value").set_value(settings->m_StrikeSettings.NoiseAmplitude);
        StrikeSettingsNode.append_child("NoiseAmpMinDistLerp").append_attribute("value").set_value(settings->m_StrikeSettings.NoiseAmpMinDistLerp);
        StrikeSettingsNode.append_child("NoiseAmpMaxDistLerp").append_attribute("value").set_value(settings->m_StrikeSettings.NoiseAmpMaxDistLerp);
        StrikeSettingsNode.append_child("SubdivisionCount").append_attribute("value").set_value(settings->m_StrikeSettings.SubdivisionCount);
        StrikeSettingsNode.append_child("OrbitDirXVariance").append_attribute("value").set_value(settings->m_StrikeSettings.OrbitDirXVariance);
        StrikeSettingsNode.append_child("OrbitDirYVarianceMin").append_attribute("value").set_value(settings->m_StrikeSettings.OrbitDirYVarianceMin);
        StrikeSettingsNode.append_child("OrbitDirYVarianceMax").append_attribute("value").set_value(settings->m_StrikeSettings.OrbitDirYVarianceMax);
        StrikeSettingsNode.append_child("MaxDistanceForExposureReset").append_attribute("value").set_value(settings->m_StrikeSettings.MaxDistanceForExposureReset);
        StrikeSettingsNode.append_child("AngularWidthFactor").append_attribute("value").set_value(settings->m_StrikeSettings.AngularWidthFactor);
        StrikeSettingsNode.append_child("MaxHeightForStrike").append_attribute("value").set_value(settings->m_StrikeSettings.MaxHeightForStrike);
        StrikeSettingsNode.append_child("CoronaIntensityMult").append_attribute("value").set_value(settings->m_StrikeSettings.CoronaIntensityMult);
        StrikeSettingsNode.append_child("BaseCoronaSize").append_attribute("value").set_value(settings->m_StrikeSettings.BaseCoronaSize);

        StrikeSettingsNode.append_child("BaseColor").append_attribute("value").set_value(std::format("0x{:08X}", settings->m_StrikeSettings.BaseColor.GetRawHexData()).c_str());
        StrikeSettingsNode.append_child("FogColor").append_attribute("value").set_value(std::format("0x{:08X}", settings->m_StrikeSettings.FogColor.GetRawHexData()).c_str());

        StrikeSettingsNode.append_child("CloudLightIntensityMult").append_attribute("value").set_value(settings->m_StrikeSettings.CloudLightIntensityMult);
        StrikeSettingsNode.append_child("CloudLightDeltaPos").append_attribute("value").set_value(settings->m_StrikeSettings.CloudLightDeltaPos);
        StrikeSettingsNode.append_child("CloudLightRadius").append_attribute("value").set_value(settings->m_StrikeSettings.CloudLightRadius);
        StrikeSettingsNode.append_child("CloudLightFallOffExponent").append_attribute("value").set_value(settings->m_StrikeSettings.CloudLightFallOffExponent);
        StrikeSettingsNode.append_child("MaxDistanceForBurst").append_attribute("value").set_value(settings->m_StrikeSettings.MaxDistanceForBurst);
        StrikeSettingsNode.append_child("BurstThresholdIntensity").append_attribute("value").set_value(settings->m_StrikeSettings.BurstThresholdIntensity);
        StrikeSettingsNode.append_child("LightningFadeWidth").append_attribute("value").set_value(settings->m_StrikeSettings.LightningFadeWidth);
        StrikeSettingsNode.append_child("LightningFadeWidthFalloffExp").append_attribute("value").set_value(settings->m_StrikeSettings.LightningFadeWidthFalloffExp);


        pugi::xml_node variations = StrikeSettingsNode.append_child("variations");
        {
            for (size_t i = 0; i < 3; i++)
            {
                pugi::xml_node Item = variations.append_child("Item");
                {
                    Item.append_child("DurationMin").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].DurationMin);
                    Item.append_child("DurationMax").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].DurationMax);
                    Item.append_child("AnimationTimeMin").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].AnimationTimeMin);
                    Item.append_child("AnimationTimeMax").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].AnimationTimeMax);
                    Item.append_child("EndPointOffsetXVariance").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].EndPointOffsetXVariance);
                    Item.append_child("SubdivisionPatternMask").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].SubdivisionPatternMask);

                    pugi::xml_node ZigZagSplitPointNode = Item.append_child("ZigZagSplitPoint");
                    {
                        ZigZagSplitPointNode.append_child("FractionMin").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mZigZagSplitPoint.m_FractionMin);
                        ZigZagSplitPointNode.append_child("FractionMax").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mZigZagSplitPoint.m_FractionMax);
                        ZigZagSplitPointNode.append_child("DeviationDecay").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mZigZagSplitPoint.m_DeviationDecay);
                        ZigZagSplitPointNode.append_child("DeviationRightVariance").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mZigZagSplitPoint.m_DeviationRightVariance);
                    }

                    pugi::xml_node ForkZigZagSplitPointNode = Item.append_child("ForkZigZagSplitPoint");
                    {
                        ForkZigZagSplitPointNode.append_child("FractionMin").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mForkZigZagSplitPoint.m_FractionMin);
                        ForkZigZagSplitPointNode.append_child("FractionMax").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mForkZigZagSplitPoint.m_FractionMax);
                        ForkZigZagSplitPointNode.append_child("DeviationDecay").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mForkZigZagSplitPoint.m_DeviationDecay);
                        ForkZigZagSplitPointNode.append_child("DeviationRightVariance").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mForkZigZagSplitPoint.m_DeviationRightVariance);
                    }

                    pugi::xml_node ForkPointNode = Item.append_child("ForkPoint");
                    {
                        ForkPointNode.append_child("DeviationRightVariance").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mForkPoint.m_DeviationRightVariance);
                        ForkPointNode.append_child("DeviationForwardMin").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mForkPoint.m_DeviationForwardMin);
                        ForkPointNode.append_child("DeviationForwardMax").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mForkPoint.m_DeviationForwardMax);
                        ForkPointNode.append_child("LengthMin").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mForkPoint.m_LengthMin);
                        ForkPointNode.append_child("LengthMax").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mForkPoint.m_LengthMax);
                        ForkPointNode.append_child("LengthDecay").append_attribute("value").set_value(settings->m_StrikeSettings.m_Variations[i].mForkPoint.m_LengthDecay);
                    }

                    pugi::xml_node KeyFrameData = Item.append_child("KeyFrameData");
                    pugi::xml_node LightningMainIntensity = KeyFrameData.append_child("LightningMainIntensity");

                    AppendKeyData(LightningMainIntensity, settings->m_StrikeSettings.m_Variations->mKeyFrameData.LightningMainIntensity);
                  
                    pugi::xml_node LightningBranchIntensity = KeyFrameData.append_child("LightningBranchIntensity");
                    AppendKeyData(LightningBranchIntensity, settings->m_StrikeSettings.m_Variations->mKeyFrameData.LightningBranchIntensity);

                }
            }
        }

        AppendCloudBurstCommonSettingsXmlNodes(StrikeSettingsNode, settings->m_StrikeSettings.m_CloudBurstCommonSettings);
    }

    doc.save_file(path.c_str());
}


void VfxLightningsXmlParser::AppendKeyData(pugi::xml_node& parent, ptxKeyframe& keyframe)
{
    pugi::xml_node keyData = parent.append_child("keyData");
    pugi::xml_node numKeyEntries = keyData.append_child("numKeyEntries");
    numKeyEntries.append_attribute("value").set_value(keyframe.data.GetSize());
    pugi::xml_node keyEntryData = keyData.append_child("keyEntryData");
    keyEntryData.append_attribute("content") = "float_array";

    keyEntryData.text() = GetKeyframesTextParams(keyframe).c_str();
}


std::string& VfxLightningsXmlParser::GetKeyframesTextParams(ptxKeyframe& keyframe)
{
	static std::string text;
	text.clear();
    auto& keyEntryData = keyframe.data;

	for (size_t i = 0; i < keyframe.data.GetSize(); i++)
	{
		text += std::format("\n\t\t\t\t\t\t\t  {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}",
            keyEntryData[i].vTime[0],
            keyEntryData[i].vValue[0],
            keyEntryData[i].vValue[1],
            keyEntryData[i].vValue[2],
            keyEntryData[i].vValue[3]);
	}

	text += "\n\t\t\t\t\t\t\t";
	return text;
}



void VfxLightningsXmlParser::AppendCloudBurstCommonSettingsXmlNodes(pugi::xml_node& parent, CloudBurstCommonSettings& settings)
{
	auto CloudBurstCommonSettingsNode = parent.append_child("CloudBurstCommonSettings");
	{
        CloudBurstCommonSettingsNode.append_child("BurstDurationMin").append_attribute("value").set_value(settings.BurstDurationMin);
        CloudBurstCommonSettingsNode.append_child("BurstDurationMax").append_attribute("value").set_value(settings.BurstDurationMax);
        CloudBurstCommonSettingsNode.append_child("BurstSeqCount").append_attribute("value").set_value(settings.BurstSeqCount);
        CloudBurstCommonSettingsNode.append_child("RootOrbitXVariance").append_attribute("value").set_value(settings.RootOrbitXVariance);
        CloudBurstCommonSettingsNode.append_child("RootOrbitYVarianceMin").append_attribute("value").set_value(settings.RootOrbitYVarianceMin);
        CloudBurstCommonSettingsNode.append_child("RootOrbitYVarianceMax").append_attribute("value").set_value(settings.RootOrbitYVarianceMax);
        CloudBurstCommonSettingsNode.append_child("LocalOrbitXVariance").append_attribute("value").set_value(settings.LocalOrbitXVariance);
        CloudBurstCommonSettingsNode.append_child("LocalOrbitYVariance").append_attribute("value").set_value(settings.LocalOrbitYVariance);
        CloudBurstCommonSettingsNode.append_child("BurstSeqOrbitXVariance").append_attribute("value").set_value(settings.BurstSeqOrbitXVariance);
        CloudBurstCommonSettingsNode.append_child("BurstSeqOrbitYVariance").append_attribute("value").set_value(settings.BurstSeqOrbitYVariance);
        CloudBurstCommonSettingsNode.append_child("DelayMin").append_attribute("value").set_value(settings.DelayMin);
        CloudBurstCommonSettingsNode.append_child("DelayMax").append_attribute("value").set_value(settings.DelayMax);
        CloudBurstCommonSettingsNode.append_child("SizeMin").append_attribute("value").set_value(settings.SizeMin);
        CloudBurstCommonSettingsNode.append_child("SizeMax").append_attribute("value").set_value(settings.SizeMax);
	}
}