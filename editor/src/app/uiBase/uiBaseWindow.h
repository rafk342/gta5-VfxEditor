#pragma once
#include "ImGui/imgui.h"
#include "FileListUI.h"
#include <format>

class BaseUiWindow;

class App
{
    const char* label;
public:
    App(const char* label);
    virtual ~App() {};

    const char* get_label();
   
    virtual void window() = 0;
    virtual void importData(std::filesystem::path path) = 0;
    virtual void exportData(std::filesystem::path path) = 0;
};

class BaseUiWindow : private FileListUI
{
    friend std::unique_ptr<BaseUiWindow> std::make_unique<BaseUiWindow>();
    friend std::default_delete<BaseUiWindow>;
    static std::unique_ptr<BaseUiWindow> selfInstance;

    const char* MainWindowLabel = "VisualEffects Editor";
   
    std::vector<std::unique_ptr<App>>   appsVec;
    App*                                activeApp = nullptr;

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
