#include "visualSettingsHandler.h"


namespace
{
    bool(*updateSettings)();
    
    const size_t    buff_size = 27;
    u8              origBytes[buff_size];
    u8*             bytesAddr = nullptr;
    
    //char            origSavedPath[64];
    //char*           p_toOrigPath;
    //const char*     newPath = "__VfTmpfile__";
    //u8              origPathLen;
    
    bool getting_ingame_names = false;
    std::vector<VsItemTmp> UsedGameItems;
}


void(*orig_get1)(u64 arg, const char* name, u64 defaultVal, bool mustExist);
void n_get1(u64 arg, const char* name, u64 defaultVal, bool mustExist)
{
    if (getting_ingame_names)
    {
        UsedGameItems.push_back(VsItemTmp(name));
    }

    orig_get1(arg, name, defaultVal, mustExist);
    return;
}


void(*orig_get2)(u64 arg, const char* name1, const char* name2, u64 defaultVal, bool mustExist);
void n_get2(u64 arg, const char* name1, const char* name2, u64 defaultVal, bool mustExist)
{
    if (getting_ingame_names)
    {
        UsedGameItems.push_back(std::format("{}.{}", name1, name2));
    }

    orig_get2(arg, name1, name2, defaultVal, mustExist);
    return;
}


float*(*orig_getVec4)(u64 arg, float* col, const char* name);
float* n_getVec4(u64 arg, float* col, const char* name)
{
    if (getting_ingame_names)
    {
        UsedGameItems.push_back(std::format("{}.{}", name, "x"));
        UsedGameItems.push_back(std::format("{}.{}", name, "y"));
        UsedGameItems.push_back(std::format("{}.{}", name, "z"));
        UsedGameItems.push_back(std::format("{}.{}", name, "w"));
    }

    return orig_getVec4(arg, col, name);
}


float* (*orig_getVec3)(u64 arg, float* arg2, const char* name);
float* n_getVec3(u64 arg, float* arg2, const char* name)
{
    if (getting_ingame_names)
    {
        UsedGameItems.push_back(std::format("{}.{}", name, "x"));
        UsedGameItems.push_back(std::format("{}.{}", name, "y"));
        UsedGameItems.push_back(std::format("{}.{}", name, "z"));
    }

    return orig_getVec3(arg, arg2, name);
}


float* (*orig_getColor)(u64 arg, float* arg2, const char* name);
float* n_getColor(u64 arg, float* arg2, const char* name)
{
    if (getting_ingame_names)
    {
        UsedGameItems.push_back(std::format("{}.{}", name, "red"));
        UsedGameItems.push_back(std::format("{}.{}", name, "green"));
        UsedGameItems.push_back(std::format("{}.{}", name, "blue"));
    }
    return orig_getVec3(arg, arg2, name);
}


// it's almost like VsReloader works, 
// except we're replacing bytes in the function, instead of replacing symbols in the path str,
// so it won't parse settings from the file, 
// it will update existing settings stored in the game VisualSettings data array
VisualSettingsHandler::VisualSettingsHandler()
{
    static auto addr = gmAddress::Scan("48 83 EC 28 48 8D 15 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0");
    p_Vsettings         = addr.GetRef(14).To<gVisualSettings*>();
    updateSettings = addr.ToFunc<bool()>();
   
    //p_toOrigPath        = addr.GetRef(7).To<char*>();
    //origPathLen         = strlen(p_toOrigPath) + 1;
    //std::memcpy(origSavedPath, p_toOrigPath, origPathLen);

    bytesAddr = reinterpret_cast<u8*>(addr.GetAt(4).Value);
    
    getUsedParamNames();
    mContainer.initContainer(this);
    SetFuncN_Bytes();
}



void VisualSettingsHandler::getUsedParamNames()
{
    static auto get_addr1 = gmAddress::Scan("48 89 5C 24 ?? 57 48 83 EC 30 48 8B C2 48 8B D9 33 D2");
    static auto get_addr2 = gmAddress::Scan("40 53 48 83 EC 30 0F 29 74 24 ?? 48 8B D9 0F 28 F3 E8 ?? ?? ?? ?? 44 8A 4C 24");
    static auto getVec4_addr = gmAddress::Scan("48 8B C4 48 89 58 ?? 48 89 70 ?? 57 48 83 EC 50 49 8B D8");
    static auto getVec3_addr = gmAddress::Scan("E8 ?? ?? ?? ?? 4C 8D 05 ?? ?? ?? ?? 48 8D 54 24 ?? 48 8B CF 0F 28 00").GetRef(1).To<u64*>();
    static auto getColor_addr = gmAddress::Scan("40 53 48 83 EC 50 0F 29 74 24 ?? 0F 29 7C 24 ?? F3 0F 10 3D ?? ?? ?? ?? 48 8D 15").GetAt(801).GetRef(1).To<u64*>();

    Hook::Create(get_addr1,             n_get1,         &orig_get1,         "VSget1");
    Hook::Create(get_addr2,             n_get2,         &orig_get2,         "VSget2");
    Hook::Create(getVec4_addr,          n_getVec4,      &orig_getVec4,      "VSgetVec4");
    Hook::Create(*&getVec3_addr,        n_getVec3,      &orig_getVec3,      "VSgetVec3");
    Hook::Create(*&getColor_addr,       n_getColor,     &orig_getColor,     "VSgetColor");

    UsedGameItems.clear();
    UsedGameItems.reserve(1000);

    getting_ingame_names = true;
    updateData();
    getting_ingame_names = false;
    
    Hook::Remove(get_addr1,            " VSget1 remove");
    Hook::Remove(get_addr2,            " VSget2 remove");
    Hook::Remove(getVec4_addr,         " VSget vec4 remove");
    Hook::Remove(*&getVec3_addr,       " VSget vec3 remove");
    Hook::Remove(*&getColor_addr,      " VSget vec3 remove");
  
    updateData();
}


void VisualSettingsHandler::SetFuncN_Bytes()
{
    std::memcpy(origBytes, bytesAddr, buff_size);
    std::memset(bytesAddr, 0x90, buff_size);
}


void VisualSettingsHandler::RestoreFuncBytes()
{
    std::memcpy(bytesAddr, origBytes, buff_size);
}


VisualSettingsHandler::~VisualSettingsHandler()
{
    RestoreFuncBytes();
}


bool VisualSettingsHandler::updateData()
{
    return updateSettings();
}


void VScontainer::clearContainer()
{
    for (auto& [category, vec] : paramsMap)
    {
        for (auto& item : vec)
        {
            item.found = false;
            item.gPtrItem = nullptr;
            item.is_used = false;
        }
    }
}


void VScontainer::updateContainer(VisualSettingsHandler* handler)
{
    clearContainer();

    for (auto category : categoriesOrder)
    {
        if (!paramsMap.contains(category)) 
            continue;

        auto& items = paramsMap.at(category);
        for (auto& item : items)
        {
            if (item.found)
                continue;

            for (size_t i = 0; i < handler->p_Vsettings->data.GetSize(); i++)
            {
                auto& gItem = handler->p_Vsettings->data[i];
                if (item.hash == gItem.hash)
                {
                    item.gPtrItem = &gItem;
                    item.found = true;
                    break;
                }
            }
        }
    }
    
    std::sort(UsedGameItems.begin(), UsedGameItems.end(), [](const VsItemTmp& a, const VsItemTmp& b)
            {
                return a.hash < b.hash;
            });

    UsedGameItems.erase(std::unique(UsedGameItems.begin(), UsedGameItems.end()), UsedGameItems.end());

    for (auto& inGameItem : UsedGameItems)
    {
        bool should_be_in_none_category = true;
        
        for (auto& [category, vec] : paramsMap)
        {
            for (auto& item : vec)
            {
                if (item.is_used) 
                    continue;
                    
                if (item.hash == inGameItem.hash)
                {
                    should_be_in_none_category = false;
                    item.is_used = true;
                    inGameItem.already_exists_in_container = true;

                    break;
                }
            }
        }
        
        if (should_be_in_none_category)
        {
            if (!inGameItem.already_exists_in_container)
            {
                gSettingsItem* gItem = std::find_if(handler->p_Vsettings->data.begin(), handler->p_Vsettings->data.end(),
                    [&](const gSettingsItem& item)
                    {
                        return item.hash == inGameItem.hash;
                    });
                
                if (gItem != handler->p_Vsettings->data.end())
                {
                    handler->mContainer.paramsMap["None"]
                        .push_back({ inGameItem.name.c_str(), inGameItem.name.c_str(), inGameItem.hash, V_FLOAT , gItem, true, true });
                }
            }
        }
    }
    

    if (handler->mContainer.paramsMap.contains("None"))
        handler->mContainer.categoriesOrder.push_back("None");

}


namespace 
{
    void find_and_erase_till_the_end(std::string& str, char symb)
    {
        size_t symb_pos = str.find(symb);
        if (symb_pos != -1)
            str.erase(str.begin() + symb_pos, str.end());
    };
}

void VisualSettingsParser::importData(std::string& path, VisualSettingsHandler* handler)
{
    std::ifstream finput(path);

    if (!finput.is_open())
        return;

    handler->p_Vsettings->isLoaded = false;
    auto& inGameArray = handler->p_Vsettings->data;
    inGameArray.clear();

    std::string line;
    while (std::getline(finput, line, '\n'))
    {
        line = strip_str(line);

        if (line.empty())
            continue;

        find_and_erase_till_the_end(line, '#');
        find_and_erase_till_the_end(line, '//');

        if (line.empty())
            continue;

        if (!(std::any_of(line.begin(), line.end(), [](const char& c) -> bool { return isdigit(c); } ))) {
            continue;
        }

        auto v = split_string(line, "\t ");
        if (v.size() < 2)
            continue;

        inGameArray.push_back({ rage::joaat(v[0].c_str()), static_cast<float>(atof(v[1].c_str())) }, true);
    }

    std::sort(inGameArray.begin(), inGameArray.end(), [](const gSettingsItem& v1, const gSettingsItem& v2)
        {
            return v1.hash < v2.hash;
        });

    handler->p_Vsettings->isLoaded = true;
}


void VisualSettingsHandler::importData(std::string srcPath)
{
    mContainer.clearContainer();
    parser.importData(srcPath,this);
    updateData();
    mContainer.updateContainer(this);

#if 0
    namespace fs = std::filesystem;
    
    if (!fs::exists(srcPath))
        return;

    fs::copy_file(srcPath, newPath, fs::copy_options::overwrite_existing);
    
    if (!fs::exists(newPath))
        mlogger("couldn't copy vSettings file");

    RestoreFuncBytes();

    std::memset(p_toOrigPath, 0, origPathLen);
    std::memcpy(p_toOrigPath, newPath, origPathLen);

    mContainer.clearContainer();
    bool res = updateData();
    mContainer.updateContainer(this);

    std::memset(p_toOrigPath, 0, origPathLen);
    std::memcpy(p_toOrigPath, origSavedPath, origPathLen);
   
    if (!res)
        mlogger("couldn't load visual settings data (an old game version?)");

    if (fs::exists(newPath)) 
        fs::remove(newPath);  

    SetFuncN_Bytes();
#endif
}

void VisualSettingsParser::exportData(std::string& path, VisualSettingsHandler* handler)
{
    auto& mContainer = handler->mContainer;

    if (check_str_ending(path, ".xml"))
        path = path.replace(path.end() - 4, path.end(), ".dat");

    if (!check_str_ending(path, ".dat"))
        path += ".dat";

    static std::string text;
    text.clear();
    text.reserve(100000);

    for (auto category : mContainer.categoriesOrder)
    {
        if (!mContainer.paramsMap.contains(category))
            continue;

        text += std::format("\n# {}\n\n", category);

        auto& Items = mContainer.paramsMap.at(category);
        for (auto Item : Items)
        {
            if (Item.found && Item.gPtrItem != nullptr)
                text += std::format("{:<65} {:.5f}\n", Item.paramName, Item.gPtrItem->value);
        }
    }

    std::ofstream outFile(path, std::ios::trunc);

    if (outFile.is_open())
    {
        outFile.write(text.data(), text.size());
        outFile.close();
    }
}


void VisualSettingsHandler::exportData(std::string path)
{
    parser.exportData(path, this);
}


void VScontainer::initContainer(VisualSettingsHandler* handler)
{
    using rage::joaat;

    if (!handler->p_Vsettings) 
        return;

    {
        categoriesOrder =
        {
           "Values to use with height based reflection map",
            "Values to use with rain collision map",
            "Values for rain GPU particle effect",
            "General weather configuration",
            "Config for static values for skyhat",
            "config values for cloud shadows",
            "config values for vehicle settings",
            "car headlight volume lights data",
            "heli poslight",
            "heli white head lights",
            "heli white tail lights",
            "heli interior light",
            "plane poslight",
            "plane whiteheadlight",
            "plane whitetaillight",
            "plane control-panel light",
            "plane right emergency light",
            "plane left emergency light",
            "plane inside hull light",
            "boat lights",
            "train settings",
            "emissive bits",
            "Lod distances for vehicles",
            "search lights",
            "Config values for traffic lights",
            "Config values for Tree Imposters",
            "Config values for peds",
            "Lod distances for peds",
            "Config Values for Camera Events : Val start -> Val End  /  MB start -> MB End",
            "Containers LOD",
            "distant lights",
            "Emissive night values",
            "Misc Values",
            "coronas",
            "See Through",
            "Tonemapping",
            "Exposure",
            "Adaptation",
            "Cloud generation",
            "Cloud speeds",
            "lod fade distances",
            "sky plane",
            "Rim lighting",
            "Reflection tweaks",
            "Light component cutoff",
            "Bloom",
            "Dynmaic bake paramaters",
            "Minimap alpha",
            "Puddles",
            "LOD lights - do not change these",
            "Bright Lights",
            "Vehicle Light Night/Day Settings",
            "Vehicle light fade distances",
            "Particle Shadow intensity",
            "Dark Light",
            "Secondary Dark Light",
            "Ped light",
            "ped phone light properties",
            "Adaptive DOF - PC/NG Only",
            "Multipiers over final shadow cascade.",
            "character lens flare",
            "First person motion blur",
        };
    }
    
    {
        paramsMap["Values to use with height based reflection map"] =
        {
                {"heightReflect width"                                            , "heightReflect.width"                                          , joaat("heightReflect.width")                                        , V_FLOAT },
                {"heightReflect height"                                           , "heightReflect.height"                                         , joaat("heightReflect.height")                                       , V_FLOAT },
                {"heightReflect specularoffset"                                   , "heightReflect.specularoffset"                                 , joaat("heightReflect.specularoffset")                               , V_FLOAT },
                {"heightReflect waterspeed"                                       , "heightReflect.waterspeed"                                     , joaat("heightReflect.waterspeed")                                   , V_FLOAT },
        };
        paramsMap["Values to use with rain collision map"] =
        {
                {"raincollision specularoffset"                                   , "raincollision.specularoffset"                                 , joaat("raincollision.specularoffset")                               , V_FLOAT },
                {"raincollision waterspeed"                                       , "raincollision.waterspeed"                                     , joaat("raincollision.waterspeed")                                   , V_FLOAT },
        };
        paramsMap["Values for rain GPU particle effect"] =
        {
                {"rain NumberParticles"                                           , "rain.NumberParticles"                                         , joaat("rain.NumberParticles")                                       , V_FLOAT },
                {"rain UseLitShader"                                              , "rain.UseLitShader"                                            , joaat("rain.UseLitShader")                                          , V_FLOAT },
                {"rain gravity x"                                                 , "rain.gravity.x"                                               , joaat("rain.gravity.x")                                             , V_FLOAT },
                {"rain gravity y"                                                 , "rain.gravity.y"                                               , joaat("rain.gravity.y")                                             , V_FLOAT },
                {"rain gravity z"                                                 , "rain.gravity.z"                                               , joaat("rain.gravity.z")                                             , V_FLOAT },
                {"rain fadeInScale"                                               , "rain.fadeInScale"                                             , joaat("rain.fadeInScale")                                           , V_FLOAT },
                {"rain diffuse"                                                   , "rain.diffuse"                                                 , joaat("rain.diffuse")                                               , V_FLOAT },
                {"rain ambient"                                                   , "rain.ambient"                                                 , joaat("rain.ambient")                                               , V_FLOAT },
                {"rain wrapScale"                                                 , "rain.wrapScale"                                               , joaat("rain.wrapScale")                                             , V_FLOAT },
                {"rain wrapBias"                                                  , "rain.wrapBias"                                                , joaat("rain.wrapBias")                                              , V_FLOAT },
                {"rain defaultlight"                                              , "rain.defaultlight.red"                                        , joaat("rain.defaultlight.red")                                      , V_COL3  },
                {"None"                                                           , "rain.defaultlight.green"                                      , joaat("rain.defaultlight.green")                                    , V_NONE  },
                {"None"                                                           , "rain.defaultlight.blue"                                       , joaat("rain.defaultlight.blue")                                     , V_NONE  },
                {"rain defaultlight alpha"                                        , "rain.defaultlight.alpha"                                      , joaat("rain.defaultlight.alpha")                                    , V_FLOAT },
        };
        paramsMap["General weather configuration"] =
        {
                {"weather CycleDuration"                                          , "weather.CycleDuration"                                        , joaat("weather.CycleDuration")                                      , V_FLOAT },
                {"weather ChangeCloudOnSameCloudTypeChance"                       , "weather.ChangeCloudOnSameCloudTypeChance"                     , joaat("weather.ChangeCloudOnSameCloudTypeChance")                   , V_FLOAT },
                {"weather ChangeCloudOnSameWeatherTypeChance"                     , "weather.ChangeCloudOnSameWeatherTypeChance"                   , joaat("weather.ChangeCloudOnSameWeatherTypeChance")                 , V_FLOAT },
        };
        paramsMap["Config for static values for skyhat"] =
        {
                {"sky sun centreStart"                                            , "sky.sun.centreStart"                                          , joaat("sky.sun.centreStart")                                        , V_FLOAT },
                {"sky sun centreEnd"                                              , "sky.sun.centreEnd"                                            , joaat("sky.sun.centreEnd")                                          , V_FLOAT },
                {"sky cloudWarp"                                                  , "sky.cloudWarp"                                                , joaat("sky.cloudWarp")                                              , V_FLOAT },
                {"sky cloudInscatteringRange"                                     , "sky.cloudInscatteringRange"                                   , joaat("sky.cloudInscatteringRange")                                 , V_FLOAT },
                {"sky cloudEdgeSmooth"                                            , "sky.cloudEdgeSmooth"                                          , joaat("sky.cloudEdgeSmooth")                                        , V_FLOAT },
                {"sky sunAxiasX"                                                  , "sky.sunAxiasX"                                                , joaat("sky.sunAxiasX")                                              , V_FLOAT },
                {"sky sunAxiasY"                                                  , "sky.sunAxiasY"                                                , joaat("sky.sunAxiasY")                                              , V_FLOAT },
                {"sky GameCloudSpeed"                                             , "sky.GameCloudSpeed"                                           , joaat("sky.GameCloudSpeed")                                         , V_FLOAT },
        };
        paramsMap["config values for cloud shadows"] =
        {
                {"shadows cloudtexture scale"                                     , "shadows.cloudtexture.scale"                                   , joaat("shadows.cloudtexture.scale")                                 , V_FLOAT },
                {"shadows cloudtexture rangemin"                                  , "shadows.cloudtexture.rangemin"                                , joaat("shadows.cloudtexture.rangemin")                              , V_FLOAT },
                {"shadows cloudtexture rangemax"                                  , "shadows.cloudtexture.rangemax"                                , joaat("shadows.cloudtexture.rangemax")                              , V_FLOAT },
        };
        paramsMap["config values for vehicle settings"] =
        {
                {"car interiorlight color"                                        , "car.interiorlight.color.red"                                  , joaat("car.interiorlight.color.red")                                , V_COL3  },
                {"None"                                                           , "car.interiorlight.color.green"                                , joaat("car.interiorlight.color.green")                              , V_NONE  },
                {"None"                                                           , "car.interiorlight.color.blue"                                 , joaat("car.interiorlight.color.blue")                               , V_NONE  },
                {"car interiorlight intensity"                                    , "car.interiorlight.intensity"                                  , joaat("car.interiorlight.intensity")                                , V_FLOAT },
                {"car interiorlight radius"                                       , "car.interiorlight.radius"                                     , joaat("car.interiorlight.radius")                                   , V_FLOAT },
                {"car interiorlight innerConeAngle"                               , "car.interiorlight.innerConeAngle"                             , joaat("car.interiorlight.innerConeAngle")                           , V_FLOAT },
                {"car interiorlight outerConeAngle"                               , "car.interiorlight.outerConeAngle"                             , joaat("car.interiorlight.outerConeAngle")                           , V_FLOAT },
                {"car interiorlight day emissive on"                              , "car.interiorlight.day.emissive.on"                            , joaat("car.interiorlight.day.emissive.on")                          , V_FLOAT },
                {"car interiorlight night emissive on"                            , "car.interiorlight.night.emissive.on"                          , joaat("car.interiorlight.night.emissive.on")                        , V_FLOAT },
                {"car interiorlight day emissive off"                             , "car.interiorlight.day.emissive.off"                           , joaat("car.interiorlight.day.emissive.off")                         , V_FLOAT },
                {"car interiorlight night emissive off"                           , "car.interiorlight.night.emissive.off"                         , joaat("car.interiorlight.night.emissive.off")                       , V_FLOAT },
                {"car interiorlight coronaHDR"                                    , "car.interiorlight.coronaHDR"                                  , joaat("car.interiorlight.coronaHDR")                                , V_FLOAT },
                {"car interiorlight coronaSize"                                   , "car.interiorlight.coronaSize"                                 , joaat("car.interiorlight.coronaSize")                               , V_FLOAT },
                {"car fatinteriorlight color"                                     , "car.fatinteriorlight.color.red"                               , joaat("car.fatinteriorlight.color.red")                             , V_COL3  },
                {"None"                                                           , "car.fatinteriorlight.color.green"                             , joaat("car.fatinteriorlight.color.green")                           , V_NONE  },
                {"None"                                                           , "car.fatinteriorlight.color.blue"                              , joaat("car.fatinteriorlight.color.blue")                            , V_NONE  },
                {"car fatinteriorlight intensity"                                 , "car.fatinteriorlight.intensity"                               , joaat("car.fatinteriorlight.intensity")                             , V_FLOAT },
                {"car fatinteriorlight radius"                                    , "car.fatinteriorlight.radius"                                  , joaat("car.fatinteriorlight.radius")                                , V_FLOAT },
                {"car fatinteriorlight innerConeAngle"                            , "car.fatinteriorlight.innerConeAngle"                          , joaat("car.fatinteriorlight.innerConeAngle")                        , V_FLOAT },
                {"car fatinteriorlight outerConeAngle"                            , "car.fatinteriorlight.outerConeAngle"                          , joaat("car.fatinteriorlight.outerConeAngle")                        , V_FLOAT },
                {"car fatinteriorlight day emissive on"                           , "car.fatinteriorlight.day.emissive.on"                         , joaat("car.fatinteriorlight.day.emissive.on")                       , V_FLOAT },
                {"car fatinteriorlight night emissive on"                         , "car.fatinteriorlight.night.emissive.on"                       , joaat("car.fatinteriorlight.night.emissive.on")                     , V_FLOAT },
                {"car fatinteriorlight day emissive off"                          , "car.fatinteriorlight.day.emissive.off"                        , joaat("car.fatinteriorlight.day.emissive.off")                      , V_FLOAT },
                {"car fatinteriorlight night emissive off"                        , "car.fatinteriorlight.night.emissive.off"                      , joaat("car.fatinteriorlight.night.emissive.off")                    , V_FLOAT },
                {"car fatinteriorlight coronaHDR"                                 , "car.fatinteriorlight.coronaHDR"                               , joaat("car.fatinteriorlight.coronaHDR")                             , V_FLOAT },
                {"car fatinteriorlight coronaSize"                                , "car.fatinteriorlight.coronaSize"                              , joaat("car.fatinteriorlight.coronaSize")                            , V_FLOAT },
                {"car redinteriorlight color"                                     , "car.redinteriorlight.color.red"                               , joaat("car.redinteriorlight.color.red")                             , V_COL3  },
                {"None"                                                           , "car.redinteriorlight.color.green"                             , joaat("car.redinteriorlight.color.green")                           , V_NONE  },
                {"None"                                                           , "car.redinteriorlight.color.blue"                              , joaat("car.redinteriorlight.color.blue")                            , V_NONE  },
                {"car redinteriorlight intensity"                                 , "car.redinteriorlight.intensity"                               , joaat("car.redinteriorlight.intensity")                             , V_FLOAT },
                {"car redinteriorlight radius"                                    , "car.redinteriorlight.radius"                                  , joaat("car.redinteriorlight.radius")                                , V_FLOAT },
                {"car redinteriorlight innerConeAngle"                            , "car.redinteriorlight.innerConeAngle"                          , joaat("car.redinteriorlight.innerConeAngle")                        , V_FLOAT },
                {"car redinteriorlight outerConeAngle"                            , "car.redinteriorlight.outerConeAngle"                          , joaat("car.redinteriorlight.outerConeAngle")                        , V_FLOAT },
                {"car redinteriorlight day emissive on"                           , "car.redinteriorlight.day.emissive.on"                         , joaat("car.redinteriorlight.day.emissive.on")                       , V_FLOAT },
                {"car redinteriorlight night emissive on"                         , "car.redinteriorlight.night.emissive.on"                       , joaat("car.redinteriorlight.night.emissive.on")                     , V_FLOAT },
                {"car redinteriorlight day emissive off"                          , "car.redinteriorlight.day.emissive.off"                        , joaat("car.redinteriorlight.day.emissive.off")                      , V_FLOAT },
                {"car redinteriorlight night emissive off"                        , "car.redinteriorlight.night.emissive.off"                      , joaat("car.redinteriorlight.night.emissive.off")                    , V_FLOAT },
                {"car redinteriorlight coronaHDR"                                 , "car.redinteriorlight.coronaHDR"                               , joaat("car.redinteriorlight.coronaHDR")                             , V_FLOAT },
                {"car redinteriorlight coronaSize"                                , "car.redinteriorlight.coronaSize"                              , joaat("car.redinteriorlight.coronaSize")                            , V_FLOAT },
                {"car platelight color"                                           , "car.platelight.color.red"                                     , joaat("car.platelight.color.red")                                   , V_COL3  },
                {"None"                                                           , "car.platelight.color.green"                                   , joaat("car.platelight.color.green")                                 , V_NONE  },
                {"None"                                                           , "car.platelight.color.blue"                                    , joaat("car.platelight.color.blue")                                  , V_NONE  },
                {"car platelight intensity"                                       , "car.platelight.intensity"                                     , joaat("car.platelight.intensity")                                   , V_FLOAT },
                {"car platelight radius"                                          , "car.platelight.radius"                                        , joaat("car.platelight.radius")                                      , V_FLOAT },
                {"car platelight innerConeAngle"                                  , "car.platelight.innerConeAngle"                                , joaat("car.platelight.innerConeAngle")                              , V_FLOAT },
                {"car platelight outerConeAngle"                                  , "car.platelight.outerConeAngle"                                , joaat("car.platelight.outerConeAngle")                              , V_FLOAT },
                {"car platelight day emissive on"                                 , "car.platelight.day.emissive.on"                               , joaat("car.platelight.day.emissive.on")                             , V_FLOAT },
                {"car platelight night emissive on"                               , "car.platelight.night.emissive.on"                             , joaat("car.platelight.night.emissive.on")                           , V_FLOAT },
                {"car platelight day emissive off"                                , "car.platelight.day.emissive.off"                              , joaat("car.platelight.day.emissive.off")                            , V_FLOAT },
                {"car platelight night emissive off"                              , "car.platelight.night.emissive.off"                            , joaat("car.platelight.night.emissive.off")                          , V_FLOAT },
                {"car platelight coronaHDR"                                       , "car.platelight.coronaHDR"                                     , joaat("car.platelight.coronaHDR")                                   , V_FLOAT },
                {"car platelight coronaSize"                                      , "car.platelight.coronaSize"                                    , joaat("car.platelight.coronaSize")                                  , V_FLOAT },
                {"car platelight falloffExp"                                      , "car.platelight.falloffExp"                                    , joaat("car.platelight.falloffExp")                                  , V_FLOAT },
                {"car dashlight color"                                            , "car.dashlight.color.red"                                      , joaat("car.dashlight.color.red")                                    , V_COL3  },
                {"None"                                                           , "car.dashlight.color.green"                                    , joaat("car.dashlight.color.green")                                  , V_NONE  },
                {"None"                                                           , "car.dashlight.color.blue"                                     , joaat("car.dashlight.color.blue")                                   , V_NONE  },
                {"car dashlight intensity"                                        , "car.dashlight.intensity"                                      , joaat("car.dashlight.intensity")                                    , V_FLOAT },
                {"car dashlight radius"                                           , "car.dashlight.radius"                                         , joaat("car.dashlight.radius")                                       , V_FLOAT },
                {"car dashlight innerConeAngle"                                   , "car.dashlight.innerConeAngle"                                 , joaat("car.dashlight.innerConeAngle")                               , V_FLOAT },
                {"car dashlight outerConeAngle"                                   , "car.dashlight.outerConeAngle"                                 , joaat("car.dashlight.outerConeAngle")                               , V_FLOAT },
                {"car dashlight day emissive on"                                  , "car.dashlight.day.emissive.on"                                , joaat("car.dashlight.day.emissive.on")                              , V_FLOAT },
                {"car dashlight night emissive on"                                , "car.dashlight.night.emissive.on"                              , joaat("car.dashlight.night.emissive.on")                            , V_FLOAT },
                {"car dashlight day emissive off"                                 , "car.dashlight.day.emissive.off"                               , joaat("car.dashlight.day.emissive.off")                             , V_FLOAT },
                {"car dashlight night emissive off"                               , "car.dashlight.night.emissive.off"                             , joaat("car.dashlight.night.emissive.off")                           , V_FLOAT },
                {"car dashlight coronaHDR"                                        , "car.dashlight.coronaHDR"                                      , joaat("car.dashlight.coronaHDR")                                    , V_FLOAT },
                {"car dashlight coronaSize"                                       , "car.dashlight.coronaSize"                                     , joaat("car.dashlight.coronaSize")                                   , V_FLOAT },
                {"car dashlight falloffExp"                                       , "car.dashlight.falloffExp"                                     , joaat("car.dashlight.falloffExp")                                   , V_FLOAT },
                {"car doorlight color"                                            , "car.doorlight.color.red"                                      , joaat("car.doorlight.color.red")                                    , V_COL3  },
                {"None"                                                           , "car.doorlight.color.green"                                    , joaat("car.doorlight.color.green")                                  , V_NONE  },
                {"None"                                                           , "car.doorlight.color.blue"                                     , joaat("car.doorlight.color.blue")                                   , V_NONE  },
                {"car doorlight intensity"                                        , "car.doorlight.intensity"                                      , joaat("car.doorlight.intensity")                                    , V_FLOAT },
                {"car doorlight radius"                                           , "car.doorlight.radius"                                         , joaat("car.doorlight.radius")                                       , V_FLOAT },
                {"car doorlight innerConeAngle"                                   , "car.doorlight.innerConeAngle"                                 , joaat("car.doorlight.innerConeAngle")                               , V_FLOAT },
                {"car doorlight outerConeAngle"                                   , "car.doorlight.outerConeAngle"                                 , joaat("car.doorlight.outerConeAngle")                               , V_FLOAT },
                {"car doorlight day emissive on"                                  , "car.doorlight.day.emissive.on"                                , joaat("car.doorlight.day.emissive.on")                              , V_FLOAT },
                {"car doorlight night emissive on"                                , "car.doorlight.night.emissive.on"                              , joaat("car.doorlight.night.emissive.on")                            , V_FLOAT },
                {"car doorlight day emissive off"                                 , "car.doorlight.day.emissive.off"                               , joaat("car.doorlight.day.emissive.off")                             , V_FLOAT },
                {"car doorlight night emissive off"                               , "car.doorlight.night.emissive.off"                             , joaat("car.doorlight.night.emissive.off")                           , V_FLOAT },
                {"car doorlight coronaHDR"                                        , "car.doorlight.coronaHDR"                                      , joaat("car.doorlight.coronaHDR")                                    , V_FLOAT },
                {"car doorlight coronaSize"                                       , "car.doorlight.coronaSize"                                     , joaat("car.doorlight.coronaSize")                                   , V_FLOAT },
                {"car doorlight falloffExp"                                       , "car.doorlight.falloffExp"                                     , joaat("car.doorlight.falloffExp")                                   , V_FLOAT },
                {"car taxi color"                                                 , "car.taxi.color.red"                                           , joaat("car.taxi.color.red")                                         , V_COL3  },
                {"None"                                                           , "car.taxi.color.green"                                         , joaat("car.taxi.color.green")                                       , V_NONE  },
                {"None"                                                           , "car.taxi.color.blue"                                          , joaat("car.taxi.color.blue")                                        , V_NONE  },
                {"car taxi intensity"                                             , "car.taxi.intensity"                                           , joaat("car.taxi.intensity")                                         , V_FLOAT },
                {"car taxi radius"                                                , "car.taxi.radius"                                              , joaat("car.taxi.radius")                                            , V_FLOAT },
                {"car taxi innerConeAngle"                                        , "car.taxi.innerConeAngle"                                      , joaat("car.taxi.innerConeAngle")                                    , V_FLOAT },
                {"car taxi outerConeAngle"                                        , "car.taxi.outerConeAngle"                                      , joaat("car.taxi.outerConeAngle")                                    , V_FLOAT },
                {"car taxi coronaHDR"                                             , "car.taxi.coronaHDR"                                           , joaat("car.taxi.coronaHDR")                                         , V_FLOAT },
                {"car taxi coronaSize"                                            , "car.taxi.coronaSize"                                          , joaat("car.taxi.coronaSize")                                        , V_FLOAT },
                {"car neon defaultcolor"                                          , "car.neon.defaultcolor.red"                                    , joaat("car.neon.defaultcolor.red")                                  , V_COL3  },
                {"None"                                                           , "car.neon.defaultcolor.green"                                  , joaat("car.neon.defaultcolor.green")                                , V_NONE  },
                {"None"                                                           , "car.neon.defaultcolor.blue"                                   , joaat("car.neon.defaultcolor.blue")                                 , V_NONE  },
                {"car neon intensity"                                             , "car.neon.intensity"                                           , joaat("car.neon.intensity")                                         , V_FLOAT },
                {"car neon radius"                                                , "car.neon.radius"                                              , joaat("car.neon.radius")                                            , V_FLOAT },
                {"car neon falloffexponent"                                       , "car.neon.falloffexponent"                                     , joaat("car.neon.falloffexponent")                                   , V_FLOAT },
                {"car neon capsuleextent sides"                                   , "car.neon.capsuleextent.sides"                                 , joaat("car.neon.capsuleextent.sides")                               , V_FLOAT },
                {"car neon capsuleextent frontback"                               , "car.neon.capsuleextent.frontback"                             , joaat("car.neon.capsuleextent.frontback")                           , V_FLOAT },
                {"car neon clipplaneheight"                                       , "car.neon.clipplaneheight"                                     , joaat("car.neon.clipplaneheight")                                   , V_FLOAT },
                {"car neon bikeclipplaneheight"                                   , "car.neon.bikeclipplaneheight"                                 , joaat("car.neon.bikeclipplaneheight")                               , V_FLOAT },
                {"car headlights angle"                                           , "car.headlights.angle"                                         , joaat("car.headlights.angle")                                       , V_FLOAT },
                {"car headlights split"                                           , "car.headlights.split"                                         , joaat("car.headlights.split")                                       , V_FLOAT },
                {"car headlights global HeadlightIntensityMult"                   , "car.headlights.global.HeadlightIntensityMult"                 , joaat("car.headlights.global.HeadlightIntensityMult")               , V_FLOAT },
                {"car headlights global HeadlightDistMult"                        , "car.headlights.global.HeadlightDistMult"                      , joaat("car.headlights.global.HeadlightDistMult")                    , V_FLOAT },
                {"car headlights global ConeInnerAngleMod"                        , "car.headlights.global.ConeInnerAngleMod"                      , joaat("car.headlights.global.ConeInnerAngleMod")                    , V_FLOAT },
                {"car headlights global ConeOuterAngleMod"                        , "car.headlights.global.ConeOuterAngleMod"                      , joaat("car.headlights.global.ConeOuterAngleMod")                    , V_FLOAT },
                {"car headlights global OnlyOneLightMod"                          , "car.headlights.global.OnlyOneLightMod"                        , joaat("car.headlights.global.OnlyOneLightMod")                      , V_FLOAT },
                {"car headlights global Fake2LightsAngleMod"                      , "car.headlights.global.Fake2LightsAngleMod"                    , joaat("car.headlights.global.Fake2LightsAngleMod")                  , V_FLOAT },
                {"car headlights global Fake2LightsDisplacementMod"               , "car.headlights.global.Fake2LightsDisplacementMod"             , joaat("car.headlights.global.Fake2LightsDisplacementMod")           , V_FLOAT },
                {"car headlights submarine Fake2LightsAngleMod"                   , "car.headlights.submarine.Fake2LightsAngleMod"                 , joaat("car.headlights.submarine.Fake2LightsAngleMod")               , V_FLOAT },
                {"car headlights submarine Fake2LightsDisplacementMod"            , "car.headlights.submarine.Fake2LightsDisplacementMod"          , joaat("car.headlights.submarine.Fake2LightsDisplacementMod")        , V_FLOAT },
                {"car headlights fullbeam IntensityMult"                          , "car.headlights.fullbeam.IntensityMult"                        , joaat("car.headlights.fullbeam.IntensityMult")                      , V_FLOAT },
                {"car headlights fullbeam DistMult"                               , "car.headlights.fullbeam.DistMult"                             , joaat("car.headlights.fullbeam.DistMult")                           , V_FLOAT },
                {"car headlights fullbeam CoronaIntensityMult"                    , "car.headlights.fullbeam.CoronaIntensityMult"                  , joaat("car.headlights.fullbeam.CoronaIntensityMult")                , V_FLOAT },
                {"car headlights fullbeam CoronaSizeMult"                         , "car.headlights.fullbeam.CoronaSizeMult"                       , joaat("car.headlights.fullbeam.CoronaSizeMult")                     , V_FLOAT },
                {"car headlights aim fullbeam mod"                                , "car.headlights.aim.fullbeam.mod"                              , joaat("car.headlights.aim.fullbeam.mod")                            , V_FLOAT },
                {"car headlights aim dippedbeam mod"                              , "car.headlights.aim.dippedbeam.mod"                            , joaat("car.headlights.aim.dippedbeam.mod")                          , V_FLOAT },
                {"car headlights aim fullbeam angle"                              , "car.headlights.aim.fullbeam.angle"                            , joaat("car.headlights.aim.fullbeam.angle")                          , V_FLOAT },
                {"car headlights aim dipeedbeam angle"                            , "car.headlights.aim.dipeedbeam.angle"                          , joaat("car.headlights.aim.dipeedbeam.angle")                        , V_FLOAT },
                {"car headlights player intensitymult"                            , "car.headlights.player.intensitymult"                          , joaat("car.headlights.player.intensitymult")                        , V_FLOAT },
                {"car headlights player distmult"                                 , "car.headlights.player.distmult"                               , joaat("car.headlights.player.distmult")                             , V_FLOAT },
                {"car headlights player exponentmult"                             , "car.headlights.player.exponentmult"                           , joaat("car.headlights.player.exponentmult")                         , V_FLOAT },
        };
        paramsMap["car headlight volume lights data"] =
        {
                {"car headlights volume intensityscale"                           , "car.headlights.volume.intensityscale"                         , joaat("car.headlights.volume.intensityscale")                       , V_FLOAT },
                {"car headlights volume sizescale"                                , "car.headlights.volume.sizescale"                              , joaat("car.headlights.volume.sizescale")                            , V_FLOAT },
                {"car headlights volume outerintensity"                           , "car.headlights.volume.outerintensity"                         , joaat("car.headlights.volume.outerintensity")                       , V_FLOAT },
                {"car headlights volume outerexponent"                            , "car.headlights.volume.outerexponent"                          , joaat("car.headlights.volume.outerexponent")                        , V_FLOAT },
                {"car headlights volume outerVolumeColor"                         , "car.headlights.volume.outerVolumeColor.red"                   , joaat("car.headlights.volume.outerVolumeColor.red")                 , V_COL3  },
                {"None"                                                           , "car.headlights.volume.outerVolumeColor.green"                 , joaat("car.headlights.volume.outerVolumeColor.green")               , V_NONE  },
                {"None"                                                           , "car.headlights.volume.outerVolumeColor.blue"                  , joaat("car.headlights.volume.outerVolumeColor.blue")                , V_NONE  },
                {"car coronas MainFadeRatio"                                      , "car.coronas.MainFadeRatio"                                    , joaat("car.coronas.MainFadeRatio")                                  , V_FLOAT },
                {"car coronas BeginStart"                                         , "car.coronas.BeginStart"                                       , joaat("car.coronas.BeginStart")                                     , V_FLOAT },
                {"car coronas BeginEnd"                                           , "car.coronas.BeginEnd"                                         , joaat("car.coronas.BeginEnd")                                       , V_FLOAT },
                {"car coronas CutoffStart"                                        , "car.coronas.CutoffStart"                                      , joaat("car.coronas.CutoffStart")                                    , V_FLOAT },
                {"car coronas CutoffEnd"                                          , "car.coronas.CutoffEnd"                                        , joaat("car.coronas.CutoffEnd")                                      , V_FLOAT },
                {"car coronas underwaterFade"                                     , "car.coronas.underwaterFade"                                   , joaat("car.coronas.underwaterFade")                                 , V_FLOAT },
                {"car sirens SpecularFade"                                        , "car.sirens.SpecularFade"                                      , joaat("car.sirens.SpecularFade")                                    , V_FLOAT },
                {"car sirens ShadowFade"                                          , "car.sirens.ShadowFade"                                        , joaat("car.sirens.ShadowFade")                                      , V_FLOAT },
        };
        paramsMap["heli poslight"] =
        {
                {"heli poslight nearStrength"                                     , "heli.poslight.nearStrength"                                   , joaat("heli.poslight.nearStrength")                                 , V_FLOAT },
                {"heli poslight farStrength"                                      , "heli.poslight.farStrength"                                    , joaat("heli.poslight.farStrength")                                  , V_FLOAT },
                {"heli poslight nearSize"                                         , "heli.poslight.nearSize"                                       , joaat("heli.poslight.nearSize")                                     , V_FLOAT },
                {"heli poslight farSize"                                          , "heli.poslight.farSize"                                        , joaat("heli.poslight.farSize")                                      , V_FLOAT },
                {"heli poslight intensity_typeA"                                  , "heli.poslight.intensity_typeA"                                , joaat("heli.poslight.intensity_typeA")                              , V_FLOAT },
                {"heli poslight intensity_typeB"                                  , "heli.poslight.intensity_typeB"                                , joaat("heli.poslight.intensity_typeB")                              , V_FLOAT },
                {"heli poslight radius"                                           , "heli.poslight.radius"                                         , joaat("heli.poslight.radius")                                       , V_FLOAT },
                {"heli poslight rightColor"                                       , "heli.poslight.rightColor.red"                                 , joaat("heli.poslight.rightColor.red")                               , V_COL3  },
                {"None"                                                           , "heli.poslight.rightColor.green"                               , joaat("heli.poslight.rightColor.green")                             , V_NONE  },
                {"None"                                                           , "heli.poslight.rightColor.blue"                                , joaat("heli.poslight.rightColor.blue")                              , V_NONE  },
                {"heli poslight leftColor"                                        , "heli.poslight.leftColor.red"                                  , joaat("heli.poslight.leftColor.red")                                , V_COL3  },
                {"None"                                                           , "heli.poslight.leftColor.green"                                , joaat("heli.poslight.leftColor.green")                              , V_NONE  },
                {"None"                                                           , "heli.poslight.leftColor.blue"                                 , joaat("heli.poslight.leftColor.blue")                               , V_NONE  },
        };
        paramsMap["heli white head lights"] =
        {
                {"heli whiteheadlight nearStrength"                               , "heli.whiteheadlight.nearStrength"                             , joaat("heli.whiteheadlight.nearStrength")                           , V_FLOAT },
                {"heli whiteheadlight farStrength"                                , "heli.whiteheadlight.farStrength"                              , joaat("heli.whiteheadlight.farStrength")                            , V_FLOAT },
                {"heli whiteheadlight nearSize"                                   , "heli.whiteheadlight.nearSize"                                 , joaat("heli.whiteheadlight.nearSize")                               , V_FLOAT },
                {"heli whiteheadlight farSize"                                    , "heli.whiteheadlight.farSize"                                  , joaat("heli.whiteheadlight.farSize")                                , V_FLOAT },
                {"heli whiteheadlight intensity"                                  , "heli.whiteheadlight.intensity"                                , joaat("heli.whiteheadlight.intensity")                              , V_FLOAT },
                {"heli whiteheadlight radius"                                     , "heli.whiteheadlight.radius"                                   , joaat("heli.whiteheadlight.radius")                                 , V_FLOAT },
                {" 0"                                                             , ".0"                                                           , joaat(".0")                                                         , V_FLOAT },
                {"heli whiteheadlight color"                                      , "heli.whiteheadlight.color.red"                                , joaat("heli.whiteheadlight.color.red")                              , V_COL3  },
                {"None"                                                           , "heli.whiteheadlight.color.green"                              , joaat("heli.whiteheadlight.color.green")                            , V_NONE  },
                {"None"                                                           , "heli.whiteheadlight.color.blue"                               , joaat("heli.whiteheadlight.color.blue")                             , V_NONE  },
        };
        paramsMap["heli white tail lights"] =
        {
                {"heli whitetaillight nearStrength"                               , "heli.whitetaillight.nearStrength"                             , joaat("heli.whitetaillight.nearStrength")                           , V_FLOAT },
                {"heli whitetaillight farStrength"                                , "heli.whitetaillight.farStrength"                              , joaat("heli.whitetaillight.farStrength")                            , V_FLOAT },
                {"heli whitetaillight nearSize"                                   , "heli.whitetaillight.nearSize"                                 , joaat("heli.whitetaillight.nearSize")                               , V_FLOAT },
                {"heli whitetaillight farSize"                                    , "heli.whitetaillight.farSize"                                  , joaat("heli.whitetaillight.farSize")                                , V_FLOAT },
                {"heli whitetaillight intensity"                                  , "heli.whitetaillight.intensity"                                , joaat("heli.whitetaillight.intensity")                              , V_FLOAT },
                {"heli whitetaillight radius"                                     , "heli.whitetaillight.radius"                                   , joaat("heli.whitetaillight.radius")                                 , V_FLOAT },
                {"heli whitetaillight color"                                      , "heli.whitetaillight.color.red"                                , joaat("heli.whitetaillight.color.red")                              , V_COL3  },
                {"None"                                                           , "heli.whitetaillight.color.green"                              , joaat("heli.whitetaillight.color.green")                            , V_NONE  },
                {"None"                                                           , "heli.whitetaillight.color.blue"                               , joaat("heli.whitetaillight.color.blue")                             , V_NONE  },
        };
        paramsMap["heli interior light"] =
        {
                {"heli interiorlight color"                                       , "heli.interiorlight.color.red"                                 , joaat("heli.interiorlight.color.red")                               , V_COL3  },
                {"None"                                                           , "heli.interiorlight.color.green"                               , joaat("heli.interiorlight.color.green")                             , V_NONE  },
                {"None"                                                           , "heli.interiorlight.color.blue"                                , joaat("heli.interiorlight.color.blue")                              , V_NONE  },
                {"heli interiorlight intensity"                                   , "heli.interiorlight.intensity"                                 , joaat("heli.interiorlight.intensity")                               , V_FLOAT },
                {"heli interiorlight radius"                                      , "heli.interiorlight.radius"                                    , joaat("heli.interiorlight.radius")                                  , V_FLOAT },
                {"heli interiorlight innerConeAngle"                              , "heli.interiorlight.innerConeAngle"                            , joaat("heli.interiorlight.innerConeAngle")                          , V_FLOAT },
                {"heli interiorlight outerConeAngle"                              , "heli.interiorlight.outerConeAngle"                            , joaat("heli.interiorlight.outerConeAngle")                          , V_FLOAT },
                {"heli interiorlight day emissive on"                             , "heli.interiorlight.day.emissive.on"                           , joaat("heli.interiorlight.day.emissive.on")                         , V_FLOAT },
                {"heli interiorlight night emissive on"                           , "heli.interiorlight.night.emissive.on"                         , joaat("heli.interiorlight.night.emissive.on")                       , V_FLOAT },
                {"heli interiorlight day emissive off"                            , "heli.interiorlight.day.emissive.off"                          , joaat("heli.interiorlight.day.emissive.off")                        , V_FLOAT },
                {"heli interiorlight night emissive off"                          , "heli.interiorlight.night.emissive.off"                        , joaat("heli.interiorlight.night.emissive.off")                      , V_FLOAT },
                {"heli interiorlight coronaHDR"                                   , "heli.interiorlight.coronaHDR"                                 , joaat("heli.interiorlight.coronaHDR")                               , V_FLOAT },
                {"heli interiorlight coronaSize"                                  , "heli.interiorlight.coronaSize"                                , joaat("heli.interiorlight.coronaSize")                              , V_FLOAT },

        };
        paramsMap["plane poslight"] =
        {
                {"plane poslight nearStrength"                                    , "plane.poslight.nearStrength"                                  , joaat("plane.poslight.nearStrength")                                , V_FLOAT },
                {"plane poslight farStrength"                                     , "plane.poslight.farStrength"                                   , joaat("plane.poslight.farStrength")                                 , V_FLOAT },
                {"plane poslight nearSize"                                        , "plane.poslight.nearSize"                                      , joaat("plane.poslight.nearSize")                                    , V_FLOAT },
                {"plane poslight farSize"                                         , "plane.poslight.farSize"                                       , joaat("plane.poslight.farSize")                                     , V_FLOAT },
                {"plane poslight intensity_typeA"                                 , "plane.poslight.intensity_typeA"                               , joaat("plane.poslight.intensity_typeA")                             , V_FLOAT },
                {"plane poslight intensity_typeB"                                 , "plane.poslight.intensity_typeB"                               , joaat("plane.poslight.intensity_typeB")                             , V_FLOAT },
                {"plane poslight radius"                                          , "plane.poslight.radius"                                        , joaat("plane.poslight.radius")                                      , V_FLOAT },
                {"plane poslight rightColor"                                      , "plane.poslight.rightColor.red"                                , joaat("plane.poslight.rightColor.red")                              , V_COL3  },
                {"None"                                                           , "plane.poslight.rightColor.green"                              , joaat("plane.poslight.rightColor.green")                            , V_NONE  },
                {"None"                                                           , "plane.poslight.rightColor.blue"                               , joaat("plane.poslight.rightColor.blue")                             , V_NONE  },
                {"plane poslight leftColor"                                       , "plane.poslight.leftColor.red"                                 , joaat("plane.poslight.leftColor.red")                               , V_COL3  },
                {"None"                                                           , "plane.poslight.leftColor.green"                               , joaat("plane.poslight.leftColor.green")                             , V_NONE  },
                {"None"                                                           , "plane.poslight.leftColor.blue"                                , joaat("plane.poslight.leftColor.blue")                              , V_NONE  },
        };
        paramsMap["plane whiteheadlight"] =
        {
                {"plane whiteheadlight nearStrength"                              , "plane.whiteheadlight.nearStrength"                            , joaat("plane.whiteheadlight.nearStrength")                          , V_FLOAT },
                {"plane whiteheadlight farStrength"                               , "plane.whiteheadlight.farStrength"                             , joaat("plane.whiteheadlight.farStrength")                           , V_FLOAT },
                {"plane whiteheadlight nearSize"                                  , "plane.whiteheadlight.nearSize"                                , joaat("plane.whiteheadlight.nearSize")                              , V_FLOAT },
                {"plane whiteheadlight farSize"                                   , "plane.whiteheadlight.farSize"                                 , joaat("plane.whiteheadlight.farSize")                               , V_FLOAT },
                {"plane whiteheadlight intensity"                                 , "plane.whiteheadlight.intensity"                               , joaat("plane.whiteheadlight.intensity")                             , V_FLOAT },
                {"plane whiteheadlight radius"                                    , "plane.whiteheadlight.radius"                                  , joaat("plane.whiteheadlight.radius")                                , V_FLOAT },
                {"plane whiteheadlight color"                                     , "plane.whiteheadlight.color.red"                               , joaat("plane.whiteheadlight.color.red")                             , V_COL3  },
                {"None"                                                           , "plane.whiteheadlight.color.green"                             , joaat("plane.whiteheadlight.color.green")                           , V_NONE  },
                {"None"                                                           , "plane.whiteheadlight.color.blue"                              , joaat("plane.whiteheadlight.color.blue")                            , V_NONE  },
        };
        paramsMap["plane whitetaillight"] =
        {
                {"plane whitetaillight nearStrength"                              , "plane.whitetaillight.nearStrength"                            , joaat("plane.whitetaillight.nearStrength")                          , V_FLOAT },
                {"plane whitetaillight farStrength"                               , "plane.whitetaillight.farStrength"                             , joaat("plane.whitetaillight.farStrength")                           , V_FLOAT },
                {"plane whitetaillight nearSize"                                  , "plane.whitetaillight.nearSize"                                , joaat("plane.whitetaillight.nearSize")                              , V_FLOAT },
                {"plane whitetaillight farSize"                                   , "plane.whitetaillight.farSize"                                 , joaat("plane.whitetaillight.farSize")                               , V_FLOAT },
                {"plane whitetaillight intensity"                                 , "plane.whitetaillight.intensity"                               , joaat("plane.whitetaillight.intensity")                             , V_FLOAT },
                {"plane whitetaillight radius"                                    , "plane.whitetaillight.radius"                                  , joaat("plane.whitetaillight.radius")                                , V_FLOAT },
                {"plane whitetaillight color"                                     , "plane.whitetaillight.color.red"                               , joaat("plane.whitetaillight.color.red")                             , V_COL3  },
                {"None"                                                           , "plane.whitetaillight.color.green"                             , joaat("plane.whitetaillight.color.green")                           , V_NONE  },
                {"None"                                                           , "plane.whitetaillight.color.blue"                              , joaat("plane.whitetaillight.color.blue")                            , V_NONE  },
        };
        paramsMap["plane control-panel light"] =
        {
                {"plane controlpanel light color"                                 , "plane.controlpanel.light.color.red"                           , joaat("plane.controlpanel.light.color.red")                         , V_COL3  },
                {"None"                                                           , "plane.controlpanel.light.color.green"                         , joaat("plane.controlpanel.light.color.green")                       , V_NONE  },
                {"None"                                                           , "plane.controlpanel.light.color.blue"                          , joaat("plane.controlpanel.light.color.blue")                        , V_NONE  },
                {"plane controlpanel light intensity"                             , "plane.controlpanel.light.intensity"                           , joaat("plane.controlpanel.light.intensity")                         , V_FLOAT },
                {"plane controlpanel light falloff"                               , "plane.controlpanel.light.falloff"                             , joaat("plane.controlpanel.light.falloff")                           , V_FLOAT },
                {"plane controlpanel light falloff exponent"                      , "plane.controlpanel.light.falloff.exponent"                    , joaat("plane.controlpanel.light.falloff.exponent")                  , V_FLOAT },
        };
        paramsMap["plane right emergency light"] =
        {
                {"plane emergency right light color"                              , "plane.emergency.right.light.color.red"                        , joaat("plane.emergency.right.light.color.red")                      , V_COL3  },
                {"None"                                                           , "plane.emergency.right.light.color.green"                      , joaat("plane.emergency.right.light.color.green")                    , V_NONE  },
                {"None"                                                           , "plane.emergency.right.light.color.blue"                       , joaat("plane.emergency.right.light.color.blue")                     , V_NONE  },
                {"plane emergency right light intensity"                          , "plane.emergency.right.light.intensity"                        , joaat("plane.emergency.right.light.intensity")                      , V_FLOAT },
                {"plane emergency right light falloff"                            , "plane.emergency.right.light.falloff"                          , joaat("plane.emergency.right.light.falloff")                        , V_FLOAT },
                {"plane emergency right light falloff exponent"                   , "plane.emergency.right.light.falloff.exponent"                 , joaat("plane.emergency.right.light.falloff.exponent")               , V_FLOAT },
                {"plane emergency right light inner angle"                        , "plane.emergency.right.light.inner.angle"                      , joaat("plane.emergency.right.light.inner.angle")                    , V_FLOAT },
                {"plane emergency right light outer angle"                        , "plane.emergency.right.light.outer.angle"                      , joaat("plane.emergency.right.light.outer.angle")                    , V_FLOAT },
                {"plane emergency right light rotation"                           , "plane.emergency.right.light.rotation"                         , joaat("plane.emergency.right.light.rotation")                       , V_FLOAT },
        };
        paramsMap["plane left emergency light"] =
        {
                {"plane emergency left light color"                               , "plane.emergency.left.light.color.red"                         , joaat("plane.emergency.left.light.color.red")                       , V_COL3  },
                {"None"                                                           , "plane.emergency.left.light.color.green"                       , joaat("plane.emergency.left.light.color.green")                     , V_NONE  },
                {"None"                                                           , "plane.emergency.left.light.color.blue"                        , joaat("plane.emergency.left.light.color.blue")                      , V_NONE  },
                {"plane emergency left light intensity"                           , "plane.emergency.left.light.intensity"                         , joaat("plane.emergency.left.light.intensity")                       , V_FLOAT },
                {"plane emergency left light falloff"                             , "plane.emergency.left.light.falloff"                           , joaat("plane.emergency.left.light.falloff")                         , V_FLOAT },
                {"plane emergency left light falloff exponent"                    , "plane.emergency.left.light.falloff.exponent"                  , joaat("plane.emergency.left.light.falloff.exponent")                , V_FLOAT },
                {"plane emergency left light inner angle"                         , "plane.emergency.left.light.inner.angle"                       , joaat("plane.emergency.left.light.inner.angle")                     , V_FLOAT },
                {"plane emergency left light outer angle"                         , "plane.emergency.left.light.outer.angle"                       , joaat("plane.emergency.left.light.outer.angle")                     , V_FLOAT },
                {"plane emergency left light rotation"                            , "plane.emergency.left.light.rotation"                          , joaat("plane.emergency.left.light.rotation")                        , V_FLOAT },
        };
        paramsMap["plane inside hull light"] =
        {
                {"plane insidehull light color"                                   , "plane.insidehull.light.color.red"                             , joaat("plane.insidehull.light.color.red")                           , V_COL3  },
                {"None"                                                           , "plane.insidehull.light.color.green"                           , joaat("plane.insidehull.light.color.green")                         , V_NONE  },
                {"None"                                                           , "plane.insidehull.light.color.blue"                            , joaat("plane.insidehull.light.color.blue")                          , V_NONE  },
                {"plane insidehull light intensity"                               , "plane.insidehull.light.intensity"                             , joaat("plane.insidehull.light.intensity")                           , V_FLOAT },
                {"plane insidehull light falloff"                                 , "plane.insidehull.light.falloff"                               , joaat("plane.insidehull.light.falloff")                             , V_FLOAT },
                {"plane insidehull light falloff exponent"                        , "plane.insidehull.light.falloff.exponent"                      , joaat("plane.insidehull.light.falloff.exponent")                    , V_FLOAT },
                {"plane insidehull light inner angle"                             , "plane.insidehull.light.inner.angle"                           , joaat("plane.insidehull.light.inner.angle")                         , V_FLOAT },
                {"plane insidehull light outer angle"                             , "plane.insidehull.light.outer.angle"                           , joaat("plane.insidehull.light.outer.angle")                         , V_FLOAT },
                {"plane luxe2 cabin color"                                        , "plane.luxe2.cabin.color.red"                                  , joaat("plane.luxe2.cabin.color.red")                                , V_COL3  },
                {"None"                                                           , "plane.luxe2.cabin.color.green"                                , joaat("plane.luxe2.cabin.color.green")                              , V_NONE  },
                {"None"                                                           , "plane.luxe2.cabin.color.blue"                                 , joaat("plane.luxe2.cabin.color.blue")                               , V_NONE  },
                {"plane luxe2 cabin intensity"                                    , "plane.luxe2.cabin.intensity"                                  , joaat("plane.luxe2.cabin.intensity")                                , V_FLOAT },
                {"plane luxe2 cabin radius"                                       , "plane.luxe2.cabin.radius"                                     , joaat("plane.luxe2.cabin.radius")                                   , V_FLOAT },
                {"plane luxe2 cabin innerConeAngle"                               , "plane.luxe2.cabin.innerConeAngle"                             , joaat("plane.luxe2.cabin.innerConeAngle")                           , V_FLOAT },
                {"plane luxe2 cabin outerConeAngle"                               , "plane.luxe2.cabin.outerConeAngle"                             , joaat("plane.luxe2.cabin.outerConeAngle")                           , V_FLOAT },
                {"plane luxe2 cabin falloffExp"                                   , "plane.luxe2.cabin.falloffExp"                                 , joaat("plane.luxe2.cabin.falloffExp")                               , V_FLOAT },
                {"plane luxe2 cabin useDynamicShadows"                            , "plane.luxe2.cabin.useDynamicShadows"                          , joaat("plane.luxe2.cabin.useDynamicShadows")                        , V_FLOAT },
                {"plane luxe2 cabin strip color"                                  , "plane.luxe2.cabin.strip.color.red"                            , joaat("plane.luxe2.cabin.strip.color.red")                          , V_COL3  },
                {"None"                                                           , "plane.luxe2.cabin.strip.color.green"                          , joaat("plane.luxe2.cabin.strip.color.green")                        , V_NONE  },
                {"None"                                                           , "plane.luxe2.cabin.strip.color.blue"                           , joaat("plane.luxe2.cabin.strip.color.blue")                         , V_NONE  },
                {"plane luxe2 cabin strip intensity"                              , "plane.luxe2.cabin.strip.intensity"                            , joaat("plane.luxe2.cabin.strip.intensity")                          , V_FLOAT },
                {"plane luxe2 cabin strip radius"                                 , "plane.luxe2.cabin.strip.radius"                               , joaat("plane.luxe2.cabin.strip.radius")                             , V_FLOAT },
                {"plane luxe2 cabin strip falloffExp"                             , "plane.luxe2.cabin.strip.falloffExp"                           , joaat("plane.luxe2.cabin.strip.falloffExp")                         , V_FLOAT },
                {"plane luxe2 cabin strip capsuleLength"                          , "plane.luxe2.cabin.strip.capsuleLength"                        , joaat("plane.luxe2.cabin.strip.capsuleLength")                      , V_FLOAT },
                {"plane luxe2 cabin tv color"                                     , "plane.luxe2.cabin.tv.color.red"                               , joaat("plane.luxe2.cabin.tv.color.red")                             , V_COL3  },
                {"None"                                                           , "plane.luxe2.cabin.tv.color.green"                             , joaat("plane.luxe2.cabin.tv.color.green")                           , V_NONE  },
                {"None"                                                           , "plane.luxe2.cabin.tv.color.blue"                              , joaat("plane.luxe2.cabin.tv.color.blue")                            , V_NONE  },
                {"plane luxe2 cabin tv intensity"                                 , "plane.luxe2.cabin.tv.intensity"                               , joaat("plane.luxe2.cabin.tv.intensity")                             , V_FLOAT },
                {"plane luxe2 cabin tv radius"                                    , "plane.luxe2.cabin.tv.radius"                                  , joaat("plane.luxe2.cabin.tv.radius")                                , V_FLOAT },
                {"plane luxe2 cabin tv innerConeAngle"                            , "plane.luxe2.cabin.tv.innerConeAngle"                          , joaat("plane.luxe2.cabin.tv.innerConeAngle")                        , V_FLOAT },
                {"plane luxe2 cabin tv outerConeAngle"                            , "plane.luxe2.cabin.tv.outerConeAngle"                          , joaat("plane.luxe2.cabin.tv.outerConeAngle")                        , V_FLOAT },
                {"plane luxe2 cabin tv falloffExp"                                , "plane.luxe2.cabin.tv.falloffExp"                              , joaat("plane.luxe2.cabin.tv.falloffExp")                            , V_FLOAT },
                {"plane luxe2 cabin lod color"                                    , "plane.luxe2.cabin.lod.color.red"                              , joaat("plane.luxe2.cabin.lod.color.red")                            , V_COL3  },
                {"None"                                                           , "plane.luxe2.cabin.lod.color.green"                            , joaat("plane.luxe2.cabin.lod.color.green")                          , V_NONE  },
                {"None"                                                           , "plane.luxe2.cabin.lod.color.blue"                             , joaat("plane.luxe2.cabin.lod.color.blue")                           , V_NONE  },
                {"plane luxe2 cabin lod intensity"                                , "plane.luxe2.cabin.lod.intensity"                              , joaat("plane.luxe2.cabin.lod.intensity")                            , V_FLOAT },
                {"plane luxe2 cabin lod radius"                                   , "plane.luxe2.cabin.lod.radius"                                 , joaat("plane.luxe2.cabin.lod.radius")                               , V_FLOAT },
                {"plane luxe2 cabin lod falloffExp"                               , "plane.luxe2.cabin.lod.falloffExp"                             , joaat("plane.luxe2.cabin.lod.falloffExp")                           , V_FLOAT },
                {"plane luxe2 cabin lod capsuleLength"                            , "plane.luxe2.cabin.lod.capsuleLength"                          , joaat("plane.luxe2.cabin.lod.capsuleLength")                        , V_FLOAT },
                {"plane luxe2 cabin window color"                                 , "plane.luxe2.cabin.window.color.red"                           , joaat("plane.luxe2.cabin.window.color.red")                         , V_COL3  },
                {"None"                                                           , "plane.luxe2.cabin.window.color.green"                         , joaat("plane.luxe2.cabin.window.color.green")                       , V_NONE  },
                {"None"                                                           , "plane.luxe2.cabin.window.color.blue"                          , joaat("plane.luxe2.cabin.window.color.blue")                        , V_NONE  },
                {"plane luxe2 cabin window intensity"                             , "plane.luxe2.cabin.window.intensity"                           , joaat("plane.luxe2.cabin.window.intensity")                         , V_FLOAT },
                {"plane luxe2 cabin window radius"                                , "plane.luxe2.cabin.window.radius"                              , joaat("plane.luxe2.cabin.window.radius")                            , V_FLOAT },
                {"plane luxe2 cabin window innerConeAngle"                        , "plane.luxe2.cabin.window.innerConeAngle"                      , joaat("plane.luxe2.cabin.window.innerConeAngle")                    , V_FLOAT },
                {"plane luxe2 cabin window outerConeAngle"                        , "plane.luxe2.cabin.window.outerConeAngle"                      , joaat("plane.luxe2.cabin.window.outerConeAngle")                    , V_FLOAT },
                {"plane luxe2 cabin window falloffExp"                            , "plane.luxe2.cabin.window.falloffExp"                          , joaat("plane.luxe2.cabin.window.falloffExp")                        , V_FLOAT },
                {"plane luxe2 cabin window useSun"                                , "plane.luxe2.cabin.window.useSun"                              , joaat("plane.luxe2.cabin.window.useSun")                            , V_FLOAT },
                {"heli swift2 cabin color"                                        , "heli.swift2.cabin.color.red"                                  , joaat("heli.swift2.cabin.color.red")                                , V_COL3  },
                {"None"                                                           , "heli.swift2.cabin.color.green"                                , joaat("heli.swift2.cabin.color.green")                              , V_NONE  },
                {"None"                                                           , "heli.swift2.cabin.color.blue"                                 , joaat("heli.swift2.cabin.color.blue")                               , V_NONE  },
                {"heli swift2 cabin intensity"                                    , "heli.swift2.cabin.intensity"                                  , joaat("heli.swift2.cabin.intensity")                                , V_FLOAT },
                {"heli swift2 cabin radius"                                       , "heli.swift2.cabin.radius"                                     , joaat("heli.swift2.cabin.radius")                                   , V_FLOAT },
                {"heli swift2 cabin innerConeAngle"                               , "heli.swift2.cabin.innerConeAngle"                             , joaat("heli.swift2.cabin.innerConeAngle")                           , V_FLOAT },
                {"heli swift2 cabin outerConeAngle"                               , "heli.swift2.cabin.outerConeAngle"                             , joaat("heli.swift2.cabin.outerConeAngle")                           , V_FLOAT },
                {"heli swift2 cabin falloffExp"                                   , "heli.swift2.cabin.falloffExp"                                 , joaat("heli.swift2.cabin.falloffExp")                               , V_FLOAT },
                {"heli swift2 cabin useDynamicShadows"                            , "heli.swift2.cabin.useDynamicShadows"                          , joaat("heli.swift2.cabin.useDynamicShadows")                        , V_FLOAT },
        };
        paramsMap["boat lights"] =
        {
                {"boat intensity"                                                 , "boat.intensity"                                               , joaat("boat.intensity")                                             , V_FLOAT },
                {"boat radius"                                                    , "boat.radius"                                                  , joaat("boat.radius")                                                , V_FLOAT },
                {"boat color"                                                     , "boat.color.red"                                               , joaat("boat.color.red")                                             , V_COL3  },
                {"None"                                                           , "boat.color.green"                                             , joaat("boat.color.green")                                           , V_NONE  },
                {"None"                                                           , "boat.color.blue"                                              , joaat("boat.color.blue")                                            , V_NONE  },
                {"boat light interiorshutdowndistance"                            , "boat.light.interiorshutdowndistance"                          , joaat("boat.light.interiorshutdowndistance")                        , V_FLOAT },
                {"boat light fadelength"                                          , "boat.light.fadelength"                                        , joaat("boat.light.fadelength")                                      , V_FLOAT },
                {"boat light shutdowndistance"                                    , "boat.light.shutdowndistance"                                  , joaat("boat.light.shutdowndistance")                                , V_FLOAT },
                {"boat corona fadelength"                                         , "boat.corona.fadelength"                                       , joaat("boat.corona.fadelength")                                     , V_FLOAT },
                {"boat corona size"                                               , "boat.corona.size"                                             , joaat("boat.corona.size")                                           , V_FLOAT },
                {"boat corona intensity"                                          , "boat.corona.intensity"                                        , joaat("boat.corona.intensity")                                      , V_FLOAT },
                {"boat corona zBias"                                              , "boat.corona.zBias"                                            , joaat("boat.corona.zBias")                                          , V_FLOAT },
                {"sub lightOne color"                                             , "sub.lightOne.color.red"                                       , joaat("sub.lightOne.color.red")                                     , V_COL3  },
                {"None"                                                           , "sub.lightOne.color.green"                                     , joaat("sub.lightOne.color.green")                                   , V_NONE  },
                {"None"                                                           , "sub.lightOne.color.blue"                                      , joaat("sub.lightOne.color.blue")                                    , V_NONE  },
                {"sub lightOne intensity"                                         , "sub.lightOne.intensity"                                       , joaat("sub.lightOne.intensity")                                     , V_FLOAT },
                {"sub lightOne radius"                                            , "sub.lightOne.radius"                                          , joaat("sub.lightOne.radius")                                        , V_FLOAT },
                {"sub lightOne falloffExp"                                        , "sub.lightOne.falloffExp"                                      , joaat("sub.lightOne.falloffExp")                                    , V_FLOAT },
                {"sub lightOne innerConeAngle"                                    , "sub.lightOne.innerConeAngle"                                  , joaat("sub.lightOne.innerConeAngle")                                , V_FLOAT },
                {"sub lightOne outerConeAngle"                                    , "sub.lightOne.outerConeAngle"                                  , joaat("sub.lightOne.outerConeAngle")                                , V_FLOAT },
                {"sub lightOne coronaHDR"                                         , "sub.lightOne.coronaHDR"                                       , joaat("sub.lightOne.coronaHDR")                                     , V_FLOAT },
                {"sub lightOne coronaSize"                                        , "sub.lightOne.coronaSize"                                      , joaat("sub.lightOne.coronaSize")                                    , V_FLOAT },
                {"sub lightTwo color"                                             , "sub.lightTwo.color.red"                                       , joaat("sub.lightTwo.color.red")                                     , V_COL3  },
                {"None"                                                           , "sub.lightTwo.color.green"                                     , joaat("sub.lightTwo.color.green")                                   , V_NONE  },
                {"None"                                                           , "sub.lightTwo.color.blue"                                      , joaat("sub.lightTwo.color.blue")                                    , V_NONE  },
                {"sub lightTwo intensity"                                         , "sub.lightTwo.intensity"                                       , joaat("sub.lightTwo.intensity")                                     , V_FLOAT },
                {"sub lightTwo radius"                                            , "sub.lightTwo.radius"                                          , joaat("sub.lightTwo.radius")                                        , V_FLOAT },
                {"sub lightTwo falloffExp"                                        , "sub.lightTwo.falloffExp"                                      , joaat("sub.lightTwo.falloffExp")                                    , V_FLOAT },
                {"sub lightTwo innerConeAngle"                                    , "sub.lightTwo.innerConeAngle"                                  , joaat("sub.lightTwo.innerConeAngle")                                , V_FLOAT },
                {"sub lightTwo outerConeAngle"                                    , "sub.lightTwo.outerConeAngle"                                  , joaat("sub.lightTwo.outerConeAngle")                                , V_FLOAT },
                {"sub lightTwo coronaHDR"                                         , "sub.lightTwo.coronaHDR"                                       , joaat("sub.lightTwo.coronaHDR")                                     , V_FLOAT },
                {"sub lightTwo coronaSize"                                        , "sub.lightTwo.coronaSize"                                      , joaat("sub.lightTwo.coronaSize")                                    , V_FLOAT },
        };
        paramsMap["train settings"] =
        {
                {"train light"                                                    , "train.light.r"                                                , joaat("train.light.r")                                              , V_COL3  },
                {"train light"                                                    , "train.light.g"                                                , joaat("train.light.g")                                              , V_NONE  },
                {"train light"                                                    , "train.light.b"                                                , joaat("train.light.b")                                              , V_NONE  },
                {"train light intensity"                                          , "train.light.intensity"                                        , joaat("train.light.intensity")                                      , V_FLOAT },
                {"train light falloffmax"                                         , "train.light.falloffmax"                                       , joaat("train.light.falloffmax")                                     , V_FLOAT },
                {"train light fadingdistance"                                     , "train.light.fadingdistance"                                   , joaat("train.light.fadingdistance")                                 , V_FLOAT },
                {"train light fadelength"                                         , "train.light.fadelength"                                       , joaat("train.light.fadelength")                                     , V_FLOAT },
                {"train ambientvolume intensityscaler"                            , "train.ambientvolume.intensityscaler"                          , joaat("train.ambientvolume.intensityscaler")                        , V_FLOAT },
        };
        paramsMap["emissive bits"] =
        {
                {"car headlight day emissive on"                                  , "car.headlight.day.emissive.on"                                , joaat("car.headlight.day.emissive.on")                              , V_FLOAT },
                {"car headlight night emissive on"                                , "car.headlight.night.emissive.on"                              , joaat("car.headlight.night.emissive.on")                            , V_FLOAT },
                {"car headlight day emissive off"                                 , "car.headlight.day.emissive.off"                               , joaat("car.headlight.day.emissive.off")                             , V_FLOAT },
                {"car headlight night emissive off"                               , "car.headlight.night.emissive.off"                             , joaat("car.headlight.night.emissive.off")                           , V_FLOAT },
                {"car taillight day emissive on"                                  , "car.taillight.day.emissive.on"                                , joaat("car.taillight.day.emissive.on")                              , V_FLOAT },
                {"car taillight night emissive on"                                , "car.taillight.night.emissive.on"                              , joaat("car.taillight.night.emissive.on")                            , V_FLOAT },
                {"car taillight day emissive off"                                 , "car.taillight.day.emissive.off"                               , joaat("car.taillight.day.emissive.off")                             , V_FLOAT },
                {"car taillight night emissive off"                               , "car.taillight.night.emissive.off"                             , joaat("car.taillight.night.emissive.off")                           , V_FLOAT },
                {"car indicator day emissive on"                                  , "car.indicator.day.emissive.on"                                , joaat("car.indicator.day.emissive.on")                              , V_FLOAT },
                {"car indicator night emissive on"                                , "car.indicator.night.emissive.on"                              , joaat("car.indicator.night.emissive.on")                            , V_FLOAT },
                {"car indicator day emissive off"                                 , "car.indicator.day.emissive.off"                               , joaat("car.indicator.day.emissive.off")                             , V_FLOAT },
                {"car indicator night emissive off"                               , "car.indicator.night.emissive.off"                             , joaat("car.indicator.night.emissive.off")                           , V_FLOAT },
                {"car reversinglight day emissive on"                             , "car.reversinglight.day.emissive.on"                           , joaat("car.reversinglight.day.emissive.on")                         , V_FLOAT },
                {"car reversinglight night emissive on"                           , "car.reversinglight.night.emissive.on"                         , joaat("car.reversinglight.night.emissive.on")                       , V_FLOAT },
                {"car reversinglight day emissive off"                            , "car.reversinglight.day.emissive.off"                          , joaat("car.reversinglight.day.emissive.off")                        , V_FLOAT },
                {"car reversinglight night emissive off"                          , "car.reversinglight.night.emissive.off"                        , joaat("car.reversinglight.night.emissive.off")                      , V_FLOAT },
                {"car defaultlight day emissive on"                               , "car.defaultlight.day.emissive.on"                             , joaat("car.defaultlight.day.emissive.on")                           , V_FLOAT },
                {"car defaultlight night emissive on"                             , "car.defaultlight.night.emissive.on"                           , joaat("car.defaultlight.night.emissive.on")                         , V_FLOAT },
                {"car defaultlight day emissive off"                              , "car.defaultlight.day.emissive.off"                            , joaat("car.defaultlight.day.emissive.off")                          , V_FLOAT },
                {"car defaultlight night emissive off"                            , "car.defaultlight.night.emissive.off"                          , joaat("car.defaultlight.night.emissive.off")                        , V_FLOAT },
                {"car brakelight day emissive on"                                 , "car.brakelight.day.emissive.on"                               , joaat("car.brakelight.day.emissive.on")                             , V_FLOAT },
                {"car brakelight night emissive on"                               , "car.brakelight.night.emissive.on"                             , joaat("car.brakelight.night.emissive.on")                           , V_FLOAT },
                {"car brakelight day emissive off"                                , "car.brakelight.day.emissive.off"                              , joaat("car.brakelight.day.emissive.off")                            , V_FLOAT },
                {"car brakelight night emissive off"                              , "car.brakelight.night.emissive.off"                            , joaat("car.brakelight.night.emissive.off")                          , V_FLOAT },
                {"car middlebrakelight day emissive on"                           , "car.middlebrakelight.day.emissive.on"                         , joaat("car.middlebrakelight.day.emissive.on")                       , V_FLOAT },
                {"car middlebrakelight night emissive on"                         , "car.middlebrakelight.night.emissive.on"                       , joaat("car.middlebrakelight.night.emissive.on")                     , V_FLOAT },
                {"car middlebrakelight day emissive off"                          , "car.middlebrakelight.day.emissive.off"                        , joaat("car.middlebrakelight.day.emissive.off")                      , V_FLOAT },
                {"car middlebrakelight night emissive off"                        , "car.middlebrakelight.night.emissive.off"                      , joaat("car.middlebrakelight.night.emissive.off")                    , V_FLOAT },
                {"car extralight day emissive on"                                 , "car.extralight.day.emissive.on"                               , joaat("car.extralight.day.emissive.on")                             , V_FLOAT },
                {"car extralight night emissive on"                               , "car.extralight.night.emissive.on"                             , joaat("car.extralight.night.emissive.on")                           , V_FLOAT },
                {"car extralight day emissive off"                                , "car.extralight.day.emissive.off"                              , joaat("car.extralight.day.emissive.off")                            , V_FLOAT },
                {"car extralight night emissive off"                              , "car.extralight.night.emissive.off"                            , joaat("car.extralight.night.emissive.off")                          , V_FLOAT },
                {"car sirenlight day emissive on"                                 , "car.sirenlight.day.emissive.on"                               , joaat("car.sirenlight.day.emissive.on")                             , V_FLOAT },
                {"car sirenlight night emissive on"                               , "car.sirenlight.night.emissive.on"                             , joaat("car.sirenlight.night.emissive.on")                           , V_FLOAT },
                {"car sirenlight day emissive off"                                , "car.sirenlight.day.emissive.off"                              , joaat("car.sirenlight.day.emissive.off")                            , V_FLOAT },
                {"car sirenlight night emissive off"                              , "car.sirenlight.night.emissive.off"                            , joaat("car.sirenlight.night.emissive.off")                          , V_FLOAT },
                {"car emissiveMultiplier"                                         , "car.emissiveMultiplier"                                       , joaat("car.emissiveMultiplier")                                     , V_FLOAT },
        };
        paramsMap["Lod distances for vehicles"] =
        {
                {"car lod distance high"                                          , "car.lod.distance.high"                                        , joaat("car.lod.distance.high")                                      , V_FLOAT },
                {"car lod distance low"                                           , "car.lod.distance.low"                                         , joaat("car.lod.distance.low")                                       , V_FLOAT },
        };
        paramsMap["search lights"] =
        {
                {"defaultsearchlight length"                                      , "defaultsearchlight.length"                                    , joaat("defaultsearchlight.length")                                  , V_FLOAT },
                {"defaultsearchlight offset"                                      , "defaultsearchlight.offset"                                    , joaat("defaultsearchlight.offset")                                  , V_FLOAT },
                {"defaultsearchlight color"                                       , "defaultsearchlight.color.red"                                 , joaat("defaultsearchlight.color.red")                               , V_COL3  },
                {"None"                                                           , "defaultsearchlight.color.green"                               , joaat("defaultsearchlight.color.green")                             , V_NONE  },
                {"None"                                                           , "defaultsearchlight.color.blue"                                , joaat("defaultsearchlight.color.blue")                              , V_NONE  },
                {"defaultsearchlight mainLightInfo falloffScale"                  , "defaultsearchlight.mainLightInfo.falloffScale"                , joaat("defaultsearchlight.mainLightInfo.falloffScale")              , V_FLOAT },
                {"defaultsearchlight mainLightInfo falloffExp"                    , "defaultsearchlight.mainLightInfo.falloffExp"                  , joaat("defaultsearchlight.mainLightInfo.falloffExp")                , V_FLOAT },
                {"defaultsearchlight mainLightInfo innerAngle"                    , "defaultsearchlight.mainLightInfo.innerAngle"                  , joaat("defaultsearchlight.mainLightInfo.innerAngle")                , V_FLOAT },
                {"defaultsearchlight mainLightInfo outerAngle"                    , "defaultsearchlight.mainLightInfo.outerAngle"                  , joaat("defaultsearchlight.mainLightInfo.outerAngle")                , V_FLOAT },
                {"defaultsearchlight mainLightInfo globalIntensity"               , "defaultsearchlight.mainLightInfo.globalIntensity"             , joaat("defaultsearchlight.mainLightInfo.globalIntensity")           , V_FLOAT },
                {"defaultsearchlight mainLightInfo lightIntensityScale"           , "defaultsearchlight.mainLightInfo.lightIntensityScale"         , joaat("defaultsearchlight.mainLightInfo.lightIntensityScale")       , V_FLOAT },
                {"defaultsearchlight mainLightInfo volumeIntensityScale"          , "defaultsearchlight.mainLightInfo.volumeIntensityScale"        , joaat("defaultsearchlight.mainLightInfo.volumeIntensityScale")      , V_FLOAT },
                {"defaultsearchlight mainLightInfo volumeSizeScale"               , "defaultsearchlight.mainLightInfo.volumeSizeScale"             , joaat("defaultsearchlight.mainLightInfo.volumeSizeScale")           , V_FLOAT },
                {"defaultsearchlight mainLightInfo outerVolumeColor color"        , "defaultsearchlight.mainLightInfo.outerVolumeColor.color.red"  , joaat("defaultsearchlight.mainLightInfo.outerVolumeColor.color.red"), V_COL3  },
                {"None"                                                           , "defaultsearchlight.mainLightInfo.outerVolumeColor.color.green", joaat("defaultsearchlight.mainLightInfo.outerVolumeColor.color.green"), V_NONE  },
                {"None"                                                           , "defaultsearchlight.mainLightInfo.outerVolumeColor.color.blue" , joaat("defaultsearchlight.mainLightInfo.outerVolumeColor.color.blue"), V_NONE  },
                {"defaultsearchlight mainLightInfo outerVolumeIntensity"          , "defaultsearchlight.mainLightInfo.outerVolumeIntensity"        , joaat("defaultsearchlight.mainLightInfo.outerVolumeIntensity")      , V_FLOAT },
                {"defaultsearchlight mainLightInfo outerVolumeFallOffExponent"    , "defaultsearchlight.mainLightInfo.outerVolumeFallOffExponent"  , joaat("defaultsearchlight.mainLightInfo.outerVolumeFallOffExponent"), V_FLOAT },
                {"defaultsearchlight mainLightInfo enable"                        , "defaultsearchlight.mainLightInfo.enable"                      , joaat("defaultsearchlight.mainLightInfo.enable")                    , V_FLOAT },
                {"defaultsearchlight mainLightInfo specular"                      , "defaultsearchlight.mainLightInfo.specular"                    , joaat("defaultsearchlight.mainLightInfo.specular")                  , V_FLOAT },
                {"defaultsearchlight mainLightInfo shadow"                        , "defaultsearchlight.mainLightInfo.shadow"                      , joaat("defaultsearchlight.mainLightInfo.shadow")                    , V_FLOAT },
                {"defaultsearchlight mainLightInfo volume"                        , "defaultsearchlight.mainLightInfo.volume"                      , joaat("defaultsearchlight.mainLightInfo.volume")                    , V_FLOAT },
                {"defaultsearchlight mainLightInfo coronaIntensity"               , "defaultsearchlight.mainLightInfo.coronaIntensity"             , joaat("defaultsearchlight.mainLightInfo.coronaIntensity")           , V_FLOAT },
                {"defaultsearchlight mainLightInfo coronaIntensityFar"            , "defaultsearchlight.mainLightInfo.coronaIntensityFar"          , joaat("defaultsearchlight.mainLightInfo.coronaIntensityFar")        , V_FLOAT },
                {"defaultsearchlight mainLightInfo coronaSize"                    , "defaultsearchlight.mainLightInfo.coronaSize"                  , joaat("defaultsearchlight.mainLightInfo.coronaSize")                , V_FLOAT },
                {"defaultsearchlight mainLightInfo coronaSizeFar"                 , "defaultsearchlight.mainLightInfo.coronaSizeFar"               , joaat("defaultsearchlight.mainLightInfo.coronaSizeFar")             , V_FLOAT },
                {"defaultsearchlight mainLightInfo coronaZBias"                   , "defaultsearchlight.mainLightInfo.coronaZBias"                 , joaat("defaultsearchlight.mainLightInfo.coronaZBias")               , V_FLOAT },
                {"defaultsearchlight mainLightInfo coronaOffset"                  , "defaultsearchlight.mainLightInfo.coronaOffset"                , joaat("defaultsearchlight.mainLightInfo.coronaOffset")              , V_FLOAT },
                {"defaultsearchlight mainLightInfo lightFadeDist"                 , "defaultsearchlight.mainLightInfo.lightFadeDist"               , joaat("defaultsearchlight.mainLightInfo.lightFadeDist")             , V_FLOAT },
                {"helisearchlight length"                                         , "helisearchlight.length"                                       , joaat("helisearchlight.length")                                     , V_FLOAT },
                {"helisearchlight offset"                                         , "helisearchlight.offset"                                       , joaat("helisearchlight.offset")                                     , V_FLOAT },
                {"helisearchlight color"                                          , "helisearchlight.color.red"                                    , joaat("helisearchlight.color.red")                                  , V_COL3  },
                {"None"                                                           , "helisearchlight.color.green"                                  , joaat("helisearchlight.color.green")                                , V_NONE  },
                {"None"                                                           , "helisearchlight.color.blue"                                   , joaat("helisearchlight.color.blue")                                 , V_NONE  },
                {"helisearchlight mainLightInfo falloffScale"                     , "helisearchlight.mainLightInfo.falloffScale"                   , joaat("helisearchlight.mainLightInfo.falloffScale")                 , V_FLOAT },
                {"helisearchlight mainLightInfo falloffExp"                       , "helisearchlight.mainLightInfo.falloffExp"                     , joaat("helisearchlight.mainLightInfo.falloffExp")                   , V_FLOAT },
                {"helisearchlight mainLightInfo innerAngle"                       , "helisearchlight.mainLightInfo.innerAngle"                     , joaat("helisearchlight.mainLightInfo.innerAngle")                   , V_FLOAT },
                {"helisearchlight mainLightInfo outerAngle"                       , "helisearchlight.mainLightInfo.outerAngle"                     , joaat("helisearchlight.mainLightInfo.outerAngle")                   , V_FLOAT },
                {"helisearchlight mainLightInfo globalIntensity"                  , "helisearchlight.mainLightInfo.globalIntensity"                , joaat("helisearchlight.mainLightInfo.globalIntensity")              , V_FLOAT },
                {"helisearchlight mainLightInfo lightIntensityScale"              , "helisearchlight.mainLightInfo.lightIntensityScale"            , joaat("helisearchlight.mainLightInfo.lightIntensityScale")          , V_FLOAT },
                {"helisearchlight mainLightInfo volumeIntensityScale"             , "helisearchlight.mainLightInfo.volumeIntensityScale"           , joaat("helisearchlight.mainLightInfo.volumeIntensityScale")         , V_FLOAT },
                {"helisearchlight mainLightInfo volumeSizeScale"                  , "helisearchlight.mainLightInfo.volumeSizeScale"                , joaat("helisearchlight.mainLightInfo.volumeSizeScale")              , V_FLOAT },
                {"helisearchlight mainLightInfo outerVolumeColor color"           , "helisearchlight.mainLightInfo.outerVolumeColor.color.red"     , joaat("helisearchlight.mainLightInfo.outerVolumeColor.color.red")   , V_COL3  },
                {"None"                                                           , "helisearchlight.mainLightInfo.outerVolumeColor.color.green"   , joaat("helisearchlight.mainLightInfo.outerVolumeColor.color.green") , V_NONE  },
                {"None"                                                           , "helisearchlight.mainLightInfo.outerVolumeColor.color.blue"    , joaat("helisearchlight.mainLightInfo.outerVolumeColor.color.blue")  , V_NONE  },
                {"helisearchlight mainLightInfo outerVolumeIntensity"             , "helisearchlight.mainLightInfo.outerVolumeIntensity"           , joaat("helisearchlight.mainLightInfo.outerVolumeIntensity")         , V_FLOAT },
                {"helisearchlight mainLightInfo outerVolumeFallOffExponent"       , "helisearchlight.mainLightInfo.outerVolumeFallOffExponent"     , joaat("helisearchlight.mainLightInfo.outerVolumeFallOffExponent")   , V_FLOAT },
                {"helisearchlight mainLightInfo enable"                           , "helisearchlight.mainLightInfo.enable"                         , joaat("helisearchlight.mainLightInfo.enable")                       , V_FLOAT },
                {"helisearchlight mainLightInfo specular"                         , "helisearchlight.mainLightInfo.specular"                       , joaat("helisearchlight.mainLightInfo.specular")                     , V_FLOAT },
                {"helisearchlight mainLightInfo shadow"                           , "helisearchlight.mainLightInfo.shadow"                         , joaat("helisearchlight.mainLightInfo.shadow")                       , V_FLOAT },
                {"helisearchlight mainLightInfo volume"                           , "helisearchlight.mainLightInfo.volume"                         , joaat("helisearchlight.mainLightInfo.volume")                       , V_FLOAT },
                {"helisearchlight mainLightInfo coronaIntensity"                  , "helisearchlight.mainLightInfo.coronaIntensity"                , joaat("helisearchlight.mainLightInfo.coronaIntensity")              , V_FLOAT },
                {"helisearchlight mainLightInfo coronaIntensityFar"               , "helisearchlight.mainLightInfo.coronaIntensityFar"             , joaat("helisearchlight.mainLightInfo.coronaIntensityFar")           , V_FLOAT },
                {"helisearchlight mainLightInfo coronaSize"                       , "helisearchlight.mainLightInfo.coronaSize"                     , joaat("helisearchlight.mainLightInfo.coronaSize")                   , V_FLOAT },
                {"helisearchlight mainLightInfo coronaSizeFar"                    , "helisearchlight.mainLightInfo.coronaSizeFar"                  , joaat("helisearchlight.mainLightInfo.coronaSizeFar")                , V_FLOAT },
                {"helisearchlight mainLightInfo coronaZBias"                      , "helisearchlight.mainLightInfo.coronaZBias"                    , joaat("helisearchlight.mainLightInfo.coronaZBias")                  , V_FLOAT },
                {"helisearchlight mainLightInfo coronaOffset"                     , "helisearchlight.mainLightInfo.coronaOffset"                   , joaat("helisearchlight.mainLightInfo.coronaOffset")                 , V_FLOAT },
                {"helisearchlight mainLightInfo lightFadeDist"                    , "helisearchlight.mainLightInfo.lightFadeDist"                  , joaat("helisearchlight.mainLightInfo.lightFadeDist")                , V_FLOAT },
                {"boatsearchlight length"                                         , "boatsearchlight.length"                                       , joaat("boatsearchlight.length")                                     , V_FLOAT },
                {"boatsearchlight offset"                                         , "boatsearchlight.offset"                                       , joaat("boatsearchlight.offset")                                     , V_FLOAT },
                {"boatsearchlight color"                                          , "boatsearchlight.color.red"                                    , joaat("boatsearchlight.color.red")                                  , V_COL3  },
                {"None"                                                           , "boatsearchlight.color.green"                                  , joaat("boatsearchlight.color.green")                                , V_NONE  },
                {"None"                                                           , "boatsearchlight.color.blue"                                   , joaat("boatsearchlight.color.blue")                                 , V_NONE  },
                {"boatsearchlight mainLightInfo falloffScale"                     , "boatsearchlight.mainLightInfo.falloffScale"                   , joaat("boatsearchlight.mainLightInfo.falloffScale")                 , V_FLOAT },
                {"boatsearchlight mainLightInfo falloffExp"                       , "boatsearchlight.mainLightInfo.falloffExp"                     , joaat("boatsearchlight.mainLightInfo.falloffExp")                   , V_FLOAT },
                {"boatsearchlight mainLightInfo innerAngle"                       , "boatsearchlight.mainLightInfo.innerAngle"                     , joaat("boatsearchlight.mainLightInfo.innerAngle")                   , V_FLOAT },
                {"boatsearchlight mainLightInfo outerAngle"                       , "boatsearchlight.mainLightInfo.outerAngle"                     , joaat("boatsearchlight.mainLightInfo.outerAngle")                   , V_FLOAT },
                {"boatsearchlight mainLightInfo globalIntensity"                  , "boatsearchlight.mainLightInfo.globalIntensity"                , joaat("boatsearchlight.mainLightInfo.globalIntensity")              , V_FLOAT },
                {"boatsearchlight mainLightInfo lightIntensityScale"              , "boatsearchlight.mainLightInfo.lightIntensityScale"            , joaat("boatsearchlight.mainLightInfo.lightIntensityScale")          , V_FLOAT },
                {"boatsearchlight mainLightInfo volumeIntensityScale"             , "boatsearchlight.mainLightInfo.volumeIntensityScale"           , joaat("boatsearchlight.mainLightInfo.volumeIntensityScale")         , V_FLOAT },
                {"boatsearchlight mainLightInfo volumeSizeScale"                  , "boatsearchlight.mainLightInfo.volumeSizeScale"                , joaat("boatsearchlight.mainLightInfo.volumeSizeScale")              , V_FLOAT },
                {"boatsearchlight mainLightInfo outerVolumeColor color"           , "boatsearchlight.mainLightInfo.outerVolumeColor.color.red"     , joaat("boatsearchlight.mainLightInfo.outerVolumeColor.color.red")   , V_COL3  },
                {"None"                                                           , "boatsearchlight.mainLightInfo.outerVolumeColor.color.green"   , joaat("boatsearchlight.mainLightInfo.outerVolumeColor.color.green") , V_NONE  },
                {"None"                                                           , "boatsearchlight.mainLightInfo.outerVolumeColor.color.blue"    , joaat("boatsearchlight.mainLightInfo.outerVolumeColor.color.blue")  , V_NONE  },
                {"boatsearchlight mainLightInfo outerVolumeIntensity"             , "boatsearchlight.mainLightInfo.outerVolumeIntensity"           , joaat("boatsearchlight.mainLightInfo.outerVolumeIntensity")         , V_FLOAT },
                {"boatsearchlight mainLightInfo outerVolumeFallOffExponent"       , "boatsearchlight.mainLightInfo.outerVolumeFallOffExponent"     , joaat("boatsearchlight.mainLightInfo.outerVolumeFallOffExponent")   , V_FLOAT },
                {"boatsearchlight mainLightInfo enable"                           , "boatsearchlight.mainLightInfo.enable"                         , joaat("boatsearchlight.mainLightInfo.enable")                       , V_FLOAT },
                {"boatsearchlight mainLightInfo specular"                         , "boatsearchlight.mainLightInfo.specular"                       , joaat("boatsearchlight.mainLightInfo.specular")                     , V_FLOAT },
                {"boatsearchlight mainLightInfo shadow"                           , "boatsearchlight.mainLightInfo.shadow"                         , joaat("boatsearchlight.mainLightInfo.shadow")                       , V_FLOAT },
                {"boatsearchlight mainLightInfo volume"                           , "boatsearchlight.mainLightInfo.volume"                         , joaat("boatsearchlight.mainLightInfo.volume")                       , V_FLOAT },
                {"boatsearchlight mainLightInfo coronaIntensity"                  , "boatsearchlight.mainLightInfo.coronaIntensity"                , joaat("boatsearchlight.mainLightInfo.coronaIntensity")              , V_FLOAT },
                {"boatsearchlight mainLightInfo coronaIntensityFar"               , "boatsearchlight.mainLightInfo.coronaIntensityFar"             , joaat("boatsearchlight.mainLightInfo.coronaIntensityFar")           , V_FLOAT },
                {"boatsearchlight mainLightInfo coronaSize"                       , "boatsearchlight.mainLightInfo.coronaSize"                     , joaat("boatsearchlight.mainLightInfo.coronaSize")                   , V_FLOAT },
                {"boatsearchlight mainLightInfo coronaSizeFar"                    , "boatsearchlight.mainLightInfo.coronaSizeFar"                  , joaat("boatsearchlight.mainLightInfo.coronaSizeFar")                , V_FLOAT },
                {"boatsearchlight mainLightInfo coronaZBias"                      , "boatsearchlight.mainLightInfo.coronaZBias"                    , joaat("boatsearchlight.mainLightInfo.coronaZBias")                  , V_FLOAT },
                {"boatsearchlight mainLightInfo coronaOffset"                     , "boatsearchlight.mainLightInfo.coronaOffset"                   , joaat("boatsearchlight.mainLightInfo.coronaOffset")                 , V_FLOAT },
                {"boatsearchlight mainLightInfo lightFadeDist"                    , "boatsearchlight.mainLightInfo.lightFadeDist"                  , joaat("boatsearchlight.mainLightInfo.lightFadeDist")                , V_FLOAT },
        };
        paramsMap["Config values for traffic lights"] =
        {
                {"trafficLight red color"                                         , "trafficLight.red.color.red"                                   , joaat("trafficLight.red.color.red")                                 , V_COL3  },
                {"None"                                                           , "trafficLight.red.color.green"                                 , joaat("trafficLight.red.color.green")                               , V_NONE  },
                {"None"                                                           , "trafficLight.red.color.blue"                                  , joaat("trafficLight.red.color.blue")                                , V_NONE  },
                {"trafficLight amber color"                                       , "trafficLight.amber.color.red"                                 , joaat("trafficLight.amber.color.red")                               , V_COL3  },
                {"None"                                                           , "trafficLight.amber.color.green"                               , joaat("trafficLight.amber.color.green")                             , V_NONE  },
                {"None"                                                           , "trafficLight.amber.color.blue"                                , joaat("trafficLight.amber.color.blue")                              , V_NONE  },
                {"trafficLight green color"                                       , "trafficLight.green.color.red"                                 , joaat("trafficLight.green.color.red")                               , V_COL3  },
                {"None"                                                           , "trafficLight.green.color.green"                               , joaat("trafficLight.green.color.green")                             , V_NONE  },
                {"None"                                                           , "trafficLight.green.color.blue"                                , joaat("trafficLight.green.color.blue")                              , V_NONE  },
                {"trafficLight walk color"                                        , "trafficLight.walk.color.red"                                  , joaat("trafficLight.walk.color.red")                                , V_COL3  },
                {"None"                                                           , "trafficLight.walk.color.green"                                , joaat("trafficLight.walk.color.green")                              , V_NONE  },
                {"None"                                                           , "trafficLight.walk.color.blue"                                 , joaat("trafficLight.walk.color.blue")                               , V_NONE  },
                {"trafficLight dontwalk color"                                    , "trafficLight.dontwalk.color.red"                              , joaat("trafficLight.dontwalk.color.red")                            , V_COL3  },
                {"None"                                                           , "trafficLight.dontwalk.color.green"                            , joaat("trafficLight.dontwalk.color.green")                          , V_NONE  },
                {"None"                                                           , "trafficLight.dontwalk.color.blue"                             , joaat("trafficLight.dontwalk.color.blue")                           , V_NONE  },
                {"trafficLight near na color"                                     , "trafficLight.near.na.color.red"                               , joaat("trafficLight.near.na.color.red")                             , V_COL3  },
                {"None"                                                           , "trafficLight.near.na.color.green"                             , joaat("trafficLight.near.na.color.green")                           , V_NONE  },
                {"None"                                                           , "trafficLight.near.na.color.blue"                              , joaat("trafficLight.near.na.color.blue")                            , V_NONE  },
                {"trafficLight near intensity"                                    , "trafficLight.near.intensity"                                  , joaat("trafficLight.near.intensity")                                , V_FLOAT },
                {"trafficLight near radius"                                       , "trafficLight.near.radius"                                     , joaat("trafficLight.near.radius")                                   , V_FLOAT },
                {"trafficlight near falloffExp"                                   , "trafficlight.near.falloffExp"                                 , joaat("trafficlight.near.falloffExp")                               , V_FLOAT },
                {"trafficLight near innerConeAngle"                               , "trafficLight.near.innerConeAngle"                             , joaat("trafficLight.near.innerConeAngle")                           , V_FLOAT },
                {"trafficLight near outerConeAngle"                               , "trafficLight.near.outerConeAngle"                             , joaat("trafficLight.near.outerConeAngle")                           , V_FLOAT },
                {"trafficLight near coronaHDR"                                    , "trafficLight.near.coronaHDR"                                  , joaat("trafficLight.near.coronaHDR")                                , V_FLOAT },
                {"trafficLight near coronaSize"                                   , "trafficLight.near.coronaSize"                                 , joaat("trafficLight.near.coronaSize")                               , V_FLOAT },
                {"trafficLight farFadeStart"                                      , "trafficLight.farFadeStart"                                    , joaat("trafficLight.farFadeStart")                                  , V_FLOAT },
                {"trafficLight farFadeEnd"                                        , "trafficLight.farFadeEnd"                                      , joaat("trafficLight.farFadeEnd")                                    , V_FLOAT },
                {"trafficLight nearFadeStart"                                     , "trafficLight.nearFadeStart"                                   , joaat("trafficLight.nearFadeStart")                                 , V_FLOAT },
                {"trafficLight nearFadeEnd"                                       , "trafficLight.nearFadeEnd"                                     , joaat("trafficLight.nearFadeEnd")                                   , V_FLOAT },
        };
        paramsMap["Config values for Tree Imposters"] =
        {
                {"imposter color blendRange"                                      , "imposter.color.blendRange"                                    , joaat("imposter.color.blendRange")                                  , V_FLOAT },
                {"imposter color blendBias"                                       , "imposter.color.blendBias"                                     , joaat("imposter.color.blendBias")                                   , V_FLOAT },
                {"imposter color size1"                                           , "imposter.color.size1"                                         , joaat("imposter.color.size1")                                       , V_FLOAT },
                {"imposter color amt1"                                            , "imposter.color.amt1"                                          , joaat("imposter.color.amt1")                                        , V_FLOAT },
                {"imposter color size2"                                           , "imposter.color.size2"                                         , joaat("imposter.color.size2")                                       , V_FLOAT },
                {"imposter color amt2"                                            , "imposter.color.amt2"                                          , joaat("imposter.color.amt2")                                        , V_FLOAT },
                {"imposter backgroundColor"                                       , "imposter.backgroundColor.red"                                 , joaat("imposter.backgroundColor.red")                               , V_COL3  },
                {"None"                                                           , "imposter.backgroundColor.green"                               , joaat("imposter.backgroundColor.green")                             , V_NONE  },
                {"None"                                                           , "imposter.backgroundColor.blue"                                , joaat("imposter.backgroundColor.blue")                              , V_NONE  },
                {"imposter shadow blendRange"                                     , "imposter.shadow.blendRange"                                   , joaat("imposter.shadow.blendRange")                                 , V_FLOAT },
                {"imposter shadow blendBias"                                      , "imposter.shadow.blendBias"                                    , joaat("imposter.shadow.blendBias")                                  , V_FLOAT },
                {"imposter shadow size1"                                          , "imposter.shadow.size1"                                        , joaat("imposter.shadow.size1")                                      , V_FLOAT },
                {"imposter shadow amt1"                                           , "imposter.shadow.amt1"                                         , joaat("imposter.shadow.amt1")                                       , V_FLOAT },
                {"imposter shadow size2"                                          , "imposter.shadow.size2"                                        , joaat("imposter.shadow.size2")                                      , V_FLOAT },
                {"imposter shadow amt2"                                           , "imposter.shadow.amt2"                                         , joaat("imposter.shadow.amt2")                                       , V_FLOAT },
        };
        paramsMap["Config values for peds"] =
        {
                {"ped ambientvolume maxstrength"                                  , "ped.ambientvolume.maxstrength"                                , joaat("ped.ambientvolume.maxstrength")                              , V_FLOAT },
                {"ped ambientvolume fadestart"                                    , "ped.ambientvolume.fadestart"                                  , joaat("ped.ambientvolume.fadestart")                                , V_FLOAT },
                {"ped ambientvolume fadeend"                                      , "ped.ambientvolume.fadeend"                                    , joaat("ped.ambientvolume.fadeend")                                  , V_FLOAT },
                {"ped ambientvolume baseintensity"                                , "ped.ambientvolume.baseintensity"                              , joaat("ped.ambientvolume.baseintensity")                            , V_FLOAT },
                {"ped incarAmbientScale"                                          , "ped.incarAmbientScale"                                        , joaat("ped.incarAmbientScale")                                      , V_FLOAT },
                {"pedLight color"                                                 , "pedLight.color.red"                                           , joaat("pedLight.color.red")                                         , V_COL3  },
                {"None"                                                           , "pedLight.color.green"                                         , joaat("pedLight.color.green")                                       , V_NONE  },
                {"None"                                                           , "pedLight.color.blue"                                          , joaat("pedLight.color.blue")                                        , V_NONE  },
                {"pedlight intensity"                                             , "pedlight.intensity"                                           , joaat("pedlight.intensity")                                         , V_FLOAT },
                {"pedlight radius"                                                , "pedlight.radius"                                              , joaat("pedlight.radius")                                            , V_FLOAT },
                {"pedlight innerConeAngle"                                        , "pedlight.innerConeAngle"                                      , joaat("pedlight.innerConeAngle")                                    , V_FLOAT },
                {"pedlight outerConeAngle"                                        , "pedlight.outerConeAngle"                                      , joaat("pedlight.outerConeAngle")                                    , V_FLOAT },
                {"pedlight coronaHDR"                                             , "pedlight.coronaHDR"                                           , joaat("pedlight.coronaHDR")                                         , V_FLOAT },
                {"pedlight coronaSize"                                            , "pedlight.coronaSize"                                          , joaat("pedlight.coronaSize")                                        , V_FLOAT },
                {"pedlight falloffExp"                                            , "pedlight.falloffExp"                                          , joaat("pedlight.falloffExp")                                        , V_FLOAT },
                {"pedlight volumeIntensity"                                       , "pedlight.volumeIntensity"                                     , joaat("pedlight.volumeIntensity")                                   , V_FLOAT },
                {"pedlight volumeSize"                                            , "pedlight.volumeSize"                                          , joaat("pedlight.volumeSize")                                        , V_FLOAT },
                {"pedlight volumeExponent"                                        , "pedlight.volumeExponent"                                      , joaat("pedlight.volumeExponent")                                    , V_FLOAT },
                {"pedLight volumeColor x"                                         , "pedLight.volumeColor.x"                                       , joaat("pedLight.volumeColor.x")                                     , V_FLOAT },
                {"pedLight volumeColor y"                                         , "pedLight.volumeColor.y"                                       , joaat("pedLight.volumeColor.y")                                     , V_FLOAT },
                {"pedLight volumeColor z"                                         , "pedLight.volumeColor.z"                                       , joaat("pedLight.volumeColor.z")                                     , V_FLOAT },
                {"pedLight volumeColor w"                                         , "pedLight.volumeColor.w"                                       , joaat("pedLight.volumeColor.w")                                     , V_FLOAT },
                {"pedlight fade"                                                  , "pedlight.fade"                                                , joaat("pedlight.fade")                                              , V_FLOAT },
                {"pedlight shadowFade"                                            , "pedlight.shadowFade"                                          , joaat("pedlight.shadowFade")                                        , V_FLOAT },
                {"pedlight specularFade"                                          , "pedlight.specularFade"                                        , joaat("pedlight.specularFade")                                      , V_FLOAT },
                {"pedFpsLight color"                                              , "pedFpsLight.color.red"                                        , joaat("pedFpsLight.color.red")                                      , V_COL3  },
                {"None"                                                           , "pedFpsLight.color.green"                                      , joaat("pedFpsLight.color.green")                                    , V_NONE  },
                {"None"                                                           , "pedFpsLight.color.blue"                                       , joaat("pedFpsLight.color.blue")                                     , V_NONE  },
                {"pedFpslight intensity"                                          , "pedFpslight.intensity"                                        , joaat("pedFpslight.intensity")                                      , V_FLOAT },
                {"pedFpslight radius"                                             , "pedFpslight.radius"                                           , joaat("pedFpslight.radius")                                         , V_FLOAT },
                {"pedFpslight innerConeAngle"                                     , "pedFpslight.innerConeAngle"                                   , joaat("pedFpslight.innerConeAngle")                                 , V_FLOAT },
                {"pedFpslight outerConeAngle"                                     , "pedFpslight.outerConeAngle"                                   , joaat("pedFpslight.outerConeAngle")                                 , V_FLOAT },
                {"pedFpslight coronaHDR"                                          , "pedFpslight.coronaHDR"                                        , joaat("pedFpslight.coronaHDR")                                      , V_FLOAT },
                {"pedFpslight coronaSize"                                         , "pedFpslight.coronaSize"                                       , joaat("pedFpslight.coronaSize")                                     , V_FLOAT },
                {"pedFpslight falloffExp"                                         , "pedFpslight.falloffExp"                                       , joaat("pedFpslight.falloffExp")                                     , V_FLOAT },
                {"pedFpslight volumeIntensity"                                    , "pedFpslight.volumeIntensity"                                  , joaat("pedFpslight.volumeIntensity")                                , V_FLOAT },
                {"pedFpslight volumeSize"                                         , "pedFpslight.volumeSize"                                       , joaat("pedFpslight.volumeSize")                                     , V_FLOAT },
                {"pedFpslight volumeExponent"                                     , "pedFpslight.volumeExponent"                                   , joaat("pedFpslight.volumeExponent")                                 , V_FLOAT },
                {"pedFpsLight volumeColor x"                                      , "pedFpsLight.volumeColor.x"                                    , joaat("pedFpsLight.volumeColor.x")                                  , V_FLOAT },
                {"pedFpsLight volumeColor y"                                      , "pedFpsLight.volumeColor.y"                                    , joaat("pedFpsLight.volumeColor.y")                                  , V_FLOAT },
                {"pedFpsLight volumeColor z"                                      , "pedFpsLight.volumeColor.z"                                    , joaat("pedFpsLight.volumeColor.z")                                  , V_FLOAT },
                {"pedFpsLight volumeColor w"                                      , "pedFpsLight.volumeColor.w"                                    , joaat("pedFpsLight.volumeColor.w")                                  , V_FLOAT },
                {"pedFpslight fade"                                               , "pedFpslight.fade"                                             , joaat("pedFpslight.fade")                                           , V_FLOAT },
                {"pedFpslight shadowFade"                                         , "pedFpslight.shadowFade"                                       , joaat("pedFpslight.shadowFade")                                     , V_FLOAT },
                {"pedFpslight specularFade"                                       , "pedFpslight.specularFade"                                     , joaat("pedFpslight.specularFade")                                   , V_FLOAT },
                {"pedFootLight color"                                             , "pedFootLight.color.red"                                       , joaat("pedFootLight.color.red")                                     , V_COL3  },
                {"None"                                                           , "pedFootLight.color.green"                                     , joaat("pedFootLight.color.green")                                   , V_NONE  },
                {"None"                                                           , "pedFootLight.color.blue"                                      , joaat("pedFootLight.color.blue")                                    , V_NONE  },
                {"pedFootLight intensity"                                         , "pedFootLight.intensity"                                       , joaat("pedFootLight.intensity")                                     , V_FLOAT },
                {"pedFootLight radius"                                            , "pedFootLight.radius"                                          , joaat("pedFootLight.radius")                                        , V_FLOAT },
                {"pedFootLight capsuleLength"                                     , "pedFootLight.capsuleLength"                                   , joaat("pedFootLight.capsuleLength")                                 , V_FLOAT },
                {"pedFootLight offset x"                                          , "pedFootLight.offset.x"                                        , joaat("pedFootLight.offset.x")                                      , V_FLOAT },
                {"pedFootLight offset y"                                          , "pedFootLight.offset.y"                                        , joaat("pedFootLight.offset.y")                                      , V_FLOAT },
                {"pedFootLight offset z"                                          , "pedFootLight.offset.z"                                        , joaat("pedFootLight.offset.z")                                      , V_FLOAT },
                {"pedFootLight offset w"                                          , "pedFootLight.offset.w"                                        , joaat("pedFootLight.offset.w")                                      , V_FLOAT },
                {"pedFootLight falloffExp"                                        , "pedFootLight.falloffExp"                                      , joaat("pedFootLight.falloffExp")                                    , V_FLOAT },
                {"pedFootLight fade"                                              , "pedFootLight.fade"                                            , joaat("pedFootLight.fade")                                          , V_FLOAT },
                {"pedFootLight specularFade"                                      , "pedFootLight.specularFade"                                    , joaat("pedFootLight.specularFade")                                  , V_FLOAT },
        };
        paramsMap["Lod distances for peds"] =
        {
                {"ped lod distance high"                                          , "ped.lod.distance.high"                                        , joaat("ped.lod.distance.high")                                      , V_FLOAT },
                {"ped lod distance medium"                                        , "ped.lod.distance.medium"                                      , joaat("ped.lod.distance.medium")                                    , V_FLOAT },
                {"ped lod distance low"                                           , "ped.lod.distance.low"                                         , joaat("ped.lod.distance.low")                                       , V_FLOAT },
                {"pedincar lod distance high"                                     , "pedincar.lod.distance.high"                                   , joaat("pedincar.lod.distance.high")                                 , V_FLOAT },
                {"pedincar lod distance high x64"                                 , "pedincar.lod.distance.high.x64"                               , joaat("pedincar.lod.distance.high.x64")                             , V_FLOAT },
        };
        paramsMap["Config Values for Camera Events : Val start -> Val End  /  MB start -> MB End"] =
        {
                {"cam followped blur zoom x"                                      , "cam.followped.blur.zoom.x"                                    , joaat("cam.followped.blur.zoom.x")                                  , V_FLOAT },
                {"cam followped blur zoom y"                                      , "cam.followped.blur.zoom.y"                                    , joaat("cam.followped.blur.zoom.y")                                  , V_FLOAT },
                {"cam followped blur zoom z"                                      , "cam.followped.blur.zoom.z"                                    , joaat("cam.followped.blur.zoom.z")                                  , V_FLOAT },
                {"cam followped blur zoom w"                                      , "cam.followped.blur.zoom.w"                                    , joaat("cam.followped.blur.zoom.w")                                  , V_FLOAT },
                {"cam followped blur damage x"                                    , "cam.followped.blur.damage.x"                                  , joaat("cam.followped.blur.damage.x")                                , V_FLOAT },
                {"cam followped blur damage y"                                    , "cam.followped.blur.damage.y"                                  , joaat("cam.followped.blur.damage.y")                                , V_FLOAT },
                {"cam followped blur damage z"                                    , "cam.followped.blur.damage.z"                                  , joaat("cam.followped.blur.damage.z")                                , V_FLOAT },
                {"cam followped blur damage w"                                    , "cam.followped.blur.damage.w"                                  , joaat("cam.followped.blur.damage.w")                                , V_FLOAT },
                {"cam followped blur falling x"                                   , "cam.followped.blur.falling.x"                                 , joaat("cam.followped.blur.falling.x")                               , V_FLOAT },
                {"cam followped blur falling y"                                   , "cam.followped.blur.falling.y"                                 , joaat("cam.followped.blur.falling.y")                               , V_FLOAT },
                {"cam followped blur falling z"                                   , "cam.followped.blur.falling.z"                                 , joaat("cam.followped.blur.falling.z")                               , V_FLOAT },
                {"cam followped blur falling w"                                   , "cam.followped.blur.falling.w"                                 , joaat("cam.followped.blur.falling.w")                               , V_FLOAT },
                {"cam followped blur beta x"                                      , "cam.followped.blur.beta.x"                                    , joaat("cam.followped.blur.beta.x")                                  , V_FLOAT },
                {"cam followped blur beta y"                                      , "cam.followped.blur.beta.y"                                    , joaat("cam.followped.blur.beta.y")                                  , V_FLOAT },
                {"cam followped blur beta z"                                      , "cam.followped.blur.beta.z"                                    , joaat("cam.followped.blur.beta.z")                                  , V_FLOAT },
                {"cam followped blur beta w"                                      , "cam.followped.blur.beta.w"                                    , joaat("cam.followped.blur.beta.w")                                  , V_FLOAT },
                {"cam followped blur damage time"                                 , "cam.followped.blur.damage.time"                               , joaat("cam.followped.blur.damage.time")                             , V_FLOAT },
                {"cam followped blur damage attacktime"                           , "cam.followped.blur.damage.attacktime"                         , joaat("cam.followped.blur.damage.attacktime")                       , V_FLOAT },
                {"cam followped blur damage decaytime"                            , "cam.followped.blur.damage.decaytime"                          , joaat("cam.followped.blur.damage.decaytime")                        , V_FLOAT },
                {"cam followped blur cap"                                         , "cam.followped.blur.cap"                                       , joaat("cam.followped.blur.cap")                                     , V_FLOAT },
                {"cam aimweapon blur zoom x"                                      , "cam.aimweapon.blur.zoom.x"                                    , joaat("cam.aimweapon.blur.zoom.x")                                  , V_FLOAT },
                {"cam aimweapon blur zoom y"                                      , "cam.aimweapon.blur.zoom.y"                                    , joaat("cam.aimweapon.blur.zoom.y")                                  , V_FLOAT },
                {"cam aimweapon blur zoom z"                                      , "cam.aimweapon.blur.zoom.z"                                    , joaat("cam.aimweapon.blur.zoom.z")                                  , V_FLOAT },
                {"cam aimweapon blur zoom w"                                      , "cam.aimweapon.blur.zoom.w"                                    , joaat("cam.aimweapon.blur.zoom.w")                                  , V_FLOAT },
                {"cam aimweapon blur damage x"                                    , "cam.aimweapon.blur.damage.x"                                  , joaat("cam.aimweapon.blur.damage.x")                                , V_FLOAT },
                {"cam aimweapon blur damage y"                                    , "cam.aimweapon.blur.damage.y"                                  , joaat("cam.aimweapon.blur.damage.y")                                , V_FLOAT },
                {"cam aimweapon blur damage z"                                    , "cam.aimweapon.blur.damage.z"                                  , joaat("cam.aimweapon.blur.damage.z")                                , V_FLOAT },
                {"cam aimweapon blur damage w"                                    , "cam.aimweapon.blur.damage.w"                                  , joaat("cam.aimweapon.blur.damage.w")                                , V_FLOAT },
                {"cam aimweapon blur damage time"                                 , "cam.aimweapon.blur.damage.time"                               , joaat("cam.aimweapon.blur.damage.time")                             , V_FLOAT },
                {"cam aimweapon blur interp"                                      , "cam.aimweapon.blur.interp"                                    , joaat("cam.aimweapon.blur.interp")                                  , V_FLOAT },
                {"cam aimweapon blur damage attacktime"                           , "cam.aimweapon.blur.damage.attacktime"                         , joaat("cam.aimweapon.blur.damage.attacktime")                       , V_FLOAT },
                {"cam aimweapon blur damage decaytime"                            , "cam.aimweapon.blur.damage.decaytime"                          , joaat("cam.aimweapon.blur.damage.decaytime")                        , V_FLOAT },
                {"cam aimweapon blur cap"                                         , "cam.aimweapon.blur.cap"                                       , joaat("cam.aimweapon.blur.cap")                                     , V_FLOAT },
                {"cam followvehicle blur zoom x"                                  , "cam.followvehicle.blur.zoom.x"                                , joaat("cam.followvehicle.blur.zoom.x")                              , V_FLOAT },
                {"cam followvehicle blur zoom y"                                  , "cam.followvehicle.blur.zoom.y"                                , joaat("cam.followvehicle.blur.zoom.y")                              , V_FLOAT },
                {"cam followvehicle blur zoom z"                                  , "cam.followvehicle.blur.zoom.z"                                , joaat("cam.followvehicle.blur.zoom.z")                              , V_FLOAT },
                {"cam followvehicle blur zoom w"                                  , "cam.followvehicle.blur.zoom.w"                                , joaat("cam.followvehicle.blur.zoom.w")                              , V_FLOAT },
                {"cam followvehicle blur speed x"                                 , "cam.followvehicle.blur.speed.x"                               , joaat("cam.followvehicle.blur.speed.x")                             , V_FLOAT },
                {"cam followvehicle blur speed y"                                 , "cam.followvehicle.blur.speed.y"                               , joaat("cam.followvehicle.blur.speed.y")                             , V_FLOAT },
                {"cam followvehicle blur speed z"                                 , "cam.followvehicle.blur.speed.z"                               , joaat("cam.followvehicle.blur.speed.z")                             , V_FLOAT },
                {"cam followvehicle blur speed w"                                 , "cam.followvehicle.blur.speed.w"                               , joaat("cam.followvehicle.blur.speed.w")                             , V_FLOAT },
                {"cam followvehicle blur damage x"                                , "cam.followvehicle.blur.damage.x"                              , joaat("cam.followvehicle.blur.damage.x")                            , V_FLOAT },
                {"cam followvehicle blur damage y"                                , "cam.followvehicle.blur.damage.y"                              , joaat("cam.followvehicle.blur.damage.y")                            , V_FLOAT },
                {"cam followvehicle blur damage z"                                , "cam.followvehicle.blur.damage.z"                              , joaat("cam.followvehicle.blur.damage.z")                            , V_FLOAT },
                {"cam followvehicle blur damage w"                                , "cam.followvehicle.blur.damage.w"                              , joaat("cam.followvehicle.blur.damage.w")                            , V_FLOAT },
                {"cam followvehicle blur damage time"                             , "cam.followvehicle.blur.damage.time"                           , joaat("cam.followvehicle.blur.damage.time")                         , V_FLOAT },
                {"cam followvehicle blur cap"                                     , "cam.followvehicle.blur.cap"                                   , joaat("cam.followvehicle.blur.cap")                                 , V_FLOAT },
                {"cam game blur wasted"                                           , "cam.game.blur.wasted"                                         , joaat("cam.game.blur.wasted")                                       , V_FLOAT },
                {"cam game blur wasted fadetime"                                  , "cam.game.blur.wasted.fadetime"                                , joaat("cam.game.blur.wasted.fadetime")                              , V_FLOAT },
                {"cam game blur busted"                                           , "cam.game.blur.busted"                                         , joaat("cam.game.blur.busted")                                       , V_FLOAT },
                {"cam game blur busted fadetime"                                  , "cam.game.blur.busted.fadetime"                                , joaat("cam.game.blur.busted.fadetime")                              , V_FLOAT },
                {"cam game blur cap"                                              , "cam.game.blur.cap"                                            , joaat("cam.game.blur.cap")                                          , V_FLOAT },
                {"cam fpsweapon blur"                                             , "cam.fpsweapon.blur"                                           , joaat("cam.fpsweapon.blur")                                         , V_FLOAT },
                {"cam fpsweapon sniperinitime"                                    , "cam.fpsweapon.sniperinitime"                                  , joaat("cam.fpsweapon.sniperinitime")                                , V_FLOAT },
                {"cam fpsweapon blur cap"                                         , "cam.fpsweapon.blur.cap"                                       , joaat("cam.fpsweapon.blur.cap")                                     , V_FLOAT },
                {"cam intermezzo stuntjump blur"                                  , "cam.intermezzo.stuntjump.blur"                                , joaat("cam.intermezzo.stuntjump.blur")                              , V_FLOAT },
        };
        paramsMap["Containers LOD"] =
        {
                {"lod container caploddist"                                       , "lod.container.caploddist"                                     , joaat("lod.container.caploddist")                                   , V_FLOAT },
        };
        paramsMap["distant lights"] =
        {
                {"distantlights inlandHeight"                                     , "distantlights.inlandHeight"                                   , joaat("distantlights.inlandHeight")                                 , V_FLOAT },
                {"distantlights size"                                             , "distantlights.size"                                           , joaat("distantlights.size")                                         , V_FLOAT },
                {"distantlights sizeReflections"                                  , "distantlights.sizeReflections"                                , joaat("distantlights.sizeReflections")                              , V_FLOAT },
                {"distantlights sizeMin"                                          , "distantlights.sizeMin"                                        , joaat("distantlights.sizeMin")                                      , V_FLOAT },
                {"distantlights sizeUpscale"                                      , "distantlights.sizeUpscale"                                    , joaat("distantlights.sizeUpscale")                                  , V_FLOAT },
                {"distantlights sizeUpscaleReflections"                           , "distantlights.sizeUpscaleReflections"                         , joaat("distantlights.sizeUpscaleReflections")                       , V_FLOAT },
                {"distantlights flicker"                                          , "distantlights.flicker"                                        , joaat("distantlights.flicker")                                      , V_FLOAT },
                {"distantlights twinkleAmount"                                    , "distantlights.twinkleAmount"                                  , joaat("distantlights.twinkleAmount")                                , V_FLOAT },
                {"distantlights twinkleSpeed"                                     , "distantlights.twinkleSpeed"                                   , joaat("distantlights.twinkleSpeed")                                 , V_FLOAT },
                {"distantlights carlightZOffset"                                  , "distantlights.carlightZOffset"                                , joaat("distantlights.carlightZOffset")                              , V_FLOAT },
                {"distantlights hourStart"                                        , "distantlights.hourStart"                                      , joaat("distantlights.hourStart")                                    , V_FLOAT },
                {"distantlights hourEnd"                                          , "distantlights.hourEnd"                                        , joaat("distantlights.hourEnd")                                      , V_FLOAT },
                {"distantlights streetLightHourStart"                             , "distantlights.streetLightHourStart"                           , joaat("distantlights.streetLightHourStart")                         , V_FLOAT },
                {"distantlights streetLighthourEnd"                               , "distantlights.streetLighthourEnd"                             , joaat("distantlights.streetLighthourEnd")                           , V_FLOAT },
                {"distantlights sizeDistStart"                                    , "distantlights.sizeDistStart"                                  , joaat("distantlights.sizeDistStart")                                , V_FLOAT },
                {"distantlights sizeDist"                                         , "distantlights.sizeDist"                                       , joaat("distantlights.sizeDist")                                     , V_FLOAT },
                {"distantlights speed0"                                           , "distantlights.speed0"                                         , joaat("distantlights.speed0")                                       , V_FLOAT },
                {"distantlights speed1"                                           , "distantlights.speed1"                                         , joaat("distantlights.speed1")                                       , V_FLOAT },
                {"distantlights speed2"                                           , "distantlights.speed2"                                         , joaat("distantlights.speed2")                                       , V_FLOAT },
                {"distantlights speed3"                                           , "distantlights.speed3"                                         , joaat("distantlights.speed3")                                       , V_FLOAT },
                {"distantlights density0Spacing"                                  , "distantlights.density0Spacing"                                , joaat("distantlights.density0Spacing")                              , V_FLOAT },
                {"distantlights density15Spacing"                                 , "distantlights.density15Spacing"                               , joaat("distantlights.density15Spacing")                             , V_FLOAT },
                {"distantlights speed0Speed"                                      , "distantlights.speed0Speed"                                    , joaat("distantlights.speed0Speed")                                  , V_FLOAT },
                {"distantlights speed3Speed"                                      , "distantlights.speed3Speed"                                    , joaat("distantlights.speed3Speed")                                  , V_FLOAT },
                {"distantlights randomizeSpeed sp"                                , "distantlights.randomizeSpeed.sp"                              , joaat("distantlights.randomizeSpeed.sp")                            , V_FLOAT },
                {"distantlights randomizeSpacing sp"                              , "distantlights.randomizeSpacing.sp"                            , joaat("distantlights.randomizeSpacing.sp")                          , V_FLOAT },
                {"distantlights randomizeSpeed mp"                                , "distantlights.randomizeSpeed.mp"                              , joaat("distantlights.randomizeSpeed.mp")                            , V_FLOAT },
                {"distantlights randomizeSpacing mp"                              , "distantlights.randomizeSpacing.mp"                            , joaat("distantlights.randomizeSpacing.mp")                          , V_FLOAT },
                {"distantlights carlight HDRIntensity"                            , "distantlights.carlight.HDRIntensity"                          , joaat("distantlights.carlight.HDRIntensity")                        , V_FLOAT },
                {"distantlights carlight nearFade"                                , "distantlights.carlight.nearFade"                              , joaat("distantlights.carlight.nearFade")                            , V_FLOAT },
                {"distantlights carlight farFade"                                 , "distantlights.carlight.farFade"                               , joaat("distantlights.carlight.farFade")                             , V_FLOAT },
                {"distantlights carlight1 spacing sp"                             , "distantlights.carlight1.spacing.sp"                           , joaat("distantlights.carlight1.spacing.sp")                         , V_FLOAT },
                {"distantlights carlight1 speed sp"                               , "distantlights.carlight1.speed.sp"                             , joaat("distantlights.carlight1.speed.sp")                           , V_FLOAT },
                {"distantlights carlight1 spacing mp"                             , "distantlights.carlight1.spacing.mp"                           , joaat("distantlights.carlight1.spacing.mp")                         , V_FLOAT },
                {"distantlights carlight1 speed mp"                               , "distantlights.carlight1.speed.mp"                             , joaat("distantlights.carlight1.speed.mp")                           , V_FLOAT },
                {"distantlights carlight1 color"                                  , "distantlights.carlight1.color.red"                            , joaat("distantlights.carlight1.color.red")                          , V_COL3  },
                {"None"                                                           , "distantlights.carlight1.color.green"                          , joaat("distantlights.carlight1.color.green")                        , V_NONE  },
                {"None"                                                           , "distantlights.carlight1.color.blue"                           , joaat("distantlights.carlight1.color.blue")                         , V_NONE  },
                {"distantlights carlight2 spacing sp"                             , "distantlights.carlight2.spacing.sp"                           , joaat("distantlights.carlight2.spacing.sp")                         , V_FLOAT },
                {"distantlights carlight2 speed sp"                               , "distantlights.carlight2.speed.sp"                             , joaat("distantlights.carlight2.speed.sp")                           , V_FLOAT },
                {"distantlights carlight2 spacing mp"                             , "distantlights.carlight2.spacing.mp"                           , joaat("distantlights.carlight2.spacing.mp")                         , V_FLOAT },
                {"distantlights carlight2 speed mp"                               , "distantlights.carlight2.speed.mp"                             , joaat("distantlights.carlight2.speed.mp")                           , V_FLOAT },
                {"distantlights carlight2 color"                                  , "distantlights.carlight2.color.red"                            , joaat("distantlights.carlight2.color.red")                          , V_COL3  },
                {"None"                                                           , "distantlights.carlight2.color.green"                          , joaat("distantlights.carlight2.color.green")                        , V_NONE  },
                {"None"                                                           , "distantlights.carlight2.color.blue"                           , joaat("distantlights.carlight2.color.blue")                         , V_NONE  },
                {"distantlights streetlight HDRIntensity"                         , "distantlights.streetlight.HDRIntensity"                       , joaat("distantlights.streetlight.HDRIntensity")                     , V_FLOAT },
                {"distantlights streetlight ZOffset"                              , "distantlights.streetlight.ZOffset"                            , joaat("distantlights.streetlight.ZOffset")                          , V_FLOAT },
                {"distantlights streetlight Spacing"                              , "distantlights.streetlight.Spacing"                            , joaat("distantlights.streetlight.Spacing")                          , V_FLOAT },
                {"distantlights streetlight color"                                , "distantlights.streetlight.color.red"                          , joaat("distantlights.streetlight.color.red")                        , V_COL3  },
                {"None"                                                           , "distantlights.streetlight.color.green"                        , joaat("distantlights.streetlight.color.green")                      , V_NONE  },
                {"None"                                                           , "distantlights.streetlight.color.blue"                         , joaat("distantlights.streetlight.color.blue")                       , V_NONE  },
        };
        paramsMap["Emissive night values"] =
        {
                {"emissive night start time"                                      , "emissive.night.start.time"                                    , joaat("emissive.night.start.time")                                  , V_FLOAT },
                {"emissive night end time"                                        , "emissive.night.end.time"                                      , joaat("emissive.night.end.time")                                    , V_FLOAT },
        };
        paramsMap["Misc Values"] =
        {
                {"misc DOFBlurMultiplier HD"                                      , "misc.DOFBlurMultiplier.HD"                                    , joaat("misc.DOFBlurMultiplier.HD")                                  , V_FLOAT },
                {"misc DOFBlurMultiplier SD"                                      , "misc.DOFBlurMultiplier.SD"                                    , joaat("misc.DOFBlurMultiplier.SD")                                  , V_FLOAT },
                {"misc Multiplier heightStart"                                    , "misc.Multiplier.heightStart"                                  , joaat("misc.Multiplier.heightStart")                                , V_FLOAT },
                {"misc Multiplier heightEnd"                                      , "misc.Multiplier.heightEnd"                                    , joaat("misc.Multiplier.heightEnd")                                  , V_FLOAT },
                {"misc Multiplier farClipMultiplier"                              , "misc.Multiplier.farClipMultiplier"                            , joaat("misc.Multiplier.farClipMultiplier")                          , V_FLOAT },
                {"misc Multiplier nearFogMultiplier"                              , "misc.Multiplier.nearFogMultiplier"                            , joaat("misc.Multiplier.nearFogMultiplier")                          , V_FLOAT },
                {"misc 3dMarkers FrontLightIntensity"                             , "misc.3dMarkers.FrontLightIntensity"                           , joaat("misc.3dMarkers.FrontLightIntensity")                         , V_FLOAT },
                {"misc 3dMarkers frontLightExponent"                              , "misc.3dMarkers.frontLightExponent"                            , joaat("misc.3dMarkers.frontLightExponent")                          , V_FLOAT },
        };
        paramsMap["coronas"] =
        {
                {"misc coronas sizeScaleGlobal"                                   , "misc.coronas.sizeScaleGlobal"                                 , joaat("misc.coronas.sizeScaleGlobal")                               , V_FLOAT },
                {"misc coronas intensityScaleGlobal"                              , "misc.coronas.intensityScaleGlobal"                            , joaat("misc.coronas.intensityScaleGlobal")                          , V_FLOAT },
                {"misc coronas intensityScaleWater"                               , "misc.coronas.intensityScaleWater"                             , joaat("misc.coronas.intensityScaleWater")                           , V_FLOAT },
                {"misc coronas sizeScaleWater"                                    , "misc.coronas.sizeScaleWater"                                  , joaat("misc.coronas.sizeScaleWater")                                , V_FLOAT },
                {"misc coronas sizeScaleParaboloid"                               , "misc.coronas.sizeScaleParaboloid"                             , joaat("misc.coronas.sizeScaleParaboloid")                           , V_FLOAT },
                {"misc coronas screenspaceExpansion"                              , "misc.coronas.screenspaceExpansion"                            , joaat("misc.coronas.screenspaceExpansion")                          , V_FLOAT },
                {"misc coronas screenspaceExpansionWater"                         , "misc.coronas.screenspaceExpansionWater"                       , joaat("misc.coronas.screenspaceExpansionWater")                     , V_FLOAT },
                {"misc coronas zBiasMultiplier"                                   , "misc.coronas.zBiasMultiplier"                                 , joaat("misc.coronas.zBiasMultiplier")                               , V_FLOAT },
                {"misc coronas zBiasDistanceNear"                                 , "misc.coronas.zBiasDistanceNear"                               , joaat("misc.coronas.zBiasDistanceNear")                             , V_FLOAT },
                {"misc coronas zBiasDistanceFar"                                  , "misc.coronas.zBiasDistanceFar"                                , joaat("misc.coronas.zBiasDistanceFar")                              , V_FLOAT },
                {"misc coronas zBias fromFinalSizeMultiplier"                     , "misc.coronas.zBias.fromFinalSizeMultiplier"                   , joaat("misc.coronas.zBias.fromFinalSizeMultiplier")                 , V_FLOAT },
                {"misc coronas occlusionSizeScale"                                , "misc.coronas.occlusionSizeScale"                              , joaat("misc.coronas.occlusionSizeScale")                            , V_FLOAT },
                {"misc coronas occlusionSizeMax"                                  , "misc.coronas.occlusionSizeMax"                                , joaat("misc.coronas.occlusionSizeMax")                              , V_FLOAT },
                {"misc coronas flareCoronaSizeRatio"                              , "misc.coronas.flareCoronaSizeRatio"                            , joaat("misc.coronas.flareCoronaSizeRatio")                          , V_FLOAT },
                {"misc coronas flareMinAngleDegToFadeIn"                          , "misc.coronas.flareMinAngleDegToFadeIn"                        , joaat("misc.coronas.flareMinAngleDegToFadeIn")                      , V_FLOAT },
                {"misc coronas flareScreenCenterOffsetSizeMult"                   , "misc.coronas.flareScreenCenterOffsetSizeMult"                 , joaat("misc.coronas.flareScreenCenterOffsetSizeMult")               , V_FLOAT },
                {"misc coronas flareColShiftR"                                    , "misc.coronas.flareColShiftR"                                  , joaat("misc.coronas.flareColShiftR")                                , V_FLOAT },
                {"misc coronas flareColShiftG"                                    , "misc.coronas.flareColShiftG"                                  , joaat("misc.coronas.flareColShiftG")                                , V_FLOAT },
                {"misc coronas flareColShiftB"                                    , "misc.coronas.flareColShiftB"                                  , joaat("misc.coronas.flareColShiftB")                                , V_FLOAT },
                {"misc coronas baseValueR"                                        , "misc.coronas.baseValueR"                                      , joaat("misc.coronas.baseValueR")                                    , V_FLOAT },
                {"misc coronas baseValueG"                                        , "misc.coronas.baseValueG"                                      , joaat("misc.coronas.baseValueG")                                    , V_FLOAT },
                {"misc coronas baseValueB"                                        , "misc.coronas.baseValueB"                                      , joaat("misc.coronas.baseValueB")                                    , V_FLOAT },
                {"misc coronas zBias min"                                         , "misc.coronas.zBias.min"                                       , joaat("misc.coronas.zBias.min")                                     , V_FLOAT },
                {"misc coronas zBias max"                                         , "misc.coronas.zBias.max"                                       , joaat("misc.coronas.zBias.max")                                     , V_FLOAT },
                {"misc coronas zBias additionalBias WaterReflection"              , "misc.coronas.zBias.additionalBias.WaterReflection"            , joaat("misc.coronas.zBias.additionalBias.WaterReflection")          , V_FLOAT },
                {"misc coronas zBias additionalBias EnvReflection"                , "misc.coronas.zBias.additionalBias.EnvReflection"              , joaat("misc.coronas.zBias.additionalBias.EnvReflection")            , V_FLOAT },
                {"misc coronas zBias additionalBias MirrorReflection"             , "misc.coronas.zBias.additionalBias.MirrorReflection"           , joaat("misc.coronas.zBias.additionalBias.MirrorReflection")         , V_FLOAT },
                {"misc coronas dir mult fadeoff start"                            , "misc.coronas.dir.mult.fadeoff.start"                          , joaat("misc.coronas.dir.mult.fadeoff.start")                        , V_FLOAT },
                {"misc coronas dir mult fadeoff dist"                             , "misc.coronas.dir.mult.fadeoff.dist"                           , joaat("misc.coronas.dir.mult.fadeoff.dist")                         , V_FLOAT },
                {"misc coronas flareScreenCenterOffsetVerticalSizeMult"           , "misc.coronas.flareScreenCenterOffsetVerticalSizeMult"         , joaat("misc.coronas.flareScreenCenterOffsetVerticalSizeMult")       , V_FLOAT },
                {"misc coronas underwaterFadeDist"                                , "misc.coronas.underwaterFadeDist"                              , joaat("misc.coronas.underwaterFadeDist")                            , V_FLOAT },
                {"misc coronas m_doNGCoronas"                                     , "misc.coronas.m_doNGCoronas"                                   , joaat("misc.coronas.m_doNGCoronas")                                 , V_FLOAT },
                {"misc coronas m_rotationSpeed"                                   , "misc.coronas.m_rotationSpeed"                                 , joaat("misc.coronas.m_rotationSpeed")                               , V_FLOAT },
                {"misc coronas m_rotationSpeedRampedUp"                           , "misc.coronas.m_rotationSpeedRampedUp"                         , joaat("misc.coronas.m_rotationSpeedRampedUp")                       , V_FLOAT },
                {"misc coronas m_layer1Muliplier"                                 , "misc.coronas.m_layer1Muliplier"                               , joaat("misc.coronas.m_layer1Muliplier")                             , V_FLOAT },
                {"misc coronas m_layer2Muliplier"                                 , "misc.coronas.m_layer2Muliplier"                               , joaat("misc.coronas.m_layer2Muliplier")                             , V_FLOAT },
                {"misc coronas m_scaleRampUp"                                     , "misc.coronas.m_scaleRampUp"                                   , joaat("misc.coronas.m_scaleRampUp")                                 , V_FLOAT },
                {"misc coronas m_scaleRampUpAngle"                                , "misc.coronas.m_scaleRampUpAngle"                              , joaat("misc.coronas.m_scaleRampUpAngle")                            , V_FLOAT },
                {"misc coronas flareScreenCenterOffsetVerticalSizeMult"           , "misc.coronas.flareScreenCenterOffsetVerticalSizeMult"         , joaat("misc.coronas.flareScreenCenterOffsetVerticalSizeMult")       , V_FLOAT },
                {"misc coronas screenEdgeMinDistForFade"                          , "misc.coronas.screenEdgeMinDistForFade"                        , joaat("misc.coronas.screenEdgeMinDistForFade")                      , V_FLOAT },
                {"misc BloomIntensityClamp HD"                                    , "misc.BloomIntensityClamp.HD"                                  , joaat("misc.BloomIntensityClamp.HD")                                , V_FLOAT },
                {"misc BloomIntensityClamp SD"                                    , "misc.BloomIntensityClamp.SD"                                  , joaat("misc.BloomIntensityClamp.SD")                                , V_FLOAT },
                {"misc CrossPMultiplier RimLight"                                 , "misc.CrossPMultiplier.RimLight"                               , joaat("misc.CrossPMultiplier.RimLight")                             , V_FLOAT },
                {"misc CrossPMultiplier GlobalEnvironmentReflection"              , "misc.CrossPMultiplier.GlobalEnvironmentReflection"            , joaat("misc.CrossPMultiplier.GlobalEnvironmentReflection")          , V_FLOAT },
                {"misc CrossPMultiplier Gamma"                                    , "misc.CrossPMultiplier.Gamma"                                  , joaat("misc.CrossPMultiplier.Gamma")                                , V_FLOAT },
                {"misc CrossPMultiplier MidBlur"                                  , "misc.CrossPMultiplier.MidBlur"                                , joaat("misc.CrossPMultiplier.MidBlur")                              , V_FLOAT },
                {"misc CrossPMultiplier Farblur"                                  , "misc.CrossPMultiplier.Farblur"                                , joaat("misc.CrossPMultiplier.Farblur")                              , V_FLOAT },
                {"misc CrossPMultiplier SkyMultiplier"                            , "misc.CrossPMultiplier.SkyMultiplier"                          , joaat("misc.CrossPMultiplier.SkyMultiplier")                        , V_FLOAT },
                {"misc CrossPMultiplier Desaturation"                             , "misc.CrossPMultiplier.Desaturation"                           , joaat("misc.CrossPMultiplier.Desaturation")                         , V_FLOAT },
                {"misc HiDof nearBlur"                                            , "misc.HiDof.nearBlur"                                          , joaat("misc.HiDof.nearBlur")                                        , V_FLOAT },
                {"misc HiDof midBlur"                                             , "misc.HiDof.midBlur"                                           , joaat("misc.HiDof.midBlur")                                         , V_FLOAT },
                {"misc HiDof farBlur"                                             , "misc.HiDof.farBlur"                                           , joaat("misc.HiDof.farBlur")                                         , V_FLOAT },
                {"misc cutscene nearBlurMin"                                      , "misc.cutscene.nearBlurMin"                                    , joaat("misc.cutscene.nearBlurMin")                                  , V_FLOAT },
                {"misc HiDof nearBlur"                                            , "misc.HiDof.nearBlur"                                          , joaat("misc.HiDof.nearBlur")                                        , V_FLOAT },
                {"misc HiDof midBlur"                                             , "misc.HiDof.midBlur"                                           , joaat("misc.HiDof.midBlur")                                         , V_FLOAT },
                {"misc HiDof farBlur"                                             , "misc.HiDof.farBlur"                                           , joaat("misc.HiDof.farBlur")                                         , V_FLOAT },
                {"misc cutscene nearBlurMin"                                      , "misc.cutscene.nearBlurMin"                                    , joaat("misc.cutscene.nearBlurMin")                                  , V_FLOAT },
                {"misc PedKillOverlay duration"                                   , "misc.PedKillOverlay.duration"                                 , joaat("misc.PedKillOverlay.duration")                               , V_FLOAT },
                {"misc DamageOverlay RampUpDuration"                              , "misc.DamageOverlay.RampUpDuration"                            , joaat("misc.DamageOverlay.RampUpDuration")                          , V_FLOAT },
                {"misc DamageOverlay HoldDuration"                                , "misc.DamageOverlay.HoldDuration"                              , joaat("misc.DamageOverlay.HoldDuration")                            , V_FLOAT },
                {"misc DamageOverlay RampDownDuration"                            , "misc.DamageOverlay.RampDownDuration"                          , joaat("misc.DamageOverlay.RampDownDuration")                        , V_FLOAT },
                {"misc DamageOverlay ColorBottom"                                 , "misc.DamageOverlay.ColorBottom.red"                           , joaat("misc.DamageOverlay.ColorBottom.red")                         , V_COL3  },
                {"None"                                                           , "misc.DamageOverlay.ColorBottom.green"                         , joaat("misc.DamageOverlay.ColorBottom.green")                       , V_NONE  },
                {"None"                                                           , "misc.DamageOverlay.ColorBottom.blue"                          , joaat("misc.DamageOverlay.ColorBottom.blue")                        , V_NONE  },
                {"misc DamageOverlay ColorTop"                                    , "misc.DamageOverlay.ColorTop.red"                              , joaat("misc.DamageOverlay.ColorTop.red")                            , V_COL3  },
                {"None"                                                           , "misc.DamageOverlay.ColorTop.green"                            , joaat("misc.DamageOverlay.ColorTop.green")                          , V_NONE  },
                {"None"                                                           , "misc.DamageOverlay.ColorTop.blue"                             , joaat("misc.DamageOverlay.ColorTop.blue")                           , V_NONE  },
                {"misc DamageOverlay GlobalAlphaBottom"                           , "misc.DamageOverlay.GlobalAlphaBottom"                         , joaat("misc.DamageOverlay.GlobalAlphaBottom")                       , V_FLOAT },
                {"misc DamageOverlay GlobalAlphaTop"                              , "misc.DamageOverlay.GlobalAlphaTop"                            , joaat("misc.DamageOverlay.GlobalAlphaTop")                          , V_FLOAT },
                {"misc DamageOverlay SpriteLength"                                , "misc.DamageOverlay.SpriteLength"                              , joaat("misc.DamageOverlay.SpriteLength")                            , V_FLOAT },
                {"misc DamageOverlay SpriteBaseWidth"                             , "misc.DamageOverlay.SpriteBaseWidth"                           , joaat("misc.DamageOverlay.SpriteBaseWidth")                         , V_FLOAT },
                {"misc DamageOverlay SpriteTipWidth"                              , "misc.DamageOverlay.SpriteTipWidth"                            , joaat("misc.DamageOverlay.SpriteTipWidth")                          , V_FLOAT },
                {"misc DamageOverlay HorizontalFadeSoftness"                      , "misc.DamageOverlay.HorizontalFadeSoftness"                    , joaat("misc.DamageOverlay.HorizontalFadeSoftness")                  , V_FLOAT },
                {"misc DamageOverlay VerticalFadeOffset"                          , "misc.DamageOverlay.VerticalFadeOffset"                        , joaat("misc.DamageOverlay.VerticalFadeOffset")                      , V_FLOAT },
                {"misc DamageOverlay AngleScalingMult"                            , "misc.DamageOverlay.AngleScalingMult"                          , joaat("misc.DamageOverlay.AngleScalingMult")                        , V_FLOAT },
                {"misc DamageOverlay FP RampUpDuration"                           , "misc.DamageOverlay.FP.RampUpDuration"                         , joaat("misc.DamageOverlay.FP.RampUpDuration")                       , V_FLOAT },
                {"misc DamageOverlay FP HoldDuration"                             , "misc.DamageOverlay.FP.HoldDuration"                           , joaat("misc.DamageOverlay.FP.HoldDuration")                         , V_FLOAT },
                {"misc DamageOverlay FP RampDownDuration"                         , "misc.DamageOverlay.FP.RampDownDuration"                       , joaat("misc.DamageOverlay.FP.RampDownDuration")                     , V_FLOAT },
                {"misc DamageOverlay FP ColorBottom"                              , "misc.DamageOverlay.FP.ColorBottom.red"                        , joaat("misc.DamageOverlay.FP.ColorBottom.red")                      , V_COL3  },
                {"None"                                                           , "misc.DamageOverlay.FP.ColorBottom.green"                      , joaat("misc.DamageOverlay.FP.ColorBottom.green")                    , V_NONE  },
                {"None"                                                           , "misc.DamageOverlay.FP.ColorBottom.blue"                       , joaat("misc.DamageOverlay.FP.ColorBottom.blue")                     , V_NONE  },
                {"misc DamageOverlay FP ColorTop"                                 , "misc.DamageOverlay.FP.ColorTop.red"                           , joaat("misc.DamageOverlay.FP.ColorTop.red")                         , V_COL3  },
                {"None"                                                           , "misc.DamageOverlay.FP.ColorTop.green"                         , joaat("misc.DamageOverlay.FP.ColorTop.green")                       , V_NONE  },
                {"None"                                                           , "misc.DamageOverlay.FP.ColorTop.blue"                          , joaat("misc.DamageOverlay.FP.ColorTop.blue")                        , V_NONE  },
                {"misc DamageOverlay FP GlobalAlphaBottom"                        , "misc.DamageOverlay.FP.GlobalAlphaBottom"                      , joaat("misc.DamageOverlay.FP.GlobalAlphaBottom")                    , V_FLOAT },
                {"misc DamageOverlay FP GlobalAlphaTop"                           , "misc.DamageOverlay.FP.GlobalAlphaTop"                         , joaat("misc.DamageOverlay.FP.GlobalAlphaTop")                       , V_FLOAT },
                {"misc DamageOverlay FP SpriteLength"                             , "misc.DamageOverlay.FP.SpriteLength"                           , joaat("misc.DamageOverlay.FP.SpriteLength")                         , V_FLOAT },
                {"misc DamageOverlay FP SpriteBaseWidth"                          , "misc.DamageOverlay.FP.SpriteBaseWidth"                        , joaat("misc.DamageOverlay.FP.SpriteBaseWidth")                      , V_FLOAT },
                {"misc DamageOverlay FP SpriteTipWidth"                           , "misc.DamageOverlay.FP.SpriteTipWidth"                         , joaat("misc.DamageOverlay.FP.SpriteTipWidth")                       , V_FLOAT },
                {"misc DamageOverlay FP HorizontalFadeSoftness"                   , "misc.DamageOverlay.FP.HorizontalFadeSoftness"                 , joaat("misc.DamageOverlay.FP.HorizontalFadeSoftness")               , V_FLOAT },
                {"misc DamageOverlay FP VerticalFadeOffset"                       , "misc.DamageOverlay.FP.VerticalFadeOffset"                     , joaat("misc.DamageOverlay.FP.VerticalFadeOffset")                   , V_FLOAT },
                {"misc DamageOverlay FP AngleScalingMult"                         , "misc.DamageOverlay.FP.AngleScalingMult"                       , joaat("misc.DamageOverlay.FP.AngleScalingMult")                     , V_FLOAT },
                {"misc DamageOverlay ScreenSafeZoneLength"                        , "misc.DamageOverlay.ScreenSafeZoneLength"                      , joaat("misc.DamageOverlay.ScreenSafeZoneLength")                    , V_FLOAT },
                {"misc SniperRifleDof Enabled"                                    , "misc.SniperRifleDof.Enabled"                                  , joaat("misc.SniperRifleDof.Enabled")                                , V_FLOAT },
                {"misc SniperRifleDof NearStart"                                  , "misc.SniperRifleDof.NearStart"                                , joaat("misc.SniperRifleDof.NearStart")                              , V_FLOAT },
                {"misc SniperRifleDof NearEnd"                                    , "misc.SniperRifleDof.NearEnd"                                  , joaat("misc.SniperRifleDof.NearEnd")                                , V_FLOAT },
                {"misc SniperRifleDof FarStart"                                   , "misc.SniperRifleDof.FarStart"                                 , joaat("misc.SniperRifleDof.FarStart")                               , V_FLOAT },
                {"misc SniperRifleDof FarEnd"                                     , "misc.SniperRifleDof.FarEnd"                                   , joaat("misc.SniperRifleDof.FarEnd")                                 , V_FLOAT },
                {"misc MoonDimMult"                                               , "misc.MoonDimMult"                                             , joaat("misc.MoonDimMult")                                           , V_FLOAT },
                {"misc NextGenModifier"                                           , "misc.NextGenModifier"                                         , joaat("misc.NextGenModifier")                                       , V_FLOAT },
        };
        paramsMap["See Through"] =
        {
                {"seeThrough PedQuadSize"                                         , "seeThrough.PedQuadSize"                                       , joaat("seeThrough.PedQuadSize")                                     , V_FLOAT },
                {"seeThrough PedQuadStartDistance"                                , "seeThrough.PedQuadStartDistance"                              , joaat("seeThrough.PedQuadStartDistance")                            , V_FLOAT },
                {"seeThrough PedQuadEndDistance"                                  , "seeThrough.PedQuadEndDistance"                                , joaat("seeThrough.PedQuadEndDistance")                              , V_FLOAT },
                {"seeThrough PedQuadScale"                                        , "seeThrough.PedQuadScale"                                      , joaat("seeThrough.PedQuadScale")                                    , V_FLOAT },
                {"seeThrough FadeStartDistance"                                   , "seeThrough.FadeStartDistance"                                 , joaat("seeThrough.FadeStartDistance")                               , V_FLOAT },
                {"seeThrough FadeEndDistance"                                     , "seeThrough.FadeEndDistance"                                   , joaat("seeThrough.FadeEndDistance")                                 , V_FLOAT },
                {"seeThrough MaxThickness"                                        , "seeThrough.MaxThickness"                                      , joaat("seeThrough.MaxThickness")                                    , V_FLOAT },
                {"seeThrough MinNoiseAmount"                                      , "seeThrough.MinNoiseAmount"                                    , joaat("seeThrough.MinNoiseAmount")                                  , V_FLOAT },
                {"seeThrough MaxNoiseAmount"                                      , "seeThrough.MaxNoiseAmount"                                    , joaat("seeThrough.MaxNoiseAmount")                                  , V_FLOAT },
                {"seeThrough HiLightIntensity"                                    , "seeThrough.HiLightIntensity"                                  , joaat("seeThrough.HiLightIntensity")                                , V_FLOAT },
                {"seeThrough HiLightNoise"                                        , "seeThrough.HiLightNoise"                                      , joaat("seeThrough.HiLightNoise")                                    , V_FLOAT },
                {"seeThrough ColorNear"                                           , "seeThrough.ColorNear.red"                                     , joaat("seeThrough.ColorNear.red")                                   , V_COL3  },
                {"None"                                                           , "seeThrough.ColorNear.green"                                   , joaat("seeThrough.ColorNear.green")                                 , V_NONE  },
                {"None"                                                           , "seeThrough.ColorNear.blue"                                    , joaat("seeThrough.ColorNear.blue")                                  , V_NONE  },
                {"seeThrough ColorFar"                                            , "seeThrough.ColorFar.red"                                      , joaat("seeThrough.ColorFar.red")                                    , V_COL3  },
                {"None"                                                           , "seeThrough.ColorFar.green"                                    , joaat("seeThrough.ColorFar.green")                                  , V_NONE  },
                {"None"                                                           , "seeThrough.ColorFar.blue"                                     , joaat("seeThrough.ColorFar.blue")                                   , V_NONE  },
                {"seeThrough ColorVisibleBase"                                    , "seeThrough.ColorVisibleBase.red"                              , joaat("seeThrough.ColorVisibleBase.red")                            , V_COL3  },
                {"None"                                                           , "seeThrough.ColorVisibleBase.green"                            , joaat("seeThrough.ColorVisibleBase.green")                          , V_NONE  },
                {"None"                                                           , "seeThrough.ColorVisibleBase.blue"                             , joaat("seeThrough.ColorVisibleBase.blue")                           , V_NONE  },
                {"seeThrough ColorVisibleWarm"                                    , "seeThrough.ColorVisibleWarm.red"                              , joaat("seeThrough.ColorVisibleWarm.red")                            , V_COL3  },
                {"None"                                                           , "seeThrough.ColorVisibleWarm.green"                            , joaat("seeThrough.ColorVisibleWarm.green")                          , V_NONE  },
                {"None"                                                           , "seeThrough.ColorVisibleWarm.blue"                             , joaat("seeThrough.ColorVisibleWarm.blue")                           , V_NONE  },
                {"seeThrough ColorVisibleHot"                                     , "seeThrough.ColorVisibleHot.red"                               , joaat("seeThrough.ColorVisibleHot.red")                             , V_COL3  },
                {"None"                                                           , "seeThrough.ColorVisibleHot.green"                             , joaat("seeThrough.ColorVisibleHot.green")                           , V_NONE  },
                {"None"                                                           , "seeThrough.ColorVisibleHot.blue"                              , joaat("seeThrough.ColorVisibleHot.blue")                            , V_NONE  },
        };
        paramsMap["Tonemapping"] =
        {
                {"Tonemapping bright filmic A"                                    , "Tonemapping.bright.filmic.A"                                  , joaat("Tonemapping.bright.filmic.A")                                , V_FLOAT },
                {"Tonemapping bright filmic B"                                    , "Tonemapping.bright.filmic.B"                                  , joaat("Tonemapping.bright.filmic.B")                                , V_FLOAT },
                {"Tonemapping bright filmic C"                                    , "Tonemapping.bright.filmic.C"                                  , joaat("Tonemapping.bright.filmic.C")                                , V_FLOAT },
                {"Tonemapping bright filmic D"                                    , "Tonemapping.bright.filmic.D"                                  , joaat("Tonemapping.bright.filmic.D")                                , V_FLOAT },
                {"Tonemapping bright filmic E"                                    , "Tonemapping.bright.filmic.E"                                  , joaat("Tonemapping.bright.filmic.E")                                , V_FLOAT },
                {"Tonemapping bright filmic F"                                    , "Tonemapping.bright.filmic.F"                                  , joaat("Tonemapping.bright.filmic.F")                                , V_FLOAT },
                {"Tonemapping bright filmic W"                                    , "Tonemapping.bright.filmic.W"                                  , joaat("Tonemapping.bright.filmic.W")                                , V_FLOAT },
                {"Tonemapping bright filmic exposure"                             , "Tonemapping.bright.filmic.exposure"                           , joaat("Tonemapping.bright.filmic.exposure")                         , V_FLOAT },
                {"Tonemapping dark filmic A"                                      , "Tonemapping.dark.filmic.A"                                    , joaat("Tonemapping.dark.filmic.A")                                  , V_FLOAT },
                {"Tonemapping dark filmic B"                                      , "Tonemapping.dark.filmic.B"                                    , joaat("Tonemapping.dark.filmic.B")                                  , V_FLOAT },
                {"Tonemapping dark filmic C"                                      , "Tonemapping.dark.filmic.C"                                    , joaat("Tonemapping.dark.filmic.C")                                  , V_FLOAT },
                {"Tonemapping dark filmic D"                                      , "Tonemapping.dark.filmic.D"                                    , joaat("Tonemapping.dark.filmic.D")                                  , V_FLOAT },
                {"Tonemapping dark filmic E"                                      , "Tonemapping.dark.filmic.E"                                    , joaat("Tonemapping.dark.filmic.E")                                  , V_FLOAT },
                {"Tonemapping dark filmic F"                                      , "Tonemapping.dark.filmic.F"                                    , joaat("Tonemapping.dark.filmic.F")                                  , V_FLOAT },
                {"Tonemapping dark filmic W"                                      , "Tonemapping.dark.filmic.W"                                    , joaat("Tonemapping.dark.filmic.W")                                  , V_FLOAT },
                {"Tonemapping dark filmic exposure"                               , "Tonemapping.dark.filmic.exposure"                             , joaat("Tonemapping.dark.filmic.exposure")                           , V_FLOAT },
        };
        paramsMap["Exposure"] =
        {
                {"Exposure curve scale"                                           , "Exposure.curve.scale"                                         , joaat("Exposure.curve.scale")                                       , V_FLOAT },
                {"Exposure curve power"                                           , "Exposure.curve.power"                                         , joaat("Exposure.curve.power")                                       , V_FLOAT },
                {"Exposure curve offset"                                          , "Exposure.curve.offset"                                        , joaat("Exposure.curve.offset")                                      , V_FLOAT },
        };
        paramsMap["Adaptation"] =
        {
                {"Adaptation min step size"                                       , "Adaptation.min.step.size"                                     , joaat("Adaptation.min.step.size")                                   , V_FLOAT },
                {"Adaptation max step size"                                       , "Adaptation.max.step.size"                                     , joaat("Adaptation.max.step.size")                                   , V_FLOAT },
                {"Adaptation step size mult"                                      , "Adaptation.step.size.mult"                                    , joaat("Adaptation.step.size.mult")                                  , V_FLOAT },
                {"Adaptation threshold"                                           , "Adaptation.threshold"                                         , joaat("Adaptation.threshold")                                       , V_FLOAT },
                {"Adaptation sun exposure tweak"                                  , "Adaptation.sun.exposure.tweak"                                , joaat("Adaptation.sun.exposure.tweak")                              , V_FLOAT },
        };
        paramsMap["Cloud generation"] =
        {
                {"cloudgen frequency"                                             , "cloudgen.frequency"                                           , joaat("cloudgen.frequency")                                         , V_FLOAT },
                {"cloudgen scale"                                                 , "cloudgen.scale"                                               , joaat("cloudgen.scale")                                             , V_FLOAT },
                {"cloudgen edge detail scale"                                     , "cloudgen.edge.detail.scale"                                   , joaat("cloudgen.edge.detail.scale")                                 , V_FLOAT },
                {"cloudgen overlay detail scale"                                  , "cloudgen.overlay.detail.scale"                                , joaat("cloudgen.overlay.detail.scale")                              , V_FLOAT },
        };
        paramsMap["Cloud speeds"] =
        {
                {"cloud speed large"                                              , "cloud.speed.large"                                            , joaat("cloud.speed.large")                                          , V_FLOAT },
                {"cloud speed small"                                              , "cloud.speed.small"                                            , joaat("cloud.speed.small")                                          , V_FLOAT },
                {"cloud speed overall"                                            , "cloud.speed.overall"                                          , joaat("cloud.speed.overall")                                        , V_FLOAT },
                {"cloud speed edge"                                               , "cloud.speed.edge"                                             , joaat("cloud.speed.edge")                                           , V_FLOAT },
                {"cloud speed hats"                                               , "cloud.speed.hats"                                             , joaat("cloud.speed.hats")                                           , V_FLOAT },
        };
        paramsMap["lod fade distances"] =
        {
                {"lod fadedist orphanhd"                                          , "lod.fadedist.orphanhd"                                        , joaat("lod.fadedist.orphanhd")                                      , V_FLOAT },
                {"lod fadedist hd"                                                , "lod.fadedist.hd"                                              , joaat("lod.fadedist.hd")                                            , V_FLOAT },
                {"lod fadedist lod"                                               , "lod.fadedist.lod"                                             , joaat("lod.fadedist.lod")                                           , V_FLOAT },
                {"lod fadedist slod1"                                             , "lod.fadedist.slod1"                                           , joaat("lod.fadedist.slod1")                                         , V_FLOAT },
                {"lod fadedist slod2"                                             , "lod.fadedist.slod2"                                           , joaat("lod.fadedist.slod2")                                         , V_FLOAT },
                {"lod fadedist slod3"                                             , "lod.fadedist.slod3"                                           , joaat("lod.fadedist.slod3")                                         , V_FLOAT },
                {"lod fadedist slod4"                                             , "lod.fadedist.slod4"                                           , joaat("lod.fadedist.slod4")                                         , V_FLOAT },
        };
        paramsMap["sky plane"] =
        {
                {"skyplane offset"                                                , "skyplane.offset"                                              , joaat("skyplane.offset")                                            , V_FLOAT },
                {"skyplane fog fade start"                                        , "skyplane.fog.fade.start"                                      , joaat("skyplane.fog.fade.start")                                    , V_FLOAT },
                {"skyplane fog fade end"                                          , "skyplane.fog.fade.end"                                        , joaat("skyplane.fog.fade.end")                                      , V_FLOAT },
        };
        paramsMap["Rim lighting"] =
        {
                {"rim lighting width start"                                       , "rim.lighting.width.start"                                     , joaat("rim.lighting.width.start")                                   , V_FLOAT },
                {"rim lighting width end"                                         , "rim.lighting.width.end"                                       , joaat("rim.lighting.width.end")                                     , V_FLOAT },
                {"rim lighting transition angle"                                  , "rim.lighting.transition.angle"                                , joaat("rim.lighting.transition.angle")                              , V_FLOAT },
                {"rim lighting downmask angle"                                    , "rim.lighting.downmask.angle"                                  , joaat("rim.lighting.downmask.angle")                                , V_FLOAT },
                {"rim lighting main colour intensity"                             , "rim.lighting.main.colour.intensity"                           , joaat("rim.lighting.main.colour.intensity")                         , V_FLOAT },
                {"rim lighting offangle colour intensity"                         , "rim.lighting.offangle.colour.intensity"                       , joaat("rim.lighting.offangle.colour.intensity")                     , V_FLOAT },
        };
        paramsMap["Reflection tweaks"] =
        {
                {"reflection tweaks interior artificial boost"                    , "reflection.tweaks.interior.artificial.boost"                  , joaat("reflection.tweaks.interior.artificial.boost")                , V_FLOAT },
                {"reflection tweaks exterior artificial boost"                    , "reflection.tweaks.exterior.artificial.boost"                  , joaat("reflection.tweaks.exterior.artificial.boost")                , V_FLOAT },
                {"reflection tweaks emissive boost"                               , "reflection.tweaks.emissive.boost"                             , joaat("reflection.tweaks.emissive.boost")                           , V_FLOAT },
        };
        paramsMap["Light component cutoff"] =
        {
                {"lights cutoff distance"                                         , "lights.cutoff.distance"                                       , joaat("lights.cutoff.distance")                                     , V_FLOAT },
                {"lights cutoff shadow distance"                                  , "lights.cutoff.shadow.distance"                                , joaat("lights.cutoff.shadow.distance")                              , V_FLOAT },
                {"lights cutoff specular distance"                                , "lights.cutoff.specular.distance"                              , joaat("lights.cutoff.specular.distance")                            , V_FLOAT },
                {"lights cutoff volumetric distance"                              , "lights.cutoff.volumetric.distance"                            , joaat("lights.cutoff.volumetric.distance")                          , V_FLOAT },
                {"lights cutoff nonGbuf distance"                                 , "lights.cutoff.nonGbuf.distance"                               , joaat("lights.cutoff.nonGbuf.distance")                             , V_FLOAT },
                {"lights cutoff map distance"                                     , "lights.cutoff.map.distance"                                   , joaat("lights.cutoff.map.distance")                                 , V_FLOAT },
        };
        paramsMap["Bloom"] =
        {
                {"bloom threshold min"                                            , "bloom.threshold.min"                                          , joaat("bloom.threshold.min")                                        , V_FLOAT },
                {"bloom threshold expsoure diff min"                              , "bloom.threshold.expsoure.diff.min"                            , joaat("bloom.threshold.expsoure.diff.min")                          , V_FLOAT },
                {"bloom threshold expsoure diff max"                              , "bloom.threshold.expsoure.diff.max"                            , joaat("bloom.threshold.expsoure.diff.max")                          , V_FLOAT },
                {"bloom threshold power"                                          , "bloom.threshold.power"                                        , joaat("bloom.threshold.power")                                      , V_FLOAT },
                {"bloom blurblendmult large"                                      , "bloom.blurblendmult.large"                                    , joaat("bloom.blurblendmult.large")                                  , V_FLOAT },
                {"bloom blurblendmult med"                                        , "bloom.blurblendmult.med"                                      , joaat("bloom.blurblendmult.med")                                    , V_FLOAT },
                {"bloom blurblendmult small"                                      , "bloom.blurblendmult.small"                                    , joaat("bloom.blurblendmult.small")                                  , V_FLOAT },
        };
        paramsMap["Dynmaic bake paramaters"] =
        {
                {"dynamic bake start min"                                         , "dynamic.bake.start.min"                                       , joaat("dynamic.bake.start.min")                                     , V_FLOAT },
                {"dynamic bake start max"                                         , "dynamic.bake.start.max"                                       , joaat("dynamic.bake.start.max")                                     , V_FLOAT },
                {"dynamic bake end min"                                           , "dynamic.bake.end.min"                                         , joaat("dynamic.bake.end.min")                                       , V_FLOAT },
                {"dynamic bake end max"                                           , "dynamic.bake.end.max"                                         , joaat("dynamic.bake.end.max")                                       , V_FLOAT },
        };
        paramsMap["Minimap alpha"] =
        {
                {"UI minimap alpha"                                               , "UI.minimap.alpha"                                             , joaat("UI.minimap.alpha")                                           , V_FLOAT },
        };
        paramsMap["Puddles"] =
        {
                {"puddles scale"                                                  , "puddles.scale"                                                , joaat("puddles.scale")                                              , V_FLOAT },
                {"puddles range"                                                  , "puddles.range"                                                , joaat("puddles.range")                                              , V_FLOAT },
                {"puddles depth"                                                  , "puddles.depth"                                                , joaat("puddles.depth")                                              , V_FLOAT },
                {"puddles depthtest"                                              , "puddles.depthtest"                                            , joaat("puddles.depthtest")                                          , V_FLOAT },
                {"puddles strength"                                               , "puddles.strength"                                             , joaat("puddles.strength")                                           , V_FLOAT },
                {"puddles animspeed"                                              , "puddles.animspeed"                                            , joaat("puddles.animspeed")                                          , V_FLOAT },
                {"puddles ripples minsize"                                        , "puddles.ripples.minsize"                                      , joaat("puddles.ripples.minsize")                                    , V_FLOAT },
                {"puddles ripples maxsize"                                        , "puddles.ripples.maxsize"                                      , joaat("puddles.ripples.maxsize")                                    , V_FLOAT },
                {"puddles ripples speed"                                          , "puddles.ripples.speed"                                        , joaat("puddles.ripples.speed")                                      , V_FLOAT },
                {"puddles ripples minduration"                                    , "puddles.ripples.minduration"                                  , joaat("puddles.ripples.minduration")                                , V_FLOAT },
                {"puddles ripples maxduration"                                    , "puddles.ripples.maxduration"                                  , joaat("puddles.ripples.maxduration")                                , V_FLOAT },
                {"puddles ripples dropfactor"                                     , "puddles.ripples.dropfactor"                                   , joaat("puddles.ripples.dropfactor")                                 , V_FLOAT },
                {"puddles ripples windstrength"                                   , "puddles.ripples.windstrength"                                 , joaat("puddles.ripples.windstrength")                               , V_FLOAT },
                {"puddles ripples ripplestrength"                                 , "puddles.ripples.ripplestrength"                               , joaat("puddles.ripples.ripplestrength")                             , V_FLOAT },
                {"puddles ripples winddamping"                                    , "puddles.ripples.winddamping"                                  , joaat("puddles.ripples.winddamping")                                , V_FLOAT },
                {"puddles ripples playerspeed"                                    , "puddles.ripples.playerspeed"                                  , joaat("puddles.ripples.playerspeed")                                , V_FLOAT },
                {"puddles ripples playermapsize"                                  , "puddles.ripples.playermapsize"                                , joaat("puddles.ripples.playermapsize")                              , V_FLOAT },
        };
        paramsMap["LOD lights - do not change these"] =
        {
                {"lodlight small range"                                           , "lodlight.small.range"                                         , joaat("lodlight.small.range")                                       , V_FLOAT },
                {"lodlight medium range"                                          , "lodlight.medium.range"                                        , joaat("lodlight.medium.range")                                      , V_FLOAT },
                {"lodlight large range"                                           , "lodlight.large.range"                                         , joaat("lodlight.large.range")                                       , V_FLOAT },
                {"lodlight small fade"                                            , "lodlight.small.fade"                                          , joaat("lodlight.small.fade")                                        , V_FLOAT },
                {"lodlight medium fade"                                           , "lodlight.medium.fade"                                         , joaat("lodlight.medium.fade")                                       , V_FLOAT },
                {"lodlight large fade"                                            , "lodlight.large.fade"                                          , joaat("lodlight.large.fade")                                        , V_FLOAT },
                {"lodlight small corona range"                                    , "lodlight.small.corona.range"                                  , joaat("lodlight.small.corona.range")                                , V_FLOAT },
                {"lodlight medium corona range"                                   , "lodlight.medium.corona.range"                                 , joaat("lodlight.medium.corona.range")                               , V_FLOAT },
                {"lodlight large corona range"                                    , "lodlight.large.corona.range"                                  , joaat("lodlight.large.corona.range")                                , V_FLOAT },
                {"lodlight corona size"                                           , "lodlight.corona.size"                                         , joaat("lodlight.corona.size")                                       , V_FLOAT },
                {"lodlight sphere expand size mult"                               , "lodlight.sphere.expand.size.mult"                             , joaat("lodlight.sphere.expand.size.mult")                           , V_FLOAT },
                {"lodlight sphere expand start"                                   , "lodlight.sphere.expand.start"                                 , joaat("lodlight.sphere.expand.start")                               , V_FLOAT },
                {"lodlight sphere expand end"                                     , "lodlight.sphere.expand.end"                                   , joaat("lodlight.sphere.expand.end")                                 , V_FLOAT },
        };
        paramsMap["Bright Lights"] =
        {
                {"misc brightlight distculled FadeStart"                          , "misc.brightlight.distculled.FadeStart"                        , joaat("misc.brightlight.distculled.FadeStart")                      , V_FLOAT },
                {"misc brightlight distculled FadeEnd"                            , "misc.brightlight.distculled.FadeEnd"                          , joaat("misc.brightlight.distculled.FadeEnd")                        , V_FLOAT },
                {"misc brightlight notdistculled FadeStart"                       , "misc.brightlight.notdistculled.FadeStart"                     , joaat("misc.brightlight.notdistculled.FadeStart")                   , V_FLOAT },
                {"misc brightlight notdistculled FadeEnd"                         , "misc.brightlight.notdistculled.FadeEnd"                       , joaat("misc.brightlight.notdistculled.FadeEnd")                     , V_FLOAT },
                {"misc brightlight globalIntensity"                               , "misc.brightlight.globalIntensity"                             , joaat("misc.brightlight.globalIntensity")                           , V_FLOAT },
                {"misc brightlight railwayItensity"                               , "misc.brightlight.railwayItensity"                             , joaat("misc.brightlight.railwayItensity")                           , V_FLOAT },
                {"misc brightlight coronaZBias"                                   , "misc.brightlight.coronaZBias"                                 , joaat("misc.brightlight.coronaZBias")                               , V_FLOAT },
        };
        paramsMap["Vehicle Light Night/Day Settings"] =
        {
                {"vehicle lights sunrise"                                         , "vehicle.lights.sunrise"                                       , joaat("vehicle.lights.sunrise")                                     , V_FLOAT },
                {"vehicle lights sunset"                                          , "vehicle.lights.sunset"                                        , joaat("vehicle.lights.sunset")                                      , V_FLOAT },
        };
        paramsMap["Vehicle light fade distances"] =
        {
                {"vehicle lights fadeDistance main"                               , "vehicle.lights.fadeDistance.main"                             , joaat("vehicle.lights.fadeDistance.main")                           , V_FLOAT },
                {"vehicle lights fadeDistance secondary"                          , "vehicle.lights.fadeDistance.secondary"                        , joaat("vehicle.lights.fadeDistance.secondary")                      , V_FLOAT },
                {"vehicle lights fadeDistance sirens"                             , "vehicle.lights.fadeDistance.sirens"                           , joaat("vehicle.lights.fadeDistance.sirens")                         , V_FLOAT },
                {"vehicle lights fadeDistance AO"                                 , "vehicle.lights.fadeDistance.AO"                               , joaat("vehicle.lights.fadeDistance.AO")                             , V_FLOAT },
                {"vehicle lights fadeDistance neon"                               , "vehicle.lights.fadeDistance.neon"                             , joaat("vehicle.lights.fadeDistance.neon")                           , V_FLOAT },
                {"vehicle lights fadeLength"                                      , "vehicle.lights.fadeLength"                                    , joaat("vehicle.lights.fadeLength")                                  , V_FLOAT },
        };
        paramsMap["Particle Shadow intensity"] =
        {
                {"particles shadowintensity"                                      , "particles.shadowintensity"                                    , joaat("particles.shadowintensity")                                  , V_FLOAT },
        };
        paramsMap["Dark Light"] =
        {
                {"dark light color"                                               , "dark.light.color.red"                                         , joaat("dark.light.color.red")                                       , V_COL3  },
                {"None"                                                           , "dark.light.color.green"                                       , joaat("dark.light.color.green")                                     , V_NONE  },
                {"None"                                                           , "dark.light.color.blue"                                        , joaat("dark.light.color.blue")                                      , V_NONE  },
                {"dark light intensity"                                           , "dark.light.intensity"                                         , joaat("dark.light.intensity")                                       , V_FLOAT },
                {"dark light radius"                                              , "dark.light.radius"                                            , joaat("dark.light.radius")                                          , V_FLOAT },
                {"dark light falloffExp"                                          , "dark.light.falloffExp"                                        , joaat("dark.light.falloffExp")                                      , V_FLOAT },
                {"dark light innerConeAngle"                                      , "dark.light.innerConeAngle"                                    , joaat("dark.light.innerConeAngle")                                  , V_FLOAT },
                {"dark light outerConeAngle"                                      , "dark.light.outerConeAngle"                                    , joaat("dark.light.outerConeAngle")                                  , V_FLOAT },
                {"dark light coronaHDR"                                           , "dark.light.coronaHDR"                                         , joaat("dark.light.coronaHDR")                                       , V_FLOAT },
                {"dark light coronaSize"                                          , "dark.light.coronaSize"                                        , joaat("dark.light.coronaSize")                                      , V_FLOAT },
                {"dark light offset x"                                            , "dark.light.offset.x"                                          , joaat("dark.light.offset.x")                                        , V_FLOAT },
                {"dark light offset y"                                            , "dark.light.offset.y"                                          , joaat("dark.light.offset.y")                                        , V_FLOAT },
                {"dark light offset z"                                            , "dark.light.offset.z"                                          , joaat("dark.light.offset.z")                                        , V_FLOAT },
                {"dark fpv light offset x"                                        , "dark.fpv.light.offset.x"                                      , joaat("dark.fpv.light.offset.x")                                    , V_FLOAT },
                {"dark fpv light offset y"                                        , "dark.fpv.light.offset.y"                                      , joaat("dark.fpv.light.offset.y")                                    , V_FLOAT },
                {"dark fpv light offset z"                                        , "dark.fpv.light.offset.z"                                      , joaat("dark.fpv.light.offset.z")                                    , V_FLOAT },
                {"dark useSpec"                                                   , "dark.useSpec"                                                 , joaat("dark.useSpec")                                               , V_FLOAT },
                {"dark useShadows"                                                , "dark.useShadows"                                              , joaat("dark.useShadows")                                            , V_FLOAT },
                {"dark useVolumes"                                                , "dark.useVolumes"                                              , joaat("dark.useVolumes")                                            , V_FLOAT },
                {"dark shadowNearCLip"                                            , "dark.shadowNearCLip"                                          , joaat("dark.shadowNearCLip")                                        , V_FLOAT },
                {"dark shadowBlur"                                                , "dark.shadowBlur"                                              , joaat("dark.shadowBlur")                                            , V_FLOAT },
                {"dark volumeIntensity"                                           , "dark.volumeIntensity"                                         , joaat("dark.volumeIntensity")                                       , V_FLOAT },
                {"dark volumeSize"                                                , "dark.volumeSize"                                              , joaat("dark.volumeSize")                                            , V_FLOAT },
                {"dark volumeExponent"                                            , "dark.volumeExponent"                                          , joaat("dark.volumeExponent")                                        , V_FLOAT },
                {"dark volumeColor x"                                             , "dark.volumeColor.x"                                           , joaat("dark.volumeColor.x")                                         , V_FLOAT },
                {"dark volumeColor y"                                             , "dark.volumeColor.y"                                           , joaat("dark.volumeColor.y")                                         , V_FLOAT },
                {"dark volumeColor z"                                             , "dark.volumeColor.z"                                           , joaat("dark.volumeColor.z")                                         , V_FLOAT },
                {"dark volumeColor w"                                             , "dark.volumeColor.w"                                           , joaat("dark.volumeColor.w")                                         , V_FLOAT },
                {"dark lightDirBase"                                              , "dark.lightDirBase"                                            , joaat("dark.lightDirBase")                                          , V_FLOAT },
        };
        paramsMap["Secondary Dark Light"] =
        {
                {"dark secLight enable"                                           , "dark.secLight.enable"                                         , joaat("dark.secLight.enable")                                       , V_FLOAT },
                {"dark secLight color"                                            , "dark.secLight.color.red"                                      , joaat("dark.secLight.color.red")                                    , V_COL3  },
                {"None"                                                           , "dark.secLight.color.green"                                    , joaat("dark.secLight.color.green")                                  , V_NONE  },
                {"None"                                                           , "dark.secLight.color.blue"                                     , joaat("dark.secLight.color.blue")                                   , V_NONE  },
                {"dark secLight intensity"                                        , "dark.secLight.intensity"                                      , joaat("dark.secLight.intensity")                                    , V_FLOAT },
                {"dark secLight radius"                                           , "dark.secLight.radius"                                         , joaat("dark.secLight.radius")                                       , V_FLOAT },
                {"dark secLight falloffExp"                                       , "dark.secLight.falloffExp"                                     , joaat("dark.secLight.falloffExp")                                   , V_FLOAT },
                {"dark secLight innerConeAngle"                                   , "dark.secLight.innerConeAngle"                                 , joaat("dark.secLight.innerConeAngle")                               , V_FLOAT },
                {"dark secLight outerConeAngle"                                   , "dark.secLight.outerConeAngle"                                 , joaat("dark.secLight.outerConeAngle")                               , V_FLOAT },
                {"dark secLight coronaHDR"                                        , "dark.secLight.coronaHDR"                                      , joaat("dark.secLight.coronaHDR")                                    , V_FLOAT },
                {"dark secLight coronaSize"                                       , "dark.secLight.coronaSize"                                     , joaat("dark.secLight.coronaSize")                                   , V_FLOAT },
                {"dark secLight offset x"                                         , "dark.secLight.offset.x"                                       , joaat("dark.secLight.offset.x")                                     , V_FLOAT },
                {"dark secLight offset y"                                         , "dark.secLight.offset.y"                                       , joaat("dark.secLight.offset.y")                                     , V_FLOAT },
                {"dark secLight offset z"                                         , "dark.secLight.offset.z"                                       , joaat("dark.secLight.offset.z")                                     , V_FLOAT },
                {"dark secLight useSpec"                                          , "dark.secLight.useSpec"                                        , joaat("dark.secLight.useSpec")                                      , V_FLOAT },
                {"dark secLight lightDirBase"                                     , "dark.secLight.lightDirBase"                                   , joaat("dark.secLight.lightDirBase")                                 , V_FLOAT },
        };
        paramsMap["Ped light"] =
        {
                {"ped light direction x"                                          , "ped.light.direction.x"                                        , joaat("ped.light.direction.x")                                      , V_FLOAT },
                {"ped light direction y"                                          , "ped.light.direction.y"                                        , joaat("ped.light.direction.y")                                      , V_FLOAT },
                {"ped light direction z"                                          , "ped.light.direction.z"                                        , joaat("ped.light.direction.z")                                      , V_FLOAT },
                {"ped light fade day start"                                       , "ped.light.fade.day.start"                                     , joaat("ped.light.fade.day.start")                                   , V_FLOAT },
                {"ped light fade day end"                                         , "ped.light.fade.day.end"                                       , joaat("ped.light.fade.day.end")                                     , V_FLOAT },
                {"ped light fade night start"                                     , "ped.light.fade.night.start"                                   , joaat("ped.light.fade.night.start")                                 , V_FLOAT },
                {"ped light fade night end"                                       , "ped.light.fade.night.end"                                     , joaat("ped.light.fade.night.end")                                   , V_FLOAT },
        };
        paramsMap["ped phone light properties"] =
        {
                {"ped phonelight color"                                           , "ped.phonelight.color.r"                                       , joaat("ped.phonelight.color.r")                                     , V_COL3  },
                {"ped phonelight color"                                           , "ped.phonelight.color.g"                                       , joaat("ped.phonelight.color.g")                                     , V_NONE  },
                {"ped phonelight color"                                           , "ped.phonelight.color.b"                                       , joaat("ped.phonelight.color.b")                                     , V_NONE  },
                {"ped phonelight intensity"                                       , "ped.phonelight.intensity"                                     , joaat("ped.phonelight.intensity")                                   , V_FLOAT },
                {"ped phonelight radius"                                          , "ped.phonelight.radius"                                        , joaat("ped.phonelight.radius")                                      , V_FLOAT },
                {"ped phonelight falloffexp"                                      , "ped.phonelight.falloffexp"                                    , joaat("ped.phonelight.falloffexp")                                  , V_FLOAT },
                {"ped phonelight cone inner"                                      , "ped.phonelight.cone.inner"                                    , joaat("ped.phonelight.cone.inner")                                  , V_FLOAT },
                {"ped phonelight cone outer"                                      , "ped.phonelight.cone.outer"                                    , joaat("ped.phonelight.cone.outer")                                  , V_FLOAT },
        };
        paramsMap["Adaptive DOF - PC/NG Only"] =
        {
                {"adaptivedof range verynear"                                     , "adaptivedof.range.verynear"                                   , joaat("adaptivedof.range.verynear")                                 , V_FLOAT },
                {"adaptivedof range near"                                         , "adaptivedof.range.near"                                       , joaat("adaptivedof.range.near")                                     , V_FLOAT },
                {"adaptivedof range mid"                                          , "adaptivedof.range.mid"                                        , joaat("adaptivedof.range.mid")                                      , V_FLOAT },
                {"adaptivedof range far"                                          , "adaptivedof.range.far"                                        , joaat("adaptivedof.range.far")                                      , V_FLOAT },
                {"adaptivedof range veryfar"                                      , "adaptivedof.range.veryfar"                                    , joaat("adaptivedof.range.veryfar")                                  , V_FLOAT },
                {"adaptivedof day verynear nearout"                               , "adaptivedof.day.verynear.nearout"                             , joaat("adaptivedof.day.verynear.nearout")                           , V_FLOAT },
                {"adaptivedof day verynear nearin"                                , "adaptivedof.day.verynear.nearin"                              , joaat("adaptivedof.day.verynear.nearin")                            , V_FLOAT },
                {"adaptivedof day verynear farin"                                 , "adaptivedof.day.verynear.farin"                               , joaat("adaptivedof.day.verynear.farin")                             , V_FLOAT },
                {"adaptivedof day verynear farout"                                , "adaptivedof.day.verynear.farout"                              , joaat("adaptivedof.day.verynear.farout")                            , V_FLOAT },
                {"adaptivedof day near nearout"                                   , "adaptivedof.day.near.nearout"                                 , joaat("adaptivedof.day.near.nearout")                               , V_FLOAT },
                {"adaptivedof day near nearin"                                    , "adaptivedof.day.near.nearin"                                  , joaat("adaptivedof.day.near.nearin")                                , V_FLOAT },
                {"adaptivedof day near farin"                                     , "adaptivedof.day.near.farin"                                   , joaat("adaptivedof.day.near.farin")                                 , V_FLOAT },
                {"adaptivedof day near farout"                                    , "adaptivedof.day.near.farout"                                  , joaat("adaptivedof.day.near.farout")                                , V_FLOAT },
                {"adaptivedof day mid nearout"                                    , "adaptivedof.day.mid.nearout"                                  , joaat("adaptivedof.day.mid.nearout")                                , V_FLOAT },
                {"adaptivedof day mid nearin"                                     , "adaptivedof.day.mid.nearin"                                   , joaat("adaptivedof.day.mid.nearin")                                 , V_FLOAT },
                {"adaptivedof day mid farin"                                      , "adaptivedof.day.mid.farin"                                    , joaat("adaptivedof.day.mid.farin")                                  , V_FLOAT },
                {"adaptivedof day mid farout"                                     , "adaptivedof.day.mid.farout"                                   , joaat("adaptivedof.day.mid.farout")                                 , V_FLOAT },
                {"adaptivedof day far nearout"                                    , "adaptivedof.day.far.nearout"                                  , joaat("adaptivedof.day.far.nearout")                                , V_FLOAT },
                {"adaptivedof day far nearin"                                     , "adaptivedof.day.far.nearin"                                   , joaat("adaptivedof.day.far.nearin")                                 , V_FLOAT },
                {"adaptivedof day far farin"                                      , "adaptivedof.day.far.farin"                                    , joaat("adaptivedof.day.far.farin")                                  , V_FLOAT },
                {"adaptivedof day far farout"                                     , "adaptivedof.day.far.farout"                                   , joaat("adaptivedof.day.far.farout")                                 , V_FLOAT },
                {"adaptivedof day veryfar nearout"                                , "adaptivedof.day.veryfar.nearout"                              , joaat("adaptivedof.day.veryfar.nearout")                            , V_FLOAT },
                {"adaptivedof day veryfar nearin"                                 , "adaptivedof.day.veryfar.nearin"                               , joaat("adaptivedof.day.veryfar.nearin")                             , V_FLOAT },
                {"adaptivedof day veryfar farin"                                  , "adaptivedof.day.veryfar.farin"                                , joaat("adaptivedof.day.veryfar.farin")                              , V_FLOAT },
                {"adaptivedof day veryfar farout"                                 , "adaptivedof.day.veryfar.farout"                               , joaat("adaptivedof.day.veryfar.farout")                             , V_FLOAT },
                {"adaptivedof night verynear nearout"                             , "adaptivedof.night.verynear.nearout"                           , joaat("adaptivedof.night.verynear.nearout")                         , V_FLOAT },
                {"adaptivedof night verynear nearin"                              , "adaptivedof.night.verynear.nearin"                            , joaat("adaptivedof.night.verynear.nearin")                          , V_FLOAT },
                {"adaptivedof night verynear farin"                               , "adaptivedof.night.verynear.farin"                             , joaat("adaptivedof.night.verynear.farin")                           , V_FLOAT },
                {"adaptivedof night verynear farout"                              , "adaptivedof.night.verynear.farout"                            , joaat("adaptivedof.night.verynear.farout")                          , V_FLOAT },
                {"adaptivedof night near nearout"                                 , "adaptivedof.night.near.nearout"                               , joaat("adaptivedof.night.near.nearout")                             , V_FLOAT },
                {"adaptivedof night near nearin"                                  , "adaptivedof.night.near.nearin"                                , joaat("adaptivedof.night.near.nearin")                              , V_FLOAT },
                {"adaptivedof night near farin"                                   , "adaptivedof.night.near.farin"                                 , joaat("adaptivedof.night.near.farin")                               , V_FLOAT },
                {"adaptivedof night near farout"                                  , "adaptivedof.night.near.farout"                                , joaat("adaptivedof.night.near.farout")                              , V_FLOAT },
                {"adaptivedof night mid nearout"                                  , "adaptivedof.night.mid.nearout"                                , joaat("adaptivedof.night.mid.nearout")                              , V_FLOAT },
                {"adaptivedof night mid nearin"                                   , "adaptivedof.night.mid.nearin"                                 , joaat("adaptivedof.night.mid.nearin")                               , V_FLOAT },
                {"adaptivedof night mid farin"                                    , "adaptivedof.night.mid.farin"                                  , joaat("adaptivedof.night.mid.farin")                                , V_FLOAT },
                {"adaptivedof night mid farout"                                   , "adaptivedof.night.mid.farout"                                 , joaat("adaptivedof.night.mid.farout")                               , V_FLOAT },
                {"adaptivedof night far nearout"                                  , "adaptivedof.night.far.nearout"                                , joaat("adaptivedof.night.far.nearout")                              , V_FLOAT },
                {"adaptivedof night far nearin"                                   , "adaptivedof.night.far.nearin"                                 , joaat("adaptivedof.night.far.nearin")                               , V_FLOAT },
                {"adaptivedof night far farin"                                    , "adaptivedof.night.far.farin"                                  , joaat("adaptivedof.night.far.farin")                                , V_FLOAT },
                {"adaptivedof night far farout"                                   , "adaptivedof.night.far.farout"                                 , joaat("adaptivedof.night.far.farout")                               , V_FLOAT },
                {"adaptivedof night veryfar nearout"                              , "adaptivedof.night.veryfar.nearout"                            , joaat("adaptivedof.night.veryfar.nearout")                          , V_FLOAT },
                {"adaptivedof night veryfar nearin"                               , "adaptivedof.night.veryfar.nearin"                             , joaat("adaptivedof.night.veryfar.nearin")                           , V_FLOAT },
                {"adaptivedof night veryfar farin"                                , "adaptivedof.night.veryfar.farin"                              , joaat("adaptivedof.night.veryfar.farin")                            , V_FLOAT },
                {"adaptivedof night veryfar farout"                               , "adaptivedof.night.veryfar.farout"                             , joaat("adaptivedof.night.veryfar.farout")                           , V_FLOAT },
                {"adaptivedof missedrayvalue"                                     , "adaptivedof.missedrayvalue"                                   , joaat("adaptivedof.missedrayvalue")                                 , V_FLOAT },
                {"adaptivedof gridsize"                                           , "adaptivedof.gridsize"                                         , joaat("adaptivedof.gridsize")                                       , V_FLOAT },
                {"adaptivedof gridscalex"                                         , "adaptivedof.gridscalex"                                       , joaat("adaptivedof.gridscalex")                                     , V_FLOAT },
                {"adaptivedof gridscaley"                                         , "adaptivedof.gridscaley"                                       , joaat("adaptivedof.gridscaley")                                     , V_FLOAT },
                {"adaptivedof gridscalexincover"                                  , "adaptivedof.gridscalexincover"                                , joaat("adaptivedof.gridscalexincover")                              , V_FLOAT },
                {"adaptivedof gridscaleyincover"                                  , "adaptivedof.gridscaleyincover"                                , joaat("adaptivedof.gridscaleyincover")                              , V_FLOAT },
                {"adaptivedof gridscalexaiming"                                   , "adaptivedof.gridscalexaiming"                                 , joaat("adaptivedof.gridscalexaiming")                               , V_FLOAT },
                {"adaptivedof gridscaleyaiming"                                   , "adaptivedof.gridscaleyaiming"                                 , joaat("adaptivedof.gridscaleyaiming")                               , V_FLOAT },
                {"adaptivedof gridscalexrungun"                                   , "adaptivedof.gridscalexrungun"                                 , joaat("adaptivedof.gridscalexrungun")                               , V_FLOAT },
                {"adaptivedof gridscaleyrungun"                                   , "adaptivedof.gridscaleyrungun"                                 , joaat("adaptivedof.gridscaleyrungun")                               , V_FLOAT },
                {"adaptivedof smoothtime"                                         , "adaptivedof.smoothtime"                                       , joaat("adaptivedof.smoothtime")                                     , V_FLOAT },
                {"adaptivedof nearplayeradjust"                                   , "adaptivedof.nearplayeradjust"                                 , joaat("adaptivedof.nearplayeradjust")                               , V_FLOAT },
                {"adaptivedof meleefocal nearout"                                 , "adaptivedof.meleefocal.nearout"                               , joaat("adaptivedof.meleefocal.nearout")                             , V_FLOAT },
                {"adaptivedof meleefocal nearin"                                  , "adaptivedof.meleefocal.nearin"                                , joaat("adaptivedof.meleefocal.nearin")                              , V_FLOAT },
                {"adaptivedof meleefocal farin"                                   , "adaptivedof.meleefocal.farin"                                 , joaat("adaptivedof.meleefocal.farin")                               , V_FLOAT },
                {"adaptivedof meleefocal farout"                                  , "adaptivedof.meleefocal.farout"                                , joaat("adaptivedof.meleefocal.farout")                              , V_FLOAT },
                {"adaptivedof aimingfocal nearout"                                , "adaptivedof.aimingfocal.nearout"                              , joaat("adaptivedof.aimingfocal.nearout")                            , V_FLOAT },
                {"adaptivedof aimingfocal nearin"                                 , "adaptivedof.aimingfocal.nearin"                               , joaat("adaptivedof.aimingfocal.nearin")                             , V_FLOAT },
                {"adaptivedof aimingfocal farin"                                  , "adaptivedof.aimingfocal.farin"                                , joaat("adaptivedof.aimingfocal.farin")                              , V_FLOAT },
                {"adaptivedof aimingfocal farout"                                 , "adaptivedof.aimingfocal.farout"                               , joaat("adaptivedof.aimingfocal.farout")                             , V_FLOAT },
                {"adaptivedof rungunfocal nearout"                                , "adaptivedof.rungunfocal.nearout"                              , joaat("adaptivedof.rungunfocal.nearout")                            , V_FLOAT },
                {"adaptivedof rungunfocal nearin"                                 , "adaptivedof.rungunfocal.nearin"                               , joaat("adaptivedof.rungunfocal.nearin")                             , V_FLOAT },
                {"adaptivedof rungunfocal farin"                                  , "adaptivedof.rungunfocal.farin"                                , joaat("adaptivedof.rungunfocal.farin")                              , V_FLOAT },
                {"adaptivedof rungunfocal farout"                                 , "adaptivedof.rungunfocal.farout"                               , joaat("adaptivedof.rungunfocal.farout")                             , V_FLOAT },
                {"adaptivedof incoverfocal nearout"                               , "adaptivedof.incoverfocal.nearout"                             , joaat("adaptivedof.incoverfocal.nearout")                           , V_FLOAT },
                {"adaptivedof incoverfocal nearin"                                , "adaptivedof.incoverfocal.nearin"                              , joaat("adaptivedof.incoverfocal.nearin")                            , V_FLOAT },
                {"adaptivedof incoverfocal farin"                                 , "adaptivedof.incoverfocal.farin"                               , joaat("adaptivedof.incoverfocal.farin")                             , V_FLOAT },
                {"adaptivedof incoverfocal farout"                                , "adaptivedof.incoverfocal.farout"                              , joaat("adaptivedof.incoverfocal.farout")                            , V_FLOAT },
                {"adaptivedof exposureDay"                                        , "adaptivedof.exposureDay"                                      , joaat("adaptivedof.exposureDay")                                    , V_FLOAT },
                {"adaptivedof exposureNight"                                      , "adaptivedof.exposureNight"                                    , joaat("adaptivedof.exposureNight")                                  , V_FLOAT },
                {"adaptivedof exposureMin"                                        , "adaptivedof.exposureMin"                                      , joaat("adaptivedof.exposureMin")                                    , V_FLOAT },
                {"adaptivedof exposureMax"                                        , "adaptivedof.exposureMax"                                      , joaat("adaptivedof.exposureMax")                                    , V_FLOAT },
                {"bokeh brightnessThresholdMin"                                   , "bokeh.brightnessThresholdMin"                                 , joaat("bokeh.brightnessThresholdMin")                               , V_FLOAT },
                {"bokeh brightnessThresholdMax"                                   , "bokeh.brightnessThresholdMax"                                 , joaat("bokeh.brightnessThresholdMax")                               , V_FLOAT },
                {"bokeh brightnessExposureMin"                                    , "bokeh.brightnessExposureMin"                                  , joaat("bokeh.brightnessExposureMin")                                , V_FLOAT },
                {"bokeh brightnessExposureMax"                                    , "bokeh.brightnessExposureMax"                                  , joaat("bokeh.brightnessExposureMax")                                , V_FLOAT },
                {"bokeh cocThreshold"                                             , "bokeh.cocThreshold"                                           , joaat("bokeh.cocThreshold")                                         , V_FLOAT },
                {"bokeh fadeRangeMin"                                             , "bokeh.fadeRangeMin"                                           , joaat("bokeh.fadeRangeMin")                                         , V_FLOAT },
                {"bokeh fadeRangeMax"                                             , "bokeh.fadeRangeMax"                                           , joaat("bokeh.fadeRangeMax")                                         , V_FLOAT },
                {"bokeh sizeMultiplier"                                           , "bokeh.sizeMultiplier"                                         , joaat("bokeh.sizeMultiplier")                                       , V_FLOAT },
                {"bokeh globalAlpha"                                              , "bokeh.globalAlpha"                                            , joaat("bokeh.globalAlpha")                                          , V_FLOAT },
                {"bokeh alphaCutoff"                                              , "bokeh.alphaCutoff"                                            , joaat("bokeh.alphaCutoff")                                          , V_FLOAT },
                {"bokeh shapeExposureRangeMin"                                    , "bokeh.shapeExposureRangeMin"                                  , joaat("bokeh.shapeExposureRangeMin")                                , V_FLOAT },
                {"bokeh shapeExposureRangeMax"                                    , "bokeh.shapeExposureRangeMax"                                  , joaat("bokeh.shapeExposureRangeMax")                                , V_FLOAT },
                {"defaultmotionblur enabled"                                      , "defaultmotionblur.enabled"                                    , joaat("defaultmotionblur.enabled")                                  , V_FLOAT },
                {"defaultmotionblur strength"                                     , "defaultmotionblur.strength"                                   , joaat("defaultmotionblur.strength")                                 , V_FLOAT },
                {"procObjects minRadiusForHeightMap"                              , "procObjects.minRadiusForHeightMap"                            , joaat("procObjects.minRadiusForHeightMap")                          , V_FLOAT },
        };
        paramsMap["Multipiers over final shadow cascade."] =
        {
                {"fogray fadestart"                                               , "fogray.fadestart"                                             , joaat("fogray.fadestart")                                           , V_FLOAT },
                {"fogray fadeend"                                                 , "fogray.fadeend"                                               , joaat("fogray.fadeend")                                             , V_FLOAT },
        };
        paramsMap["character lens flare"] =
        {
                {"lensflare switch length"                                        , "lensflare.switch.length"                                      , joaat("lensflare.switch.length")                                    , V_FLOAT },
                {"lensflare hide length"                                          , "lensflare.hide.length"                                        , joaat("lensflare.hide.length")                                      , V_FLOAT },
                {"lensflare show length"                                          , "lensflare.show.length"                                        , joaat("lensflare.show.length")                                      , V_FLOAT },
                {"lensflare desaturation boost"                                   , "lensflare.desaturation.boost"                                 , joaat("lensflare.desaturation.boost")                               , V_FLOAT },
        };
        paramsMap["First person motion blur"] =
        {
                {"fpv motionblur weight0"                                         , "fpv.motionblur.weight0"                                       , joaat("fpv.motionblur.weight0")                                     , V_FLOAT },
                {"fpv motionblur weight1"                                         , "fpv.motionblur.weight1"                                       , joaat("fpv.motionblur.weight1")                                     , V_FLOAT },
                {"fpv motionblur weight2"                                         , "fpv.motionblur.weight2"                                       , joaat("fpv.motionblur.weight2")                                     , V_FLOAT },
                {"fpv motionblur weight3"                                         , "fpv.motionblur.weight3"                                       , joaat("fpv.motionblur.weight3")                                     , V_FLOAT },
                {"fpv motionblur max velocity"                                    , "fpv.motionblur.max.velocity"                                  , joaat("fpv.motionblur.max.velocity")                                , V_FLOAT },
        };
    }

    updateContainer(handler);
}

    //  ---> Order vec

    //def has_digits(input_string):
    //    return any(char.isdigit() for char in input_string)

    //arr1 = []
    //with open("visualsettings.dat", "r") as f:
    //    arr1.extend(f)
    //    
    //arr1 = [item.strip() for item in arr1 if item.strip()]
    //
    //for line in arr1:
    //   if "//" in line or "#" in line:
    //       if (has_digits(line)):
    //            continue
    //       if "#" in line:  line = line[1:]
    //       else:            line = line[2:]
    //       
    //       print(f'"{line.strip()}",')



//          ---> Map
//
   //def has_digits(input_string):
   //     return any(char.isdigit() for char in input_string)

   // vfloat = "V_FLOAT"
   // vcol3 = "V_COL3"
   // vcol4 = "V_COL4"
   // vnone = "V_NONE"

   // tType = ""

   // arr1 = []
   // with open("E:\\GTAV\\timecycles\\visualsettings.dat", "r") as f:
   //     arr1.extend(f)
   //     
   // arr1 = [item.strip() for item in arr1 if item.strip()]

   // for i, line in enumerate(arr1):
   //     if "//" in line or "#" in line:
   //         if (has_digits(line)):
   //             #print(f"=============================   {line}")
   //             continue
   //         if "#"  in line:  line = line[1:]
   //         if "//" in line:  line = line[2:]
   //        
   //         print("};\n" + "paramsMap" + f'["{line.strip()}"] = \n' + "{")
   //    
   //     else:
   //         if (has_digits(line)):
   //             line = line.split()[0]
   //             
   //             if line.endswith(".red") and arr1[i + 1].split()[0].endswith(".green") and arr1[i + 2].split()[0].endswith(".blue"):
   //                 lable_name = line[:-4].replace(".", " ")
   //                 tType = vcol3
   //             
   //             elif line.endswith(".r") and arr1[i + 1].split()[0].endswith(".g") and arr1[i + 2].split()[0].endswith(".b"):
   //                 lable_name = line[:-2].replace(".", " ")
   //                 tType = vcol3

   //             elif line.endswith(".green") or line.endswith(".blue"):
   //                 lable_name = "None"
   //                 tType = vnone
   //                 
   //             elif line.endswith(".g") or line.endswith(".b"):
   //                 tType = "None"
   //                 tType = vnone
   //             
   //             else:
   //                 lable_name = line.replace(".", " ")
   //                 tType = vfloat

   //             
   //             print("\t{" + f'"{lable_name}"'.ljust(65) + f', "{line}"'.ljust(65) + f', joaat("{line}")'.ljust(70) + ", " + tType.ljust(8) + "},")
   //            
   //     

