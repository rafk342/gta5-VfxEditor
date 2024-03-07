#include "uiBaseWindow.h"

#include "timecycle/ui/timecycleUI_impl.h"
#include "CloudSettings/ui/CloudSettingsUI.h"
#include "gameClock/Ui/ClockUi.h"

BaseUiWindow* BaseUiWindow::instance = nullptr;

App::App(BaseUiWindow* base, const char* label) : label(label)
{
    base->PushApp(this);
}

BaseUiWindow::BaseUiWindow()
{
    // these instances should be static 
    // so they won't be local and they won't be deleted on the constructor exiting

    static timecycleUI tcUi(this, "Timecycles");
    static CloudSettingsUI clUi(this, "Cloudkeyframes");
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
    renderState = true;

    ImGui::SetNextWindowSize({ 712 ,885 }, ImGuiCond_FirstUseEver);

    ImGui::Begin(MainWindowLabel);

    file_section();
    ImGui::SameLine();
    ClockUi::timeWindow();

    if (ImGui::BeginTabBar("TabBarEditor"))
    {
        for (App* app : appsVec)
        {
            if (ImGui::BeginTabItem(app->get_label()))
            {
                if (ImGui::BeginChild("MainParamsWindow", ImVec2(-FLT_MIN, -FLT_MIN), ImGuiChildFlags_Border))
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
   
    renderState = false;
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

    activeApp->exportData(selectPathForSaveAsBtn());

    io.WantCaptureKeyboard = true;
    io.WantCaptureMouse = true;
}

void BaseUiWindow::SaveBtn()
{
    activeApp->exportData(getPathOfSelectedFile());
}


void BaseUiWindow::Create()
{
    if (instance == nullptr) {
        instance = new BaseUiWindow;
    }
}

void BaseUiWindow::Destroy()
{
    while (instance->renderState) {}

    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

BaseUiWindow* BaseUiWindow::GetInstance()
{
    if (instance == nullptr) {
        return nullptr;
    }
    return instance;
}

BaseUiWindow::~BaseUiWindow()
{
}

const char* App::get_label()
{
    return label;
}
