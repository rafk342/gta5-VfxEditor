#include "visualSettingsHandler.h"


namespace
{
    bool(*updateSettings)();
    const size_t buff_size = 27;
    u8 origBytes[buff_size];
    u8* bytesAddr = nullptr;
}

// it's almost like VsReloader works, 
// except we're replacing bytes in the function, instead of replacing symbols in the path str,
// so it won't parse settings from the file, 
// it will update existing settings stored in the game VisualSettings data array

VisualSettingsHandler::VisualSettingsHandler()
{
    mlogger("searching for gVisualSettings");
    static auto addr = gmAddress::Scan("48 83 EC 28 48 8D 15 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0");
    mlogger(std::format("found at 0x{:08X}", addr.Value));

    p_Vsettings = addr.GetRef(14).To<gVisualSettings*>();
    updateSettings = addr.ToFunc<bool()>();

    mlogger(std::format("p_Vsettings data size : {}", p_Vsettings->data.GetSize()));

    bytesAddr = reinterpret_cast<u8*>(addr.GetAt(4).Value);
    std::memcpy(origBytes, bytesAddr, buff_size);
    std::memset(bytesAddr, 0x90, buff_size);
   
    mContainer.initContainer(this);
}

VisualSettingsHandler::~VisualSettingsHandler()
{
    std::memcpy(bytesAddr, origBytes, buff_size);
}

bool VisualSettingsHandler::updateData()
{
    return updateSettings();
}

void VScontainer::initContainer(VisualSettingsHandler* handler)
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
        "car.headlights.volume.outerVolumeColor",
        "heli poslight",
        "heli white head lights",
        "heli white tail lights",
        "heli interior light",
        "the corona size is actually used as the bone offset for this particular light",
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
        "search lights...",
        "Config values for traffic lights",
        "Config values for Tree Imposters",
        "Config values for peds",
        "Lod distances for peds",
        "Config Values for Camera Events      Val start       Val End MB start        MB End",
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
        "LOD lights - do not change these ranges without talking to code because it requires data changes",
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

   paramsMap["Values to use with height based reflection map"] =
{
        {"heightReflect.width"                                            , rage::joaat("heightReflect.width")                                            , 0.0f , V_FLOAT },
        {"heightReflect.height"                                           , rage::joaat("heightReflect.height")                                           , 0.0f , V_FLOAT },
        {"heightReflect.specularoffset"                                   , rage::joaat("heightReflect.specularoffset")                                   , 0.0f , V_FLOAT },
        {"heightReflect.waterspeed"                                       , rage::joaat("heightReflect.waterspeed")                                       , 0.0f , V_FLOAT },
    };
    paramsMap["Values to use with rain collision map"] =
    {
            {"raincollision.specularoffset"                                   , rage::joaat("raincollision.specularoffset")                                   , 0.0f , V_FLOAT },
            {"raincollision.waterspeed"                                       , rage::joaat("raincollision.waterspeed")                                       , 0.0f , V_FLOAT },
    };
    paramsMap["Values for rain GPU particle effect"] =
    {
            {"rain.NumberParticles"                                           , rage::joaat("rain.NumberParticles")                                           , 0.0f , V_FLOAT },
            {"rain.UseLitShader"                                              , rage::joaat("rain.UseLitShader")                                              , 0.0f , V_FLOAT },
            {"rain.gravity.x"                                                 , rage::joaat("rain.gravity.x")                                                 , 0.0f , V_FLOAT },
            {"rain.gravity.y"                                                 , rage::joaat("rain.gravity.y")                                                 , 0.0f , V_FLOAT },
            {"rain.gravity.z"                                                 , rage::joaat("rain.gravity.z")                                                 , 0.0f , V_FLOAT },
            {"rain.fadeInScale"                                               , rage::joaat("rain.fadeInScale")                                               , 0.0f , V_FLOAT },
            {"rain.diffuse"                                                   , rage::joaat("rain.diffuse")                                                   , 0.0f , V_FLOAT },
            {"rain.ambient"                                                   , rage::joaat("rain.ambient")                                                   , 0.0f , V_FLOAT },
            {"rain.wrapScale"                                                 , rage::joaat("rain.wrapScale")                                                 , 0.0f , V_FLOAT },
            {"rain.wrapBias"                                                  , rage::joaat("rain.wrapBias")                                                  , 0.0f , V_FLOAT },
            {"rain.defaultlight.red"                                          , rage::joaat("rain.defaultlight.red")                                          , 0.0f , V_COL3  },
            {"rain.defaultlight.green"                                        , rage::joaat("rain.defaultlight.green")                                        , 0.0f , V_NONE  },
            {"rain.defaultlight.blue"                                         , rage::joaat("rain.defaultlight.blue")                                         , 0.0f , V_NONE  },
            {"rain.defaultlight.alpha"                                        , rage::joaat("rain.defaultlight.alpha")                                        , 0.0f , V_FLOAT },
    };
    paramsMap["General weather configuration"] =
    {
            {"weather.CycleDuration"                                          , rage::joaat("weather.CycleDuration")                                          , 0.0f , V_FLOAT },
            {"weather.ChangeCloudOnSameCloudTypeChance"                       , rage::joaat("weather.ChangeCloudOnSameCloudTypeChance")                       , 0.0f , V_FLOAT },
            {"weather.ChangeCloudOnSameWeatherTypeChance"                     , rage::joaat("weather.ChangeCloudOnSameWeatherTypeChance")                     , 0.0f , V_FLOAT },
    };
    paramsMap["Config for static values for skyhat"] =
    {
            {"sky.sun.centreStart"                                            , rage::joaat("sky.sun.centreStart")                                            , 0.0f , V_FLOAT },
            {"sky.sun.centreEnd"                                              , rage::joaat("sky.sun.centreEnd")                                              , 0.0f , V_FLOAT },
            {"sky.cloudWarp"                                                  , rage::joaat("sky.cloudWarp")                                                  , 0.0f , V_FLOAT },
            {"sky.cloudInscatteringRange"                                     , rage::joaat("sky.cloudInscatteringRange")                                     , 0.0f , V_FLOAT },
            {"sky.cloudEdgeSmooth"                                            , rage::joaat("sky.cloudEdgeSmooth")                                            , 0.0f , V_FLOAT },
            {"sky.sunAxiasX"                                                  , rage::joaat("sky.sunAxiasX")                                                  , 0.0f , V_FLOAT },
            {"sky.sunAxiasY"                                                  , rage::joaat("sky.sunAxiasY")                                                  , 0.0f , V_FLOAT },
            {"sky.GameCloudSpeed"                                             , rage::joaat("sky.GameCloudSpeed")                                             , 0.0f , V_FLOAT },
    };
    paramsMap["config values for cloud shadows"] =
    {
            {"shadows.cloudtexture.scale"                                     , rage::joaat("shadows.cloudtexture.scale")                                     , 0.0f , V_FLOAT },
            {"shadows.cloudtexture.rangemin"                                  , rage::joaat("shadows.cloudtexture.rangemin")                                  , 0.0f , V_FLOAT },
            {"shadows.cloudtexture.rangemax"                                  , rage::joaat("shadows.cloudtexture.rangemax")                                  , 0.0f , V_FLOAT },
    };
    paramsMap["config values for vehicle settings"] =
    {
            {"car.interiorlight.color.red"                                    , rage::joaat("car.interiorlight.color.red")                                    , 0.0f , V_COL3  },
            {"car.interiorlight.color.green"                                  , rage::joaat("car.interiorlight.color.green")                                  , 0.0f , V_NONE  },
            {"car.interiorlight.color.blue"                                   , rage::joaat("car.interiorlight.color.blue")                                   , 0.0f , V_NONE  },
            {"car.interiorlight.intensity"                                    , rage::joaat("car.interiorlight.intensity")                                    , 0.0f , V_FLOAT },
            {"car.interiorlight.radius"                                       , rage::joaat("car.interiorlight.radius")                                       , 0.0f , V_FLOAT },
            {"car.interiorlight.innerConeAngle"                               , rage::joaat("car.interiorlight.innerConeAngle")                               , 0.0f , V_FLOAT },
            {"car.interiorlight.outerConeAngle"                               , rage::joaat("car.interiorlight.outerConeAngle")                               , 0.0f , V_FLOAT },
            {"car.interiorlight.day.emissive.on"                              , rage::joaat("car.interiorlight.day.emissive.on")                              , 0.0f , V_FLOAT },
            {"car.interiorlight.night.emissive.on"                            , rage::joaat("car.interiorlight.night.emissive.on")                            , 0.0f , V_FLOAT },
            {"car.interiorlight.day.emissive.off"                             , rage::joaat("car.interiorlight.day.emissive.off")                             , 0.0f , V_FLOAT },
            {"car.interiorlight.night.emissive.off"                           , rage::joaat("car.interiorlight.night.emissive.off")                           , 0.0f , V_FLOAT },
            {"car.interiorlight.coronaHDR"                                    , rage::joaat("car.interiorlight.coronaHDR")                                    , 0.0f , V_FLOAT },
            {"car.interiorlight.coronaSize"                                   , rage::joaat("car.interiorlight.coronaSize")                                   , 0.0f , V_FLOAT },
            {"car.fatinteriorlight.color.red"                                 , rage::joaat("car.fatinteriorlight.color.red")                                 , 0.0f , V_COL3  },
            {"car.fatinteriorlight.color.green"                               , rage::joaat("car.fatinteriorlight.color.green")                               , 0.0f , V_NONE  },
            {"car.fatinteriorlight.color.blue"                                , rage::joaat("car.fatinteriorlight.color.blue")                                , 0.0f , V_NONE  },
            {"car.fatinteriorlight.intensity"                                 , rage::joaat("car.fatinteriorlight.intensity")                                 , 0.0f , V_FLOAT },
            {"car.fatinteriorlight.radius"                                    , rage::joaat("car.fatinteriorlight.radius")                                    , 0.0f , V_FLOAT },
            {"car.fatinteriorlight.innerConeAngle"                            , rage::joaat("car.fatinteriorlight.innerConeAngle")                            , 0.0f , V_FLOAT },
            {"car.fatinteriorlight.outerConeAngle"                            , rage::joaat("car.fatinteriorlight.outerConeAngle")                            , 0.0f , V_FLOAT },
            {"car.fatinteriorlight.day.emissive.on"                           , rage::joaat("car.fatinteriorlight.day.emissive.on")                           , 0.0f , V_FLOAT },
            {"car.fatinteriorlight.night.emissive.on"                         , rage::joaat("car.fatinteriorlight.night.emissive.on")                         , 0.0f , V_FLOAT },
            {"car.fatinteriorlight.day.emissive.off"                          , rage::joaat("car.fatinteriorlight.day.emissive.off")                          , 0.0f , V_FLOAT },
            {"car.fatinteriorlight.night.emissive.off"                        , rage::joaat("car.fatinteriorlight.night.emissive.off")                        , 0.0f , V_FLOAT },
            {"car.fatinteriorlight.coronaHDR"                                 , rage::joaat("car.fatinteriorlight.coronaHDR")                                 , 0.0f , V_FLOAT },
            {"car.fatinteriorlight.coronaSize"                                , rage::joaat("car.fatinteriorlight.coronaSize")                                , 0.0f , V_FLOAT },
            {"car.redinteriorlight.color.red"                                 , rage::joaat("car.redinteriorlight.color.red")                                 , 0.0f , V_COL3  },
            {"car.redinteriorlight.color.green"                               , rage::joaat("car.redinteriorlight.color.green")                               , 0.0f , V_NONE  },
            {"car.redinteriorlight.color.blue"                                , rage::joaat("car.redinteriorlight.color.blue")                                , 0.0f , V_NONE  },
            {"car.redinteriorlight.intensity"                                 , rage::joaat("car.redinteriorlight.intensity")                                 , 0.0f , V_FLOAT },
            {"car.redinteriorlight.radius"                                    , rage::joaat("car.redinteriorlight.radius")                                    , 0.0f , V_FLOAT },
            {"car.redinteriorlight.innerConeAngle"                            , rage::joaat("car.redinteriorlight.innerConeAngle")                            , 0.0f , V_FLOAT },
            {"car.redinteriorlight.outerConeAngle"                            , rage::joaat("car.redinteriorlight.outerConeAngle")                            , 0.0f , V_FLOAT },
            {"car.redinteriorlight.day.emissive.on"                           , rage::joaat("car.redinteriorlight.day.emissive.on")                           , 0.0f , V_FLOAT },
            {"car.redinteriorlight.night.emissive.on"                         , rage::joaat("car.redinteriorlight.night.emissive.on")                         , 0.0f , V_FLOAT },
            {"car.redinteriorlight.day.emissive.off"                          , rage::joaat("car.redinteriorlight.day.emissive.off")                          , 0.0f , V_FLOAT },
            {"car.redinteriorlight.night.emissive.off"                        , rage::joaat("car.redinteriorlight.night.emissive.off")                        , 0.0f , V_FLOAT },
            {"car.redinteriorlight.coronaHDR"                                 , rage::joaat("car.redinteriorlight.coronaHDR")                                 , 0.0f , V_FLOAT },
            {"car.redinteriorlight.coronaSize"                                , rage::joaat("car.redinteriorlight.coronaSize")                                , 0.0f , V_FLOAT },
            {"car.platelight.color.red"                                       , rage::joaat("car.platelight.color.red")                                       , 0.0f , V_COL3  },
            {"car.platelight.color.green"                                     , rage::joaat("car.platelight.color.green")                                     , 0.0f , V_NONE  },
            {"car.platelight.color.blue"                                      , rage::joaat("car.platelight.color.blue")                                      , 0.0f , V_NONE  },
            {"car.platelight.intensity"                                       , rage::joaat("car.platelight.intensity")                                       , 0.0f , V_FLOAT },
            {"car.platelight.radius"                                          , rage::joaat("car.platelight.radius")                                          , 0.0f , V_FLOAT },
            {"car.platelight.innerConeAngle"                                  , rage::joaat("car.platelight.innerConeAngle")                                  , 0.0f , V_FLOAT },
            {"car.platelight.outerConeAngle"                                  , rage::joaat("car.platelight.outerConeAngle")                                  , 0.0f , V_FLOAT },
            {"car.platelight.day.emissive.on"                                 , rage::joaat("car.platelight.day.emissive.on")                                 , 0.0f , V_FLOAT },
            {"car.platelight.night.emissive.on"                               , rage::joaat("car.platelight.night.emissive.on")                               , 0.0f , V_FLOAT },
            {"car.platelight.day.emissive.off"                                , rage::joaat("car.platelight.day.emissive.off")                                , 0.0f , V_FLOAT },
            {"car.platelight.night.emissive.off"                              , rage::joaat("car.platelight.night.emissive.off")                              , 0.0f , V_FLOAT },
            {"car.platelight.coronaHDR"                                       , rage::joaat("car.platelight.coronaHDR")                                       , 0.0f , V_FLOAT },
            {"car.platelight.coronaSize"                                      , rage::joaat("car.platelight.coronaSize")                                      , 0.0f , V_FLOAT },
            {"car.platelight.falloffExp"                                      , rage::joaat("car.platelight.falloffExp")                                      , 0.0f , V_FLOAT },
            {"car.dashlight.color.red"                                        , rage::joaat("car.dashlight.color.red")                                        , 0.0f , V_COL3  },
            {"car.dashlight.color.green"                                      , rage::joaat("car.dashlight.color.green")                                      , 0.0f , V_NONE  },
            {"car.dashlight.color.blue"                                       , rage::joaat("car.dashlight.color.blue")                                       , 0.0f , V_NONE  },
            {"car.dashlight.intensity"                                        , rage::joaat("car.dashlight.intensity")                                        , 0.0f , V_FLOAT },
            {"car.dashlight.radius"                                           , rage::joaat("car.dashlight.radius")                                           , 0.0f , V_FLOAT },
            {"car.dashlight.innerConeAngle"                                   , rage::joaat("car.dashlight.innerConeAngle")                                   , 0.0f , V_FLOAT },
            {"car.dashlight.outerConeAngle"                                   , rage::joaat("car.dashlight.outerConeAngle")                                   , 0.0f , V_FLOAT },
            {"car.dashlight.day.emissive.on"                                  , rage::joaat("car.dashlight.day.emissive.on")                                  , 0.0f , V_FLOAT },
            {"car.dashlight.night.emissive.on"                                , rage::joaat("car.dashlight.night.emissive.on")                                , 0.0f , V_FLOAT },
            {"car.dashlight.day.emissive.off"                                 , rage::joaat("car.dashlight.day.emissive.off")                                 , 0.0f , V_FLOAT },
            {"car.dashlight.night.emissive.off"                               , rage::joaat("car.dashlight.night.emissive.off")                               , 0.0f , V_FLOAT },
            {"car.dashlight.coronaHDR"                                        , rage::joaat("car.dashlight.coronaHDR")                                        , 0.0f , V_FLOAT },
            {"car.dashlight.coronaSize"                                       , rage::joaat("car.dashlight.coronaSize")                                       , 0.0f , V_FLOAT },
            {"car.dashlight.falloffExp"                                       , rage::joaat("car.dashlight.falloffExp")                                       , 0.0f , V_FLOAT },
            {"car.doorlight.color.red"                                        , rage::joaat("car.doorlight.color.red")                                        , 0.0f , V_COL3  },
            {"car.doorlight.color.green"                                      , rage::joaat("car.doorlight.color.green")                                      , 0.0f , V_NONE  },
            {"car.doorlight.color.blue"                                       , rage::joaat("car.doorlight.color.blue")                                       , 0.0f , V_NONE  },
            {"car.doorlight.intensity"                                        , rage::joaat("car.doorlight.intensity")                                        , 0.0f , V_FLOAT },
            {"car.doorlight.radius"                                           , rage::joaat("car.doorlight.radius")                                           , 0.0f , V_FLOAT },
            {"car.doorlight.innerConeAngle"                                   , rage::joaat("car.doorlight.innerConeAngle")                                   , 0.0f , V_FLOAT },
            {"car.doorlight.outerConeAngle"                                   , rage::joaat("car.doorlight.outerConeAngle")                                   , 0.0f , V_FLOAT },
            {"car.doorlight.day.emissive.on"                                  , rage::joaat("car.doorlight.day.emissive.on")                                  , 0.0f , V_FLOAT },
            {"car.doorlight.night.emissive.on"                                , rage::joaat("car.doorlight.night.emissive.on")                                , 0.0f , V_FLOAT },
            {"car.doorlight.day.emissive.off"                                 , rage::joaat("car.doorlight.day.emissive.off")                                 , 0.0f , V_FLOAT },
            {"car.doorlight.night.emissive.off"                               , rage::joaat("car.doorlight.night.emissive.off")                               , 0.0f , V_FLOAT },
            {"car.doorlight.coronaHDR"                                        , rage::joaat("car.doorlight.coronaHDR")                                        , 0.0f , V_FLOAT },
            {"car.doorlight.coronaSize"                                       , rage::joaat("car.doorlight.coronaSize")                                       , 0.0f , V_FLOAT },
            {"car.doorlight.falloffExp"                                       , rage::joaat("car.doorlight.falloffExp")                                       , 0.0f , V_FLOAT },
            {"car.taxi.color.red"                                             , rage::joaat("car.taxi.color.red")                                             , 0.0f , V_COL3  },
            {"car.taxi.color.green"                                           , rage::joaat("car.taxi.color.green")                                           , 0.0f , V_NONE  },
            {"car.taxi.color.blue"                                            , rage::joaat("car.taxi.color.blue")                                            , 0.0f , V_NONE  },
            {"car.taxi.intensity"                                             , rage::joaat("car.taxi.intensity")                                             , 0.0f , V_FLOAT },
            {"car.taxi.radius"                                                , rage::joaat("car.taxi.radius")                                                , 0.0f , V_FLOAT },
            {"car.taxi.innerConeAngle"                                        , rage::joaat("car.taxi.innerConeAngle")                                        , 0.0f , V_FLOAT },
            {"car.taxi.outerConeAngle"                                        , rage::joaat("car.taxi.outerConeAngle")                                        , 0.0f , V_FLOAT },
            {"car.taxi.coronaHDR"                                             , rage::joaat("car.taxi.coronaHDR")                                             , 0.0f , V_FLOAT },
            {"car.taxi.coronaSize"                                            , rage::joaat("car.taxi.coronaSize")                                            , 0.0f , V_FLOAT },
            {"car.neon.defaultcolor.red"                                      , rage::joaat("car.neon.defaultcolor.red")                                      , 0.0f , V_COL3  },
            {"car.neon.defaultcolor.green"                                    , rage::joaat("car.neon.defaultcolor.green")                                    , 0.0f , V_NONE  },
            {"car.neon.defaultcolor.blue"                                     , rage::joaat("car.neon.defaultcolor.blue")                                     , 0.0f , V_NONE  },
            {"car.neon.intensity"                                             , rage::joaat("car.neon.intensity")                                             , 0.0f , V_FLOAT },
            {"car.neon.radius"                                                , rage::joaat("car.neon.radius")                                                , 0.0f , V_FLOAT },
            {"car.neon.falloffexponent"                                       , rage::joaat("car.neon.falloffexponent")                                       , 0.0f , V_FLOAT },
            {"car.neon.capsuleextent.sides"                                   , rage::joaat("car.neon.capsuleextent.sides")                                   , 0.0f , V_FLOAT },
            {"car.neon.capsuleextent.frontback"                               , rage::joaat("car.neon.capsuleextent.frontback")                               , 0.0f , V_FLOAT },
            {"car.neon.clipplaneheight"                                       , rage::joaat("car.neon.clipplaneheight")                                       , 0.0f , V_FLOAT },
            {"car.neon.bikeclipplaneheight"                                   , rage::joaat("car.neon.bikeclipplaneheight")                                   , 0.0f , V_FLOAT },
            {"car.headlights.angle"                                           , rage::joaat("car.headlights.angle")                                           , 0.0f , V_FLOAT },
            {"car.headlights.split"                                           , rage::joaat("car.headlights.split")                                           , 0.0f , V_FLOAT },
            {"car.headlights.global.HeadlightIntensityMult"                   , rage::joaat("car.headlights.global.HeadlightIntensityMult")                   , 0.0f , V_FLOAT },
            {"car.headlights.global.HeadlightDistMult"                        , rage::joaat("car.headlights.global.HeadlightDistMult")                        , 0.0f , V_FLOAT },
            {"car.headlights.global.ConeInnerAngleMod"                        , rage::joaat("car.headlights.global.ConeInnerAngleMod")                        , 0.0f , V_FLOAT },
            {"car.headlights.global.ConeOuterAngleMod"                        , rage::joaat("car.headlights.global.ConeOuterAngleMod")                        , 0.0f , V_FLOAT },
            {"car.headlights.global.OnlyOneLightMod"                          , rage::joaat("car.headlights.global.OnlyOneLightMod")                          , 0.0f , V_FLOAT },
            {"car.headlights.global.Fake2LightsAngleMod"                      , rage::joaat("car.headlights.global.Fake2LightsAngleMod")                      , 0.0f , V_FLOAT },
            {"car.headlights.global.Fake2LightsDisplacementMod"               , rage::joaat("car.headlights.global.Fake2LightsDisplacementMod")               , 0.0f , V_FLOAT },
            {"car.headlights.submarine.Fake2LightsAngleMod"                   , rage::joaat("car.headlights.submarine.Fake2LightsAngleMod")                   , 0.0f , V_FLOAT },
            {"car.headlights.submarine.Fake2LightsDisplacementMod"            , rage::joaat("car.headlights.submarine.Fake2LightsDisplacementMod")            , 0.0f , V_FLOAT },
            {"car.headlights.fullbeam.IntensityMult"                          , rage::joaat("car.headlights.fullbeam.IntensityMult")                          , 0.0f , V_FLOAT },
            {"car.headlights.fullbeam.DistMult"                               , rage::joaat("car.headlights.fullbeam.DistMult")                               , 0.0f , V_FLOAT },
            {"car.headlights.fullbeam.CoronaIntensityMult"                    , rage::joaat("car.headlights.fullbeam.CoronaIntensityMult")                    , 0.0f , V_FLOAT },
            {"car.headlights.fullbeam.CoronaSizeMult"                         , rage::joaat("car.headlights.fullbeam.CoronaSizeMult")                         , 0.0f , V_FLOAT },
            {"car.headlights.aim.fullbeam.mod"                                , rage::joaat("car.headlights.aim.fullbeam.mod")                                , 0.0f , V_FLOAT },
            {"car.headlights.aim.dippedbeam.mod"                              , rage::joaat("car.headlights.aim.dippedbeam.mod")                              , 0.0f , V_FLOAT },
            {"car.headlights.aim.fullbeam.angle"                              , rage::joaat("car.headlights.aim.fullbeam.angle")                              , 0.0f , V_FLOAT },
            {"car.headlights.aim.dipeedbeam.angle"                            , rage::joaat("car.headlights.aim.dipeedbeam.angle")                            , 0.0f , V_FLOAT },
            {"car.headlights.player.intensitymult"                            , rage::joaat("car.headlights.player.intensitymult")                            , 0.0f , V_FLOAT },
            {"car.headlights.player.distmult"                                 , rage::joaat("car.headlights.player.distmult")                                 , 0.0f , V_FLOAT },
            {"car.headlights.player.exponentmult"                             , rage::joaat("car.headlights.player.exponentmult")                             , 0.0f , V_FLOAT },
    };
    paramsMap["car headlight volume lights data"] =
    {
            {"car.headlights.volume.intensityscale"                           , rage::joaat("car.headlights.volume.intensityscale")                           , 0.0f , V_FLOAT },
            {"car.headlights.volume.sizescale"                                , rage::joaat("car.headlights.volume.sizescale")                                , 0.0f , V_FLOAT },
            {"car.headlights.volume.outerintensity"                           , rage::joaat("car.headlights.volume.outerintensity")                           , 0.0f , V_FLOAT },
            {"car.headlights.volume.outerexponent"                            , rage::joaat("car.headlights.volume.outerexponent")                            , 0.0f , V_FLOAT },
    };
    paramsMap["car.headlights.volume.outerVolumeColor"] =
    {
            {"car.headlights.volume.outerVolumeColor.red"                     , rage::joaat("car.headlights.volume.outerVolumeColor.red")                     , 0.0f , V_COL3  },
            {"car.headlights.volume.outerVolumeColor.green"                   , rage::joaat("car.headlights.volume.outerVolumeColor.green")                   , 0.0f , V_NONE  },
            {"car.headlights.volume.outerVolumeColor.blue"                    , rage::joaat("car.headlights.volume.outerVolumeColor.blue")                    , 0.0f , V_NONE  },
            {"car.coronas.MainFadeRatio"                                      , rage::joaat("car.coronas.MainFadeRatio")                                      , 0.0f , V_FLOAT },
            {"car.coronas.BeginStart"                                         , rage::joaat("car.coronas.BeginStart")                                         , 0.0f , V_FLOAT },
            {"car.coronas.BeginEnd"                                           , rage::joaat("car.coronas.BeginEnd")                                           , 0.0f , V_FLOAT },
            {"car.coronas.CutoffStart"                                        , rage::joaat("car.coronas.CutoffStart")                                        , 0.0f , V_FLOAT },
            {"car.coronas.CutoffEnd"                                          , rage::joaat("car.coronas.CutoffEnd")                                          , 0.0f , V_FLOAT },
            {"car.coronas.underwaterFade"                                     , rage::joaat("car.coronas.underwaterFade")                                     , 0.0f , V_FLOAT },
            {"car.sirens.SpecularFade"                                        , rage::joaat("car.sirens.SpecularFade")                                        , 0.0f , V_FLOAT },
            {"car.sirens.ShadowFade"                                          , rage::joaat("car.sirens.ShadowFade")                                          , 0.0f , V_FLOAT },
    };
    paramsMap["heli poslight"] =
    {
            {"heli.poslight.nearStrength"                                     , rage::joaat("heli.poslight.nearStrength")                                     , 0.0f , V_FLOAT },
            {"heli.poslight.farStrength"                                      , rage::joaat("heli.poslight.farStrength")                                      , 0.0f , V_FLOAT },
            {"heli.poslight.nearSize"                                         , rage::joaat("heli.poslight.nearSize")                                         , 0.0f , V_FLOAT },
            {"heli.poslight.farSize"                                          , rage::joaat("heli.poslight.farSize")                                          , 0.0f , V_FLOAT },
            {"heli.poslight.intensity_typeA"                                  , rage::joaat("heli.poslight.intensity_typeA")                                  , 0.0f , V_FLOAT },
            {"heli.poslight.intensity_typeB"                                  , rage::joaat("heli.poslight.intensity_typeB")                                  , 0.0f , V_FLOAT },
            {"heli.poslight.radius"                                           , rage::joaat("heli.poslight.radius")                                           , 0.0f , V_FLOAT },
            {"heli.poslight.rightColor.red"                                   , rage::joaat("heli.poslight.rightColor.red")                                   , 0.0f , V_COL3  },
            {"heli.poslight.rightColor.green"                                 , rage::joaat("heli.poslight.rightColor.green")                                 , 0.0f , V_NONE  },
            {"heli.poslight.rightColor.blue"                                  , rage::joaat("heli.poslight.rightColor.blue")                                  , 0.0f , V_NONE  },
            {"heli.poslight.leftColor.red"                                    , rage::joaat("heli.poslight.leftColor.red")                                    , 0.0f , V_COL3  },
            {"heli.poslight.leftColor.green"                                  , rage::joaat("heli.poslight.leftColor.green")                                  , 0.0f , V_NONE  },
            {"heli.poslight.leftColor.blue"                                   , rage::joaat("heli.poslight.leftColor.blue")                                   , 0.0f , V_NONE  },
    };
    paramsMap["heli white head lights"] =
    {
            {"heli.whiteheadlight.nearStrength"                               , rage::joaat("heli.whiteheadlight.nearStrength")                               , 0.0f , V_FLOAT },
            {"heli.whiteheadlight.farStrength"                                , rage::joaat("heli.whiteheadlight.farStrength")                                , 0.0f , V_FLOAT },
            {"heli.whiteheadlight.nearSize"                                   , rage::joaat("heli.whiteheadlight.nearSize")                                   , 0.0f , V_FLOAT },
            {"heli.whiteheadlight.farSize"                                    , rage::joaat("heli.whiteheadlight.farSize")                                    , 0.0f , V_FLOAT },
            {"heli.whiteheadlight.intensity"                                  , rage::joaat("heli.whiteheadlight.intensity")                                  , 0.0f , V_FLOAT },
            {"heli.whiteheadlight.radius"                                     , rage::joaat("heli.whiteheadlight.radius")                                     , 0.0f , V_FLOAT },
            {".0"                                                             , rage::joaat(".0")                                                             , 0.0f , V_FLOAT },
            {"heli.whiteheadlight.color.red"                                  , rage::joaat("heli.whiteheadlight.color.red")                                  , 0.0f , V_COL3  },
            {"heli.whiteheadlight.color.green"                                , rage::joaat("heli.whiteheadlight.color.green")                                , 0.0f , V_NONE  },
            {"heli.whiteheadlight.color.blue"                                 , rage::joaat("heli.whiteheadlight.color.blue")                                 , 0.0f , V_NONE  },
    };
    paramsMap["heli white tail lights"] =
    {
            {"heli.whitetaillight.nearStrength"                               , rage::joaat("heli.whitetaillight.nearStrength")                               , 0.0f , V_FLOAT },
            {"heli.whitetaillight.farStrength"                                , rage::joaat("heli.whitetaillight.farStrength")                                , 0.0f , V_FLOAT },
            {"heli.whitetaillight.nearSize"                                   , rage::joaat("heli.whitetaillight.nearSize")                                   , 0.0f , V_FLOAT },
            {"heli.whitetaillight.farSize"                                    , rage::joaat("heli.whitetaillight.farSize")                                    , 0.0f , V_FLOAT },
            {"heli.whitetaillight.intensity"                                  , rage::joaat("heli.whitetaillight.intensity")                                  , 0.0f , V_FLOAT },
            {"heli.whitetaillight.radius"                                     , rage::joaat("heli.whitetaillight.radius")                                     , 0.0f , V_FLOAT },
            {"heli.whitetaillight.color.red"                                  , rage::joaat("heli.whitetaillight.color.red")                                  , 0.0f , V_COL3  },
            {"heli.whitetaillight.color.green"                                , rage::joaat("heli.whitetaillight.color.green")                                , 0.0f , V_NONE  },
            {"heli.whitetaillight.color.blue"                                 , rage::joaat("heli.whitetaillight.color.blue")                                 , 0.0f , V_NONE  },
    };
    paramsMap["heli interior light"] =
    {
            {"heli.interiorlight.color.red"                                   , rage::joaat("heli.interiorlight.color.red")                                   , 0.0f , V_COL3  },
            {"heli.interiorlight.color.green"                                 , rage::joaat("heli.interiorlight.color.green")                                 , 0.0f , V_NONE  },
            {"heli.interiorlight.color.blue"                                  , rage::joaat("heli.interiorlight.color.blue")                                  , 0.0f , V_NONE  },
            {"heli.interiorlight.intensity"                                   , rage::joaat("heli.interiorlight.intensity")                                   , 0.0f , V_FLOAT },
            {"heli.interiorlight.radius"                                      , rage::joaat("heli.interiorlight.radius")                                      , 0.0f , V_FLOAT },
            {"heli.interiorlight.innerConeAngle"                              , rage::joaat("heli.interiorlight.innerConeAngle")                              , 0.0f , V_FLOAT },
            {"heli.interiorlight.outerConeAngle"                              , rage::joaat("heli.interiorlight.outerConeAngle")                              , 0.0f , V_FLOAT },
            {"heli.interiorlight.day.emissive.on"                             , rage::joaat("heli.interiorlight.day.emissive.on")                             , 0.0f , V_FLOAT },
            {"heli.interiorlight.night.emissive.on"                           , rage::joaat("heli.interiorlight.night.emissive.on")                           , 0.0f , V_FLOAT },
            {"heli.interiorlight.day.emissive.off"                            , rage::joaat("heli.interiorlight.day.emissive.off")                            , 0.0f , V_FLOAT },
            {"heli.interiorlight.night.emissive.off"                          , rage::joaat("heli.interiorlight.night.emissive.off")                          , 0.0f , V_FLOAT },
            {"heli.interiorlight.coronaHDR"                                   , rage::joaat("heli.interiorlight.coronaHDR")                                   , 0.0f , V_FLOAT },
    };
    paramsMap["the corona size is actually used as the bone offset for this particular light"] =
    {
            {"heli.interiorlight.coronaSize"                                  , rage::joaat("heli.interiorlight.coronaSize")                                  , 0.0f , V_FLOAT },
    };
    paramsMap["plane poslight"] =
    {
            {"plane.poslight.nearStrength"                                    , rage::joaat("plane.poslight.nearStrength")                                    , 0.0f , V_FLOAT },
            {"plane.poslight.farStrength"                                     , rage::joaat("plane.poslight.farStrength")                                     , 0.0f , V_FLOAT },
            {"plane.poslight.nearSize"                                        , rage::joaat("plane.poslight.nearSize")                                        , 0.0f , V_FLOAT },
            {"plane.poslight.farSize"                                         , rage::joaat("plane.poslight.farSize")                                         , 0.0f , V_FLOAT },
            {"plane.poslight.intensity_typeA"                                 , rage::joaat("plane.poslight.intensity_typeA")                                 , 0.0f , V_FLOAT },
            {"plane.poslight.intensity_typeB"                                 , rage::joaat("plane.poslight.intensity_typeB")                                 , 0.0f , V_FLOAT },
            {"plane.poslight.radius"                                          , rage::joaat("plane.poslight.radius")                                          , 0.0f , V_FLOAT },
            {"plane.poslight.rightColor.red"                                  , rage::joaat("plane.poslight.rightColor.red")                                  , 0.0f , V_COL3  },
            {"plane.poslight.rightColor.green"                                , rage::joaat("plane.poslight.rightColor.green")                                , 0.0f , V_NONE  },
            {"plane.poslight.rightColor.blue"                                 , rage::joaat("plane.poslight.rightColor.blue")                                 , 0.0f , V_NONE  },
            {"plane.poslight.leftColor.red"                                   , rage::joaat("plane.poslight.leftColor.red")                                   , 0.0f , V_COL3  },
            {"plane.poslight.leftColor.green"                                 , rage::joaat("plane.poslight.leftColor.green")                                 , 0.0f , V_NONE  },
            {"plane.poslight.leftColor.blue"                                  , rage::joaat("plane.poslight.leftColor.blue")                                  , 0.0f , V_NONE  },
    };
    paramsMap["plane whiteheadlight"] =
    {
            {"plane.whiteheadlight.nearStrength"                              , rage::joaat("plane.whiteheadlight.nearStrength")                              , 0.0f , V_FLOAT },
            {"plane.whiteheadlight.farStrength"                               , rage::joaat("plane.whiteheadlight.farStrength")                               , 0.0f , V_FLOAT },
            {"plane.whiteheadlight.nearSize"                                  , rage::joaat("plane.whiteheadlight.nearSize")                                  , 0.0f , V_FLOAT },
            {"plane.whiteheadlight.farSize"                                   , rage::joaat("plane.whiteheadlight.farSize")                                   , 0.0f , V_FLOAT },
            {"plane.whiteheadlight.intensity"                                 , rage::joaat("plane.whiteheadlight.intensity")                                 , 0.0f , V_FLOAT },
            {"plane.whiteheadlight.radius"                                    , rage::joaat("plane.whiteheadlight.radius")                                    , 0.0f , V_FLOAT },
            {"plane.whiteheadlight.color.red"                                 , rage::joaat("plane.whiteheadlight.color.red")                                 , 0.0f , V_COL3  },
            {"plane.whiteheadlight.color.green"                               , rage::joaat("plane.whiteheadlight.color.green")                               , 0.0f , V_NONE  },
            {"plane.whiteheadlight.color.blue"                                , rage::joaat("plane.whiteheadlight.color.blue")                                , 0.0f , V_NONE  },
    };
    paramsMap["plane whitetaillight"] =
    {
            {"plane.whitetaillight.nearStrength"                              , rage::joaat("plane.whitetaillight.nearStrength")                              , 0.0f , V_FLOAT },
            {"plane.whitetaillight.farStrength"                               , rage::joaat("plane.whitetaillight.farStrength")                               , 0.0f , V_FLOAT },
            {"plane.whitetaillight.nearSize"                                  , rage::joaat("plane.whitetaillight.nearSize")                                  , 0.0f , V_FLOAT },
            {"plane.whitetaillight.farSize"                                   , rage::joaat("plane.whitetaillight.farSize")                                   , 0.0f , V_FLOAT },
            {"plane.whitetaillight.intensity"                                 , rage::joaat("plane.whitetaillight.intensity")                                 , 0.0f , V_FLOAT },
            {"plane.whitetaillight.radius"                                    , rage::joaat("plane.whitetaillight.radius")                                    , 0.0f , V_FLOAT },
            {"plane.whitetaillight.color.red"                                 , rage::joaat("plane.whitetaillight.color.red")                                 , 0.0f , V_COL3  },
            {"plane.whitetaillight.color.green"                               , rage::joaat("plane.whitetaillight.color.green")                               , 0.0f , V_NONE  },
            {"plane.whitetaillight.color.blue"                                , rage::joaat("plane.whitetaillight.color.blue")                                , 0.0f , V_NONE  },
    };
    paramsMap["plane control-panel light"] =
    {
            {"plane.controlpanel.light.color.red"                             , rage::joaat("plane.controlpanel.light.color.red")                             , 0.0f , V_COL3  },
            {"plane.controlpanel.light.color.green"                           , rage::joaat("plane.controlpanel.light.color.green")                           , 0.0f , V_NONE  },
            {"plane.controlpanel.light.color.blue"                            , rage::joaat("plane.controlpanel.light.color.blue")                            , 0.0f , V_NONE  },
            {"plane.controlpanel.light.intensity"                             , rage::joaat("plane.controlpanel.light.intensity")                             , 0.0f , V_FLOAT },
            {"plane.controlpanel.light.falloff"                               , rage::joaat("plane.controlpanel.light.falloff")                               , 0.0f , V_FLOAT },
            {"plane.controlpanel.light.falloff.exponent"                      , rage::joaat("plane.controlpanel.light.falloff.exponent")                      , 0.0f , V_FLOAT },
    };
    paramsMap["plane right emergency light"] =
    {
            {"plane.emergency.right.light.color.red"                          , rage::joaat("plane.emergency.right.light.color.red")                          , 0.0f , V_COL3  },
            {"plane.emergency.right.light.color.green"                        , rage::joaat("plane.emergency.right.light.color.green")                        , 0.0f , V_NONE  },
            {"plane.emergency.right.light.color.blue"                         , rage::joaat("plane.emergency.right.light.color.blue")                         , 0.0f , V_NONE  },
            {"plane.emergency.right.light.intensity"                          , rage::joaat("plane.emergency.right.light.intensity")                          , 0.0f , V_FLOAT },
            {"plane.emergency.right.light.falloff"                            , rage::joaat("plane.emergency.right.light.falloff")                            , 0.0f , V_FLOAT },
            {"plane.emergency.right.light.falloff.exponent"                   , rage::joaat("plane.emergency.right.light.falloff.exponent")                   , 0.0f , V_FLOAT },
            {"plane.emergency.right.light.inner.angle"                        , rage::joaat("plane.emergency.right.light.inner.angle")                        , 0.0f , V_FLOAT },
            {"plane.emergency.right.light.outer.angle"                        , rage::joaat("plane.emergency.right.light.outer.angle")                        , 0.0f , V_FLOAT },
            {"plane.emergency.right.light.rotation"                           , rage::joaat("plane.emergency.right.light.rotation")                           , 0.0f , V_FLOAT },
    };
    paramsMap["plane left emergency light"] =
    {
            {"plane.emergency.left.light.color.red"                           , rage::joaat("plane.emergency.left.light.color.red")                           , 0.0f , V_COL3  },
            {"plane.emergency.left.light.color.green"                         , rage::joaat("plane.emergency.left.light.color.green")                         , 0.0f , V_NONE  },
            {"plane.emergency.left.light.color.blue"                          , rage::joaat("plane.emergency.left.light.color.blue")                          , 0.0f , V_NONE  },
            {"plane.emergency.left.light.intensity"                           , rage::joaat("plane.emergency.left.light.intensity")                           , 0.0f , V_FLOAT },
            {"plane.emergency.left.light.falloff"                             , rage::joaat("plane.emergency.left.light.falloff")                             , 0.0f , V_FLOAT },
            {"plane.emergency.left.light.falloff.exponent"                    , rage::joaat("plane.emergency.left.light.falloff.exponent")                    , 0.0f , V_FLOAT },
            {"plane.emergency.left.light.inner.angle"                         , rage::joaat("plane.emergency.left.light.inner.angle")                         , 0.0f , V_FLOAT },
            {"plane.emergency.left.light.outer.angle"                         , rage::joaat("plane.emergency.left.light.outer.angle")                         , 0.0f , V_FLOAT },
            {"plane.emergency.left.light.rotation"                            , rage::joaat("plane.emergency.left.light.rotation")                            , 0.0f , V_FLOAT },
    };
    paramsMap["plane inside hull light"] =
    {
            {"plane.insidehull.light.color.red"                               , rage::joaat("plane.insidehull.light.color.red")                               , 0.0f , V_COL3  },
            {"plane.insidehull.light.color.green"                             , rage::joaat("plane.insidehull.light.color.green")                             , 0.0f , V_NONE  },
            {"plane.insidehull.light.color.blue"                              , rage::joaat("plane.insidehull.light.color.blue")                              , 0.0f , V_NONE  },
            {"plane.insidehull.light.intensity"                               , rage::joaat("plane.insidehull.light.intensity")                               , 0.0f , V_FLOAT },
            {"plane.insidehull.light.falloff"                                 , rage::joaat("plane.insidehull.light.falloff")                                 , 0.0f , V_FLOAT },
            {"plane.insidehull.light.falloff.exponent"                        , rage::joaat("plane.insidehull.light.falloff.exponent")                        , 0.0f , V_FLOAT },
            {"plane.insidehull.light.inner.angle"                             , rage::joaat("plane.insidehull.light.inner.angle")                             , 0.0f , V_FLOAT },
            {"plane.insidehull.light.outer.angle"                             , rage::joaat("plane.insidehull.light.outer.angle")                             , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.color.red"                                    , rage::joaat("plane.luxe2.cabin.color.red")                                    , 0.0f , V_COL3  },
            {"plane.luxe2.cabin.color.green"                                  , rage::joaat("plane.luxe2.cabin.color.green")                                  , 0.0f , V_NONE  },
            {"plane.luxe2.cabin.color.blue"                                   , rage::joaat("plane.luxe2.cabin.color.blue")                                   , 0.0f , V_NONE  },
            {"plane.luxe2.cabin.intensity"                                    , rage::joaat("plane.luxe2.cabin.intensity")                                    , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.radius"                                       , rage::joaat("plane.luxe2.cabin.radius")                                       , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.innerConeAngle"                               , rage::joaat("plane.luxe2.cabin.innerConeAngle")                               , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.outerConeAngle"                               , rage::joaat("plane.luxe2.cabin.outerConeAngle")                               , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.falloffExp"                                   , rage::joaat("plane.luxe2.cabin.falloffExp")                                   , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.useDynamicShadows"                            , rage::joaat("plane.luxe2.cabin.useDynamicShadows")                            , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.strip.color.red"                              , rage::joaat("plane.luxe2.cabin.strip.color.red")                              , 0.0f , V_COL3  },
            {"plane.luxe2.cabin.strip.color.green"                            , rage::joaat("plane.luxe2.cabin.strip.color.green")                            , 0.0f , V_NONE  },
            {"plane.luxe2.cabin.strip.color.blue"                             , rage::joaat("plane.luxe2.cabin.strip.color.blue")                             , 0.0f , V_NONE  },
            {"plane.luxe2.cabin.strip.intensity"                              , rage::joaat("plane.luxe2.cabin.strip.intensity")                              , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.strip.radius"                                 , rage::joaat("plane.luxe2.cabin.strip.radius")                                 , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.strip.falloffExp"                             , rage::joaat("plane.luxe2.cabin.strip.falloffExp")                             , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.strip.capsuleLength"                          , rage::joaat("plane.luxe2.cabin.strip.capsuleLength")                          , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.tv.color.red"                                 , rage::joaat("plane.luxe2.cabin.tv.color.red")                                 , 0.0f , V_COL3  },
            {"plane.luxe2.cabin.tv.color.green"                               , rage::joaat("plane.luxe2.cabin.tv.color.green")                               , 0.0f , V_NONE  },
            {"plane.luxe2.cabin.tv.color.blue"                                , rage::joaat("plane.luxe2.cabin.tv.color.blue")                                , 0.0f , V_NONE  },
            {"plane.luxe2.cabin.tv.intensity"                                 , rage::joaat("plane.luxe2.cabin.tv.intensity")                                 , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.tv.radius"                                    , rage::joaat("plane.luxe2.cabin.tv.radius")                                    , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.tv.innerConeAngle"                            , rage::joaat("plane.luxe2.cabin.tv.innerConeAngle")                            , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.tv.outerConeAngle"                            , rage::joaat("plane.luxe2.cabin.tv.outerConeAngle")                            , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.tv.falloffExp"                                , rage::joaat("plane.luxe2.cabin.tv.falloffExp")                                , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.lod.color.red"                                , rage::joaat("plane.luxe2.cabin.lod.color.red")                                , 0.0f , V_COL3  },
            {"plane.luxe2.cabin.lod.color.green"                              , rage::joaat("plane.luxe2.cabin.lod.color.green")                              , 0.0f , V_NONE  },
            {"plane.luxe2.cabin.lod.color.blue"                               , rage::joaat("plane.luxe2.cabin.lod.color.blue")                               , 0.0f , V_NONE  },
            {"plane.luxe2.cabin.lod.intensity"                                , rage::joaat("plane.luxe2.cabin.lod.intensity")                                , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.lod.radius"                                   , rage::joaat("plane.luxe2.cabin.lod.radius")                                   , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.lod.falloffExp"                               , rage::joaat("plane.luxe2.cabin.lod.falloffExp")                               , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.lod.capsuleLength"                            , rage::joaat("plane.luxe2.cabin.lod.capsuleLength")                            , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.window.color.red"                             , rage::joaat("plane.luxe2.cabin.window.color.red")                             , 0.0f , V_COL3  },
            {"plane.luxe2.cabin.window.color.green"                           , rage::joaat("plane.luxe2.cabin.window.color.green")                           , 0.0f , V_NONE  },
            {"plane.luxe2.cabin.window.color.blue"                            , rage::joaat("plane.luxe2.cabin.window.color.blue")                            , 0.0f , V_NONE  },
            {"plane.luxe2.cabin.window.intensity"                             , rage::joaat("plane.luxe2.cabin.window.intensity")                             , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.window.radius"                                , rage::joaat("plane.luxe2.cabin.window.radius")                                , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.window.innerConeAngle"                        , rage::joaat("plane.luxe2.cabin.window.innerConeAngle")                        , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.window.outerConeAngle"                        , rage::joaat("plane.luxe2.cabin.window.outerConeAngle")                        , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.window.falloffExp"                            , rage::joaat("plane.luxe2.cabin.window.falloffExp")                            , 0.0f , V_FLOAT },
            {"plane.luxe2.cabin.window.useSun"                                , rage::joaat("plane.luxe2.cabin.window.useSun")                                , 0.0f , V_FLOAT },
            {"heli.swift2.cabin.color.red"                                    , rage::joaat("heli.swift2.cabin.color.red")                                    , 0.0f , V_COL3  },
            {"heli.swift2.cabin.color.green"                                  , rage::joaat("heli.swift2.cabin.color.green")                                  , 0.0f , V_NONE  },
            {"heli.swift2.cabin.color.blue"                                   , rage::joaat("heli.swift2.cabin.color.blue")                                   , 0.0f , V_NONE  },
            {"heli.swift2.cabin.intensity"                                    , rage::joaat("heli.swift2.cabin.intensity")                                    , 0.0f , V_FLOAT },
            {"heli.swift2.cabin.radius"                                       , rage::joaat("heli.swift2.cabin.radius")                                       , 0.0f , V_FLOAT },
            {"heli.swift2.cabin.innerConeAngle"                               , rage::joaat("heli.swift2.cabin.innerConeAngle")                               , 0.0f , V_FLOAT },
            {"heli.swift2.cabin.outerConeAngle"                               , rage::joaat("heli.swift2.cabin.outerConeAngle")                               , 0.0f , V_FLOAT },
            {"heli.swift2.cabin.falloffExp"                                   , rage::joaat("heli.swift2.cabin.falloffExp")                                   , 0.0f , V_FLOAT },
            {"heli.swift2.cabin.useDynamicShadows"                            , rage::joaat("heli.swift2.cabin.useDynamicShadows")                            , 0.0f , V_FLOAT },
    };
    paramsMap["boat lights"] =
    {
            {"boat.intensity"                                                 , rage::joaat("boat.intensity")                                                 , 0.0f , V_FLOAT },
            {"boat.radius"                                                    , rage::joaat("boat.radius")                                                    , 0.0f , V_FLOAT },
            {"boat.color.red"                                                 , rage::joaat("boat.color.red")                                                 , 0.0f , V_COL3  },
            {"boat.color.green"                                               , rage::joaat("boat.color.green")                                               , 0.0f , V_NONE  },
            {"boat.color.blue"                                                , rage::joaat("boat.color.blue")                                                , 0.0f , V_NONE  },
            {"boat.light.interiorshutdowndistance"                            , rage::joaat("boat.light.interiorshutdowndistance")                            , 0.0f , V_FLOAT },
            {"boat.light.fadelength"                                          , rage::joaat("boat.light.fadelength")                                          , 0.0f , V_FLOAT },
            {"boat.light.shutdowndistance"                                    , rage::joaat("boat.light.shutdowndistance")                                    , 0.0f , V_FLOAT },
            {"boat.corona.fadelength"                                         , rage::joaat("boat.corona.fadelength")                                         , 0.0f , V_FLOAT },
            {"boat.corona.size"                                               , rage::joaat("boat.corona.size")                                               , 0.0f , V_FLOAT },
            {"boat.corona.intensity"                                          , rage::joaat("boat.corona.intensity")                                          , 0.0f , V_FLOAT },
            {"boat.corona.zBias"                                              , rage::joaat("boat.corona.zBias")                                              , 0.0f , V_FLOAT },
            {"sub.lightOne.color.red"                                         , rage::joaat("sub.lightOne.color.red")                                         , 0.0f , V_COL3  },
            {"sub.lightOne.color.green"                                       , rage::joaat("sub.lightOne.color.green")                                       , 0.0f , V_NONE  },
            {"sub.lightOne.color.blue"                                        , rage::joaat("sub.lightOne.color.blue")                                        , 0.0f , V_NONE  },
            {"sub.lightOne.intensity"                                         , rage::joaat("sub.lightOne.intensity")                                         , 0.0f , V_FLOAT },
            {"sub.lightOne.radius"                                            , rage::joaat("sub.lightOne.radius")                                            , 0.0f , V_FLOAT },
            {"sub.lightOne.falloffExp"                                        , rage::joaat("sub.lightOne.falloffExp")                                        , 0.0f , V_FLOAT },
            {"sub.lightOne.innerConeAngle"                                    , rage::joaat("sub.lightOne.innerConeAngle")                                    , 0.0f , V_FLOAT },
            {"sub.lightOne.outerConeAngle"                                    , rage::joaat("sub.lightOne.outerConeAngle")                                    , 0.0f , V_FLOAT },
            {"sub.lightOne.coronaHDR"                                         , rage::joaat("sub.lightOne.coronaHDR")                                         , 0.0f , V_FLOAT },
            {"sub.lightOne.coronaSize"                                        , rage::joaat("sub.lightOne.coronaSize")                                        , 0.0f , V_FLOAT },
            {"sub.lightTwo.color.red"                                         , rage::joaat("sub.lightTwo.color.red")                                         , 0.0f , V_COL3  },
            {"sub.lightTwo.color.green"                                       , rage::joaat("sub.lightTwo.color.green")                                       , 0.0f , V_NONE  },
            {"sub.lightTwo.color.blue"                                        , rage::joaat("sub.lightTwo.color.blue")                                        , 0.0f , V_NONE  },
            {"sub.lightTwo.intensity"                                         , rage::joaat("sub.lightTwo.intensity")                                         , 0.0f , V_FLOAT },
            {"sub.lightTwo.radius"                                            , rage::joaat("sub.lightTwo.radius")                                            , 0.0f , V_FLOAT },
            {"sub.lightTwo.falloffExp"                                        , rage::joaat("sub.lightTwo.falloffExp")                                        , 0.0f , V_FLOAT },
            {"sub.lightTwo.innerConeAngle"                                    , rage::joaat("sub.lightTwo.innerConeAngle")                                    , 0.0f , V_FLOAT },
            {"sub.lightTwo.outerConeAngle"                                    , rage::joaat("sub.lightTwo.outerConeAngle")                                    , 0.0f , V_FLOAT },
            {"sub.lightTwo.coronaHDR"                                         , rage::joaat("sub.lightTwo.coronaHDR")                                         , 0.0f , V_FLOAT },
            {"sub.lightTwo.coronaSize"                                        , rage::joaat("sub.lightTwo.coronaSize")                                        , 0.0f , V_FLOAT },
    };
    paramsMap["train settings"] =
    {
            {"train.light.r"                                                  , rage::joaat("train.light.r")                                                  , 0.0f , V_COL3  },
            {"train.light.g"                                                  , rage::joaat("train.light.g")                                                  , 0.0f , V_NONE  },
            {"train.light.b"                                                  , rage::joaat("train.light.b")                                                  , 0.0f , V_NONE  },
            {"train.light.intensity"                                          , rage::joaat("train.light.intensity")                                          , 0.0f , V_FLOAT },
            {"train.light.falloffmax"                                         , rage::joaat("train.light.falloffmax")                                         , 0.0f , V_FLOAT },
            {"train.light.fadingdistance"                                     , rage::joaat("train.light.fadingdistance")                                     , 0.0f , V_FLOAT },
            {"train.light.fadelength"                                         , rage::joaat("train.light.fadelength")                                         , 0.0f , V_FLOAT },
            {"train.ambientvolume.intensityscaler"                            , rage::joaat("train.ambientvolume.intensityscaler")                            , 0.0f , V_FLOAT },
    };
    paramsMap["emissive bits"] =
    {
            {"car.headlight.day.emissive.on"                                  , rage::joaat("car.headlight.day.emissive.on")                                  , 0.0f , V_FLOAT },
            {"car.headlight.night.emissive.on"                                , rage::joaat("car.headlight.night.emissive.on")                                , 0.0f , V_FLOAT },
            {"car.headlight.day.emissive.off"                                 , rage::joaat("car.headlight.day.emissive.off")                                 , 0.0f , V_FLOAT },
            {"car.headlight.night.emissive.off"                               , rage::joaat("car.headlight.night.emissive.off")                               , 0.0f , V_FLOAT },
            {"car.taillight.day.emissive.on"                                  , rage::joaat("car.taillight.day.emissive.on")                                  , 0.0f , V_FLOAT },
            {"car.taillight.night.emissive.on"                                , rage::joaat("car.taillight.night.emissive.on")                                , 0.0f , V_FLOAT },
            {"car.taillight.day.emissive.off"                                 , rage::joaat("car.taillight.day.emissive.off")                                 , 0.0f , V_FLOAT },
            {"car.taillight.night.emissive.off"                               , rage::joaat("car.taillight.night.emissive.off")                               , 0.0f , V_FLOAT },
            {"car.indicator.day.emissive.on"                                  , rage::joaat("car.indicator.day.emissive.on")                                  , 0.0f , V_FLOAT },
            {"car.indicator.night.emissive.on"                                , rage::joaat("car.indicator.night.emissive.on")                                , 0.0f , V_FLOAT },
            {"car.indicator.day.emissive.off"                                 , rage::joaat("car.indicator.day.emissive.off")                                 , 0.0f , V_FLOAT },
            {"car.indicator.night.emissive.off"                               , rage::joaat("car.indicator.night.emissive.off")                               , 0.0f , V_FLOAT },
            {"car.reversinglight.day.emissive.on"                             , rage::joaat("car.reversinglight.day.emissive.on")                             , 0.0f , V_FLOAT },
            {"car.reversinglight.night.emissive.on"                           , rage::joaat("car.reversinglight.night.emissive.on")                           , 0.0f , V_FLOAT },
            {"car.reversinglight.day.emissive.off"                            , rage::joaat("car.reversinglight.day.emissive.off")                            , 0.0f , V_FLOAT },
            {"car.reversinglight.night.emissive.off"                          , rage::joaat("car.reversinglight.night.emissive.off")                          , 0.0f , V_FLOAT },
            {"car.defaultlight.day.emissive.on"                               , rage::joaat("car.defaultlight.day.emissive.on")                               , 0.0f , V_FLOAT },
            {"car.defaultlight.night.emissive.on"                             , rage::joaat("car.defaultlight.night.emissive.on")                             , 0.0f , V_FLOAT },
            {"car.defaultlight.day.emissive.off"                              , rage::joaat("car.defaultlight.day.emissive.off")                              , 0.0f , V_FLOAT },
            {"car.defaultlight.night.emissive.off"                            , rage::joaat("car.defaultlight.night.emissive.off")                            , 0.0f , V_FLOAT },
            {"car.brakelight.day.emissive.on"                                 , rage::joaat("car.brakelight.day.emissive.on")                                 , 0.0f , V_FLOAT },
            {"car.brakelight.night.emissive.on"                               , rage::joaat("car.brakelight.night.emissive.on")                               , 0.0f , V_FLOAT },
            {"car.brakelight.day.emissive.off"                                , rage::joaat("car.brakelight.day.emissive.off")                                , 0.0f , V_FLOAT },
            {"car.brakelight.night.emissive.off"                              , rage::joaat("car.brakelight.night.emissive.off")                              , 0.0f , V_FLOAT },
            {"car.middlebrakelight.day.emissive.on"                           , rage::joaat("car.middlebrakelight.day.emissive.on")                           , 0.0f , V_FLOAT },
            {"car.middlebrakelight.night.emissive.on"                         , rage::joaat("car.middlebrakelight.night.emissive.on")                         , 0.0f , V_FLOAT },
            {"car.middlebrakelight.day.emissive.off"                          , rage::joaat("car.middlebrakelight.day.emissive.off")                          , 0.0f , V_FLOAT },
            {"car.middlebrakelight.night.emissive.off"                        , rage::joaat("car.middlebrakelight.night.emissive.off")                        , 0.0f , V_FLOAT },
            {"car.extralight.day.emissive.on"                                 , rage::joaat("car.extralight.day.emissive.on")                                 , 0.0f , V_FLOAT },
            {"car.extralight.night.emissive.on"                               , rage::joaat("car.extralight.night.emissive.on")                               , 0.0f , V_FLOAT },
            {"car.extralight.day.emissive.off"                                , rage::joaat("car.extralight.day.emissive.off")                                , 0.0f , V_FLOAT },
            {"car.extralight.night.emissive.off"                              , rage::joaat("car.extralight.night.emissive.off")                              , 0.0f , V_FLOAT },
            {"car.sirenlight.day.emissive.on"                                 , rage::joaat("car.sirenlight.day.emissive.on")                                 , 0.0f , V_FLOAT },
            {"car.sirenlight.night.emissive.on"                               , rage::joaat("car.sirenlight.night.emissive.on")                               , 0.0f , V_FLOAT },
            {"car.sirenlight.day.emissive.off"                                , rage::joaat("car.sirenlight.day.emissive.off")                                , 0.0f , V_FLOAT },
            {"car.sirenlight.night.emissive.off"                              , rage::joaat("car.sirenlight.night.emissive.off")                              , 0.0f , V_FLOAT },
            {"car.emissiveMultiplier"                                         , rage::joaat("car.emissiveMultiplier")                                         , 0.0f , V_FLOAT },
    };
    paramsMap["Lod distances for vehicles"] =
    {
            {"car.lod.distance.high"                                          , rage::joaat("car.lod.distance.high")                                          , 0.0f , V_FLOAT },
            {"car.lod.distance.low"                                           , rage::joaat("car.lod.distance.low")                                           , 0.0f , V_FLOAT },
    };
    paramsMap["search lights..."] =
    {
            {"defaultsearchlight.length"                                      , rage::joaat("defaultsearchlight.length")                                      , 0.0f , V_FLOAT },
            {"defaultsearchlight.offset"                                      , rage::joaat("defaultsearchlight.offset")                                      , 0.0f , V_FLOAT },
            {"defaultsearchlight.color.red"                                   , rage::joaat("defaultsearchlight.color.red")                                   , 0.0f , V_COL3  },
            {"defaultsearchlight.color.green"                                 , rage::joaat("defaultsearchlight.color.green")                                 , 0.0f , V_NONE  },
            {"defaultsearchlight.color.blue"                                  , rage::joaat("defaultsearchlight.color.blue")                                  , 0.0f , V_NONE  },
            {"defaultsearchlight.mainLightInfo.falloffScale"                  , rage::joaat("defaultsearchlight.mainLightInfo.falloffScale")                  , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.falloffExp"                    , rage::joaat("defaultsearchlight.mainLightInfo.falloffExp")                    , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.innerAngle"                    , rage::joaat("defaultsearchlight.mainLightInfo.innerAngle")                    , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.outerAngle"                    , rage::joaat("defaultsearchlight.mainLightInfo.outerAngle")                    , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.globalIntensity"               , rage::joaat("defaultsearchlight.mainLightInfo.globalIntensity")               , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.lightIntensityScale"           , rage::joaat("defaultsearchlight.mainLightInfo.lightIntensityScale")           , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.volumeIntensityScale"          , rage::joaat("defaultsearchlight.mainLightInfo.volumeIntensityScale")          , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.volumeSizeScale"               , rage::joaat("defaultsearchlight.mainLightInfo.volumeSizeScale")               , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.outerVolumeColor.color.red"    , rage::joaat("defaultsearchlight.mainLightInfo.outerVolumeColor.color.red")    , 0.0f , V_COL3  },
            {"defaultsearchlight.mainLightInfo.outerVolumeColor.color.green"  , rage::joaat("defaultsearchlight.mainLightInfo.outerVolumeColor.color.green")  , 0.0f , V_NONE  },
            {"defaultsearchlight.mainLightInfo.outerVolumeColor.color.blue"   , rage::joaat("defaultsearchlight.mainLightInfo.outerVolumeColor.color.blue")   , 0.0f , V_NONE  },
            {"defaultsearchlight.mainLightInfo.outerVolumeIntensity"          , rage::joaat("defaultsearchlight.mainLightInfo.outerVolumeIntensity")          , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.outerVolumeFallOffExponent"    , rage::joaat("defaultsearchlight.mainLightInfo.outerVolumeFallOffExponent")    , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.enable"                        , rage::joaat("defaultsearchlight.mainLightInfo.enable")                        , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.specular"                      , rage::joaat("defaultsearchlight.mainLightInfo.specular")                      , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.shadow"                        , rage::joaat("defaultsearchlight.mainLightInfo.shadow")                        , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.volume"                        , rage::joaat("defaultsearchlight.mainLightInfo.volume")                        , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.coronaIntensity"               , rage::joaat("defaultsearchlight.mainLightInfo.coronaIntensity")               , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.coronaIntensityFar"            , rage::joaat("defaultsearchlight.mainLightInfo.coronaIntensityFar")            , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.coronaSize"                    , rage::joaat("defaultsearchlight.mainLightInfo.coronaSize")                    , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.coronaSizeFar"                 , rage::joaat("defaultsearchlight.mainLightInfo.coronaSizeFar")                 , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.coronaZBias"                   , rage::joaat("defaultsearchlight.mainLightInfo.coronaZBias")                   , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.coronaOffset"                  , rage::joaat("defaultsearchlight.mainLightInfo.coronaOffset")                  , 0.0f , V_FLOAT },
            {"defaultsearchlight.mainLightInfo.lightFadeDist"                 , rage::joaat("defaultsearchlight.mainLightInfo.lightFadeDist")                 , 0.0f , V_FLOAT },
            {"helisearchlight.length"                                         , rage::joaat("helisearchlight.length")                                         , 0.0f , V_FLOAT },
            {"helisearchlight.offset"                                         , rage::joaat("helisearchlight.offset")                                         , 0.0f , V_FLOAT },
            {"helisearchlight.color.red"                                      , rage::joaat("helisearchlight.color.red")                                      , 0.0f , V_COL3  },
            {"helisearchlight.color.green"                                    , rage::joaat("helisearchlight.color.green")                                    , 0.0f , V_NONE  },
            {"helisearchlight.color.blue"                                     , rage::joaat("helisearchlight.color.blue")                                     , 0.0f , V_NONE  },
            {"helisearchlight.mainLightInfo.falloffScale"                     , rage::joaat("helisearchlight.mainLightInfo.falloffScale")                     , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.falloffExp"                       , rage::joaat("helisearchlight.mainLightInfo.falloffExp")                       , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.innerAngle"                       , rage::joaat("helisearchlight.mainLightInfo.innerAngle")                       , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.outerAngle"                       , rage::joaat("helisearchlight.mainLightInfo.outerAngle")                       , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.globalIntensity"                  , rage::joaat("helisearchlight.mainLightInfo.globalIntensity")                  , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.lightIntensityScale"              , rage::joaat("helisearchlight.mainLightInfo.lightIntensityScale")              , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.volumeIntensityScale"             , rage::joaat("helisearchlight.mainLightInfo.volumeIntensityScale")             , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.volumeSizeScale"                  , rage::joaat("helisearchlight.mainLightInfo.volumeSizeScale")                  , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.outerVolumeColor.color.red"       , rage::joaat("helisearchlight.mainLightInfo.outerVolumeColor.color.red")       , 0.0f , V_COL3  },
            {"helisearchlight.mainLightInfo.outerVolumeColor.color.green"     , rage::joaat("helisearchlight.mainLightInfo.outerVolumeColor.color.green")     , 0.0f , V_NONE  },
            {"helisearchlight.mainLightInfo.outerVolumeColor.color.blue"      , rage::joaat("helisearchlight.mainLightInfo.outerVolumeColor.color.blue")      , 0.0f , V_NONE  },
            {"helisearchlight.mainLightInfo.outerVolumeIntensity"             , rage::joaat("helisearchlight.mainLightInfo.outerVolumeIntensity")             , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.outerVolumeFallOffExponent"       , rage::joaat("helisearchlight.mainLightInfo.outerVolumeFallOffExponent")       , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.enable"                           , rage::joaat("helisearchlight.mainLightInfo.enable")                           , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.specular"                         , rage::joaat("helisearchlight.mainLightInfo.specular")                         , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.shadow"                           , rage::joaat("helisearchlight.mainLightInfo.shadow")                           , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.volume"                           , rage::joaat("helisearchlight.mainLightInfo.volume")                           , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.coronaIntensity"                  , rage::joaat("helisearchlight.mainLightInfo.coronaIntensity")                  , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.coronaIntensityFar"               , rage::joaat("helisearchlight.mainLightInfo.coronaIntensityFar")               , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.coronaSize"                       , rage::joaat("helisearchlight.mainLightInfo.coronaSize")                       , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.coronaSizeFar"                    , rage::joaat("helisearchlight.mainLightInfo.coronaSizeFar")                    , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.coronaZBias"                      , rage::joaat("helisearchlight.mainLightInfo.coronaZBias")                      , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.coronaOffset"                     , rage::joaat("helisearchlight.mainLightInfo.coronaOffset")                     , 0.0f , V_FLOAT },
            {"helisearchlight.mainLightInfo.lightFadeDist"                    , rage::joaat("helisearchlight.mainLightInfo.lightFadeDist")                    , 0.0f , V_FLOAT },
            {"boatsearchlight.length"                                         , rage::joaat("boatsearchlight.length")                                         , 0.0f , V_FLOAT },
            {"boatsearchlight.offset"                                         , rage::joaat("boatsearchlight.offset")                                         , 0.0f , V_FLOAT },
            {"boatsearchlight.color.red"                                      , rage::joaat("boatsearchlight.color.red")                                      , 0.0f , V_COL3  },
            {"boatsearchlight.color.green"                                    , rage::joaat("boatsearchlight.color.green")                                    , 0.0f , V_NONE  },
            {"boatsearchlight.color.blue"                                     , rage::joaat("boatsearchlight.color.blue")                                     , 0.0f , V_NONE  },
            {"boatsearchlight.mainLightInfo.falloffScale"                     , rage::joaat("boatsearchlight.mainLightInfo.falloffScale")                     , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.falloffExp"                       , rage::joaat("boatsearchlight.mainLightInfo.falloffExp")                       , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.innerAngle"                       , rage::joaat("boatsearchlight.mainLightInfo.innerAngle")                       , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.outerAngle"                       , rage::joaat("boatsearchlight.mainLightInfo.outerAngle")                       , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.globalIntensity"                  , rage::joaat("boatsearchlight.mainLightInfo.globalIntensity")                  , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.lightIntensityScale"              , rage::joaat("boatsearchlight.mainLightInfo.lightIntensityScale")              , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.volumeIntensityScale"             , rage::joaat("boatsearchlight.mainLightInfo.volumeIntensityScale")             , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.volumeSizeScale"                  , rage::joaat("boatsearchlight.mainLightInfo.volumeSizeScale")                  , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.outerVolumeColor.color.red"       , rage::joaat("boatsearchlight.mainLightInfo.outerVolumeColor.color.red")       , 0.0f , V_COL3  },
            {"boatsearchlight.mainLightInfo.outerVolumeColor.color.green"     , rage::joaat("boatsearchlight.mainLightInfo.outerVolumeColor.color.green")     , 0.0f , V_NONE  },
            {"boatsearchlight.mainLightInfo.outerVolumeColor.color.blue"      , rage::joaat("boatsearchlight.mainLightInfo.outerVolumeColor.color.blue")      , 0.0f , V_NONE  },
            {"boatsearchlight.mainLightInfo.outerVolumeIntensity"             , rage::joaat("boatsearchlight.mainLightInfo.outerVolumeIntensity")             , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.outerVolumeFallOffExponent"       , rage::joaat("boatsearchlight.mainLightInfo.outerVolumeFallOffExponent")       , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.enable"                           , rage::joaat("boatsearchlight.mainLightInfo.enable")                           , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.specular"                         , rage::joaat("boatsearchlight.mainLightInfo.specular")                         , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.shadow"                           , rage::joaat("boatsearchlight.mainLightInfo.shadow")                           , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.volume"                           , rage::joaat("boatsearchlight.mainLightInfo.volume")                           , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.coronaIntensity"                  , rage::joaat("boatsearchlight.mainLightInfo.coronaIntensity")                  , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.coronaIntensityFar"               , rage::joaat("boatsearchlight.mainLightInfo.coronaIntensityFar")               , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.coronaSize"                       , rage::joaat("boatsearchlight.mainLightInfo.coronaSize")                       , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.coronaSizeFar"                    , rage::joaat("boatsearchlight.mainLightInfo.coronaSizeFar")                    , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.coronaZBias"                      , rage::joaat("boatsearchlight.mainLightInfo.coronaZBias")                      , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.coronaOffset"                     , rage::joaat("boatsearchlight.mainLightInfo.coronaOffset")                     , 0.0f , V_FLOAT },
            {"boatsearchlight.mainLightInfo.lightFadeDist"                    , rage::joaat("boatsearchlight.mainLightInfo.lightFadeDist")                    , 0.0f , V_FLOAT },
    };
    paramsMap["Config values for traffic lights"] =
    {
            {"trafficLight.red.color.red"                                     , rage::joaat("trafficLight.red.color.red")                                     , 0.0f , V_COL3  },
            {"trafficLight.red.color.green"                                   , rage::joaat("trafficLight.red.color.green")                                   , 0.0f , V_NONE  },
            {"trafficLight.red.color.blue"                                    , rage::joaat("trafficLight.red.color.blue")                                    , 0.0f , V_NONE  },
            {"trafficLight.amber.color.red"                                   , rage::joaat("trafficLight.amber.color.red")                                   , 0.0f , V_COL3  },
            {"trafficLight.amber.color.green"                                 , rage::joaat("trafficLight.amber.color.green")                                 , 0.0f , V_NONE  },
            {"trafficLight.amber.color.blue"                                  , rage::joaat("trafficLight.amber.color.blue")                                  , 0.0f , V_NONE  },
            {"trafficLight.green.color.red"                                   , rage::joaat("trafficLight.green.color.red")                                   , 0.0f , V_COL3  },
            {"trafficLight.green.color.green"                                 , rage::joaat("trafficLight.green.color.green")                                 , 0.0f , V_NONE  },
            {"trafficLight.green.color.blue"                                  , rage::joaat("trafficLight.green.color.blue")                                  , 0.0f , V_NONE  },
            {"trafficLight.walk.color.red"                                    , rage::joaat("trafficLight.walk.color.red")                                    , 0.0f , V_COL3  },
            {"trafficLight.walk.color.green"                                  , rage::joaat("trafficLight.walk.color.green")                                  , 0.0f , V_NONE  },
            {"trafficLight.walk.color.blue"                                   , rage::joaat("trafficLight.walk.color.blue")                                   , 0.0f , V_NONE  },
            {"trafficLight.dontwalk.color.red"                                , rage::joaat("trafficLight.dontwalk.color.red")                                , 0.0f , V_COL3  },
            {"trafficLight.dontwalk.color.green"                              , rage::joaat("trafficLight.dontwalk.color.green")                              , 0.0f , V_NONE  },
            {"trafficLight.dontwalk.color.blue"                               , rage::joaat("trafficLight.dontwalk.color.blue")                               , 0.0f , V_NONE  },
            {"trafficLight.near.na.color.red"                                 , rage::joaat("trafficLight.near.na.color.red")                                 , 0.0f , V_COL3  },
            {"trafficLight.near.na.color.green"                               , rage::joaat("trafficLight.near.na.color.green")                               , 0.0f , V_NONE  },
            {"trafficLight.near.na.color.blue"                                , rage::joaat("trafficLight.near.na.color.blue")                                , 0.0f , V_NONE  },
            {"trafficLight.near.intensity"                                    , rage::joaat("trafficLight.near.intensity")                                    , 0.0f , V_FLOAT },
            {"trafficLight.near.radius"                                       , rage::joaat("trafficLight.near.radius")                                       , 0.0f , V_FLOAT },
            {"trafficlight.near.falloffExp"                                   , rage::joaat("trafficlight.near.falloffExp")                                   , 0.0f , V_FLOAT },
            {"trafficLight.near.innerConeAngle"                               , rage::joaat("trafficLight.near.innerConeAngle")                               , 0.0f , V_FLOAT },
            {"trafficLight.near.outerConeAngle"                               , rage::joaat("trafficLight.near.outerConeAngle")                               , 0.0f , V_FLOAT },
            {"trafficLight.near.coronaHDR"                                    , rage::joaat("trafficLight.near.coronaHDR")                                    , 0.0f , V_FLOAT },
            {"trafficLight.near.coronaSize"                                   , rage::joaat("trafficLight.near.coronaSize")                                   , 0.0f , V_FLOAT },
            {"trafficLight.farFadeStart"                                      , rage::joaat("trafficLight.farFadeStart")                                      , 0.0f , V_FLOAT },
            {"trafficLight.farFadeEnd"                                        , rage::joaat("trafficLight.farFadeEnd")                                        , 0.0f , V_FLOAT },
            {"trafficLight.nearFadeStart"                                     , rage::joaat("trafficLight.nearFadeStart")                                     , 0.0f , V_FLOAT },
            {"trafficLight.nearFadeEnd"                                       , rage::joaat("trafficLight.nearFadeEnd")                                       , 0.0f , V_FLOAT },
    };
    paramsMap["Config values for Tree Imposters"] =
    {
            {"imposter.color.blendRange"                                      , rage::joaat("imposter.color.blendRange")                                      , 0.0f , V_FLOAT },
            {"imposter.color.blendBias"                                       , rage::joaat("imposter.color.blendBias")                                       , 0.0f , V_FLOAT },
            {"imposter.color.size1"                                           , rage::joaat("imposter.color.size1")                                           , 0.0f , V_FLOAT },
            {"imposter.color.amt1"                                            , rage::joaat("imposter.color.amt1")                                            , 0.0f , V_FLOAT },
            {"imposter.color.size2"                                           , rage::joaat("imposter.color.size2")                                           , 0.0f , V_FLOAT },
            {"imposter.color.amt2"                                            , rage::joaat("imposter.color.amt2")                                            , 0.0f , V_FLOAT },
            {"imposter.backgroundColor.red"                                   , rage::joaat("imposter.backgroundColor.red")                                   , 0.0f , V_COL3  },
            {"imposter.backgroundColor.green"                                 , rage::joaat("imposter.backgroundColor.green")                                 , 0.0f , V_NONE  },
            {"imposter.backgroundColor.blue"                                  , rage::joaat("imposter.backgroundColor.blue")                                  , 0.0f , V_NONE  },
            {"imposter.shadow.blendRange"                                     , rage::joaat("imposter.shadow.blendRange")                                     , 0.0f , V_FLOAT },
            {"imposter.shadow.blendBias"                                      , rage::joaat("imposter.shadow.blendBias")                                      , 0.0f , V_FLOAT },
            {"imposter.shadow.size1"                                          , rage::joaat("imposter.shadow.size1")                                          , 0.0f , V_FLOAT },
            {"imposter.shadow.amt1"                                           , rage::joaat("imposter.shadow.amt1")                                           , 0.0f , V_FLOAT },
            {"imposter.shadow.size2"                                          , rage::joaat("imposter.shadow.size2")                                          , 0.0f , V_FLOAT },
            {"imposter.shadow.amt2"                                           , rage::joaat("imposter.shadow.amt2")                                           , 0.0f , V_FLOAT },
    };
    paramsMap["Config values for peds"] =
    {
            {"ped.ambientvolume.maxstrength"                                  , rage::joaat("ped.ambientvolume.maxstrength")                                  , 0.0f , V_FLOAT },
            {"ped.ambientvolume.fadestart"                                    , rage::joaat("ped.ambientvolume.fadestart")                                    , 0.0f , V_FLOAT },
            {"ped.ambientvolume.fadeend"                                      , rage::joaat("ped.ambientvolume.fadeend")                                      , 0.0f , V_FLOAT },
            {"ped.ambientvolume.baseintensity"                                , rage::joaat("ped.ambientvolume.baseintensity")                                , 0.0f , V_FLOAT },
            {"ped.incarAmbientScale"                                          , rage::joaat("ped.incarAmbientScale")                                          , 0.0f , V_FLOAT },
            {"pedLight.color.red"                                             , rage::joaat("pedLight.color.red")                                             , 0.0f , V_COL3  },
            {"pedLight.color.green"                                           , rage::joaat("pedLight.color.green")                                           , 0.0f , V_NONE  },
            {"pedLight.color.blue"                                            , rage::joaat("pedLight.color.blue")                                            , 0.0f , V_NONE  },
            {"pedlight.intensity"                                             , rage::joaat("pedlight.intensity")                                             , 0.0f , V_FLOAT },
            {"pedlight.radius"                                                , rage::joaat("pedlight.radius")                                                , 0.0f , V_FLOAT },
            {"pedlight.innerConeAngle"                                        , rage::joaat("pedlight.innerConeAngle")                                        , 0.0f , V_FLOAT },
            {"pedlight.outerConeAngle"                                        , rage::joaat("pedlight.outerConeAngle")                                        , 0.0f , V_FLOAT },
            {"pedlight.coronaHDR"                                             , rage::joaat("pedlight.coronaHDR")                                             , 0.0f , V_FLOAT },
            {"pedlight.coronaSize"                                            , rage::joaat("pedlight.coronaSize")                                            , 0.0f , V_FLOAT },
            {"pedlight.falloffExp"                                            , rage::joaat("pedlight.falloffExp")                                            , 0.0f , V_FLOAT },
            {"pedlight.volumeIntensity"                                       , rage::joaat("pedlight.volumeIntensity")                                       , 0.0f , V_FLOAT },
            {"pedlight.volumeSize"                                            , rage::joaat("pedlight.volumeSize")                                            , 0.0f , V_FLOAT },
            {"pedlight.volumeExponent"                                        , rage::joaat("pedlight.volumeExponent")                                        , 0.0f , V_FLOAT },
            {"pedLight.volumeColor.x"                                         , rage::joaat("pedLight.volumeColor.x")                                         , 0.0f , V_FLOAT },
            {"pedLight.volumeColor.y"                                         , rage::joaat("pedLight.volumeColor.y")                                         , 0.0f , V_FLOAT },
            {"pedLight.volumeColor.z"                                         , rage::joaat("pedLight.volumeColor.z")                                         , 0.0f , V_FLOAT },
            {"pedLight.volumeColor.w"                                         , rage::joaat("pedLight.volumeColor.w")                                         , 0.0f , V_FLOAT },
            {"pedlight.fade"                                                  , rage::joaat("pedlight.fade")                                                  , 0.0f , V_FLOAT },
            {"pedlight.shadowFade"                                            , rage::joaat("pedlight.shadowFade")                                            , 0.0f , V_FLOAT },
            {"pedlight.specularFade"                                          , rage::joaat("pedlight.specularFade")                                          , 0.0f , V_FLOAT },
            {"pedFpsLight.color.red"                                          , rage::joaat("pedFpsLight.color.red")                                          , 0.0f , V_COL3  },
            {"pedFpsLight.color.green"                                        , rage::joaat("pedFpsLight.color.green")                                        , 0.0f , V_NONE  },
            {"pedFpsLight.color.blue"                                         , rage::joaat("pedFpsLight.color.blue")                                         , 0.0f , V_NONE  },
            {"pedFpslight.intensity"                                          , rage::joaat("pedFpslight.intensity")                                          , 0.0f , V_FLOAT },
            {"pedFpslight.radius"                                             , rage::joaat("pedFpslight.radius")                                             , 0.0f , V_FLOAT },
            {"pedFpslight.innerConeAngle"                                     , rage::joaat("pedFpslight.innerConeAngle")                                     , 0.0f , V_FLOAT },
            {"pedFpslight.outerConeAngle"                                     , rage::joaat("pedFpslight.outerConeAngle")                                     , 0.0f , V_FLOAT },
            {"pedFpslight.coronaHDR"                                          , rage::joaat("pedFpslight.coronaHDR")                                          , 0.0f , V_FLOAT },
            {"pedFpslight.coronaSize"                                         , rage::joaat("pedFpslight.coronaSize")                                         , 0.0f , V_FLOAT },
            {"pedFpslight.falloffExp"                                         , rage::joaat("pedFpslight.falloffExp")                                         , 0.0f , V_FLOAT },
            {"pedFpslight.volumeIntensity"                                    , rage::joaat("pedFpslight.volumeIntensity")                                    , 0.0f , V_FLOAT },
            {"pedFpslight.volumeSize"                                         , rage::joaat("pedFpslight.volumeSize")                                         , 0.0f , V_FLOAT },
            {"pedFpslight.volumeExponent"                                     , rage::joaat("pedFpslight.volumeExponent")                                     , 0.0f , V_FLOAT },
            {"pedFpsLight.volumeColor.x"                                      , rage::joaat("pedFpsLight.volumeColor.x")                                      , 0.0f , V_FLOAT },
            {"pedFpsLight.volumeColor.y"                                      , rage::joaat("pedFpsLight.volumeColor.y")                                      , 0.0f , V_FLOAT },
            {"pedFpsLight.volumeColor.z"                                      , rage::joaat("pedFpsLight.volumeColor.z")                                      , 0.0f , V_FLOAT },
            {"pedFpsLight.volumeColor.w"                                      , rage::joaat("pedFpsLight.volumeColor.w")                                      , 0.0f , V_FLOAT },
            {"pedFpslight.fade"                                               , rage::joaat("pedFpslight.fade")                                               , 0.0f , V_FLOAT },
            {"pedFpslight.shadowFade"                                         , rage::joaat("pedFpslight.shadowFade")                                         , 0.0f , V_FLOAT },
            {"pedFpslight.specularFade"                                       , rage::joaat("pedFpslight.specularFade")                                       , 0.0f , V_FLOAT },
            {"pedFootLight.color.red"                                         , rage::joaat("pedFootLight.color.red")                                         , 0.0f , V_COL3  },
            {"pedFootLight.color.green"                                       , rage::joaat("pedFootLight.color.green")                                       , 0.0f , V_NONE  },
            {"pedFootLight.color.blue"                                        , rage::joaat("pedFootLight.color.blue")                                        , 0.0f , V_NONE  },
            {"pedFootLight.intensity"                                         , rage::joaat("pedFootLight.intensity")                                         , 0.0f , V_FLOAT },
            {"pedFootLight.radius"                                            , rage::joaat("pedFootLight.radius")                                            , 0.0f , V_FLOAT },
            {"pedFootLight.capsuleLength"                                     , rage::joaat("pedFootLight.capsuleLength")                                     , 0.0f , V_FLOAT },
            {"pedFootLight.offset.x"                                          , rage::joaat("pedFootLight.offset.x")                                          , 0.0f , V_FLOAT },
            {"pedFootLight.offset.y"                                          , rage::joaat("pedFootLight.offset.y")                                          , 0.0f , V_FLOAT },
            {"pedFootLight.offset.z"                                          , rage::joaat("pedFootLight.offset.z")                                          , 0.0f , V_FLOAT },
            {"pedFootLight.offset.w"                                          , rage::joaat("pedFootLight.offset.w")                                          , 0.0f , V_FLOAT },
            {"pedFootLight.falloffExp"                                        , rage::joaat("pedFootLight.falloffExp")                                        , 0.0f , V_FLOAT },
            {"pedFootLight.fade"                                              , rage::joaat("pedFootLight.fade")                                              , 0.0f , V_FLOAT },
            {"pedFootLight.specularFade"                                      , rage::joaat("pedFootLight.specularFade")                                      , 0.0f , V_FLOAT },
    };
    paramsMap["Lod distances for peds"] =
    {
            {"ped.lod.distance.high"                                          , rage::joaat("ped.lod.distance.high")                                          , 0.0f , V_FLOAT },
            {"ped.lod.distance.medium"                                        , rage::joaat("ped.lod.distance.medium")                                        , 0.0f , V_FLOAT },
            {"ped.lod.distance.low"                                           , rage::joaat("ped.lod.distance.low")                                           , 0.0f , V_FLOAT },
            {"pedincar.lod.distance.high"                                     , rage::joaat("pedincar.lod.distance.high")                                     , 0.0f , V_FLOAT },
            {"pedincar.lod.distance.high.x64"                                 , rage::joaat("pedincar.lod.distance.high.x64")                                 , 0.0f , V_FLOAT },
    };
    paramsMap["Config Values for Camera Events      Val start       Val End MB start        MB End"] =
    {
            {"cam.followped.blur.zoom.x"                                      , rage::joaat("cam.followped.blur.zoom.x")                                      , 0.0f , V_FLOAT },
            {"cam.followped.blur.zoom.y"                                      , rage::joaat("cam.followped.blur.zoom.y")                                      , 0.0f , V_FLOAT },
            {"cam.followped.blur.zoom.z"                                      , rage::joaat("cam.followped.blur.zoom.z")                                      , 0.0f , V_FLOAT },
            {"cam.followped.blur.zoom.w"                                      , rage::joaat("cam.followped.blur.zoom.w")                                      , 0.0f , V_FLOAT },
            {"cam.followped.blur.damage.x"                                    , rage::joaat("cam.followped.blur.damage.x")                                    , 0.0f , V_FLOAT },
            {"cam.followped.blur.damage.y"                                    , rage::joaat("cam.followped.blur.damage.y")                                    , 0.0f , V_FLOAT },
            {"cam.followped.blur.damage.z"                                    , rage::joaat("cam.followped.blur.damage.z")                                    , 0.0f , V_FLOAT },
            {"cam.followped.blur.damage.w"                                    , rage::joaat("cam.followped.blur.damage.w")                                    , 0.0f , V_FLOAT },
            {"cam.followped.blur.falling.x"                                   , rage::joaat("cam.followped.blur.falling.x")                                   , 0.0f , V_FLOAT },
            {"cam.followped.blur.falling.y"                                   , rage::joaat("cam.followped.blur.falling.y")                                   , 0.0f , V_FLOAT },
            {"cam.followped.blur.falling.z"                                   , rage::joaat("cam.followped.blur.falling.z")                                   , 0.0f , V_FLOAT },
            {"cam.followped.blur.falling.w"                                   , rage::joaat("cam.followped.blur.falling.w")                                   , 0.0f , V_FLOAT },
            {"cam.followped.blur.beta.x"                                      , rage::joaat("cam.followped.blur.beta.x")                                      , 0.0f , V_FLOAT },
            {"cam.followped.blur.beta.y"                                      , rage::joaat("cam.followped.blur.beta.y")                                      , 0.0f , V_FLOAT },
            {"cam.followped.blur.beta.z"                                      , rage::joaat("cam.followped.blur.beta.z")                                      , 0.0f , V_FLOAT },
            {"cam.followped.blur.beta.w"                                      , rage::joaat("cam.followped.blur.beta.w")                                      , 0.0f , V_FLOAT },
            {"cam.followped.blur.damage.time"                                 , rage::joaat("cam.followped.blur.damage.time")                                 , 0.0f , V_FLOAT },
            {"cam.followped.blur.damage.attacktime"                           , rage::joaat("cam.followped.blur.damage.attacktime")                           , 0.0f , V_FLOAT },
            {"cam.followped.blur.damage.decaytime"                            , rage::joaat("cam.followped.blur.damage.decaytime")                            , 0.0f , V_FLOAT },
            {"cam.followped.blur.cap"                                         , rage::joaat("cam.followped.blur.cap")                                         , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.zoom.x"                                      , rage::joaat("cam.aimweapon.blur.zoom.x")                                      , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.zoom.y"                                      , rage::joaat("cam.aimweapon.blur.zoom.y")                                      , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.zoom.z"                                      , rage::joaat("cam.aimweapon.blur.zoom.z")                                      , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.zoom.w"                                      , rage::joaat("cam.aimweapon.blur.zoom.w")                                      , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.damage.x"                                    , rage::joaat("cam.aimweapon.blur.damage.x")                                    , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.damage.y"                                    , rage::joaat("cam.aimweapon.blur.damage.y")                                    , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.damage.z"                                    , rage::joaat("cam.aimweapon.blur.damage.z")                                    , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.damage.w"                                    , rage::joaat("cam.aimweapon.blur.damage.w")                                    , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.damage.time"                                 , rage::joaat("cam.aimweapon.blur.damage.time")                                 , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.interp"                                      , rage::joaat("cam.aimweapon.blur.interp")                                      , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.damage.attacktime"                           , rage::joaat("cam.aimweapon.blur.damage.attacktime")                           , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.damage.decaytime"                            , rage::joaat("cam.aimweapon.blur.damage.decaytime")                            , 0.0f , V_FLOAT },
            {"cam.aimweapon.blur.cap"                                         , rage::joaat("cam.aimweapon.blur.cap")                                         , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.zoom.x"                                  , rage::joaat("cam.followvehicle.blur.zoom.x")                                  , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.zoom.y"                                  , rage::joaat("cam.followvehicle.blur.zoom.y")                                  , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.zoom.z"                                  , rage::joaat("cam.followvehicle.blur.zoom.z")                                  , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.zoom.w"                                  , rage::joaat("cam.followvehicle.blur.zoom.w")                                  , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.speed.x"                                 , rage::joaat("cam.followvehicle.blur.speed.x")                                 , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.speed.y"                                 , rage::joaat("cam.followvehicle.blur.speed.y")                                 , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.speed.z"                                 , rage::joaat("cam.followvehicle.blur.speed.z")                                 , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.speed.w"                                 , rage::joaat("cam.followvehicle.blur.speed.w")                                 , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.damage.x"                                , rage::joaat("cam.followvehicle.blur.damage.x")                                , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.damage.y"                                , rage::joaat("cam.followvehicle.blur.damage.y")                                , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.damage.z"                                , rage::joaat("cam.followvehicle.blur.damage.z")                                , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.damage.w"                                , rage::joaat("cam.followvehicle.blur.damage.w")                                , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.damage.time"                             , rage::joaat("cam.followvehicle.blur.damage.time")                             , 0.0f , V_FLOAT },
            {"cam.followvehicle.blur.cap"                                     , rage::joaat("cam.followvehicle.blur.cap")                                     , 0.0f , V_FLOAT },
            {"cam.game.blur.wasted"                                           , rage::joaat("cam.game.blur.wasted")                                           , 0.0f , V_FLOAT },
            {"cam.game.blur.wasted.fadetime"                                  , rage::joaat("cam.game.blur.wasted.fadetime")                                  , 0.0f , V_FLOAT },
            {"cam.game.blur.busted"                                           , rage::joaat("cam.game.blur.busted")                                           , 0.0f , V_FLOAT },
            {"cam.game.blur.busted.fadetime"                                  , rage::joaat("cam.game.blur.busted.fadetime")                                  , 0.0f , V_FLOAT },
            {"cam.game.blur.cap"                                              , rage::joaat("cam.game.blur.cap")                                              , 0.0f , V_FLOAT },
            {"cam.fpsweapon.blur"                                             , rage::joaat("cam.fpsweapon.blur")                                             , 0.0f , V_FLOAT },
            {"cam.fpsweapon.sniperinitime"                                    , rage::joaat("cam.fpsweapon.sniperinitime")                                    , 0.0f , V_FLOAT },
            {"cam.fpsweapon.blur.cap"                                         , rage::joaat("cam.fpsweapon.blur.cap")                                         , 0.0f , V_FLOAT },
            {"cam.intermezzo.stuntjump.blur"                                  , rage::joaat("cam.intermezzo.stuntjump.blur")                                  , 0.0f , V_FLOAT },
    };
    paramsMap["Containers LOD"] =
    {
            {"lod.container.caploddist"                                       , rage::joaat("lod.container.caploddist")                                       , 0.0f , V_FLOAT },
    };
    paramsMap["distant lights"] =
    {
            {"distantlights.inlandHeight"                                     , rage::joaat("distantlights.inlandHeight")                                     , 0.0f , V_FLOAT },
            {"distantlights.size"                                             , rage::joaat("distantlights.size")                                             , 0.0f , V_FLOAT },
            {"distantlights.sizeReflections"                                  , rage::joaat("distantlights.sizeReflections")                                  , 0.0f , V_FLOAT },
            {"distantlights.sizeMin"                                          , rage::joaat("distantlights.sizeMin")                                          , 0.0f , V_FLOAT },
            {"distantlights.sizeUpscale"                                      , rage::joaat("distantlights.sizeUpscale")                                      , 0.0f , V_FLOAT },
            {"distantlights.sizeUpscaleReflections"                           , rage::joaat("distantlights.sizeUpscaleReflections")                           , 0.0f , V_FLOAT },
            {"distantlights.flicker"                                          , rage::joaat("distantlights.flicker")                                          , 0.0f , V_FLOAT },
            {"distantlights.twinkleAmount"                                    , rage::joaat("distantlights.twinkleAmount")                                    , 0.0f , V_FLOAT },
            {"distantlights.twinkleSpeed"                                     , rage::joaat("distantlights.twinkleSpeed")                                     , 0.0f , V_FLOAT },
            {"distantlights.carlightZOffset"                                  , rage::joaat("distantlights.carlightZOffset")                                  , 0.0f , V_FLOAT },
            {"distantlights.hourStart"                                        , rage::joaat("distantlights.hourStart")                                        , 0.0f , V_FLOAT },
            {"distantlights.hourEnd"                                          , rage::joaat("distantlights.hourEnd")                                          , 0.0f , V_FLOAT },
            {"distantlights.streetLightHourStart"                             , rage::joaat("distantlights.streetLightHourStart")                             , 0.0f , V_FLOAT },
            {"distantlights.streetLighthourEnd"                               , rage::joaat("distantlights.streetLighthourEnd")                               , 0.0f , V_FLOAT },
            {"distantlights.sizeDistStart"                                    , rage::joaat("distantlights.sizeDistStart")                                    , 0.0f , V_FLOAT },
            {"distantlights.sizeDist"                                         , rage::joaat("distantlights.sizeDist")                                         , 0.0f , V_FLOAT },
            {"distantlights.speed0"                                           , rage::joaat("distantlights.speed0")                                           , 0.0f , V_FLOAT },
            {"distantlights.speed1"                                           , rage::joaat("distantlights.speed1")                                           , 0.0f , V_FLOAT },
            {"distantlights.speed2"                                           , rage::joaat("distantlights.speed2")                                           , 0.0f , V_FLOAT },
            {"distantlights.speed3"                                           , rage::joaat("distantlights.speed3")                                           , 0.0f , V_FLOAT },
            {"distantlights.density0Spacing"                                  , rage::joaat("distantlights.density0Spacing")                                  , 0.0f , V_FLOAT },
            {"distantlights.density15Spacing"                                 , rage::joaat("distantlights.density15Spacing")                                 , 0.0f , V_FLOAT },
            {"distantlights.speed0Speed"                                      , rage::joaat("distantlights.speed0Speed")                                      , 0.0f , V_FLOAT },
            {"distantlights.speed3Speed"                                      , rage::joaat("distantlights.speed3Speed")                                      , 0.0f , V_FLOAT },
            {"distantlights.randomizeSpeed.sp"                                , rage::joaat("distantlights.randomizeSpeed.sp")                                , 0.0f , V_FLOAT },
            {"distantlights.randomizeSpacing.sp"                              , rage::joaat("distantlights.randomizeSpacing.sp")                              , 0.0f , V_FLOAT },
            {"distantlights.randomizeSpeed.mp"                                , rage::joaat("distantlights.randomizeSpeed.mp")                                , 0.0f , V_FLOAT },
            {"distantlights.randomizeSpacing.mp"                              , rage::joaat("distantlights.randomizeSpacing.mp")                              , 0.0f , V_FLOAT },
            {"distantlights.carlight.HDRIntensity"                            , rage::joaat("distantlights.carlight.HDRIntensity")                            , 0.0f , V_FLOAT },
            {"distantlights.carlight.nearFade"                                , rage::joaat("distantlights.carlight.nearFade")                                , 0.0f , V_FLOAT },
            {"distantlights.carlight.farFade"                                 , rage::joaat("distantlights.carlight.farFade")                                 , 0.0f , V_FLOAT },
            {"distantlights.carlight1.spacing.sp"                             , rage::joaat("distantlights.carlight1.spacing.sp")                             , 0.0f , V_FLOAT },
            {"distantlights.carlight1.speed.sp"                               , rage::joaat("distantlights.carlight1.speed.sp")                               , 0.0f , V_FLOAT },
            {"distantlights.carlight1.spacing.mp"                             , rage::joaat("distantlights.carlight1.spacing.mp")                             , 0.0f , V_FLOAT },
            {"distantlights.carlight1.speed.mp"                               , rage::joaat("distantlights.carlight1.speed.mp")                               , 0.0f , V_FLOAT },
            {"distantlights.carlight1.color.red"                              , rage::joaat("distantlights.carlight1.color.red")                              , 0.0f , V_COL3  },
            {"distantlights.carlight1.color.green"                            , rage::joaat("distantlights.carlight1.color.green")                            , 0.0f , V_NONE  },
            {"distantlights.carlight1.color.blue"                             , rage::joaat("distantlights.carlight1.color.blue")                             , 0.0f , V_NONE  },
            {"distantlights.carlight2.spacing.sp"                             , rage::joaat("distantlights.carlight2.spacing.sp")                             , 0.0f , V_FLOAT },
            {"distantlights.carlight2.speed.sp"                               , rage::joaat("distantlights.carlight2.speed.sp")                               , 0.0f , V_FLOAT },
            {"distantlights.carlight2.spacing.mp"                             , rage::joaat("distantlights.carlight2.spacing.mp")                             , 0.0f , V_FLOAT },
            {"distantlights.carlight2.speed.mp"                               , rage::joaat("distantlights.carlight2.speed.mp")                               , 0.0f , V_FLOAT },
            {"distantlights.carlight2.color.red"                              , rage::joaat("distantlights.carlight2.color.red")                              , 0.0f , V_COL3  },
            {"distantlights.carlight2.color.green"                            , rage::joaat("distantlights.carlight2.color.green")                            , 0.0f , V_NONE  },
            {"distantlights.carlight2.color.blue"                             , rage::joaat("distantlights.carlight2.color.blue")                             , 0.0f , V_NONE  },
            {"distantlights.streetlight.HDRIntensity"                         , rage::joaat("distantlights.streetlight.HDRIntensity")                         , 0.0f , V_FLOAT },
            {"distantlights.streetlight.ZOffset"                              , rage::joaat("distantlights.streetlight.ZOffset")                              , 0.0f , V_FLOAT },
            {"distantlights.streetlight.Spacing"                              , rage::joaat("distantlights.streetlight.Spacing")                              , 0.0f , V_FLOAT },
            {"distantlights.streetlight.color.red"                            , rage::joaat("distantlights.streetlight.color.red")                            , 0.0f , V_COL3  },
            {"distantlights.streetlight.color.green"                          , rage::joaat("distantlights.streetlight.color.green")                          , 0.0f , V_NONE  },
            {"distantlights.streetlight.color.blue"                           , rage::joaat("distantlights.streetlight.color.blue")                           , 0.0f , V_NONE  },
    };
    paramsMap["Emissive night values"] =
    {
            {"emissive.night.start.time"                                      , rage::joaat("emissive.night.start.time")                                      , 0.0f , V_FLOAT },
            {"emissive.night.end.time"                                        , rage::joaat("emissive.night.end.time")                                        , 0.0f , V_FLOAT },
    };
    paramsMap["Misc Values"] =
    {
            {"misc.DOFBlurMultiplier.HD"                                      , rage::joaat("misc.DOFBlurMultiplier.HD")                                      , 0.0f , V_FLOAT },
            {"misc.DOFBlurMultiplier.SD"                                      , rage::joaat("misc.DOFBlurMultiplier.SD")                                      , 0.0f , V_FLOAT },
            {"misc.Multiplier.heightStart"                                    , rage::joaat("misc.Multiplier.heightStart")                                    , 0.0f , V_FLOAT },
            {"misc.Multiplier.heightEnd"                                      , rage::joaat("misc.Multiplier.heightEnd")                                      , 0.0f , V_FLOAT },
            {"misc.Multiplier.farClipMultiplier"                              , rage::joaat("misc.Multiplier.farClipMultiplier")                              , 0.0f , V_FLOAT },
            {"misc.Multiplier.nearFogMultiplier"                              , rage::joaat("misc.Multiplier.nearFogMultiplier")                              , 0.0f , V_FLOAT },
            {"misc.3dMarkers.FrontLightIntensity"                             , rage::joaat("misc.3dMarkers.FrontLightIntensity")                             , 0.0f , V_FLOAT },
            {"misc.3dMarkers.frontLightExponent"                              , rage::joaat("misc.3dMarkers.frontLightExponent")                              , 0.0f , V_FLOAT },
    };
    paramsMap["coronas"] =
    {
            {"misc.coronas.sizeScaleGlobal"                                   , rage::joaat("misc.coronas.sizeScaleGlobal")                                   , 0.0f , V_FLOAT },
            {"misc.coronas.intensityScaleGlobal"                              , rage::joaat("misc.coronas.intensityScaleGlobal")                              , 0.0f , V_FLOAT },
            {"misc.coronas.intensityScaleWater"                               , rage::joaat("misc.coronas.intensityScaleWater")                               , 0.0f , V_FLOAT },
            {"misc.coronas.sizeScaleWater"                                    , rage::joaat("misc.coronas.sizeScaleWater")                                    , 0.0f , V_FLOAT },
            {"misc.coronas.sizeScaleParaboloid"                               , rage::joaat("misc.coronas.sizeScaleParaboloid")                               , 0.0f , V_FLOAT },
            {"misc.coronas.screenspaceExpansion"                              , rage::joaat("misc.coronas.screenspaceExpansion")                              , 0.0f , V_FLOAT },
            {"misc.coronas.screenspaceExpansionWater"                         , rage::joaat("misc.coronas.screenspaceExpansionWater")                         , 0.0f , V_FLOAT },
            {"misc.coronas.zBiasMultiplier"                                   , rage::joaat("misc.coronas.zBiasMultiplier")                                   , 0.0f , V_FLOAT },
            {"misc.coronas.zBiasDistanceNear"                                 , rage::joaat("misc.coronas.zBiasDistanceNear")                                 , 0.0f , V_FLOAT },
            {"misc.coronas.zBiasDistanceFar"                                  , rage::joaat("misc.coronas.zBiasDistanceFar")                                  , 0.0f , V_FLOAT },
            {"misc.coronas.zBias.fromFinalSizeMultiplier"                     , rage::joaat("misc.coronas.zBias.fromFinalSizeMultiplier")                     , 0.0f , V_FLOAT },
            {"misc.coronas.occlusionSizeScale"                                , rage::joaat("misc.coronas.occlusionSizeScale")                                , 0.0f , V_FLOAT },
            {"misc.coronas.occlusionSizeMax"                                  , rage::joaat("misc.coronas.occlusionSizeMax")                                  , 0.0f , V_FLOAT },
            {"misc.coronas.flareCoronaSizeRatio"                              , rage::joaat("misc.coronas.flareCoronaSizeRatio")                              , 0.0f , V_FLOAT },
            {"misc.coronas.flareMinAngleDegToFadeIn"                          , rage::joaat("misc.coronas.flareMinAngleDegToFadeIn")                          , 0.0f , V_FLOAT },
            {"misc.coronas.flareScreenCenterOffsetSizeMult"                   , rage::joaat("misc.coronas.flareScreenCenterOffsetSizeMult")                   , 0.0f , V_FLOAT },
            {"misc.coronas.flareColShiftR"                                    , rage::joaat("misc.coronas.flareColShiftR")                                    , 0.0f , V_FLOAT },
            {"misc.coronas.flareColShiftG"                                    , rage::joaat("misc.coronas.flareColShiftG")                                    , 0.0f , V_FLOAT },
            {"misc.coronas.flareColShiftB"                                    , rage::joaat("misc.coronas.flareColShiftB")                                    , 0.0f , V_FLOAT },
            {"misc.coronas.baseValueR"                                        , rage::joaat("misc.coronas.baseValueR")                                        , 0.0f , V_FLOAT },
            {"misc.coronas.baseValueG"                                        , rage::joaat("misc.coronas.baseValueG")                                        , 0.0f , V_FLOAT },
            {"misc.coronas.baseValueB"                                        , rage::joaat("misc.coronas.baseValueB")                                        , 0.0f , V_FLOAT },
            {"misc.coronas.zBias.min"                                         , rage::joaat("misc.coronas.zBias.min")                                         , 0.0f , V_FLOAT },
            {"misc.coronas.zBias.max"                                         , rage::joaat("misc.coronas.zBias.max")                                         , 0.0f , V_FLOAT },
            {"misc.coronas.zBias.additionalBias.WaterReflection"              , rage::joaat("misc.coronas.zBias.additionalBias.WaterReflection")              , 0.0f , V_FLOAT },
            {"misc.coronas.zBias.additionalBias.EnvReflection"                , rage::joaat("misc.coronas.zBias.additionalBias.EnvReflection")                , 0.0f , V_FLOAT },
            {"misc.coronas.zBias.additionalBias.MirrorReflection"             , rage::joaat("misc.coronas.zBias.additionalBias.MirrorReflection")             , 0.0f , V_FLOAT },
            {"misc.coronas.dir.mult.fadeoff.start"                            , rage::joaat("misc.coronas.dir.mult.fadeoff.start")                            , 0.0f , V_FLOAT },
            {"misc.coronas.dir.mult.fadeoff.dist"                             , rage::joaat("misc.coronas.dir.mult.fadeoff.dist")                             , 0.0f , V_FLOAT },
            {"misc.coronas.flareScreenCenterOffsetVerticalSizeMult"           , rage::joaat("misc.coronas.flareScreenCenterOffsetVerticalSizeMult")           , 0.0f , V_FLOAT },
            {"misc.coronas.underwaterFadeDist"                                , rage::joaat("misc.coronas.underwaterFadeDist")                                , 0.0f , V_FLOAT },
            {"misc.coronas.m_doNGCoronas"                                     , rage::joaat("misc.coronas.m_doNGCoronas")                                     , 0.0f , V_FLOAT },
            {"misc.coronas.m_rotationSpeed"                                   , rage::joaat("misc.coronas.m_rotationSpeed")                                   , 0.0f , V_FLOAT },
            {"misc.coronas.m_rotationSpeedRampedUp"                           , rage::joaat("misc.coronas.m_rotationSpeedRampedUp")                           , 0.0f , V_FLOAT },
            {"misc.coronas.m_layer1Muliplier"                                 , rage::joaat("misc.coronas.m_layer1Muliplier")                                 , 0.0f , V_FLOAT },
            {"misc.coronas.m_layer2Muliplier"                                 , rage::joaat("misc.coronas.m_layer2Muliplier")                                 , 0.0f , V_FLOAT },
            {"misc.coronas.m_scaleRampUp"                                     , rage::joaat("misc.coronas.m_scaleRampUp")                                     , 0.0f , V_FLOAT },
            {"misc.coronas.m_scaleRampUpAngle"                                , rage::joaat("misc.coronas.m_scaleRampUpAngle")                                , 0.0f , V_FLOAT },
            {"misc.coronas.flareScreenCenterOffsetVerticalSizeMult"           , rage::joaat("misc.coronas.flareScreenCenterOffsetVerticalSizeMult")           , 0.0f , V_FLOAT },
            {"misc.coronas.screenEdgeMinDistForFade"                          , rage::joaat("misc.coronas.screenEdgeMinDistForFade")                          , 0.0f , V_FLOAT },
            {"misc.BloomIntensityClamp.HD"                                    , rage::joaat("misc.BloomIntensityClamp.HD")                                    , 0.0f , V_FLOAT },
            {"misc.BloomIntensityClamp.SD"                                    , rage::joaat("misc.BloomIntensityClamp.SD")                                    , 0.0f , V_FLOAT },
            {"misc.CrossPMultiplier.RimLight"                                 , rage::joaat("misc.CrossPMultiplier.RimLight")                                 , 0.0f , V_FLOAT },
            {"misc.CrossPMultiplier.GlobalEnvironmentReflection"              , rage::joaat("misc.CrossPMultiplier.GlobalEnvironmentReflection")              , 0.0f , V_FLOAT },
            {"misc.CrossPMultiplier.Gamma"                                    , rage::joaat("misc.CrossPMultiplier.Gamma")                                    , 0.0f , V_FLOAT },
            {"misc.CrossPMultiplier.MidBlur"                                  , rage::joaat("misc.CrossPMultiplier.MidBlur")                                  , 0.0f , V_FLOAT },
            {"misc.CrossPMultiplier.Farblur"                                  , rage::joaat("misc.CrossPMultiplier.Farblur")                                  , 0.0f , V_FLOAT },
            {"misc.CrossPMultiplier.SkyMultiplier"                            , rage::joaat("misc.CrossPMultiplier.SkyMultiplier")                            , 0.0f , V_FLOAT },
            {"misc.CrossPMultiplier.Desaturation"                             , rage::joaat("misc.CrossPMultiplier.Desaturation")                             , 0.0f , V_FLOAT },
            {"misc.HiDof.nearBlur"                                            , rage::joaat("misc.HiDof.nearBlur")                                            , 0.0f , V_FLOAT },
            {"misc.HiDof.midBlur"                                             , rage::joaat("misc.HiDof.midBlur")                                             , 0.0f , V_FLOAT },
            {"misc.HiDof.farBlur"                                             , rage::joaat("misc.HiDof.farBlur")                                             , 0.0f , V_FLOAT },
            {"misc.cutscene.nearBlurMin"                                      , rage::joaat("misc.cutscene.nearBlurMin")                                      , 0.0f , V_FLOAT },
            {"misc.HiDof.nearBlur"                                            , rage::joaat("misc.HiDof.nearBlur")                                            , 0.0f , V_FLOAT },
            {"misc.HiDof.midBlur"                                             , rage::joaat("misc.HiDof.midBlur")                                             , 0.0f , V_FLOAT },
            {"misc.HiDof.farBlur"                                             , rage::joaat("misc.HiDof.farBlur")                                             , 0.0f , V_FLOAT },
            {"misc.cutscene.nearBlurMin"                                      , rage::joaat("misc.cutscene.nearBlurMin")                                      , 0.0f , V_FLOAT },
            {"misc.PedKillOverlay.duration"                                   , rage::joaat("misc.PedKillOverlay.duration")                                   , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.RampUpDuration"                              , rage::joaat("misc.DamageOverlay.RampUpDuration")                              , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.HoldDuration"                                , rage::joaat("misc.DamageOverlay.HoldDuration")                                , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.RampDownDuration"                            , rage::joaat("misc.DamageOverlay.RampDownDuration")                            , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.ColorBottom.red"                             , rage::joaat("misc.DamageOverlay.ColorBottom.red")                             , 0.0f , V_COL3  },
            {"misc.DamageOverlay.ColorBottom.green"                           , rage::joaat("misc.DamageOverlay.ColorBottom.green")                           , 0.0f , V_NONE  },
            {"misc.DamageOverlay.ColorBottom.blue"                            , rage::joaat("misc.DamageOverlay.ColorBottom.blue")                            , 0.0f , V_NONE  },
            {"misc.DamageOverlay.ColorTop.red"                                , rage::joaat("misc.DamageOverlay.ColorTop.red")                                , 0.0f , V_COL3  },
            {"misc.DamageOverlay.ColorTop.green"                              , rage::joaat("misc.DamageOverlay.ColorTop.green")                              , 0.0f , V_NONE  },
            {"misc.DamageOverlay.ColorTop.blue"                               , rage::joaat("misc.DamageOverlay.ColorTop.blue")                               , 0.0f , V_NONE  },
            {"misc.DamageOverlay.GlobalAlphaBottom"                           , rage::joaat("misc.DamageOverlay.GlobalAlphaBottom")                           , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.GlobalAlphaTop"                              , rage::joaat("misc.DamageOverlay.GlobalAlphaTop")                              , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.SpriteLength"                                , rage::joaat("misc.DamageOverlay.SpriteLength")                                , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.SpriteBaseWidth"                             , rage::joaat("misc.DamageOverlay.SpriteBaseWidth")                             , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.SpriteTipWidth"                              , rage::joaat("misc.DamageOverlay.SpriteTipWidth")                              , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.HorizontalFadeSoftness"                      , rage::joaat("misc.DamageOverlay.HorizontalFadeSoftness")                      , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.VerticalFadeOffset"                          , rage::joaat("misc.DamageOverlay.VerticalFadeOffset")                          , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.AngleScalingMult"                            , rage::joaat("misc.DamageOverlay.AngleScalingMult")                            , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.RampUpDuration"                           , rage::joaat("misc.DamageOverlay.FP.RampUpDuration")                           , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.HoldDuration"                             , rage::joaat("misc.DamageOverlay.FP.HoldDuration")                             , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.RampDownDuration"                         , rage::joaat("misc.DamageOverlay.FP.RampDownDuration")                         , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.ColorBottom.red"                          , rage::joaat("misc.DamageOverlay.FP.ColorBottom.red")                          , 0.0f , V_COL3  },
            {"misc.DamageOverlay.FP.ColorBottom.green"                        , rage::joaat("misc.DamageOverlay.FP.ColorBottom.green")                        , 0.0f , V_NONE  },
            {"misc.DamageOverlay.FP.ColorBottom.blue"                         , rage::joaat("misc.DamageOverlay.FP.ColorBottom.blue")                         , 0.0f , V_NONE  },
            {"misc.DamageOverlay.FP.ColorTop.red"                             , rage::joaat("misc.DamageOverlay.FP.ColorTop.red")                             , 0.0f , V_COL3  },
            {"misc.DamageOverlay.FP.ColorTop.green"                           , rage::joaat("misc.DamageOverlay.FP.ColorTop.green")                           , 0.0f , V_NONE  },
            {"misc.DamageOverlay.FP.ColorTop.blue"                            , rage::joaat("misc.DamageOverlay.FP.ColorTop.blue")                            , 0.0f , V_NONE  },
            {"misc.DamageOverlay.FP.GlobalAlphaBottom"                        , rage::joaat("misc.DamageOverlay.FP.GlobalAlphaBottom")                        , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.GlobalAlphaTop"                           , rage::joaat("misc.DamageOverlay.FP.GlobalAlphaTop")                           , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.SpriteLength"                             , rage::joaat("misc.DamageOverlay.FP.SpriteLength")                             , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.SpriteBaseWidth"                          , rage::joaat("misc.DamageOverlay.FP.SpriteBaseWidth")                          , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.SpriteTipWidth"                           , rage::joaat("misc.DamageOverlay.FP.SpriteTipWidth")                           , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.HorizontalFadeSoftness"                   , rage::joaat("misc.DamageOverlay.FP.HorizontalFadeSoftness")                   , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.VerticalFadeOffset"                       , rage::joaat("misc.DamageOverlay.FP.VerticalFadeOffset")                       , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.FP.AngleScalingMult"                         , rage::joaat("misc.DamageOverlay.FP.AngleScalingMult")                         , 0.0f , V_FLOAT },
            {"misc.DamageOverlay.ScreenSafeZoneLength"                        , rage::joaat("misc.DamageOverlay.ScreenSafeZoneLength")                        , 0.0f , V_FLOAT },
            {"misc.SniperRifleDof.Enabled"                                    , rage::joaat("misc.SniperRifleDof.Enabled")                                    , 0.0f , V_FLOAT },
            {"misc.SniperRifleDof.NearStart"                                  , rage::joaat("misc.SniperRifleDof.NearStart")                                  , 0.0f , V_FLOAT },
            {"misc.SniperRifleDof.NearEnd"                                    , rage::joaat("misc.SniperRifleDof.NearEnd")                                    , 0.0f , V_FLOAT },
            {"misc.SniperRifleDof.FarStart"                                   , rage::joaat("misc.SniperRifleDof.FarStart")                                   , 0.0f , V_FLOAT },
            {"misc.SniperRifleDof.FarEnd"                                     , rage::joaat("misc.SniperRifleDof.FarEnd")                                     , 0.0f , V_FLOAT },
            {"misc.MoonDimMult"                                               , rage::joaat("misc.MoonDimMult")                                               , 0.0f , V_FLOAT },
            {"misc.NextGenModifier"                                           , rage::joaat("misc.NextGenModifier")                                           , 0.0f , V_FLOAT },
    };
    paramsMap["See Through"] =
    {
            {"seeThrough.PedQuadSize"                                         , rage::joaat("seeThrough.PedQuadSize")                                         , 0.0f , V_FLOAT },
            {"seeThrough.PedQuadStartDistance"                                , rage::joaat("seeThrough.PedQuadStartDistance")                                , 0.0f , V_FLOAT },
            {"seeThrough.PedQuadEndDistance"                                  , rage::joaat("seeThrough.PedQuadEndDistance")                                  , 0.0f , V_FLOAT },
            {"seeThrough.PedQuadScale"                                        , rage::joaat("seeThrough.PedQuadScale")                                        , 0.0f , V_FLOAT },
            {"seeThrough.FadeStartDistance"                                   , rage::joaat("seeThrough.FadeStartDistance")                                   , 0.0f , V_FLOAT },
            {"seeThrough.FadeEndDistance"                                     , rage::joaat("seeThrough.FadeEndDistance")                                     , 0.0f , V_FLOAT },
            {"seeThrough.MaxThickness"                                        , rage::joaat("seeThrough.MaxThickness")                                        , 0.0f , V_FLOAT },
            {"seeThrough.MinNoiseAmount"                                      , rage::joaat("seeThrough.MinNoiseAmount")                                      , 0.0f , V_FLOAT },
            {"seeThrough.MaxNoiseAmount"                                      , rage::joaat("seeThrough.MaxNoiseAmount")                                      , 0.0f , V_FLOAT },
            {"seeThrough.HiLightIntensity"                                    , rage::joaat("seeThrough.HiLightIntensity")                                    , 0.0f , V_FLOAT },
            {"seeThrough.HiLightNoise"                                        , rage::joaat("seeThrough.HiLightNoise")                                        , 0.0f , V_FLOAT },
            {"seeThrough.ColorNear.red"                                       , rage::joaat("seeThrough.ColorNear.red")                                       , 0.0f , V_COL3  },
            {"seeThrough.ColorNear.green"                                     , rage::joaat("seeThrough.ColorNear.green")                                     , 0.0f , V_NONE  },
            {"seeThrough.ColorNear.blue"                                      , rage::joaat("seeThrough.ColorNear.blue")                                      , 0.0f , V_NONE  },
            {"seeThrough.ColorFar.red"                                        , rage::joaat("seeThrough.ColorFar.red")                                        , 0.0f , V_COL3  },
            {"seeThrough.ColorFar.green"                                      , rage::joaat("seeThrough.ColorFar.green")                                      , 0.0f , V_NONE  },
            {"seeThrough.ColorFar.blue"                                       , rage::joaat("seeThrough.ColorFar.blue")                                       , 0.0f , V_NONE  },
            {"seeThrough.ColorVisibleBase.red"                                , rage::joaat("seeThrough.ColorVisibleBase.red")                                , 0.0f , V_COL3  },
            {"seeThrough.ColorVisibleBase.green"                              , rage::joaat("seeThrough.ColorVisibleBase.green")                              , 0.0f , V_NONE  },
            {"seeThrough.ColorVisibleBase.blue"                               , rage::joaat("seeThrough.ColorVisibleBase.blue")                               , 0.0f , V_NONE  },
            {"seeThrough.ColorVisibleWarm.red"                                , rage::joaat("seeThrough.ColorVisibleWarm.red")                                , 0.0f , V_COL3  },
            {"seeThrough.ColorVisibleWarm.green"                              , rage::joaat("seeThrough.ColorVisibleWarm.green")                              , 0.0f , V_NONE  },
            {"seeThrough.ColorVisibleWarm.blue"                               , rage::joaat("seeThrough.ColorVisibleWarm.blue")                               , 0.0f , V_NONE  },
            {"seeThrough.ColorVisibleHot.red"                                 , rage::joaat("seeThrough.ColorVisibleHot.red")                                 , 0.0f , V_COL3  },
            {"seeThrough.ColorVisibleHot.green"                               , rage::joaat("seeThrough.ColorVisibleHot.green")                               , 0.0f , V_NONE  },
            {"seeThrough.ColorVisibleHot.blue"                                , rage::joaat("seeThrough.ColorVisibleHot.blue")                                , 0.0f , V_NONE  },
    };
    paramsMap["Tonemapping"] =
    {
            {"Tonemapping.bright.filmic.A"                                    , rage::joaat("Tonemapping.bright.filmic.A")                                    , 0.0f , V_FLOAT },
            {"Tonemapping.bright.filmic.B"                                    , rage::joaat("Tonemapping.bright.filmic.B")                                    , 0.0f , V_FLOAT },
            {"Tonemapping.bright.filmic.C"                                    , rage::joaat("Tonemapping.bright.filmic.C")                                    , 0.0f , V_FLOAT },
            {"Tonemapping.bright.filmic.D"                                    , rage::joaat("Tonemapping.bright.filmic.D")                                    , 0.0f , V_FLOAT },
            {"Tonemapping.bright.filmic.E"                                    , rage::joaat("Tonemapping.bright.filmic.E")                                    , 0.0f , V_FLOAT },
            {"Tonemapping.bright.filmic.F"                                    , rage::joaat("Tonemapping.bright.filmic.F")                                    , 0.0f , V_FLOAT },
            {"Tonemapping.bright.filmic.W"                                    , rage::joaat("Tonemapping.bright.filmic.W")                                    , 0.0f , V_FLOAT },
            {"Tonemapping.bright.filmic.exposure"                             , rage::joaat("Tonemapping.bright.filmic.exposure")                             , 0.0f , V_FLOAT },
            {"Tonemapping.dark.filmic.A"                                      , rage::joaat("Tonemapping.dark.filmic.A")                                      , 0.0f , V_FLOAT },
            {"Tonemapping.dark.filmic.B"                                      , rage::joaat("Tonemapping.dark.filmic.B")                                      , 0.0f , V_FLOAT },
            {"Tonemapping.dark.filmic.C"                                      , rage::joaat("Tonemapping.dark.filmic.C")                                      , 0.0f , V_FLOAT },
            {"Tonemapping.dark.filmic.D"                                      , rage::joaat("Tonemapping.dark.filmic.D")                                      , 0.0f , V_FLOAT },
            {"Tonemapping.dark.filmic.E"                                      , rage::joaat("Tonemapping.dark.filmic.E")                                      , 0.0f , V_FLOAT },
            {"Tonemapping.dark.filmic.F"                                      , rage::joaat("Tonemapping.dark.filmic.F")                                      , 0.0f , V_FLOAT },
            {"Tonemapping.dark.filmic.W"                                      , rage::joaat("Tonemapping.dark.filmic.W")                                      , 0.0f , V_FLOAT },
            {"Tonemapping.dark.filmic.exposure"                               , rage::joaat("Tonemapping.dark.filmic.exposure")                               , 0.0f , V_FLOAT },
    };
    paramsMap["Exposure"] =
    {
            {"Exposure.curve.scale"                                           , rage::joaat("Exposure.curve.scale")                                           , 0.0f , V_FLOAT },
            {"Exposure.curve.power"                                           , rage::joaat("Exposure.curve.power")                                           , 0.0f , V_FLOAT },
            {"Exposure.curve.offset"                                          , rage::joaat("Exposure.curve.offset")                                          , 0.0f , V_FLOAT },
    };
    paramsMap["Adaptation"] =
    {
            {"Adaptation.min.step.size"                                       , rage::joaat("Adaptation.min.step.size")                                       , 0.0f , V_FLOAT },
            {"Adaptation.max.step.size"                                       , rage::joaat("Adaptation.max.step.size")                                       , 0.0f , V_FLOAT },
            {"Adaptation.step.size.mult"                                      , rage::joaat("Adaptation.step.size.mult")                                      , 0.0f , V_FLOAT },
            {"Adaptation.threshold"                                           , rage::joaat("Adaptation.threshold")                                           , 0.0f , V_FLOAT },
            {"Adaptation.sun.exposure.tweak"                                  , rage::joaat("Adaptation.sun.exposure.tweak")                                  , 0.0f , V_FLOAT },
    };
    paramsMap["Cloud generation"] =
    {
            {"cloudgen.frequency"                                             , rage::joaat("cloudgen.frequency")                                             , 0.0f , V_FLOAT },
            {"cloudgen.scale"                                                 , rage::joaat("cloudgen.scale")                                                 , 0.0f , V_FLOAT },
            {"cloudgen.edge.detail.scale"                                     , rage::joaat("cloudgen.edge.detail.scale")                                     , 0.0f , V_FLOAT },
            {"cloudgen.overlay.detail.scale"                                  , rage::joaat("cloudgen.overlay.detail.scale")                                  , 0.0f , V_FLOAT },
    };
    paramsMap["Cloud speeds"] =
    {
            {"cloud.speed.large"                                              , rage::joaat("cloud.speed.large")                                              , 0.0f , V_FLOAT },
            {"cloud.speed.small"                                              , rage::joaat("cloud.speed.small")                                              , 0.0f , V_FLOAT },
            {"cloud.speed.overall"                                            , rage::joaat("cloud.speed.overall")                                            , 0.0f , V_FLOAT },
            {"cloud.speed.edge"                                               , rage::joaat("cloud.speed.edge")                                               , 0.0f , V_FLOAT },
            {"cloud.speed.hats"                                               , rage::joaat("cloud.speed.hats")                                               , 0.0f , V_FLOAT },
    };
    paramsMap["lod fade distances"] =
    {
            {"lod.fadedist.orphanhd"                                          , rage::joaat("lod.fadedist.orphanhd")                                          , 0.0f , V_FLOAT },
            {"lod.fadedist.hd"                                                , rage::joaat("lod.fadedist.hd")                                                , 0.0f , V_FLOAT },
            {"lod.fadedist.lod"                                               , rage::joaat("lod.fadedist.lod")                                               , 0.0f , V_FLOAT },
            {"lod.fadedist.slod1"                                             , rage::joaat("lod.fadedist.slod1")                                             , 0.0f , V_FLOAT },
            {"lod.fadedist.slod2"                                             , rage::joaat("lod.fadedist.slod2")                                             , 0.0f , V_FLOAT },
            {"lod.fadedist.slod3"                                             , rage::joaat("lod.fadedist.slod3")                                             , 0.0f , V_FLOAT },
            {"lod.fadedist.slod4"                                             , rage::joaat("lod.fadedist.slod4")                                             , 0.0f , V_FLOAT },
    };
    paramsMap["sky plane"] =
    {
            {"skyplane.offset"                                                , rage::joaat("skyplane.offset")                                                , 0.0f , V_FLOAT },
            {"skyplane.fog.fade.start"                                        , rage::joaat("skyplane.fog.fade.start")                                        , 0.0f , V_FLOAT },
            {"skyplane.fog.fade.end"                                          , rage::joaat("skyplane.fog.fade.end")                                          , 0.0f , V_FLOAT },
    };
    paramsMap["Rim lighting"] =
    {
            {"rim.lighting.width.start"                                       , rage::joaat("rim.lighting.width.start")                                       , 0.0f , V_FLOAT },
            {"rim.lighting.width.end"                                         , rage::joaat("rim.lighting.width.end")                                         , 0.0f , V_FLOAT },
            {"rim.lighting.transition.angle"                                  , rage::joaat("rim.lighting.transition.angle")                                  , 0.0f , V_FLOAT },
            {"rim.lighting.downmask.angle"                                    , rage::joaat("rim.lighting.downmask.angle")                                    , 0.0f , V_FLOAT },
            {"rim.lighting.main.colour.intensity"                             , rage::joaat("rim.lighting.main.colour.intensity")                             , 0.0f , V_FLOAT },
            {"rim.lighting.offangle.colour.intensity"                         , rage::joaat("rim.lighting.offangle.colour.intensity")                         , 0.0f , V_FLOAT },
    };
    paramsMap["Reflection tweaks"] =
    {
            {"reflection.tweaks.interior.artificial.boost"                    , rage::joaat("reflection.tweaks.interior.artificial.boost")                    , 0.0f , V_FLOAT },
            {"reflection.tweaks.exterior.artificial.boost"                    , rage::joaat("reflection.tweaks.exterior.artificial.boost")                    , 0.0f , V_FLOAT },
            {"reflection.tweaks.emissive.boost"                               , rage::joaat("reflection.tweaks.emissive.boost")                               , 0.0f , V_FLOAT },
    };
    paramsMap["Light component cutoff"] =
    {
            {"lights.cutoff.distance"                                         , rage::joaat("lights.cutoff.distance")                                         , 0.0f , V_FLOAT },
            {"lights.cutoff.shadow.distance"                                  , rage::joaat("lights.cutoff.shadow.distance")                                  , 0.0f , V_FLOAT },
            {"lights.cutoff.specular.distance"                                , rage::joaat("lights.cutoff.specular.distance")                                , 0.0f , V_FLOAT },
            {"lights.cutoff.volumetric.distance"                              , rage::joaat("lights.cutoff.volumetric.distance")                              , 0.0f , V_FLOAT },
            {"lights.cutoff.nonGbuf.distance"                                 , rage::joaat("lights.cutoff.nonGbuf.distance")                                 , 0.0f , V_FLOAT },
            {"lights.cutoff.map.distance"                                     , rage::joaat("lights.cutoff.map.distance")                                     , 0.0f , V_FLOAT },
    };
    paramsMap["Bloom"] =
    {
            {"bloom.threshold.min"                                            , rage::joaat("bloom.threshold.min")                                            , 0.0f , V_FLOAT },
            {"bloom.threshold.expsoure.diff.min"                              , rage::joaat("bloom.threshold.expsoure.diff.min")                              , 0.0f , V_FLOAT },
            {"bloom.threshold.expsoure.diff.max"                              , rage::joaat("bloom.threshold.expsoure.diff.max")                              , 0.0f , V_FLOAT },
            {"bloom.threshold.power"                                          , rage::joaat("bloom.threshold.power")                                          , 0.0f , V_FLOAT },
            {"bloom.blurblendmult.large"                                      , rage::joaat("bloom.blurblendmult.large")                                      , 0.0f , V_FLOAT },
            {"bloom.blurblendmult.med"                                        , rage::joaat("bloom.blurblendmult.med")                                        , 0.0f , V_FLOAT },
            {"bloom.blurblendmult.small"                                      , rage::joaat("bloom.blurblendmult.small")                                      , 0.0f , V_FLOAT },
    };
    paramsMap["Dynmaic bake paramaters"] =
    {
            {"dynamic.bake.start.min"                                         , rage::joaat("dynamic.bake.start.min")                                         , 0.0f , V_FLOAT },
            {"dynamic.bake.start.max"                                         , rage::joaat("dynamic.bake.start.max")                                         , 0.0f , V_FLOAT },
            {"dynamic.bake.end.min"                                           , rage::joaat("dynamic.bake.end.min")                                           , 0.0f , V_FLOAT },
            {"dynamic.bake.end.max"                                           , rage::joaat("dynamic.bake.end.max")                                           , 0.0f , V_FLOAT },
    };
    paramsMap["Minimap alpha"] =
    {
            {"UI.minimap.alpha"                                               , rage::joaat("UI.minimap.alpha")                                               , 0.0f , V_FLOAT },
    };
    paramsMap["Puddles"] =
    {
            {"puddles.scale"                                                  , rage::joaat("puddles.scale")                                                  , 0.0f , V_FLOAT },
            {"puddles.range"                                                  , rage::joaat("puddles.range")                                                  , 0.0f , V_FLOAT },
            {"puddles.depth"                                                  , rage::joaat("puddles.depth")                                                  , 0.0f , V_FLOAT },
            {"puddles.depthtest"                                              , rage::joaat("puddles.depthtest")                                              , 0.0f , V_FLOAT },
            {"puddles.strength"                                               , rage::joaat("puddles.strength")                                               , 0.0f , V_FLOAT },
            {"puddles.animspeed"                                              , rage::joaat("puddles.animspeed")                                              , 0.0f , V_FLOAT },
            {"puddles.ripples.minsize"                                        , rage::joaat("puddles.ripples.minsize")                                        , 0.0f , V_FLOAT },
            {"puddles.ripples.maxsize"                                        , rage::joaat("puddles.ripples.maxsize")                                        , 0.0f , V_FLOAT },
            {"puddles.ripples.speed"                                          , rage::joaat("puddles.ripples.speed")                                          , 0.0f , V_FLOAT },
            {"puddles.ripples.minduration"                                    , rage::joaat("puddles.ripples.minduration")                                    , 0.0f , V_FLOAT },
            {"puddles.ripples.maxduration"                                    , rage::joaat("puddles.ripples.maxduration")                                    , 0.0f , V_FLOAT },
            {"puddles.ripples.dropfactor"                                     , rage::joaat("puddles.ripples.dropfactor")                                     , 0.0f , V_FLOAT },
            {"puddles.ripples.windstrength"                                   , rage::joaat("puddles.ripples.windstrength")                                   , 0.0f , V_FLOAT },
            {"puddles.ripples.ripplestrength"                                 , rage::joaat("puddles.ripples.ripplestrength")                                 , 0.0f , V_FLOAT },
            {"puddles.ripples.winddamping"                                    , rage::joaat("puddles.ripples.winddamping")                                    , 0.0f , V_FLOAT },
            {"puddles.ripples.playerspeed"                                    , rage::joaat("puddles.ripples.playerspeed")                                    , 0.0f , V_FLOAT },
            {"puddles.ripples.playermapsize"                                  , rage::joaat("puddles.ripples.playermapsize")                                  , 0.0f , V_FLOAT },
    };
    paramsMap["LOD lights - do not change these ranges without talking to code because it requires data changes"] =
    {
            {"lodlight.small.range"                                           , rage::joaat("lodlight.small.range")                                           , 0.0f , V_FLOAT },
            {"lodlight.medium.range"                                          , rage::joaat("lodlight.medium.range")                                          , 0.0f , V_FLOAT },
            {"lodlight.large.range"                                           , rage::joaat("lodlight.large.range")                                           , 0.0f , V_FLOAT },
            {"lodlight.small.fade"                                            , rage::joaat("lodlight.small.fade")                                            , 0.0f , V_FLOAT },
            {"lodlight.medium.fade"                                           , rage::joaat("lodlight.medium.fade")                                           , 0.0f , V_FLOAT },
            {"lodlight.large.fade"                                            , rage::joaat("lodlight.large.fade")                                            , 0.0f , V_FLOAT },
            {"lodlight.small.corona.range"                                    , rage::joaat("lodlight.small.corona.range")                                    , 0.0f , V_FLOAT },
            {"lodlight.medium.corona.range"                                   , rage::joaat("lodlight.medium.corona.range")                                   , 0.0f , V_FLOAT },
            {"lodlight.large.corona.range"                                    , rage::joaat("lodlight.large.corona.range")                                    , 0.0f , V_FLOAT },
            {"lodlight.corona.size"                                           , rage::joaat("lodlight.corona.size")                                           , 0.0f , V_FLOAT },
            {"lodlight.sphere.expand.size.mult"                               , rage::joaat("lodlight.sphere.expand.size.mult")                               , 0.0f , V_FLOAT },
            {"lodlight.sphere.expand.start"                                   , rage::joaat("lodlight.sphere.expand.start")                                   , 0.0f , V_FLOAT },
            {"lodlight.sphere.expand.end"                                     , rage::joaat("lodlight.sphere.expand.end")                                     , 0.0f , V_FLOAT },
    };
    paramsMap["Bright Lights"] =
    {
            {"misc.brightlight.distculled.FadeStart"                          , rage::joaat("misc.brightlight.distculled.FadeStart")                          , 0.0f , V_FLOAT },
            {"misc.brightlight.distculled.FadeEnd"                            , rage::joaat("misc.brightlight.distculled.FadeEnd")                            , 0.0f , V_FLOAT },
            {"misc.brightlight.notdistculled.FadeStart"                       , rage::joaat("misc.brightlight.notdistculled.FadeStart")                       , 0.0f , V_FLOAT },
            {"misc.brightlight.notdistculled.FadeEnd"                         , rage::joaat("misc.brightlight.notdistculled.FadeEnd")                         , 0.0f , V_FLOAT },
            {"misc.brightlight.globalIntensity"                               , rage::joaat("misc.brightlight.globalIntensity")                               , 0.0f , V_FLOAT },
            {"misc.brightlight.railwayItensity"                               , rage::joaat("misc.brightlight.railwayItensity")                               , 0.0f , V_FLOAT },
            {"misc.brightlight.coronaZBias"                                   , rage::joaat("misc.brightlight.coronaZBias")                                   , 0.0f , V_FLOAT },
    };
    paramsMap["Vehicle Light Night/Day Settings"] =
    {
            {"vehicle.lights.sunrise"                                         , rage::joaat("vehicle.lights.sunrise")                                         , 0.0f , V_FLOAT },
            {"vehicle.lights.sunset"                                          , rage::joaat("vehicle.lights.sunset")                                          , 0.0f , V_FLOAT },
    };
    paramsMap["Vehicle light fade distances"] =
    {
            {"vehicle.lights.fadeDistance.main"                               , rage::joaat("vehicle.lights.fadeDistance.main")                               , 0.0f , V_FLOAT },
            {"vehicle.lights.fadeDistance.secondary"                          , rage::joaat("vehicle.lights.fadeDistance.secondary")                          , 0.0f , V_FLOAT },
            {"vehicle.lights.fadeDistance.sirens"                             , rage::joaat("vehicle.lights.fadeDistance.sirens")                             , 0.0f , V_FLOAT },
            {"vehicle.lights.fadeDistance.AO"                                 , rage::joaat("vehicle.lights.fadeDistance.AO")                                 , 0.0f , V_FLOAT },
            {"vehicle.lights.fadeDistance.neon"                               , rage::joaat("vehicle.lights.fadeDistance.neon")                               , 0.0f , V_FLOAT },
            {"vehicle.lights.fadeLength"                                      , rage::joaat("vehicle.lights.fadeLength")                                      , 0.0f , V_FLOAT },
    };
    paramsMap["Particle Shadow intensity"] =
    {
            {"particles.shadowintensity"                                      , rage::joaat("particles.shadowintensity")                                      , 0.0f , V_FLOAT },
    };
    paramsMap["Dark Light"] =
    {
            {"dark.light.color.red"                                           , rage::joaat("dark.light.color.red")                                           , 0.0f , V_COL3  },
            {"dark.light.color.green"                                         , rage::joaat("dark.light.color.green")                                         , 0.0f , V_NONE  },
            {"dark.light.color.blue"                                          , rage::joaat("dark.light.color.blue")                                          , 0.0f , V_NONE  },
            {"dark.light.intensity"                                           , rage::joaat("dark.light.intensity")                                           , 0.0f , V_FLOAT },
            {"dark.light.radius"                                              , rage::joaat("dark.light.radius")                                              , 0.0f , V_FLOAT },
            {"dark.light.falloffExp"                                          , rage::joaat("dark.light.falloffExp")                                          , 0.0f , V_FLOAT },
            {"dark.light.innerConeAngle"                                      , rage::joaat("dark.light.innerConeAngle")                                      , 0.0f , V_FLOAT },
            {"dark.light.outerConeAngle"                                      , rage::joaat("dark.light.outerConeAngle")                                      , 0.0f , V_FLOAT },
            {"dark.light.coronaHDR"                                           , rage::joaat("dark.light.coronaHDR")                                           , 0.0f , V_FLOAT },
            {"dark.light.coronaSize"                                          , rage::joaat("dark.light.coronaSize")                                          , 0.0f , V_FLOAT },
            {"dark.light.offset.x"                                            , rage::joaat("dark.light.offset.x")                                            , 0.0f , V_FLOAT },
            {"dark.light.offset.y"                                            , rage::joaat("dark.light.offset.y")                                            , 0.0f , V_FLOAT },
            {"dark.light.offset.z"                                            , rage::joaat("dark.light.offset.z")                                            , 0.0f , V_FLOAT },
            {"dark.fpv.light.offset.x"                                        , rage::joaat("dark.fpv.light.offset.x")                                        , 0.0f , V_FLOAT },
            {"dark.fpv.light.offset.y"                                        , rage::joaat("dark.fpv.light.offset.y")                                        , 0.0f , V_FLOAT },
            {"dark.fpv.light.offset.z"                                        , rage::joaat("dark.fpv.light.offset.z")                                        , 0.0f , V_FLOAT },
            {"dark.useSpec"                                                   , rage::joaat("dark.useSpec")                                                   , 0.0f , V_FLOAT },
            {"dark.useShadows"                                                , rage::joaat("dark.useShadows")                                                , 0.0f , V_FLOAT },
            {"dark.useVolumes"                                                , rage::joaat("dark.useVolumes")                                                , 0.0f , V_FLOAT },
            {"dark.shadowNearCLip"                                            , rage::joaat("dark.shadowNearCLip")                                            , 0.0f , V_FLOAT },
            {"dark.shadowBlur"                                                , rage::joaat("dark.shadowBlur")                                                , 0.0f , V_FLOAT },
            {"dark.volumeIntensity"                                           , rage::joaat("dark.volumeIntensity")                                           , 0.0f , V_FLOAT },
            {"dark.volumeSize"                                                , rage::joaat("dark.volumeSize")                                                , 0.0f , V_FLOAT },
            {"dark.volumeExponent"                                            , rage::joaat("dark.volumeExponent")                                            , 0.0f , V_FLOAT },
            {"dark.volumeColor.x"                                             , rage::joaat("dark.volumeColor.x")                                             , 0.0f , V_FLOAT },
            {"dark.volumeColor.y"                                             , rage::joaat("dark.volumeColor.y")                                             , 0.0f , V_FLOAT },
            {"dark.volumeColor.z"                                             , rage::joaat("dark.volumeColor.z")                                             , 0.0f , V_FLOAT },
            {"dark.volumeColor.w"                                             , rage::joaat("dark.volumeColor.w")                                             , 0.0f , V_FLOAT },
            {"dark.lightDirBase"                                              , rage::joaat("dark.lightDirBase")                                              , 0.0f , V_FLOAT },
    };
    paramsMap["Secondary Dark Light"] =
    {
            {"dark.secLight.enable"                                           , rage::joaat("dark.secLight.enable")                                           , 0.0f , V_FLOAT },
            {"dark.secLight.color.red"                                        , rage::joaat("dark.secLight.color.red")                                        , 0.0f , V_COL3  },
            {"dark.secLight.color.green"                                      , rage::joaat("dark.secLight.color.green")                                      , 0.0f , V_NONE  },
            {"dark.secLight.color.blue"                                       , rage::joaat("dark.secLight.color.blue")                                       , 0.0f , V_NONE  },
            {"dark.secLight.intensity"                                        , rage::joaat("dark.secLight.intensity")                                        , 0.0f , V_FLOAT },
            {"dark.secLight.radius"                                           , rage::joaat("dark.secLight.radius")                                           , 0.0f , V_FLOAT },
            {"dark.secLight.falloffExp"                                       , rage::joaat("dark.secLight.falloffExp")                                       , 0.0f , V_FLOAT },
            {"dark.secLight.innerConeAngle"                                   , rage::joaat("dark.secLight.innerConeAngle")                                   , 0.0f , V_FLOAT },
            {"dark.secLight.outerConeAngle"                                   , rage::joaat("dark.secLight.outerConeAngle")                                   , 0.0f , V_FLOAT },
            {"dark.secLight.coronaHDR"                                        , rage::joaat("dark.secLight.coronaHDR")                                        , 0.0f , V_FLOAT },
            {"dark.secLight.coronaSize"                                       , rage::joaat("dark.secLight.coronaSize")                                       , 0.0f , V_FLOAT },
            {"dark.secLight.offset.x"                                         , rage::joaat("dark.secLight.offset.x")                                         , 0.0f , V_FLOAT },
            {"dark.secLight.offset.y"                                         , rage::joaat("dark.secLight.offset.y")                                         , 0.0f , V_FLOAT },
            {"dark.secLight.offset.z"                                         , rage::joaat("dark.secLight.offset.z")                                         , 0.0f , V_FLOAT },
            {"dark.secLight.useSpec"                                          , rage::joaat("dark.secLight.useSpec")                                          , 0.0f , V_FLOAT },
            {"dark.secLight.lightDirBase"                                     , rage::joaat("dark.secLight.lightDirBase")                                     , 0.0f , V_FLOAT },
    };
    paramsMap["Ped light"] =
    {
            {"ped.light.direction.x"                                          , rage::joaat("ped.light.direction.x")                                          , 0.0f , V_FLOAT },
            {"ped.light.direction.y"                                          , rage::joaat("ped.light.direction.y")                                          , 0.0f , V_FLOAT },
            {"ped.light.direction.z"                                          , rage::joaat("ped.light.direction.z")                                          , 0.0f , V_FLOAT },
            {"ped.light.fade.day.start"                                       , rage::joaat("ped.light.fade.day.start")                                       , 0.0f , V_FLOAT },
            {"ped.light.fade.day.end"                                         , rage::joaat("ped.light.fade.day.end")                                         , 0.0f , V_FLOAT },
            {"ped.light.fade.night.start"                                     , rage::joaat("ped.light.fade.night.start")                                     , 0.0f , V_FLOAT },
            {"ped.light.fade.night.end"                                       , rage::joaat("ped.light.fade.night.end")                                       , 0.0f , V_FLOAT },
    };
    paramsMap["ped phone light properties"] =
    {
            {"ped.phonelight.color.r"                                         , rage::joaat("ped.phonelight.color.r")                                         , 0.0f , V_COL3  },
            {"ped.phonelight.color.g"                                         , rage::joaat("ped.phonelight.color.g")                                         , 0.0f , V_NONE  },
            {"ped.phonelight.color.b"                                         , rage::joaat("ped.phonelight.color.b")                                         , 0.0f , V_NONE  },
            {"ped.phonelight.intensity"                                       , rage::joaat("ped.phonelight.intensity")                                       , 0.0f , V_FLOAT },
            {"ped.phonelight.radius"                                          , rage::joaat("ped.phonelight.radius")                                          , 0.0f , V_FLOAT },
            {"ped.phonelight.falloffexp"                                      , rage::joaat("ped.phonelight.falloffexp")                                      , 0.0f , V_FLOAT },
            {"ped.phonelight.cone.inner"                                      , rage::joaat("ped.phonelight.cone.inner")                                      , 0.0f , V_FLOAT },
            {"ped.phonelight.cone.outer"                                      , rage::joaat("ped.phonelight.cone.outer")                                      , 0.0f , V_FLOAT },
    };
    paramsMap["Adaptive DOF - PC/NG Only"] =
    {
            {"adaptivedof.range.verynear"                                     , rage::joaat("adaptivedof.range.verynear")                                     , 0.0f , V_FLOAT },
            {"adaptivedof.range.near"                                         , rage::joaat("adaptivedof.range.near")                                         , 0.0f , V_FLOAT },
            {"adaptivedof.range.mid"                                          , rage::joaat("adaptivedof.range.mid")                                          , 0.0f , V_FLOAT },
            {"adaptivedof.range.far"                                          , rage::joaat("adaptivedof.range.far")                                          , 0.0f , V_FLOAT },
            {"adaptivedof.range.veryfar"                                      , rage::joaat("adaptivedof.range.veryfar")                                      , 0.0f , V_FLOAT },
            {"adaptivedof.day.verynear.nearout"                               , rage::joaat("adaptivedof.day.verynear.nearout")                               , 0.0f , V_FLOAT },
            {"adaptivedof.day.verynear.nearin"                                , rage::joaat("adaptivedof.day.verynear.nearin")                                , 0.0f , V_FLOAT },
            {"adaptivedof.day.verynear.farin"                                 , rage::joaat("adaptivedof.day.verynear.farin")                                 , 0.0f , V_FLOAT },
            {"adaptivedof.day.verynear.farout"                                , rage::joaat("adaptivedof.day.verynear.farout")                                , 0.0f , V_FLOAT },
            {"adaptivedof.day.near.nearout"                                   , rage::joaat("adaptivedof.day.near.nearout")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.day.near.nearin"                                    , rage::joaat("adaptivedof.day.near.nearin")                                    , 0.0f , V_FLOAT },
            {"adaptivedof.day.near.farin"                                     , rage::joaat("adaptivedof.day.near.farin")                                     , 0.0f , V_FLOAT },
            {"adaptivedof.day.near.farout"                                    , rage::joaat("adaptivedof.day.near.farout")                                    , 0.0f , V_FLOAT },
            {"adaptivedof.day.mid.nearout"                                    , rage::joaat("adaptivedof.day.mid.nearout")                                    , 0.0f , V_FLOAT },
            {"adaptivedof.day.mid.nearin"                                     , rage::joaat("adaptivedof.day.mid.nearin")                                     , 0.0f , V_FLOAT },
            {"adaptivedof.day.mid.farin"                                      , rage::joaat("adaptivedof.day.mid.farin")                                      , 0.0f , V_FLOAT },
            {"adaptivedof.day.mid.farout"                                     , rage::joaat("adaptivedof.day.mid.farout")                                     , 0.0f , V_FLOAT },
            {"adaptivedof.day.far.nearout"                                    , rage::joaat("adaptivedof.day.far.nearout")                                    , 0.0f , V_FLOAT },
            {"adaptivedof.day.far.nearin"                                     , rage::joaat("adaptivedof.day.far.nearin")                                     , 0.0f , V_FLOAT },
            {"adaptivedof.day.far.farin"                                      , rage::joaat("adaptivedof.day.far.farin")                                      , 0.0f , V_FLOAT },
            {"adaptivedof.day.far.farout"                                     , rage::joaat("adaptivedof.day.far.farout")                                     , 0.0f , V_FLOAT },
            {"adaptivedof.day.veryfar.nearout"                                , rage::joaat("adaptivedof.day.veryfar.nearout")                                , 0.0f , V_FLOAT },
            {"adaptivedof.day.veryfar.nearin"                                 , rage::joaat("adaptivedof.day.veryfar.nearin")                                 , 0.0f , V_FLOAT },
            {"adaptivedof.day.veryfar.farin"                                  , rage::joaat("adaptivedof.day.veryfar.farin")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.day.veryfar.farout"                                 , rage::joaat("adaptivedof.day.veryfar.farout")                                 , 0.0f , V_FLOAT },
            {"adaptivedof.night.verynear.nearout"                             , rage::joaat("adaptivedof.night.verynear.nearout")                             , 0.0f , V_FLOAT },
            {"adaptivedof.night.verynear.nearin"                              , rage::joaat("adaptivedof.night.verynear.nearin")                              , 0.0f , V_FLOAT },
            {"adaptivedof.night.verynear.farin"                               , rage::joaat("adaptivedof.night.verynear.farin")                               , 0.0f , V_FLOAT },
            {"adaptivedof.night.verynear.farout"                              , rage::joaat("adaptivedof.night.verynear.farout")                              , 0.0f , V_FLOAT },
            {"adaptivedof.night.near.nearout"                                 , rage::joaat("adaptivedof.night.near.nearout")                                 , 0.0f , V_FLOAT },
            {"adaptivedof.night.near.nearin"                                  , rage::joaat("adaptivedof.night.near.nearin")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.night.near.farin"                                   , rage::joaat("adaptivedof.night.near.farin")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.night.near.farout"                                  , rage::joaat("adaptivedof.night.near.farout")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.night.mid.nearout"                                  , rage::joaat("adaptivedof.night.mid.nearout")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.night.mid.nearin"                                   , rage::joaat("adaptivedof.night.mid.nearin")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.night.mid.farin"                                    , rage::joaat("adaptivedof.night.mid.farin")                                    , 0.0f , V_FLOAT },
            {"adaptivedof.night.mid.farout"                                   , rage::joaat("adaptivedof.night.mid.farout")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.night.far.nearout"                                  , rage::joaat("adaptivedof.night.far.nearout")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.night.far.nearin"                                   , rage::joaat("adaptivedof.night.far.nearin")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.night.far.farin"                                    , rage::joaat("adaptivedof.night.far.farin")                                    , 0.0f , V_FLOAT },
            {"adaptivedof.night.far.farout"                                   , rage::joaat("adaptivedof.night.far.farout")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.night.veryfar.nearout"                              , rage::joaat("adaptivedof.night.veryfar.nearout")                              , 0.0f , V_FLOAT },
            {"adaptivedof.night.veryfar.nearin"                               , rage::joaat("adaptivedof.night.veryfar.nearin")                               , 0.0f , V_FLOAT },
            {"adaptivedof.night.veryfar.farin"                                , rage::joaat("adaptivedof.night.veryfar.farin")                                , 0.0f , V_FLOAT },
            {"adaptivedof.night.veryfar.farout"                               , rage::joaat("adaptivedof.night.veryfar.farout")                               , 0.0f , V_FLOAT },
            {"adaptivedof.missedrayvalue"                                     , rage::joaat("adaptivedof.missedrayvalue")                                     , 0.0f , V_FLOAT },
            {"adaptivedof.gridsize"                                           , rage::joaat("adaptivedof.gridsize")                                           , 0.0f , V_FLOAT },
            {"adaptivedof.gridscalex"                                         , rage::joaat("adaptivedof.gridscalex")                                         , 0.0f , V_FLOAT },
            {"adaptivedof.gridscaley"                                         , rage::joaat("adaptivedof.gridscaley")                                         , 0.0f , V_FLOAT },
            {"adaptivedof.gridscalexincover"                                  , rage::joaat("adaptivedof.gridscalexincover")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.gridscaleyincover"                                  , rage::joaat("adaptivedof.gridscaleyincover")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.gridscalexaiming"                                   , rage::joaat("adaptivedof.gridscalexaiming")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.gridscaleyaiming"                                   , rage::joaat("adaptivedof.gridscaleyaiming")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.gridscalexrungun"                                   , rage::joaat("adaptivedof.gridscalexrungun")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.gridscaleyrungun"                                   , rage::joaat("adaptivedof.gridscaleyrungun")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.smoothtime"                                         , rage::joaat("adaptivedof.smoothtime")                                         , 0.0f , V_FLOAT },
            {"adaptivedof.nearplayeradjust"                                   , rage::joaat("adaptivedof.nearplayeradjust")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.meleefocal.nearout"                                 , rage::joaat("adaptivedof.meleefocal.nearout")                                 , 0.0f , V_FLOAT },
            {"adaptivedof.meleefocal.nearin"                                  , rage::joaat("adaptivedof.meleefocal.nearin")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.meleefocal.farin"                                   , rage::joaat("adaptivedof.meleefocal.farin")                                   , 0.0f , V_FLOAT },
            {"adaptivedof.meleefocal.farout"                                  , rage::joaat("adaptivedof.meleefocal.farout")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.aimingfocal.nearout"                                , rage::joaat("adaptivedof.aimingfocal.nearout")                                , 0.0f , V_FLOAT },
            {"adaptivedof.aimingfocal.nearin"                                 , rage::joaat("adaptivedof.aimingfocal.nearin")                                 , 0.0f , V_FLOAT },
            {"adaptivedof.aimingfocal.farin"                                  , rage::joaat("adaptivedof.aimingfocal.farin")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.aimingfocal.farout"                                 , rage::joaat("adaptivedof.aimingfocal.farout")                                 , 0.0f , V_FLOAT },
            {"adaptivedof.rungunfocal.nearout"                                , rage::joaat("adaptivedof.rungunfocal.nearout")                                , 0.0f , V_FLOAT },
            {"adaptivedof.rungunfocal.nearin"                                 , rage::joaat("adaptivedof.rungunfocal.nearin")                                 , 0.0f , V_FLOAT },
            {"adaptivedof.rungunfocal.farin"                                  , rage::joaat("adaptivedof.rungunfocal.farin")                                  , 0.0f , V_FLOAT },
            {"adaptivedof.rungunfocal.farout"                                 , rage::joaat("adaptivedof.rungunfocal.farout")                                 , 0.0f , V_FLOAT },
            {"adaptivedof.incoverfocal.nearout"                               , rage::joaat("adaptivedof.incoverfocal.nearout")                               , 0.0f , V_FLOAT },
            {"adaptivedof.incoverfocal.nearin"                                , rage::joaat("adaptivedof.incoverfocal.nearin")                                , 0.0f , V_FLOAT },
            {"adaptivedof.incoverfocal.farin"                                 , rage::joaat("adaptivedof.incoverfocal.farin")                                 , 0.0f , V_FLOAT },
            {"adaptivedof.incoverfocal.farout"                                , rage::joaat("adaptivedof.incoverfocal.farout")                                , 0.0f , V_FLOAT },
            {"adaptivedof.exposureDay"                                        , rage::joaat("adaptivedof.exposureDay")                                        , 0.0f , V_FLOAT },
            {"adaptivedof.exposureNight"                                      , rage::joaat("adaptivedof.exposureNight")                                      , 0.0f , V_FLOAT },
            {"adaptivedof.exposureMin"                                        , rage::joaat("adaptivedof.exposureMin")                                        , 0.0f , V_FLOAT },
            {"adaptivedof.exposureMax"                                        , rage::joaat("adaptivedof.exposureMax")                                        , 0.0f , V_FLOAT },
            {"bokeh.brightnessThresholdMin"                                   , rage::joaat("bokeh.brightnessThresholdMin")                                   , 0.0f , V_FLOAT },
            {"bokeh.brightnessThresholdMax"                                   , rage::joaat("bokeh.brightnessThresholdMax")                                   , 0.0f , V_FLOAT },
            {"bokeh.brightnessExposureMin"                                    , rage::joaat("bokeh.brightnessExposureMin")                                    , 0.0f , V_FLOAT },
            {"bokeh.brightnessExposureMax"                                    , rage::joaat("bokeh.brightnessExposureMax")                                    , 0.0f , V_FLOAT },
            {"bokeh.cocThreshold"                                             , rage::joaat("bokeh.cocThreshold")                                             , 0.0f , V_FLOAT },
            {"bokeh.fadeRangeMin"                                             , rage::joaat("bokeh.fadeRangeMin")                                             , 0.0f , V_FLOAT },
            {"bokeh.fadeRangeMax"                                             , rage::joaat("bokeh.fadeRangeMax")                                             , 0.0f , V_FLOAT },
            {"bokeh.sizeMultiplier"                                           , rage::joaat("bokeh.sizeMultiplier")                                           , 0.0f , V_FLOAT },
            {"bokeh.globalAlpha"                                              , rage::joaat("bokeh.globalAlpha")                                              , 0.0f , V_FLOAT },
            {"bokeh.alphaCutoff"                                              , rage::joaat("bokeh.alphaCutoff")                                              , 0.0f , V_FLOAT },
            {"bokeh.shapeExposureRangeMin"                                    , rage::joaat("bokeh.shapeExposureRangeMin")                                    , 0.0f , V_FLOAT },
            {"bokeh.shapeExposureRangeMax"                                    , rage::joaat("bokeh.shapeExposureRangeMax")                                    , 0.0f , V_FLOAT },
            {"defaultmotionblur.enabled"                                      , rage::joaat("defaultmotionblur.enabled")                                      , 0.0f , V_FLOAT },
            {"defaultmotionblur.strength"                                     , rage::joaat("defaultmotionblur.strength")                                     , 0.0f , V_FLOAT },
            {"procObjects.minRadiusForHeightMap"                              , rage::joaat("procObjects.minRadiusForHeightMap")                              , 0.0f , V_FLOAT },
    };
    paramsMap["Multipiers over final shadow cascade."] =
    {
            {"fogray.fadestart"                                               , rage::joaat("fogray.fadestart")                                               , 0.0f , V_FLOAT },
            {"fogray.fadeend"                                                 , rage::joaat("fogray.fadeend")                                                 , 0.0f , V_FLOAT },
    };
    paramsMap["character lens flare"] =
    {
            {"lensflare.switch.length"                                        , rage::joaat("lensflare.switch.length")                                        , 0.0f , V_FLOAT },
            {"lensflare.hide.length"                                          , rage::joaat("lensflare.hide.length")                                          , 0.0f , V_FLOAT },
            {"lensflare.show.length"                                          , rage::joaat("lensflare.show.length")                                          , 0.0f , V_FLOAT },
            {"lensflare.desaturation.boost"                                   , rage::joaat("lensflare.desaturation.boost")                                   , 0.0f , V_FLOAT },
    };
    paramsMap["First person motion blur"] =
    {
            {"fpv.motionblur.weight0"                                         , rage::joaat("fpv.motionblur.weight0")                                         , 0.0f , V_FLOAT },
            {"fpv.motionblur.weight1"                                         , rage::joaat("fpv.motionblur.weight1")                                         , 0.0f , V_FLOAT },
            {"fpv.motionblur.weight2"                                         , rage::joaat("fpv.motionblur.weight2")                                         , 0.0f , V_FLOAT },
            {"fpv.motionblur.weight3"                                         , rage::joaat("fpv.motionblur.weight3")                                         , 0.0f , V_FLOAT },
            {"fpv.motionblur.max.velocity"                                    , rage::joaat("fpv.motionblur.max.velocity")                                    , 0.0f , V_FLOAT },
    };


    if (!handler->p_Vsettings) {
        mlogger("something went wrong; p_Vsettings == nullptr");
        return;
    }

    for (auto category : categoriesOrder)
    {
        if (paramsMap.contains(category))
        {
            auto& items = paramsMap.at(category);
            for (auto& item : items)
            {
                for (size_t i = 0; i < handler->p_Vsettings->data.GetSize(); i++)
                {
                    auto& gItem = handler->p_Vsettings->data[i];
                    if (item.hash == gItem.hash)
                    {
                        //item.value = gItem.value;
                        item.gPtrItem = &gItem;
                        item.found = true;
                        break;
                    }
                }
            }
        } else {
            mlogger(std::format(" coudn't find {} category", category));
        }
    }

    for (size_t i = 0; i < handler->p_Vsettings->data.GetSize(); i++)
    {
        mlogger(std::format("i = {}\t\t| hash : {} \t\t| data : {}", i, handler->p_Vsettings->data[i].hash, handler->p_Vsettings->data[i].value));
    }


    for (auto& vec : paramsMap)
    {
        for (auto& item : vec.second)
        {
            if (!item.found)
            {
                mlogger("========================================");
                mlogger(std::format("could not find {} hash = {} in \t {}", item.name, item.hash, vec.first));
                mlogger("========================================");
            }
        }
    }
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
//    def has_digits(input_string):
//        return any(char.isdigit() for char in input_string)
//
//    vfloat = "V_FLOAT"
//    vcol3 = "V_COL3"
//    vnone = "V_NONE"
//
//    tType = ""
//
//    arr1 = []
//    with open("E:\\GTAV\\timecycles\\visualsettings.dat", "r") as f:
//        arr1.extend(f)
//        
//    arr1 = [item.strip() for item in arr1 if item.strip()]
//    
//    for i, line in enumerate(arr1):
//        if "//" in line or "#" in line:
//            if (has_digits(line)):
//                #print(f"=============================   {line}")
//                continue
//            if "#" in line:  line = line[1:]
//            else:            line = line[2:]
//           
//            print("};\n" + "paramsMap" + f'["{line.strip()}"] = \n' + "{")
//       
//        else:
//            if (has_digits(line)):
//                line = line.split()[0]
//
//                if line.endswith(".red") and arr1[i + 1].split()[0].endswith(".green") and arr1[i + 2].split()[0].endswith(".blue"):
//                    tType = vcol3
//                elif line.endswith(".green") or line.endswith(".blue") or line.endswith(".alpha"):
//                    tType = vnone
//                else:
//                    tType = vfloat
//
//                print("\t{" + f'"{line}"'.ljust(65) + ", 0.0f".ljust(15)+ ", " + tType.ljust(10) + f', rage::joaat("{line}")'.ljust(65) + "},")
//                


