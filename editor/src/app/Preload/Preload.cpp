#include "Preload.h"
#include "logger.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           TcParameter Names replacement
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline bool TcNamesReplacemantHandler::isLoaded() { return loaded; }

bool TcNamesReplacemantHandler::load_names_replacement(const char* filename)
{
    if (!load_from_file(filename))
    {
        save_hardcoded_params_to_file(filename);
        load_from_file(filename);
    }
    return isLoaded();
}


bool TcNamesReplacemantHandler::load_from_file(const char* filename)
{
    params_map.clear();
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open())
        return (loaded = false);

    while (std::getline(file, line))
    {
        size_t colon_pos = line.find(':');

        if (colon_pos != -1)
            params_map.insert({ strip_str(line.substr(0, colon_pos)), strip_str(line.substr(colon_pos + 1, line.size() - colon_pos)) });
    }

    for (auto& [v1, v2] : params_map) {
        size_t count = 0;
        for (auto& [v11, v22] : params_map) {
            if (v2 == v22) {
                count += 1;
                if (count > 1)
                    v22 += ' ';
            }
        }
    }

    std::string var_name;
    var_name.reserve(50);

    for (auto& var : g_varInfos)
    {
        if (var.varType == VARTYPE_NONE)
            continue;

        var_name = var.name;

        if (var_name.ends_with("_r"))
            var_name.resize(var_name.size() - 2);

        auto it = params_map.find(var_name);

        if (it != params_map.end() && !it->second.empty())
            var.menuName = it->second.c_str();
    }

    return (loaded = true);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           TcCategories
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool TcCategoriesHandler::isLoaded() { return loaded; }

bool TcCategoriesHandler::load_categories(const char* filename)
{
    if (!load_from_file(filename))
    {
        save_hardcoded_categories_to_file(filename);
        load_from_file(filename);
    }
    return isLoaded();
}


bool TcCategoriesHandler::load_from_file(const char* filename)
{
    CategoriesOrder.clear();
    CategoriesMap.clear();

    std::ifstream input(filename);

    if (!input.is_open())
        return (loaded = false);

    std::string line;
    std::vector<std::string> raw_lines;
    raw_lines.reserve(1000);
    line.reserve(100);

    while (std::getline(input, line))
    {
        line = strip_str(line);
        if (line.empty())
            continue;
        
        raw_lines.push_back(std::move(line));
    }
    RemoveDuplicatesInVector(raw_lines);

    std::string var_name;
    int category_idx = -1;
    size_t varId;
    for (auto& line : raw_lines)
    {
        if (line[0] == '@')
        {
            CategoriesOrder.push_back(line.substr(1, line.size()));
            category_idx++;
            continue;
        }

        if (std::find_if(g_varInfos, g_varInfos + std::size(g_varInfos), [&var_name, &line, &varId](tcVarInfo& var) -> bool
            {
                var_name = var.name;

                if (var_name.ends_with("_r"))
                    var_name.resize(var_name.size() - 2);

                if (var_name == line)
                {
                    varId = var.varId;
                    var.InCategory = true;
                    return true;
                }
                return false;
            }) != g_varInfos + std::size(g_varInfos))
        {
            if (category_idx != -1)
            {
                CategoriesMap[CategoriesOrder[category_idx]].push_back({ line ,varId });
            }
            else
            {
                CategoriesMap["None"].push_back({ line , varId });
            }
        }
    }

    for (auto& var : g_varInfos)
    {
        if (var.varType == VARTYPE_NONE || var.InCategory)
            continue;

        varId = var.varId;
        var_name = var.name;

        if (var_name.ends_with("_r"))
            var_name.resize(var_name.size() - 2);

        CategoriesMap["None"].push_back({ var_name , varId });
    }

    if (CategoriesMap.contains("None"))
        CategoriesOrder.push_back("None");

    return (loaded = true);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           Config
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void ConfigWrapper::readCfg(const char* filename)
{
    reader.ParseFile(filename);

    if (reader.ParseError() < 0)
    {
        writeCfg(filename);
        reader.ParseFile(filename);
    }
}

void ConfigWrapper::writeCfg(const char* filename)
{
    std::ofstream cfg(filename, std::ios::trunc);

    if (!cfg.is_open())
        return;

    cfg << "// All key codes can be found here: https://msdn.microsoft.com/library/windows/desktop/dd375731.aspx\n\n";
    cfg << "[Settings]" << "\n\n";

    cfg << "Default_path             " << " = " << "E:\\GTAV\\timecycles"                       << '\n';
    cfg << "Categories               " << " = " << "1"                                          << '\n';
    cfg << "Names_replacement        " << " = " << "1"                                          << '\n';
    cfg << "OpenClose_window_button  " << " = " << "0x2D"                                       << "\n";
    cfg << "Font_size                " << " = " << "15"                                         << '\n';
    cfg << "Bloodfx_reload_path      " << " = " << "E:\\bloodfx.dat"                            << '\n';

    cfg << "\n// In case if there's something wrong with the system cursor - set this to 1 "    << '\n';
    cfg << "CursorImgui_Impl         " << " = " << "0"                                          << '\n';

    cfg.close();
}

INIReader* ConfigWrapper::getCfgReader()
{
    return &reader;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           Preload
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Preload* Preload::self = nullptr;

Preload& Preload::Create()
{
    if (!self)
        self = new Preload();
    
    return *self;
}

void Preload::Destroy()
{
    if (self) {
        delete self;
        self = nullptr;
    }
}

Preload* Preload::Get()
{
    return self;
}


void Preload::Init()
{
    if (preload_done)
        return;
   
    CfgWrapper.readCfg(fileNames::config_file);
    auto* cfg = CfgWrapper.getCfgReader();

    if (cfg->GetBoolean("Settings", "Names_replacement", true)) 
        if (!(TcNames.load_names_replacement(fileNames::tcNames_repl_file)))
            cfg->ChangeValueIfExists("Settings", "Names_replacement", false);
    
    if (cfg->GetBoolean("Settings", "Categories", true))
        if (!(TcCategories.load_categories(fileNames::tcCategories_file))) 
            cfg->ChangeValueIfExists("Settings", "Categories", false);

    preload_done = true;
}

INIReader*              Preload::getConfigParser()          { return CfgWrapper.getCfgReader(); }
TcCategoriesHandler*    Preload::getTcCategoriesHandler()   { return &TcCategories; }



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           hardcoded data
// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void TcNamesReplacemantHandler::save_hardcoded_params_to_file(const char* filename)
{
    std::string text;
    text += R"( light_dir_col                                     : Directional Colour
 light_dir_mult                                    : Directional Intensity
 light_directional_amb_col                         : Directional Ambient - Colour
 light_directional_amb_intensity                   : Directional Ambient - Intensity
 light_directional_amb_intensity_mult              : Directional Ambient - Intensity Mult
 light_directional_amb_bounce_enabled              : Directional Ambient - Enable Bounce
 light_amb_down_wrap                               : Ambient - Down Wrap
 light_natural_amb_down_col                        : Natural Ambient - Down Colour
 light_natural_amb_down_intensity                  : Natural Ambient - Down Intensity
 light_natural_amb_up_col                          : Natural Ambient - Base Colour
 light_natural_amb_up_intensity                    : Natural Ambient - Base Intensity
 light_natural_amb_up_intensity_mult               : Natural Ambient - Base Intensity Mult
 light_natural_push                                : Natural Ambient Push
 light_ambient_bake_ramp                           : Natural/Artificial Ambient Ramp
 light_artificial_int_down_col                     : Artificial Interior Ambient - Down Colour
 light_artificial_int_down_intensity               : Artificial Interior Ambient - Down Intensity
 light_artificial_int_up_col                       : Artificial Interior Ambient - Base Colour
 light_artificial_int_up_intensity                 : Artificial Interior Ambient - Base Intensity
 light_artificial_ext_down_col                     : Artificial Exterior Ambient - Down Colour
 light_artificial_ext_down_intensity               : Artificial Exterior Ambient - Down Intensity
 light_artificial_ext_up_col                       : Artificial Exterior Ambient - Base Colour
 light_artificial_ext_up_intensity                 : Artificial Exterior Ambient - Base Intensity
 ped_light_col                                     : Ped Light Colour
 ped_light_mult                                    : Ped Light Intensity
 ped_light_direction_x                             : Ped Light Direction X
 ped_light_direction_y                             : Ped Light Direction Y
 ped_light_direction_z                             : Ped Light Direction Z
 light_amb_occ_mult                                : Ambient Occlusion Multiplier
 light_amb_occ_mult_ped                            : Ped AO Multiplier
 light_amb_occ_mult_veh                            : Vehicle AO Multiplier
 light_amb_occ_mult_prop                           : Prop AO Multiplier
 light_amb_volumes_in_diffuse                      : Ambient volumes in diffuse
 ssao_inten                                        : SSAO Intensity
 ssao_type                                         : SSAO Type
 ssao_cp_strength                                  : SSAO cp strength
 ssao_qs_strength                                  : SSAO qs strength
 light_ped_rim_mult                                : Ped Rim Multiplier
 light_dynamic_bake_tweak                          : Dynamic Bake Tweak
 light_vehicle_second_spec_override                : Vehicle 2nd Spec override
 light_vehicle_intenity_scale                      : Vehicle Intensity Scale
 light_direction_override                          : Light Position Override
 light_direction_override_overrides_sun            : Light Position Override Override Sun
 sun_direction_x                                   : Sun Position X
 sun_direction_y                                   : Sun Position Y
 sun_direction_z                                   : Sun Position Z
 moon_direction_x                                  : Moon Position X
 moon_direction_y                                  : Moon Position Y
 moon_direction_z                                  : Moon Position Z
 light_ray_col                                     : Lightrays Colour
 light_ray_mult                                    : Lightrays Intensity
 light_ray_underwater_mult                         : Underwater Lightrays Intensity
 light_ray_dist                                    : Lightrays Distance
 light_ray_heightfalloff                           : Lightrays HeightFallOff
 light_ray_height_falloff_start                    : Lightrays Height FallOffStart
 light_ray_add_reducer                             : Lightray Addative Reducer
 light_ray_blit_size                               : Lightray Blit Size
 light_ray_length                                  : Lightray Ray Length
 postfx_exposure                                   : Exposure Tweak
 postfx_exposure_min                               : Exposure Min
 postfx_exposure_max                               : Exposure Max
 postfx_bright_pass_thresh_width                   : Bright Pass Threshold (Width)
 postfx_bright_pass_thresh                         : Bright Pass Threshold (Max)
 postfx_intensity_bloom                            : Intensity Bloom
 postfx_correct_col                                : Color Correction
 postfx_correct_cutoff                             : Color Correction Upper Cutoff
 postfx_shift_col                                  : Color Shift
 postfx_shift_cutoff                               : Color Shift Upper Cutoff
 postfx_desaturation                               : Desaturation
 postfx_noise                                      : Noise
 postfx_noise_size                                 : Noise Size
 postfx_tonemap_filmic_override_dark               : Override dark settings
 postfx_tonemap_filmic_exposure_dark               : Dark Filmic Tonemap Exposure
 postfx_tonemap_filmic_a                           : Dark Filmic Tonemap A
 postfx_tonemap_filmic_c                           : Dark Filmic Tonemap C
 postfx_tonemap_filmic_d                           : Dark Filmic Tonemap D
 postfx_tonemap_filmic_e                           : Dark Filmic Tonemap E
 postfx_tonemap_filmic_f                           : Dark Filmic Tonemap F
 postfx_tonemap_filmic_w                           : Dark Filmic Tonemap W
 postfx_tonemap_filmic_override_bright             : Override bright settings
 postfx_tonemap_filmic_exposure_bright             : Bright Filmic Tonemap Exposure
 postfx_tonemap_filmic_a_bright                    : Bright Filmic Tonemap A
 postfx_tonemap_filmic_b_bright                    : Bright Filmic Tonemap B
 postfx_tonemap_filmic_c_bright                    : Bright Filmic Tonemap C
 postfx_tonemap_filmic_d_bright                    : Bright Filmic Tonemap D
 postfx_tonemap_filmic_e_bright                    : Bright Filmic Tonemap E
 postfx_tonemap_filmic_f_bright                    : Bright Filmic Tonemap F
 postfx_tonemap_filmic_w_bright                    : Bright Filmic Tonemap W
 postfx_vignetting_intensity                       : Vignetting Intensity
 postfx_vignetting_radius                          : Vignetting Radius
 postfx_vignetting_contrast                        : Vignetting Contrast
 postfx_vignetting_col                             : Vignetting Colour
 postfx_grad_top_col                               : Gradient Top Colour
 postfx_grad_middle_col                            : Gradient Middle Colour
 postfx_grad_bottom_col                            : Gradient Bottom Colour
 postfx_grad_midpoint                              : Gradient Midpoint
 postfx_grad_top_middle_midpoint                   : Gradient Top-Middle Midpoint
 postfx_grad_middle_bottom_midpoint                : Gradient Middle-Bottom Midpoint
 postfx_scanlineintensity                          : Scanline Intensity
 postfx_scanline_frequency_0                       : Scanline Frequency 0
 postfx_scanline_frequency_1                       : Scanline Frequency 1
 postfx_scanline_speed                             : Scanline Speed
 postfx_motionblurlength                           : Motion Blur Length
 dof_far                                           : DOF Far
 dof_blur_mid                                      : DOF Mid Blur
 dof_blur_far                                      : DOF Far Blur
 dof_enable_hq                                     : Enable HQ DOF
 dof_hq_smallblur                                  : HQ DOF uses Small Blur
 dof_hq_shallowdof                                 : HQ DOF uses Shallow DOF
 dof_hq_nearplane_out                              : Near start of focus plane
 dof_hq_nearplane_in                               : Near end focus plane
 dof_hq_farplane_out                               : Far start of focus plane
 dof_hq_farplane_in                                : Near end focus plane 
 environmental_blur_in                             : Environmental Blur In
 environmental_blur_out                            : Environmental Blur Out
 environmental_blur_size                           : Environmental Blur Size
 bokeh_brightness_min                              : Bokeh Brightness Threshold Min
 bokeh_brightness_max                              : Bokeh Brightness Threshold Max
 bokeh_fade_min                                    : Bokeh Brightness Threshold Fade Min
 bokeh_fade_max                                    : Bokeh Brightness Threshold Fade Max
 nv_light_dir_mult                                 : Directional Multiplier
 nv_light_amb_down_mult                            : Ambient Down Multiplier
 nv_light_amb_up_mult                              : Ambient Base Multiplier
 nv_lowLum                                         : Low Lum
 nv_highLum                                        : High Lum
 nv_topLum                                         : Top Lum
 nv_scalerLum                                      : Scaler Lum
 nv_offsetLum                                      : Offset
 nv_offsetLowLum                                   : Offset Low Lum
 nv_offsetHighLum                                  : Offset High Lum
 nv_noiseLum                                       : Noise 
 nv_noiseLowLum                                    : Noise Low Lum
 nv_noiseHighLum                                   : Noise High Lum
 nv_bloomLum                                       : Bloom
 nv_colorLum                                       : Colour
 nv_colorLowLum                                    : Low Lum Colour
 nv_colorHighLum                                   : High Lum Colour
 hh_startRange                                     : Start range
 hh_farRange                                       : Far range
 hh_minIntensity                                   : Min Intensity
 hh_maxIntensity                                   : Max Intensity
 hh_displacementU                                  : Displacement Scale U
 hh_displacementV                                  : Displacement Scale V
 hh_tex1UScale                                     : Tex1 U scale
 hh_tex1VScale                                     : Tex1 V scale
 hh_tex1UOffset                                    : Tex1 U offset
 hh_tex1VOffset                                    : Tex1 V offset
 hh_tex2UScale                                     : Tex2 U scale
 hh_tex2VScale                                     : Tex2 V scale
 hh_tex2UOffset                                    : Tex2 U offset
 hh_tex2VOffset                                    : Tex2 V offset
 hh_tex1UFrequencyOffset                           : Tex1 U Frequency offset
 hh_tex1UFrequency                                 : Tex1 U Frequency
 hh_tex1UAmplitude                                 : Tex1 U Amplitude
 hh_tex1VScrollingSpeed                            : Tex1 V Scrolling speed
 hh_tex2UFrequencyOffset                           : Tex2 U Frequency offset
 hh_tex2UFrequency                                 : Tex2 U Frequency
 hh_tex2UAmplitude                                 : Tex2 U Amplitude
 hh_tex2VScrollingSpeed                            : Tex2 V Scrolling speed
 lens_dist_coeff                                   : Distortion Coefficient 1
 lens_dist_cube_coeff                              : Distortion Coefficient 2
 chrom_aberration_coeff                            : Chrom Aberration Coefficient 1
 chrom_aberration_coeff2                           : Chrom Aberration Coefficient 2
 lens_artefacts_intensity                          : Lens Artefacts Global Multiplier
 lens_artefacts_min_exp_intensity                  : Lens Artefacts Min Exposure Fade Mult
 lens_artefacts_max_exp_intensity                  : Lens Artefacts Max Exposure Fade Mult
 blur_vignetting_radius                            : Radius
 blur_vignetting_intensity                         : Intensity
 screen_blur_intensity                             : Intensity 
 sky_zenith_transition_position                    : Zenith Transition (Vertical)
 sky_zenith_transition_east_blend                  : Zenith Transition East Blend
 sky_zenith_transition_west_blend                  : Zenith Transition West Blend
 sky_zenith_blend_start                            : Zenith Blend Start
 sky_zenith_col                                    : Zenith Color
 sky_zenith_col_inten                              : Zenith Color Intensity
 sky_zenith_transition_col                         : Zenith Transition Color
 sky_zenith_transition_col_inten                   : Zenith Transition Intensity
 sky_azimuth_transition_position                   : Azimuth Transition (Horizontal)
 sky_azimuth_east_col                              : Azimuth East Color
 sky_azimuth_east_col_inten                        : Azimuth East Intensity
 sky_azimuth_transition_col                        : Azimuth Transition Color
 sky_azimuth_transition_col_inten                  : Azimuth Transition Intensity
 sky_azimuth_west_col                              : Azimuth West Color
 sky_azimuth_west_col_inten                        : Azimuth West Intensity
 sky_hdr                                           : HDR Intensity
 sky_plane                                         : Plane Color
 sky_plane_inten                                   : Plane Intensity
 sky_sun_col                                       : Sun Color
 sky_sun_disc_col                                  : Sun Disc Color
 sky_sun_disc_size                                 : Disc Size
 sky_sun_hdr                                       : HDR Intensity 
 sky_sun_miephase                                  : Mie Phase
 sky_sun_miescatter                                : Mie Scatter
 sky_sun_mie_intensity_mult                        : Mie Intensity Mult
 sky_sun_influence_radius                          : Influence Radius
 sky_sun_scatter_inten                             : Scatter Intensity
 sky_moon_col                                      : Moon Color
 sky_moon_disc_size                                : Disc Size 
 sky_moon_iten                                     : Moon Intensity
 sky_stars_iten                                    : Starfield Intensity
 sky_moon_influence_radius                         : Moon Influence Radius
 sky_moon_scatter_inten                            : Moon Scatter Intensity
 sky_cloud_gen_frequency                           : Frequency
 sky_cloud_gen_scale                               : Scale 
 sky_cloud_gen_threshold                           : Threshold
 sky_cloud_gen_softness                            : Softness
 sky_cloud_density_mult                            : Density Multiplier
 sky_cloud_density_bias                            : Density Bias
 sky_cloud_mid_col                                 : Mid Color
 sky_cloud_base_col                                : Base Color
 sky_cloud_base_strength                           : Base Strength
 sky_cloud_shadow_col                              : Shadow Color
 sky_cloud_shadow_strength                         : Shadow Strength
 sky_cloud_gen_density_offset                      : Shadow Threshold
 sky_cloud_offset                                  : Color / Shadow Offset)";

    text += R"( sky_cloud_overall_strength                        : Detail Overlay
 sky_cloud_overall_color                           : Detail Overlay Color
 sky_cloud_edge_strength                           : Edge Detail
 sky_cloud_fadeout                                 : Fadeout
 sky_cloud_hdr                                     : HDR Intensity  
 sky_cloud_dither_strength                         : Dither Strength
 sky_small_cloud_col                               : Cloud Color
 sky_small_cloud_detail_strength                   : Strength
 sky_small_cloud_detail_scale                      : Scale  
 sky_small_cloud_density_mult                      : Density Multiplier 
 sky_small_cloud_density_bias                      : Density Bias 
 cloud_shadow_density                              : Density
 cloud_shadow_softness                             : Softness  
 cloud_shadow_opacity                              : Opacity
 dir_shadow_num_cascades                           : Num Cascades
 dir_shadow_distance_multiplier                    : Distance Multiplier
 dir_shadow_softness                               : Softness 
 dir_shadow_cascade0_scale                         : Cascade0 Scale
 sprite_brightness                                 : Brightness
 sprite_size                                       : Scale
 sprite_corona_screenspace_expansion               : Corona Screenspace Expansion
 Lensflare_visibility                              : Lensflare Visibility Mult
 sprite_distant_light_twinkle                      : Distant Lights Twinkle Mult
 water_reflection                                  : Reflection
 water_reflection_far_clip                         : Reflection Far Clip
 water_reflection_lod                              : Reflection LOD
 water_reflection_sky_flod_range                   : Reflection Sky FLOD Range
 water_reflection_lod_range_enabled                : Reflection LOD Range Enabled
 water_reflection_lod_range_hd_start               : Reflection LOD Range HD Start
 water_reflection_lod_range_hd_end                 : Reflection LOD Range HD End
 water_reflection_lod_range_orphanhd_start         : Reflection LOD Range Orphan HD Start
 water_reflection_lod_range_orphanhd_end           : Reflection LOD Range Orphan HD End
 water_reflection_lod_range_lod_start              : Reflection LOD Range LOD Start
 water_reflection_lod_range_lod_end                : Reflection LOD Range LOD End
 water_reflection_lod_range_slod1_start            : Reflection LOD Range SLOD1 Start
 water_reflection_lod_range_slod1_end              : Reflection LOD Range SLOD1 End
 water_reflection_lod_range_slod2_start            : Reflection LOD Range SLOD2 Start
 water_reflection_lod_range_slod2_end              : Reflection LOD Range SLOD2 End
 water_reflection_lod_range_slod3_start            : Reflection LOD Range SLOD3 Start
 water_reflection_lod_range_slod3_end              : Reflection LOD Range SLOD3 End
 water_reflection_lod_range_slod4_start            : Reflection LOD Range SLOD4 Start
 water_reflection_lod_range_slod4_end              : Reflection LOD Range SLOD4 End
 water_reflection_height_offset                    : Reflection Height Offset
 water_reflection_height_override                  : Reflection Height Override
 water_reflection_height_override_amount           : Reflection Height Override Amount
 water_reflection_distant_light_intensity          : Reflection Distant Light Intensity
 water_reflection_corona_intensity                 : Reflection Corona Intensity
 water_foglight                                    : Fog Light
 water_interior                                    : Fog Interior
 water_fogstreaming                                : Enable Fog Streaming
 water_foam_intensity_mult                         : Foam Intensity Mult
 water_drying_speed_mult                           : Drying Speed Mult
 water_specular_intensity                          : Specular Intensity
 mirror_reflection_local_light_intensity           : Reflection Local Light Intensity
 fog_start                                         : Start
 fog_near_col                                      : Near Colour
 fog_near_col_a                                    : 
 fog_col                                           : Sun Colour
 fog_col_a                                         :  
 fog_sun_lighting_calc_pow                         : Sun lighting calc power
 fog_moon_col                                      : Moon Colour
 fog_moon_col_a                                    :   
 fog_moon_lighting_calc_pow                        : Moon ligting calc power
 fog_east_col                                      : Far Colour
 fog_east_col_a                                    :    
 fog_density                                       : Fog Density
 fog_falloff                                       : Fog Height Falloff
 fog_base_height                                   : Fog Base Height
 fog_alpha                                         : Fog Alpha
 fog_horizon_tint_scale                            : Fog Horizon Tint Scale
 fog_hdr                                           : Fog HDR Intensity
 fog_haze_col                                      : Haze Colour
 fog_haze_density                                  : Haze Density
 fog_haze_alpha                                    : Haze Alpha
 fog_haze_hdr                                      : Haze HDR Intensity
 fog_haze_start                                    : Haze Start
 fog_shape_bottom                                  : Fog shape bottom
 fog_shape_top                                     : Fog shape top
 fog_shape_log_10_of_visibility                    : Fog log10(visibility)
 fog_shape_weight_0                                : weight 0   
 fog_shape_weight_1                                : weight 1 
 fog_shape_weight_2                                : weight 2  
 fog_shape_weight_3                                : weight 3
 fog_shadow_amount                                 : Fog Shadow Amount
 fog_shadow_falloff                                : Fog Shadow Falloff
 fog_shadow_base_height                            : Fog Shadow Base Height
 fog_volume_light_range                            : Fog Volumetric Lights Range
 fog_volume_light_fade                             : Fog Volumetric Lights Fade Range
 fog_volume_light_intensity                        : Fog Volumetric Lights Intensity Scale
 fog_volume_light_size                             : Fog Volumetric Lights Size Scale
 fogray_contrast                                   : Fog Ray Contrast
 fogray_intensity                                  : Fog Ray Intensity
 fogray_density                                    : Fog Ray Density
 fogray_nearfade                                   : Fog Ray NearFade
 fogray_farfade                                    : Fog Ray FarFade
 reflection_lod_range_start                        : Reflection LOD range - start
 reflection_lod_range_end                          : Reflection LOD range - end
 reflection_slod_range_start                       : Reflection SLOD range - start
 reflection_slod_range_end                         : Reflection SLOD range - end
 reflection_interior_range                         : Reflection Interior range
 reflection_tweak_interior_amb                     : Reflection Interior Ambient Boost
 reflection_tweak_exterior_amb                     : Reflection Exterior Ambient Boost
 reflection_tweak_emissive                         : Reflection Emissive Boost
 reflection_tweak_directional                      : Reflection Directional Multiplier
 reflection_hdr_mult                               : Reflection HDR Mult
 far_clip                                          : Far Clip
 temperature                                       : Temperature
 particle_emissive_intensity_mult                  : Particle Emissive Intensity Mult
 vfxlightning_intensity_mult                       : Vfx Lightning Intensity Mult
 vfxlightning_visibility                           : Vfx Lightning Visibility
 particle_light_intensity_mult                     : GPU Particle Light Intensity Mult
 natural_ambient_multiplier                        : Natural Ambient Multiplier
 artificial_int_ambient_multiplier                 : Artifical Ambient Multiplier
 fog_cut_off                                       : Fog Cut Off
 no_weather_fx                                     : No Weather Fx
 no_gpu_fx                                         : No GPU Fx
 no_rain                                           : No Rain
 no_rain_ripples                                   : No Rain Ripple
 fogvolume_density_scalar                          : FogVolume Density Scalar
 fogvolume_density_scalar_interior                 : FogVolume Density Scalar Interior
 fogvolume_fog_scaler                              : FogVolume Fog Scaler
 time_offset                                       : Time Offset
 vehicle_dirt_mod                                  : Vehicle Dirt Mod
 wind_speed_mult                                   : Wind Speed Mult
 entity_reject                                     : Entity reject
 lod_mult                                          : Lod Mult (Global)
 enable_occlusion                                  : Enable Occlusion
 enable_shadow_occlusion                           : Enable Shadow Occlusion
 render_exterior                                   : Enable Exterior
 portal_weight                                     : Portal Weight
 light_falloff_mult                                : Light FallOff Mult
 lodlight_range_mult                               : LOD Light Range Mult
 shadow_distance_mult                              : Shadow Distance Mult
 lod_mult_hd                                       : Lod Mult HD
 lod_mult_orphanhd                                 : Lod Mult orphan HD
 lod_mult_lod                                      : Lod Mult LOD
 lod_mult_slod1                                    : Lod Mult SLOD1
 lod_mult_slod2                                    : Lod Mult SLOD2
 lod_mult_slod3                                    : Lod Mult SLOD3
 lod_mult_slod4                                    : Lod Mult SLOD4
)";

    std::ofstream output(filename);
    if (!output.is_open())
        return;
    output.write(text.c_str(), text.size());
    output.close();
}



void TcCategoriesHandler::save_hardcoded_categories_to_file(const char* filename)
{
    const char text[] = R"(
@light variables

light_dir_col
light_dir_mult
light_directional_amb_col
light_directional_amb_intensity
light_directional_amb_intensity_mult
light_directional_amb_bounce_enabled
light_amb_down_wrap
light_natural_amb_down_col
light_natural_amb_down_intensity
light_natural_amb_up_col
light_natural_amb_up_intensity
light_natural_amb_up_intensity_mult
light_natural_push
light_ambient_bake_ramp
light_artificial_int_down_col
light_artificial_int_down_intensity
light_artificial_int_up_col
light_artificial_int_up_intensity
light_artificial_ext_down_col
light_artificial_ext_down_intensity
light_artificial_ext_up_col
light_artificial_ext_up_intensity
ped_light_col
ped_light_mult
ped_light_direction_x
ped_light_direction_y
ped_light_direction_z
light_amb_occ_mult
light_amb_occ_mult_ped
light_amb_occ_mult_veh
light_amb_occ_mult_prop
light_amb_volumes_in_diffuse
ssao_inten
ssao_type
ssao_cp_strength
ssao_qs_strength
light_ped_rim_mult
light_dynamic_bake_tweak
light_vehicle_second_spec_override
light_vehicle_intenity_scale

@Sun pos override

light_direction_override
light_direction_override_overrides_sun
sun_direction_x
sun_direction_y
sun_direction_z
moon_direction_x
moon_direction_y
moon_direction_z

@light ray variables

light_ray_col
light_ray_mult
light_ray_underwater_mult
light_ray_dist
light_ray_heightfalloff
light_ray_height_falloff_start


@Screen Space Light rays data

light_ray_add_reducer
light_ray_blit_size
light_ray_length


@post fx variables

postfx_exposure
postfx_exposure_min
postfx_exposure_max
postfx_bright_pass_thresh_width
postfx_bright_pass_thresh
postfx_intensity_bloom
postfx_correct_col
postfx_correct_cutoff
postfx_shift_col
postfx_shift_cutoff
postfx_desaturation
postfx_noise
postfx_noise_size
postfx_tonemap_filmic_override_dark
postfx_tonemap_filmic_exposure_dark
postfx_tonemap_filmic_a
postfx_tonemap_filmic_b
postfx_tonemap_filmic_c
postfx_tonemap_filmic_d
postfx_tonemap_filmic_e
postfx_tonemap_filmic_f
postfx_tonemap_filmic_w
postfx_tonemap_filmic_override_bright
postfx_tonemap_filmic_exposure_bright
postfx_tonemap_filmic_a_bright
postfx_tonemap_filmic_b_bright
postfx_tonemap_filmic_c_bright
postfx_tonemap_filmic_d_bright
postfx_tonemap_filmic_e_bright
postfx_tonemap_filmic_f_bright
postfx_tonemap_filmic_w_bright


@vignetting intensity

postfx_vignetting_intensity
postfx_vignetting_radius
postfx_vignetting_contrast
postfx_vignetting_col


@colour gradient variables

postfx_grad_top_col
postfx_grad_middle_col
postfx_grad_bottom_col
postfx_grad_midpoint
postfx_grad_top_middle_midpoint
postfx_grad_middle_bottom_midpoint
postfx_scanlineintensity
postfx_scanline_frequency_0
postfx_scanline_frequency_1
postfx_scanline_speed
postfx_motionblurlength


@depth of field variables

dof_far
dof_blur_mid
dof_blur_far
dof_enable_hq
dof_hq_smallblur
dof_hq_shallowdof
dof_hq_nearplane_out
dof_hq_nearplane_in
dof_hq_farplane_out
dof_hq_farplane_in
environmental_blur_in
environmental_blur_out
environmental_blur_size
bokeh_brightness_min
bokeh_brightness_max
bokeh_fade_min
bokeh_fade_max


@night vision variables

nv_light_dir_mult
nv_light_amb_down_mult
nv_light_amb_up_mult
nv_lowLum
nv_highLum
nv_topLum
nv_scalerLum
nv_offsetLum
nv_offsetLowLum
nv_offsetHighLum
nv_noiseLum
nv_noiseLowLum
nv_noiseHighLum
nv_bloomLum
nv_colorLum
nv_colorLowLum
nv_colorHighLum


@heat haze variables

hh_startRange
hh_farRange
hh_minIntensity
hh_maxIntensity
hh_displacementU
hh_displacementV
hh_tex1UScale
hh_tex1VScale
hh_tex1UOffset
hh_tex1VOffset
hh_tex2UScale
hh_tex2VScale
hh_tex2UOffset
hh_tex2VOffset
hh_tex1UFrequencyOffset
hh_tex1UFrequency
hh_tex1UAmplitude
hh_tex1VScrollingSpeed
hh_tex2UFrequencyOffset
hh_tex2UFrequency
hh_tex2UAmplitude
hh_tex2VScrollingSpeed


@lens distortion

lens_dist_coeff
lens_dist_cube_coeff
chrom_aberration_coeff
chrom_aberration_coeff2
lens_artefacts_intensity
lens_artefacts_min_exp_intensity
lens_artefacts_max_exp_intensity


@blur vignetting

blur_vignetting_radius
blur_vignetting_intensity


@screen blur

screen_blur_intensity


@Sky colouring

sky_zenith_transition_position
sky_zenith_transition_east_blend
sky_zenith_transition_west_blend
sky_zenith_blend_start
sky_zenith_col
sky_zenith_col_inten
sky_zenith_transition_col
sky_zenith_transition_col_inten
sky_azimuth_transition_position
sky_azimuth_east_col
sky_azimuth_east_col_inten
sky_azimuth_transition_col
sky_azimuth_transition_col_inten
sky_azimuth_west_col
sky_azimuth_west_col_inten
sky_hdr
sky_plane
sky_plane_inten


@Sun variables

sky_sun_col
sky_sun_disc_col
sky_sun_disc_size
sky_sun_hdr
sky_sun_miephase
sky_sun_miescatter
sky_sun_mie_intensity_mult
sky_sun_influence_radius
sky_sun_scatter_inten


@Stars / Moon variables

sky_moon_col
sky_moon_disc_size
sky_moon_iten
sky_stars_iten
sky_moon_influence_radius
sky_moon_scatter_inten


@Cloud generation

sky_cloud_gen_frequency
sky_cloud_gen_scale
sky_cloud_gen_threshold
sky_cloud_gen_softness
sky_cloud_density_mult
sky_cloud_density_bias


@Main clouds

sky_cloud_mid_col
sky_cloud_base_col
sky_cloud_base_strength
sky_cloud_shadow_col
sky_cloud_shadow_strength
sky_cloud_gen_density_offset
sky_cloud_offset
sky_cloud_overall_strength
sky_cloud_overall_color
sky_cloud_edge_strength
sky_cloud_fadeout
sky_cloud_hdr
sky_cloud_dither_strength
sky_small_cloud_col
sky_small_cloud_detail_strength
sky_small_cloud_detail_scale
sky_small_cloud_density_mult
sky_small_cloud_density_bias


@cloud shadows

cloud_shadow_density
cloud_shadow_softness
cloud_shadow_opacity


@directional (cascade) shadows

dir_shadow_num_cascades
dir_shadow_distance_multiplier
dir_shadow_softness
dir_shadow_cascade0_scale


@sprite (corona and distant lights) variables

sprite_brightness
sprite_size
sprite_corona_screenspace_expansion
Lensflare_visibility
sprite_distant_light_twinkle


@water variables

water_reflection
water_reflection_far_clip
water_reflection_lod
water_reflection_sky_flod_range
water_reflection_lod_range_enabled
water_reflection_lod_range_hd_start
water_reflection_lod_range_hd_end
water_reflection_lod_range_orphanhd_start
water_reflection_lod_range_orphanhd_end
water_reflection_lod_range_lod_start
water_reflection_lod_range_lod_end
water_reflection_lod_range_slod1_start
water_reflection_lod_range_slod1_end
water_reflection_lod_range_slod2_start
water_reflection_lod_range_slod2_end
water_reflection_lod_range_slod3_start
water_reflection_lod_range_slod3_end
water_reflection_lod_range_slod4_start
water_reflection_lod_range_slod4_end
water_reflection_height_offset
water_reflection_height_override
water_reflection_height_override_amount
water_reflection_distant_light_intensity
water_reflection_corona_intensity
water_foglight
water_interior
water_fogstreaming
water_foam_intensity_mult
water_drying_speed_mult
water_specular_intensity


@mirror variables

mirror_reflection_local_light_intensity


@fog variables

fog_start
fog_near_col
fog_near_col_a
fog_col
fog_col_a
fog_sun_lighting_calc_pow
fog_moon_col
fog_moon_col_a
fog_moon_lighting_calc_pow
fog_east_col
fog_east_col_a
fog_density
fog_falloff
fog_base_height
fog_alpha
fog_horizon_tint_scale
fog_hdr
fog_haze_col
fog_haze_density
fog_haze_alpha
fog_haze_hdr
fog_haze_start


@Linear piecewise fog.

fog_shape_bottom
fog_shape_top
fog_shape_log_10_of_visibility
fog_shape_weight_0
fog_shape_weight_1
fog_shape_weight_2
fog_shape_weight_3


@fog shadows

fog_shadow_amount
fog_shadow_falloff
fog_shadow_base_height


@volumetric lights

fog_volume_light_range
fog_volume_light_fade
fog_volume_light_intensity
fog_volume_light_size


@fog ray

fogray_contrast
fogray_intensity
fogray_density
fogray_nearfade
fogray_farfade


@refection variables

reflection_lod_range_start
reflection_lod_range_end
reflection_slod_range_start
reflection_slod_range_end
reflection_interior_range
reflection_tweak_interior_amb
reflection_tweak_exterior_amb
reflection_tweak_emissive
reflection_tweak_directional
reflection_hdr_mult


@misc variables

far_clip
temperature
particle_emissive_intensity_mult
vfxlightning_intensity_mult
vfxlightning_visibility
particle_light_intensity_mult
natural_ambient_multiplier
artificial_int_ambient_multiplier
fog_cut_off
no_weather_fx
no_gpu_fx
no_rain
no_rain_ripples
fogvolume_density_scalar
fogvolume_density_scalar_interior
fogvolume_fog_scaler
time_offset
vehicle_dirt_mod
wind_speed_mult
entity_reject
lod_mult
enable_occlusion
enable_shadow_occlusion
render_exterior
portal_weight
light_falloff_mult
lodlight_range_mult
shadow_distance_mult
lod_mult_hd
lod_mult_orphanhd
lod_mult_lod
lod_mult_slod1
lod_mult_slod2
lod_mult_slod3
lod_mult_slod4
)";
    
    std::ofstream output(filename);
    if (!output.is_open())
        return;
    output.write(text, std::size(text) - 1);
    output.close();
}
