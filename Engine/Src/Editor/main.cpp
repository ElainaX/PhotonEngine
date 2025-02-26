#include "Engine.h"

int main()
{
    auto* photonEngine = new photon::PhotonEngine();

    photonEngine->StartEngine();
    
    photonEngine->Run();

    photonEngine->ShutDownEngine();

    delete photonEngine;
}