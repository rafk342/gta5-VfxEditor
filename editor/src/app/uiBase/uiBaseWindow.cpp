#include "uiBaseWindow.h"

#include "timecycle/ui/timecycleUI_impl.h"
#include "CloudSettings/ui/CloudSettingsUI.h"
#include "gameClock/Ui/ClockUi.h"
#include "VfxLightningSettings/ui/VfxLightningUi.h"
#include "VisualSettings/ui/visualSettingsUi.h"


BaseUiWindow* BaseUiWindow::instance = nullptr;

App::App(BaseUiWindow* base, const char* label) : label(label)
{
    base->PushApp(this);
}

BaseUiWindow::BaseUiWindow()
{
    // these instances should be static 
    // so they won't be local and they won't be deleted on the constructor exiting
    
    static TimecycleUI TcUi(this, "Timecycles");
    static CloudSettingsUI ClUi(this, "Cloudkeyframes");
    static VisualSettingsUi VsUi(this, "VisualSettings");
    static VfxLightningUi LghtUi(this, "Lightnings");
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
    ClockUi::timeCheckBoxWindow();

    if (ImGui::BeginTabBar("TabBarEditor"))
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
    
    if (!path.empty())
        activeApp->exportData(path);

    io.WantCaptureKeyboard = true;
    io.WantCaptureMouse = true;
}

void BaseUiWindow::SaveBtn()
{
    activeApp->exportData(getPathOfSelectedFile());
}


void BaseUiWindow::Create()
{
    if (!instance) {
        instance = new BaseUiWindow;
    }
}

void BaseUiWindow::Destroy()
{
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

BaseUiWindow* BaseUiWindow::GetInstance()
{
    return instance;
}

BaseUiWindow::~BaseUiWindow() {}

const char* App::get_label()
{
    return label;
}
