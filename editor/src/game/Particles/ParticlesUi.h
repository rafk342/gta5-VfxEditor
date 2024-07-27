#pragma once
#include <iostream>

#include "uiBase/uiBaseWindow.h"
#include "scripthookTh.h"

#include "ImGui/imgui.h"
#include "helpers/helpers.h"
#include "ptxUiUniqueNums.h"
#include "ptxList.h"


namespace rage
{
    struct PtxMngr
    {
        u8 pad[0x2D0];
        atMap<const char*, ptxFxListInfo> m_fxListMap;
    };

    void ptxKeyframePropTreeNode(ptxKeyframeProp& ptxKfProp, const char* name, size_t EntryIdx, ptxKeyframeType type, const char* lb1 = "Unnamed1", const char* lb2 = "Unnamed2", const char* lb3 = "Unnamed3", const char* lb4 = "Unnamed4");
    void PtxDomainTreeNode(ptxDomainObj& DomainObj, const char* name, size_t EntryIdx);
    void ptxEffectSpawnerTreeNode(ptxEffectSpawner& spawner, const char* name, size_t EntryIdx);
    void BehavioursTreeNode(atArray<datRef<ptxBehaviour>>& behaviours, const char* name, size_t EntryIdx);
}

class ParticlesUi : public App
{
    rage::PtxMngr* ptr = nullptr;
public:
    ParticlesUi(const char* title);
    ~ParticlesUi();

    virtual void window() override;
};



