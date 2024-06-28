#pragma once
#include "ImGui/imgui.h"
#include "FileListUI.h"
#include <format>

class BaseUiWindow;

class App
{
    const char* title;
public:
    App(const char* title);
    virtual ~App() {};

    const char* get_title();
   
    virtual void window() = 0;
    virtual void importData(std::filesystem::path path) = 0;
    virtual void exportData(std::filesystem::path path) = 0;
};

class BaseUiWindow : private FileListUI
{
    using AppsVecType = std::vector<std::unique_ptr<App>>;
    static BaseUiWindow* self;

    const char* MainWindowTitle = "VisualEffects Editor";
    AppsVecType appsVec;
    App*        activeApp = nullptr;

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

    void OnRender();
};
