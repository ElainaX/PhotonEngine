#include "Engine.h"
#include "Editor.h"
#include "Function/Render/WindowSystem.h"
#include <windows.h>
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    auto* photonEngine = new photon::PhotonEngine();

    photon::WindowCreateInfo wndCreateInfo{1920, 1080};
    photonEngine->StartEngine(wndCreateInfo);
    
    auto* photonEditor = new photon::PhotonEditor();
    photonEditor->Initialize(photonEngine);

    photonEditor->Run();

    photonEditor->clear();

    photonEngine->ShutDownEngine();


    delete photonEditor;
    delete photonEngine;
}