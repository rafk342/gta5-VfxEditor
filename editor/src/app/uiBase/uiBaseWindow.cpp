#include "uiBaseWindow.h"

#include "timecycle/ui/timecycleUI_impl.h"
#include "CloudSettings/ui/CloudSettingsUI.h"
#include "gameClock/Ui/ClockUi.h"
#include "VfxLightningSettings/ui/VfxLightningUi.h"
#include "VisualSettings/ui/visualSettingsUi.h"


//BaseUiWindow* BaseUiWindow::instance = nullptr;

std::unique_ptr<BaseUiWindow> BaseUiWindow::selfInstance = nullptr;


App::App(BaseUiWindow* base, const char* label) : label(label)
{
    base->PushApp(this);
}

BaseUiWindow::BaseUiWindow()
{
    // these instances should be static 
    // so they won't be local and they won't be deleted on the constructor exiting

    static std::unique_ptr<TimecycleUI>      TcUi   = std::make_unique<TimecycleUI> (this, "Timecycles");
    static std::unique_ptr<CloudSettingsUI>  ClUi   = std::make_unique<CloudSettingsUI>(this, "Cloudkeyframes");
    static std::unique_ptr<VisualSettingsUi> VsUi   = std::make_unique<VisualSettingsUi>(this, "VisualSettings");
    static std::unique_ptr<VfxLightningUi>   LghtUi = std::make_unique<VfxLightningUi>(this, "Lightnings");
}

const char* App::get_label()
{
    return label;
}

void BaseUiWindow::setActiveApp(App* app) 
{
    activeApp = app;
}

void BaseUiWindow::PushApp(App* app) 
{ 
    appsVec.push_back(app);
}

App* BaseUiWindow::getActiveApp() const 
{
    return activeApp; 
}

void BaseUiWindow::OnRender()
{
    ImGui::SetNextWindowSize({ 712 ,885 }, ImGuiCond_FirstUseEver);
    ImGui::Begin(MainWindowLabel);

    file_section();
    ImGui::SameLine();
    ClockUi::timeWindowCheckBox();

    if (ImGui::BeginTabBar("TabBarEditor", ImGuiTabBarFlags_Reorderable))
    {
        for (App* app : appsVec)
        {
            if (ImGui::BeginTabItem(app->get_label()))
            {
                if (ImGui::BeginChild("MainParamsWindow_Base", ImVec2(-FLT_MIN, -FLT_MIN), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar))
                {
                    setActiveApp(app);
                    app->window();
                    ImGui::EndChild();
                }
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                          export/import


void BaseUiWindow::LoadSelectedBtn()
{
    activeApp->importData(getPathOfSelectedFile());
}

void BaseUiWindow::SaveAsBtn()
{
    ImGuiIO& io = ImGui::GetIO();
    io.WantCaptureKeyboard = false;
    io.WantCaptureMouse = false;

    auto path = selectPathForSaveAsBtn();
    
    if (!path.empty() && path.has_filename())
        activeApp->exportData(path);

    io.WantCaptureKeyboard = true;
    io.WantCaptureMouse = true;
}

void BaseUiWindow::SaveBtn()
{
    activeApp->exportData(getPathOfSelectedFile());
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                          singleton stuff


void BaseUiWindow::Create()
{
    if (!selfInstance) {
        selfInstance = std::make_unique<BaseUiWindow>();
    }
}

void BaseUiWindow::Destroy()
{
    if (selfInstance) {
        selfInstance.reset();
    }
}

BaseUiWindow* BaseUiWindow::GetInstance()
{
    return selfInstance.get();
}

BaseUiWindow::~BaseUiWindow() {}
