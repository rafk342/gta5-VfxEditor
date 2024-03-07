#pragma once
#include "ImGui/imgui.h"
#include "FileListUI.h"

class BaseUiWindow;

class App
{
    const char* label;
public:
    App(BaseUiWindow* base, const char* label);

    const char*  get_label();
    virtual void window() = 0;

    virtual void importData(std::string path) = 0;
    virtual void exportData(std::string path) = 0;
};

class BaseUiWindow : FileListUI
{
    static BaseUiWindow* instance;

    const char*         MainWindowLabel = "VisualSettings Editor";
    std::vector<App*>   appsVec;
    App*                activeApp = nullptr;
    bool                renderState = false;
   
    void setActiveApp(App* app);
    App* getActiveApp() const;

    virtual void LoadSelectedBtn() override;
    virtual void SaveAsBtn() override;
    virtual void SaveBtn() override;
    
    BaseUiWindow();
    ~BaseUiWindow();

public:

    static void Create();
    static void Destroy();
    static BaseUiWindow* GetInstance();

    void PushApp(App* app);
    void OnRender();
};
