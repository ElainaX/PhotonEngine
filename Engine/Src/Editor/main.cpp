#include "Engine.h"
#include <windows.h>
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    auto* photonEngine = new photon::PhotonEngine();

    photonEngine->StartEngine();
    
    photonEngine->Run();

    photonEngine->ShutDownEngine();

    delete photonEngine;
}