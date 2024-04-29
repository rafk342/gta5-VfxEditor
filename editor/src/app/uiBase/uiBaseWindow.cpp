#include "uiBaseWindow.h"

#include "timecycle/ui/timecycleUI_impl.h"
#include "CloudSettings/ui/CloudSettingsUI.h"
#include "gameClock/Ui/ClockUi.h"
#include "VfxLightningSettings/ui/VfxLightningUi.h"
#include "VisualSettings/ui/visualSettingsUi.h"


std::unique_ptr<BaseUiWindow> BaseUiWindow::selfInstance = nullptr;

App::App(const char* label) : label(label) {}

BaseUiWindow::BaseUiWindow()
{
    appsVec.push_back(std::make_unique<TimecycleUI>("Timecycles"));
    appsVec.push_back(std::make_unique<CloudSettingsUI>("Cloudkeyframes"));
    appsVec.push_back(std::make_unique<VisualSettingsUi>("VisualSettings"));
    appsVec.push_back(std::make_unique<VfxLightningUi>("Lightnings"));
}

const char* App::get_label()
{
    return label;
}

void BaseUiWindow::setActiveApp(App* app)
{
    activeApp = app;
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
        for (auto& app : appsVec)
        {
            if (ImGui::BeginTabItem(app->get_label()))
            {
                if (ImGui::BeginChild("MainParamsWindow_Base", ImVec2(-FLT_MIN, -FLT_MIN), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar))
                {
                    setActiveApp(app.get());
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
//                                          export/import


void BaseUiWindow::LoadSelectedBtn()
{
    auto path = getPathOfSelectedFile();
    if (!path.empty())
        activeApp->importData(path);
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
    auto path = getPathOfSelectedFile();
    if (!path.empty())
        activeApp->exportData(path);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          singleton stuff


void BaseUiWindow::Create()
{
    if (!selfInstance) {
        selfInstance = std::make_unique<BaseUiWindow>();
    }
}

void BaseUiWindow::Destroy()
{
    if (selfInstance) 
    {
        selfInstance.reset();
    }
}

BaseUiWindow* BaseUiWindow::GetInstance()
{
    return selfInstance.get();
}

BaseUiWindow::~BaseUiWindow() {}
