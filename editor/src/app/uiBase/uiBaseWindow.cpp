#include "uiBaseWindow.h"

#include "timecycle/ui/timecycleUI_impl.h"
#include "CloudSettings/ui/CloudSettingsUI.h"
#include "gameClock/Ui/ClockUi.h"
#include "VfxLightningSettings/ui/VfxLightningUi.h"
#include "VisualSettings/ui/visualSettingsUi.h"
#include "bloodfx/gBloodFxReloader.h"

BaseUiWindow* BaseUiWindow::self = nullptr;

App::App(const char* title) : title(title) {}

BaseUiWindow::BaseUiWindow()
{
    appsVec.push_back(std::make_unique<TimecycleUI>("Timecycles"));
    appsVec.push_back(std::make_unique<CloudSettingsUI>("Cloudkeyframes"));
    appsVec.push_back(std::make_unique<VisualSettingsUi>("VisualSettings"));
    appsVec.push_back(std::make_unique<VfxLightningUi>("Lightnings"));
    appsVec.push_back(std::make_unique<gBloodfxUi>("Bloodfx"));
}
    
const char* App::get_title()
{
    return title;
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
    ImGui::Begin(MainWindowTitle);

    file_section();
    ImGui::SameLine();
    ClockUi::timeWindowCheckBox();

    if (ImGui::BeginTabBar("TabBarEditor", ImGuiTabBarFlags_Reorderable))
    {
        for (auto& app : appsVec)
        {
            if (ImGui::BeginTabItem(app->get_title()))
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
    if (!self) {
        self = new BaseUiWindow();
    }
}

void BaseUiWindow::Destroy()
{
    if (self) {
      delete self;
    }
}

BaseUiWindow* BaseUiWindow::GetInstance()
{
    return self;
}

BaseUiWindow::~BaseUiWindow() {}
