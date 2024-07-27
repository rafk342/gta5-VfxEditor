#include "ParticlesUi.h"
#include <ranges>



using namespace rage;
static float v_speed = 0.01f;
//u32 BitsetWidgets(const char* name, atBitSet bitset);



template<size_t bits_count, class type>
type BitsetWidgets(const char* name, atFixedBitSet<bits_count, type> bitset)
{
	ImGui::SeparatorText(name);
	bool flag;
	for (size_t i = 0; i < bitset.size(); i++)
	{
		flag = bitset.test(i);
		if (ImGui::Checkbox(vfmt("n : {}##{}", i, NextUniqueNum()), &flag))
		{
			bitset.set(i, flag);
		}
	}
	ImGui::Separator();

	
	return (*bitset.data());
}



ParticlesUi::ParticlesUi(const char* title) : App(title)
{
	ptr = *gmAddress::Scan("48 8B 0D ?? ?? ?? ?? 8B D3 E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? 8B D6").GetRef(3).To<PtxMngr**>();
	scrInvoke([]
		{
			STREAMING::REQUEST_NAMED_PTFX_ASSET("core");
			while (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("core")) {}
		});
}

#define ImguiDragInt_FromDiffType(_label, _TempVarName, _Value) \
	{ \
		int _TempVarName = _Value; \
		if (ImGui::DragInt(_label, &_TempVarName, 0.1f)) \
		{ \
			_Value = _TempVarName; \
		} \
	} \

#define SEARCH_DEF1(_InputTextName)\
\
	static char search_name_buff[255]{};\
	ImGui::InputText(_InputTextName, search_name_buff, std::size(search_name_buff));\
	size_t input_len = strlen(search_name_buff);\
	static bool show_everything = true;\
\
	if (input_len == 0)\
	{\
		show_everything = true;\
	}\
	else\
	{\
		show_everything = false;\
	}\

#define SEARCH_DEF2(entry_name)\
	bool tree_node_should_be_shown = false;\
\
	if (show_everything)\
	{\
		tree_node_should_be_shown = true;\
	}\
	else\
	{\
		size_t entry_name_len = strlen(entry_name);\
\
		if (input_len > entry_name_len)\
		{\
			tree_node_should_be_shown = false;\
		}\
		else\
		{\
			tree_node_should_be_shown = true;\
			for (size_t n = 0; n < input_len; n++)\
			{\
				if (search_name_buff[n] != entry_name[n])\
				{\
					tree_node_should_be_shown = false;\
					break;\
				}\
			}\
		}\
	}\




void ParticlesUi::window()
{
	static char buff[255]{};
	static ::Vector3 pos1{};
	static ::Vector3 pos2{};
	static float col[4]{};

	static float scale = 1.0f;
	static int particle_handle = -1;

	static bool spawned = false;

	NextUniqueNum(true);	//reset for this frame




	ImGui::InputText("Buff", buff, std::size(buff));

	if (ImGui::Button("GetPlayerPos"))
	{
		scrInvoke([] { pos1 = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false); });
	}

	ImGui::Text(vfmt("x:{} y:{} z:{}", pos1.x, pos1.y, pos1.z));
	ImGui::Text("Handle id : %i", particle_handle);

	if (ImGui::Button("SpawnParticle"))
	{
		scrInvoke([]
			{
				GRAPHICS::_SET_PTFX_ASSET_NEXT_CALL("core");
				particle_handle = GRAPHICS::START_PARTICLE_FX_LOOPED_AT_COORD(buff, pos1.x, pos1.y, pos1.z, 0.0f, 0.0f, 0.0f, scale, 0, 0, 0, true);
			});
	}

	if (ImGui::DragFloat("Scale", &scale, 0.01f))
	{
		scrInvoke([] { GRAPHICS::SET_PARTICLE_FX_LOOPED_SCALE(particle_handle, scale); });
	}
	

	if (ImGui::Button("Delete"))
	{
		scrInvoke([]
			{
				GRAPHICS::STOP_PARTICLE_FX_LOOPED(particle_handle, true);
				particle_handle = -1;
			}
		);
	}

	std::vector<int> v;

	v.insert(v.begin(), 10);

	size_t index = 0;
	for (auto& it : ptr->m_fxListMap)
	{
		if (ImGui::CollapsingHeader(it.m_pFxList->name))
		{
			if (ImGui::TreeNode(vfmt("EffectRuleDictionary##_{}", index)))
			{
				auto& entries = it.m_pFxList->ptxEffectRuleDictionary->m_Entries;
				auto& codes = it.m_pFxList->ptxEffectRuleDictionary->m_Codes;

				SEARCH_DEF1(vfmt("Search## EffectRule__{}", index))

				for (size_t i = 0; i < entries.size(); i++)
				{
					ptxEffectRule& entry = (*entries[i]);

					SEARCH_DEF2(entry.m_name);
					
					if (tree_node_should_be_shown)
					if (ImGui::TreeNode(vfmt("{}## EffectRule {}{}", entry.m_name, codes[i], i)))
					{

						ImGui::Text("file_version %f", entry.m_fileVersion);

						ImGui::DragInt(vfmt("Num Loops ##{}", NextUniqueNum()),						&entry.m_numLoops, 0.1f);
						ImGui::Checkbox(vfmt("Sort Events By Distance##{}", NextUniqueNum()),		&entry.m_sortEventsByDistance);
						ImguiDragInt_FromDiffType(vfmt("Draw List Id ##{}", NextUniqueNum()), v1,	entry.m_drawListId);
						ImGui::Checkbox(vfmt("Is Short Lived##{}", NextUniqueNum()),				&entry.m_isShortLived);
						ImGui::Checkbox(vfmt("Has No Shadows##{}", NextUniqueNum()),				&entry.m_hasNoShadows);
						ImGui::DragFloat3(vfmt("vRandom Offset Pos##{}", NextUniqueNum()),			(float*)&entry.m_vRandOffsetPos, v_speed);
						ImGui::DragFloat(vfmt("PreUpdate Time##{}", NextUniqueNum()),				&entry.m_preUpdateTime, v_speed);
						ImGui::DragFloat(vfmt("PreUpdate Time Interval##{}", NextUniqueNum()),		&entry.m_preUpdateTimeInterval, v_speed);
						ImGui::DragFloat(vfmt("Duration Min##{}", NextUniqueNum()),					&entry.m_durationMin, v_speed);
						ImGui::DragFloat(vfmt("Duration Max##{}", NextUniqueNum()),					&entry.m_durationMax, v_speed);
						ImGui::DragFloat(vfmt("Playback RateScalar Min##{}", NextUniqueNum()),		&entry.m_playbackRateScalarMin, v_speed);
						ImGui::DragFloat(vfmt("Playback RateScalar Max##{}", NextUniqueNum()),		&entry.m_playbackRateScalarMax, v_speed);

						ImguiDragInt_FromDiffType(vfmt("Viewport Culling Mode ##{}", NextUniqueNum()), v2,	entry.m_viewportCullingMode);
						ImGui::Checkbox(vfmt("Render When Viewport Culled##{}", NextUniqueNum()),			&entry.m_renderWhenViewportCulled);
						ImGui::Checkbox(vfmt("Update When Viewport Culled##{}", NextUniqueNum()),			&entry.m_updateWhenViewportCulled);
						ImGui::Checkbox(vfmt("Emit When Viewport Culled##{}", NextUniqueNum()),				&entry.m_emitWhenViewportCulled);
						ImguiDragInt_FromDiffType(vfmt("Distance Culling Mode ##{}", NextUniqueNum()), v3,	entry.m_distanceCullingMode);

						ImGui::Checkbox(vfmt("Render When Distance Culled##{}", NextUniqueNum()),			&entry.m_renderWhenDistanceCulled);
						ImGui::Checkbox(vfmt("Update When Distance Culled##{}", NextUniqueNum()),			&entry.m_updateWhenDistanceCulled);
						ImGui::Checkbox(vfmt("Emit When Distance Culled##{}", NextUniqueNum()),				&entry.m_emitWhenDistanceCulled);

						ImGui::DragFloat3(vfmt("Viewport Culling Sphere Offset##{}", NextUniqueNum()),		(float*)&entry.m_viewportCullingSphereOffset, v_speed);
						ImGui::DragFloat(vfmt("Viewport Culling Sphere Radius##{}", NextUniqueNum()),		&entry.m_viewportCullingSphereRadius, v_speed);
						ImGui::DragFloat(vfmt("Distance Culling Fade Dist##{}", NextUniqueNum()),			&entry.m_distanceCullingFadeDist, v_speed);
						ImGui::DragFloat(vfmt("Distance Culling Cull Dist##{}", NextUniqueNum()),			&entry.m_distanceCullingCullDist, v_speed);

						ImGui::DragFloat(vfmt("Lod Evo Distance Min##{}", NextUniqueNum()),					&entry.m_lodEvoDistMin, v_speed);
						ImGui::DragFloat(vfmt("Lod Evo Distance Max##{}", NextUniqueNum()),					&entry.m_lodEvoDistMax, v_speed);

						ImGui::DragFloat(vfmt("Collision Range##{}", NextUniqueNum()),						&entry.m_colnRange, v_speed);
						ImGui::DragFloat(vfmt("Collision Probe Distance##{}", NextUniqueNum()),				&entry.m_colnProbeDist, v_speed);

						ImguiDragInt_FromDiffType(vfmt("Collision Type##{}", NextUniqueNum()), v4,			entry.m_colnType);

						ImGui::Checkbox(vfmt("Share Entity Collisions##{}", NextUniqueNum()),				&entry.m_shareEntityColn);
						ImGui::Checkbox(vfmt("Only Use BVH Collisions##{}", NextUniqueNum()),				&entry.m_onlyUseBVHColn);

						int temp_value = entry.m_gameFlags;
						if (ImGui::InputInt(vfmt("GameFlags##{}", NextUniqueNum()), &temp_value)) { entry.m_gameFlags = temp_value; }

						ImGui::Text(vfmt("GameFlags bits : {:08b}", entry.m_gameFlags));

						ImGui::Checkbox(vfmt("Colour Tint Max Enable##{}", NextUniqueNum()),				&entry.m_colourTintMaxEnable);
						ImGui::Checkbox(vfmt("Use Data Volume##{}", NextUniqueNum()),						&entry.m_useDataVolume);

						ImguiDragInt_FromDiffType(vfmt("Data Volume Type##{}", NextUniqueNum()), v5,		entry.m_dataVolumeType);

						ImGui::Text("Num Active Instances : %i", entry.m_numActiveInstances);

						ImGui::DragFloat(vfmt("Zoom Level##{}", NextUniqueNum()),							&entry.m_zoomableComponentScalar, v_speed);



						if (ImGui::TreeNode(vfmt("Time Line Events##{}", i)))
						{
							for (size_t event_idx = 0; event_idx < entry.m_timeline.m_events.size(); event_idx++)
							{
								auto& _event = entry.m_timeline.m_events[event_idx];

								if (ImGui::TreeNode(vfmt("Index : {}##{}", event_idx, i)))
								{
									switch (_event->m_type)
									{
									case PTXEVENT_TYPE_EMITTER:
									{
										ptxEventEmitter& EmitterEvent = static_cast<decltype(EmitterEvent)>(*(_event.Get()));

										ImGui::Text(vfmt("EmitterRule : {}", EmitterEvent.m_emitterRuleName));
										ImGui::Text(vfmt("ParticleRule : {}", EmitterEvent.m_particleRuleName));

										ImGui::DragFloat(vfmt("Start Ratio##{} ", NextUniqueNum()),						&EmitterEvent.m_startRatio, v_speed);
										ImGui::DragFloat(vfmt("End Ratio##{} ", NextUniqueNum()),						&EmitterEvent.m_endRatio, v_speed);
										ImGui::DragFloat(vfmt("Playback Rate Scalar Min##{} ", NextUniqueNum()),		&EmitterEvent.m_playbackRateScalarMin, v_speed);
										ImGui::DragFloat(vfmt("Playback Rate Scalar Max##{} ", NextUniqueNum()),		&EmitterEvent.m_playbackRateScalarMax, v_speed);
										ImGui::DragFloat(vfmt("Zoom Scalar Min##{} ", NextUniqueNum()),					&EmitterEvent.m_zoomScalarMin, v_speed);
										ImGui::DragFloat(vfmt("Zoom Scalar Max##{} ", NextUniqueNum()),					&EmitterEvent.m_zoomScalarMax, v_speed);



										rage::Vector4 col1 = EmitterEvent.m_colourTintMin.Getf_col4();
										if (ImGui::ColorEdit4(vfmt("Colour Tint Min##{}", NextUniqueNum()), (float*)&col1)) { EmitterEvent.m_colourTintMin = col1; }

										rage::Vector4 col2 = EmitterEvent.m_colourTintMax.Getf_col4();
										if (ImGui::ColorEdit4(vfmt("Colour Tint Max##{}", NextUniqueNum()), (float*)&col2)) { EmitterEvent.m_colourTintMax = col2; }



										if (ImGui::TreeNode(vfmt("Evolution List##{} {}", i, event_idx)))
										{
											if (ImGui::TreeNode(vfmt("Evolutions##{} {}", i, event_idx)))
											{
												atArray<ptxEvolution>& EvolutionsArray = EmitterEvent.m_pEvolutionList.Get()->m_evolutions;

												for (size_t idx = 0; idx < EvolutionsArray.size(); idx++)
												{
													ptxEvolution& currEvolution = EvolutionsArray[idx];
													
													ImGui::Text(currEvolution.m_name);
													ImGui::DragFloat(vfmt("Override Value##{}", NextUniqueNum()),			&currEvolution.m_overrideValue, v_speed);
													ImGui::Checkbox(vfmt("Is Overriden##{}", NextUniqueNum()),				&currEvolution.m_isOverriden);

													ImGui::Separator();
												}

												ImGui::TreePop();
											}

											if (ImGui::TreeNode(vfmt("Evolved Keyframe Props##{} {}", i, event_idx)))
											{
												atArray<ptxEvolvedKeyframeProp>& EvolvedKeyframeProps = EmitterEvent.m_pEvolutionList.Get()->m_evolvedKeyframeProps;

												for (size_t evPropIdx = 0; evPropIdx < EvolvedKeyframeProps.size(); evPropIdx++) 
												{
													ptxEvolvedKeyframeProp& CurrEvKfProp = EvolvedKeyframeProps[evPropIdx];
													
													ImguiDragInt_FromDiffType(vfmt("Blend Mode ##{}", NextUniqueNum()), v_ev_prop, CurrEvKfProp.m_blendMode);

													
													if (ImGui::TreeNode(vfmt("Evolved Keyframes ##{} {} {}", i, event_idx, evPropIdx)))
													{


														for (size_t evoKfIdx = 0; evoKfIdx < CurrEvKfProp.m_evolvedKeyframes.size(); evoKfIdx++)
														{
															ptxEvolvedKeyframe& currEvoKeyframe = CurrEvKfProp.m_evolvedKeyframes[evoKfIdx];

															ImGui::InputInt(vfmt("EvolutionID ##{}",		NextUniqueNum()),			&currEvoKeyframe.m_evolutionIdx);
															ImGui::Checkbox(vfmt("Is Lod Evolution##{}",	NextUniqueNum()),			&currEvoKeyframe.m_isLodEvolution);


															if (ImGui::TreeNode(vfmt("Keyframe ##{} {} {} {}", i, event_idx, evPropIdx, evoKfIdx)))
															{
																auto& data = currEvoKeyframe.m_keyframe.data;

																ImGui::Separator();
																
																for (size_t keyframe_idx = 0; keyframe_idx < data.size(); keyframe_idx++)
																{
																	auto& currKeyframe = data[keyframe_idx];

																	ImGui::DragFloat4(vfmt("KeyframeTime ## {}", NextUniqueNum()),		currKeyframe.vTime, v_speed);
																	ImGui::DragFloat4(vfmt("KeyframeValue ## {}", NextUniqueNum()),		currKeyframe.vValue, v_speed);

																	ImGui::Separator();
																}



																ImGui::TreePop();
															}
															ImGui::Separator();
														}
													
														
														ImGui::TreePop();
													}
												}
												ImGui::TreePop();
											}
											ImGui::TreePop();
										}
									}
									break;

									default:
										break;
									}
									ImGui::TreePop();
								}
							}

							ImGui::TreePop();
						}

						if (ImGui::TreeNode(vfmt("Keyframe Props##_{}", i)))
						{
							ptxKeyframePropTreeNode(entry.m_colourTintMinKFP, "Colour Tint Min KFP", i, PTX_KF_COL4);
							ptxKeyframePropTreeNode(entry.m_colourTintMaxKFP, "Colour Tint Max KFP", i, PTX_KF_COL4);
							ptxKeyframePropTreeNode(entry.m_zoomScalarKFP, "Zoom Scalar KFP", i, PTX_KF_FLOAT2);
							ptxKeyframePropTreeNode(entry.m_dataSphereKFP, "Data Sphere KFP", i, PTX_KF_FLOAT4);
							ptxKeyframePropTreeNode(entry.m_dataCapsuleKFP, "Data Capsule KFP", i, PTX_KF_FLOAT4);

							ImGui::TreePop();
						}
					
						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}
		
			if (ImGui::TreeNode(vfmt("EmitterRuleDictionary##_{}", index)))
			{

				auto& entries = it.m_pFxList->ptxEmitterRuleDictionary->m_Entries;
				auto& codes = it.m_pFxList->ptxEmitterRuleDictionary->m_Codes;

				SEARCH_DEF1(vfmt("Search## EmitterRule__{}", index))

				for (size_t i = 0; i < entries.size(); i++)
				{
					auto& entry = entries[i].GetRef();
					auto& e = entry;
				
					SEARCH_DEF2(e.m_name)

					if (tree_node_should_be_shown) 
					{
						if (ImGui::TreeNode(vfmt("{}## EmitterRule {}", entry.m_name, i)))
						{
							ImGui::Text("FileVersion : %f", e.m_fileVersion);

							PtxDomainTreeNode(e.m_creationDomainObj, "Creation Domain", i);
							PtxDomainTreeNode(e.m_creationDomainObj, "Target Domain", i);
							PtxDomainTreeNode(e.m_creationDomainObj, "Attractor Domain", i);
							
							ImGui::Separator();
							
							ptxKeyframePropTreeNode(e.m_spawnRateOverTimeKFP, "Spawn Rate Over Time ##KFP", i, PTX_KF_FLOAT3);
							ptxKeyframePropTreeNode(e.m_spawnRateOverDistKFP, "Spawn Rate Over Distance ##KFP", i, PTX_KF_FLOAT2);
							ptxKeyframePropTreeNode(e.m_particleLifeKFP, "Particle Life ##KFP", i, PTX_KF_FLOAT2);
							ptxKeyframePropTreeNode(e.m_playbackRateScalarKFP, "Playback Rate Scalar ##KFP", i, PTX_KF_FLOAT);
							ptxKeyframePropTreeNode(e.m_speedScalarKFP, "Speed Scalar ##KFP", i, PTX_KF_FLOAT2);
							ptxKeyframePropTreeNode(e.m_sizeScalarKFP, "Size Scalar ##KFP", i, PTX_KF_FLOAT3);
							ptxKeyframePropTreeNode(e.m_accnScalarKFP, "Acceleration Scalar ##KFP", i, PTX_KF_FLOAT3);
							ptxKeyframePropTreeNode(e.m_dampeningScalarKFP, "Dampening Scalar ##KFP", i, PTX_KF_FLOAT3);
							ptxKeyframePropTreeNode(e.m_matrixWeightScalarKFP, "Matrix Weight Scalar ##KFP", i, PTX_KF_FLOAT);
							ptxKeyframePropTreeNode(e.m_inheritVelocityKFP, "Inherit Velocity ##KFP", i, PTX_KF_FLOAT2);

							ImGui::Separator();

							ImGui::TreePop();
						}
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode(vfmt("ParticleRuleDictionary##_{}", index)))
			{
				auto& entries = it.m_pFxList->ptxParticleRuleDictionary.GetRef().m_Entries;

				SEARCH_DEF1(vfmt("Search## ParticleRule__{}", index))

				for (size_t i = 0; i < entries.size(); i++)
				{
					auto& e = entries[i].GetRef();

					SEARCH_DEF2(e.m_name);

					if (tree_node_should_be_shown)
					{
						if (ImGui::TreeNode(vfmt("{}## ParticleRule {}", e.m_name, i)))
						{
							ImGui::Text("FileVersion : %f", e.m_fileVersion);


							ImGui::SeparatorText("Render State");

							ImGui::DragInt(vfmt("Cull Mode##{}", NextUniqueNum()), &e.m_renderState.m_cullMode,0.5f,0,1000);
							ImGui::DragInt(vfmt("Blend Set##{}", NextUniqueNum()), &e.m_renderState.m_blendSet);
							ImGui::DragInt(vfmt("Lighting Mode##{}", NextUniqueNum()), &e.m_renderState.m_lightingMode);

							ImGui::Checkbox(vfmt("Depth Write##{}", NextUniqueNum()), &e.m_renderState.m_depthWrite);
							ImGui::Checkbox(vfmt("Depth Test##{}", NextUniqueNum()), &e.m_renderState.m_depthTest);
							ImGui::Checkbox(vfmt("Alpha Blend##{}", NextUniqueNum()), &e.m_renderState.m_alphaBlend);

							ImGui::Separator();

							ImGui::InputInt(vfmt("Tex Frame Id Min ##{}", NextUniqueNum()), &e.m_texFrameIdMin);
							ImGui::InputInt(vfmt("Tex Frame Id Max ##{}", NextUniqueNum()), &e.m_texFrameIdMax);

							ptxEffectSpawnerTreeNode(e.m_effectSpawnerAtRatio, "Effect Spawner At Ratio", i);
							ptxEffectSpawnerTreeNode(e.m_effectSpawnerOnColn, "Effect Spawner On Collision", i);

							
							BehavioursTreeNode(e.m_allBehaviours, "All Behaviours", i);

							ImGui::SeparatorText("ptxBiasLink");
							
							for (size_t bias_link_idx = 0; bias_link_idx < e.m_biasLinks.size(); bias_link_idx++)
							{
								auto& bias_link = e.m_biasLinks[bias_link_idx];

								ImGui::Text(vfmt("Name : {}",bias_link.m_name));

								if (ImGui::TreeNode(vfmt("KeyframePropsIds##{} {}",i,bias_link_idx)))
								{
									for (auto& v : bias_link.m_keyframePropIds)
									{
										ImGui::Text(vfmt("KeyframePropId : %i", v));
									}
									ImGui::TreePop();
								}

							}

							ImGui::SeparatorText("ptxTechniqueDesc");

							ImGui::Text(vfmt("ShaderFile : {}", e.m_shaderInst.m_shaderTemplateName));
							ImGui::Text(vfmt("ShaderTechnique : {} id : {}", e.m_shaderInst.m_shaderTemplateTechniqueName, e.m_shaderInst.m_shaderTemplateTechniqueId));
							{
								ImGui::SeparatorText("ptxTechniqueDesc");
								
								ImGui::InputInt(vfmt("DiffuseMode##{}", NextUniqueNum()),	(int*)& e.m_shaderInst.m_techniqueDesc.m_diffuseMode);
								ImGui::InputInt(vfmt("ProjectionMode##{}", NextUniqueNum()),(int*)&e.m_shaderInst.m_techniqueDesc.m_projMode);

								ImGui::Checkbox(vfmt("Is Lit##{}", NextUniqueNum()),	&e.m_shaderInst.m_techniqueDesc.m_isLit);
								ImGui::Checkbox(vfmt("Is Soft##{}", NextUniqueNum()),&e.m_shaderInst.m_techniqueDesc.m_isSoft);
								ImGui::Checkbox(vfmt("Is Screen Space##{}", NextUniqueNum()),&e.m_shaderInst.m_techniqueDesc.m_isScreenSpace);
								ImGui::Checkbox(vfmt("Is Refract##{}", NextUniqueNum()),&e.m_shaderInst.m_techniqueDesc.m_isRefract);
								ImGui::Checkbox(vfmt("Is NormalSpec##{}", NextUniqueNum()),&e.m_shaderInst.m_techniqueDesc.m_isNormalSpec);

								ImGui::Separator();

								if (ImGui::TreeNode(vfmt("ShaderVars## {}", i)))
								{
									for (size_t shader_var_idx = 0; shader_var_idx < e.m_shaderInst.m_instVars.size(); shader_var_idx++)
									{
										auto& shader_var = e.m_shaderInst.m_instVars[shader_var_idx].GetRef();

										ImGui::Text(vfmt("hash : {}", shader_var.m_hashName));
										ImGui::Text(vfmt("Type : {}", shader_var.GetTypeStr()));
										ImGui::Text(vfmt("Id : {}", shader_var.m_id));

										ImGui::Checkbox(vfmt("Is Keyframeable##{}", NextUniqueNum()), &shader_var.m_isKeyframeable);
										ImGui::Text(vfmt("Owns Info : {}", shader_var.m_ownsInfo));

										ImGui::Separator();
									}
									ImGui::TreePop();
								}
							}
							ImGui::Separator();


							if (ImGui::TreeNode(vfmt("Drawables ## {}", i)))
							{
								for (auto& drawable : e.m_drawables)
								{
									ImGui::Text(drawable.m_name);
									ImGui::DragFloat4(vfmt("BoundInfo##{}", NextUniqueNum()), (float*)&drawable.m_vBoundInfo);
									ImGui::Separator();
								}

								ImGui::TreePop();
							}

							ImguiDragInt_FromDiffType(vfmt("SortType##{}", NextUniqueNum()), v, e.m_sortType);
							ImguiDragInt_FromDiffType(vfmt("DrawType##{}", NextUniqueNum()), v, e.m_drawType);

							e.m_flags = BitsetWidgets<8,u8>("Flags", e.m_flags);

							ImGui::Text(vfmt("flags : {:08b}", e.m_flags));
							ImGui::Text(vfmt("Runtime flags : {:08b}", e.m_runtimeFlags));

							ImGui::TreePop();
						}
					}
					
				}

				ImGui::TreePop();
			}
		}

		index++;
	}
}


//u32 BitsetWidgets(const char* name, atBitSet bitset)
//{
//	ImGui::SeparatorText(name);
//	bool flag;
//	for (size_t i = 0; i < bitset.size(); i++)
//	{
//		flag = bitset.test(i);
//		if (ImGui::Checkbox(vfmt("##{}", i), &flag))
//		{
//			bitset.set(i, flag);
//		}
//	}
//	ImGui::Separator();
//	return *bitset.data();
//}
//

void rage::BehavioursTreeNode(atArray<datRef<ptxBehaviour>>& behaviours, const char* name, size_t EntryIdx)
{
	if (ImGui::TreeNode(vfmt("{} ##{}", name, EntryIdx)))
	{
		for (size_t i = 0; i < behaviours.size(); i++)
		{
			auto& behaviour = behaviours[i].GetRef();	

			behaviour.UiWidgets(EntryIdx);

			//ImGui::Text("Name : %s", behaviour.GetName());	//ImGui::Text("hash : %i", behaviour.m_hashName);
			//ImGui::Text("Order : %i", behaviour.GetOrder());

			////ImGui::Text("CpuUpdateTicks : %i", behaviour.m_accumCPUUpdateTicks);

			//if (ImGui::TreeNode(vfmt("KeyframesList {}##{}",i,EntryIdx)))
			//{
			//	auto& KeyframeProp_list = behaviour.m_keyframePropList.m_pKeyframeProps;

			//	for (size_t j = 0; j < KeyframeProp_list.size(); j++)
			//	{
			//		auto& keyframeProp = KeyframeProp_list[j].GetRef();

			//		//ImGui::Text("Property Id : %i", keyframeProp.m_propertyId);
			//		ImGui::Text("Property Name : %s", keyframeProp.getPropertyName());
			//		
			//		ImGui::Checkbox(vfmt("Invert Bias Link##{}", NextUniqueNum()), &keyframeProp.m_invertBiasLink);
			//		ptxKeyframePropTreeNode(keyframeProp, vfmt("KeyframeProp {} ## ptxBehaviour",j), EntryIdx, PTX_KF_FLOAT4);
			//	}
			//	ImGui::TreePop();
			//}
			//ImGui::Separator();
		}
		ImGui::TreePop();
	}
}


void ptxSpawnedEffectScalarsWidgets(ptxSpawnedEffectScalars& scalars, const char* name)
{
    ImGui::DragFloat(vfmt("Duration Scalar {}##{}",name,NextUniqueNum()), & scalars.m_durationScalar, 0.01f);
    ImGui::DragFloat(vfmt("Playback Rate Scalar {}##{}", name, NextUniqueNum()), &scalars.m_playbackRateScalar, 0.01f);
	ImGui::vColor32Edit4(vfmt("Colour Tint Scalar {}##{}", name, NextUniqueNum()), scalars.m_colourTintScalar, ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar );
	ImGui::DragFloat(vfmt("Zoom Scalar {}##{}", name, NextUniqueNum()), &scalars.m_zoomScalar, 0.01f);
	ImGui::Text(vfmt("Flags : {:08b}", u32(scalars.m_flags)));
}

void rage::ptxEffectSpawnerTreeNode(ptxEffectSpawner& spawner, const char* name, size_t EntryIdx)
{
	if (ImGui::TreeNode(vfmt("{}##{}", name, EntryIdx)))
	{
		ptxSpawnedEffectScalarsWidgets(spawner.m_spawnedEffectScalarsMin, "Min");
		ptxSpawnedEffectScalarsWidgets(spawner.m_spawnedEffectScalarsMax, "Max");	

		ImGui::Text(spawner.m_name);

		ImGui::DragFloat(vfmt("TriggerInfo##{}", NextUniqueNum()), &spawner.m_triggerInfo);

		ImGui::Checkbox(vfmt("InheritsPointLife##{}", NextUniqueNum()), &spawner.m_inheritsPointLife);
		ImGui::Checkbox(vfmt("TracksPointPos##{}", NextUniqueNum()), &spawner.m_tracksPointPos);
		ImGui::Checkbox(vfmt("TracksPointDir##{}", NextUniqueNum()), &spawner.m_tracksPointDir);
		ImGui::Checkbox(vfmt("TracksPointNegDir##{}", NextUniqueNum()), &spawner.m_tracksPointNegDir);

		ImGui::TreePop();
	}
	
}


void rage::PtxDomainTreeNode(ptxDomainObj& DomainObj, const char* name, size_t EntryIdx)
{
	auto* ptr = DomainObj.m_pDomain.Get();
	if (!ptr)
	{
		ImGui::Text(vfmt("{} : NULL", name));
		return;
	}

	auto& domain = DomainObj.m_pDomain.GetRef();


	if (ImGui::TreeNode(vfmt("{}_domain_{}", name, EntryIdx)))
	{
		ImGui::Text("FileVersion %f", domain.m_fileVersion);

		ImGui::InputInt(vfmt("Type_{}", NextUniqueNum()),					&domain.m_type);
		ImGui::InputInt(vfmt("Shape_{}", NextUniqueNum()),					&domain.m_shape);

		ImGui::Checkbox(vfmt("IsWorldSpace ##{}", NextUniqueNum()),			&domain.m_isWorldSpace);
		ImGui::Checkbox(vfmt("IsPointRelative ##{}", NextUniqueNum()),		&domain.m_isPointRelative);
		ImGui::Checkbox(vfmt("IsCreationRelative ##{}", NextUniqueNum()),	&domain.m_isCreationRelative);
		ImGui::Checkbox(vfmt("IsTargetRelatve ##{}", NextUniqueNum()),		&domain.m_isTargetRelative);

		ptxKeyframePropTreeNode(domain.m_positionKFP, "PositionKFP##Domain", EntryIdx, PTX_KF_FLOAT4);
		ptxKeyframePropTreeNode(domain.m_rotationKFP, "RotationKFP##Domain", EntryIdx, PTX_KF_FLOAT4);
		ptxKeyframePropTreeNode(domain.m_sizeOuterKFP, "SizeOuterKFP##Domain", EntryIdx, PTX_KF_FLOAT4);
		ptxKeyframePropTreeNode(domain.m_sizeInnerKFP, "SizeInnerKFP##Domain", EntryIdx, PTX_KF_FLOAT4);

		ImGui::TreePop();
	}

}


void rage::ptxKeyframePropTreeNode(
	ptxKeyframeProp& ptxKfProp, 
	const char* name, 
	size_t EntryIdx, 
	ptxKeyframeType type, 
	const char* lb1 ,
	const char* lb2,
	const char* lb3,
	const char* lb4)
{
	if (ImGui::TreeNode(vfmt("{}## {}", name, EntryIdx)))
	{
		ImGui::Checkbox(vfmt("Invert Bias Link## fkProp {}", NextUniqueNum()), &ptxKfProp.m_invertBiasLink);
		//ImguiDragInt_FromDiffType(vfmt("Rand Index## fkProp {} {}", EntryIdx, NameId), v1, ptxKfProp.m_randIndex);
		
		if (ImGui::TreeNode(vfmt("Keyframes ## fkProp {}", EntryIdx)))
		{
			auto& keyframes = ptxKfProp.m_keyframe.data;

			for (size_t j = 0; j < keyframes.size(); j++)
			{
				auto& CurrKf = keyframes[j];
				ImGui::Separator();
				ImGui::DragFloat4(vfmt("KeyframeTime ## kfProp {}", NextUniqueNum()), CurrKf.vTime, v_speed);
				
				const char* vValue_Label = vfmt("KeyframeValue ## kfProp {}", NextUniqueNum());
				switch (type)
				{
				case PTX_KF_FLOAT:
					
					ImGui::DragFloat(vValue_Label, CurrKf.vValue, v_speed);
					break;

				case PTX_KF_FLOAT2:
					ImGui::DragFloat2(vValue_Label, CurrKf.vValue, v_speed);
					break;

				case PTX_KF_FLOAT3:
					ImGui::DragFloat3(vValue_Label, CurrKf.vValue, v_speed);
					break;

				case PTX_KF_FLOAT4:
					ImGui::DragFloat4(vValue_Label, CurrKf.vValue, v_speed);
					break;

				case PTX_KF_COL3:
					ImGui::ColorEdit3(vValue_Label, CurrKf.vValue, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_HDR);
					break;

				case PTX_KF_COL4:
					ImGui::ColorEdit4(vValue_Label, CurrKf.vValue, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_HDR);
					break;

				default:
					break;
				}
			}
			ImGui::Separator();

			if (ImGui::Button("Append Item")) 
			{
				keyframes.push_back(ptxKeyframeEntry());
			}

			if (ImGui::Button("Clear"))
			{
				keyframes.clear();
			}



			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}




ParticlesUi::~ParticlesUi()
{
	scrInvoke([]
		{
			STREAMING::_REMOVE_NAMED_PTFX_ASSET("core");
		});
}

//// name						// type			// default				//			xmin    xmax  xdelta				// ymin	  ymax  ydelta			// labels
//static ptxKeyframeDefn s_ColourTintMinKFP_Defn("Colour Tint Min KFP", PTX_KF_COL4, rage::Vector4(1.0f), rage::Vector3(0.0f, 1.0f, 0.01f));
//static ptxKeyframeDefn s_ColourTintMaxKFP_Defn("Colour Tint Max KFP", PTX_KF_COL4, rage::Vector4(1.0f), rage::Vector3(0.0f, 1.0f, 0.01f));
//static ptxKeyframeDefn s_ZoomScalarKFP_Defn("Zoom Scalar KFP", PTX_KF_FLOAT2, rage::Vector4(100.0f), rage::Vector3(0.0f, 1.0f, 0.01f), rage::Vector3(0.0f, 10000.0f, 0.01f), "Min", "Max");
//static ptxKeyframeDefn s_DataSphereKFP_Defn("Data Sphere KFP", PTX_KF_FLOAT4, rage::Vector4(0.0f), rage::Vector3(0.0f, 1.0f, 0.01f), rage::Vector3(1000.0f, 1000.0f, 0.01f), "Pos X", "Pos Y", "Pos Z", "Radius");
//static ptxKeyframeDefn s_DataSphereKFP_Defn("Data Sphere KFP", PTX_KF_FLOAT4, rage::Vector4(0.0f), rage::Vector3(0.0f, 1.0f, 0.01f), rage::Vector3(1000.0f, 1000.0f, 0.01f), "Rot X", "Rot Y", "Rot Z", "Length");

