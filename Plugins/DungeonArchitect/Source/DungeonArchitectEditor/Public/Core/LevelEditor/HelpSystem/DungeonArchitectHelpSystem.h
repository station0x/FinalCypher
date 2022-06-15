//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"

class FDungeonArchitectHelpSystem {
public:
    void Initialize();
    void Release();

    static void LaunchHelpSystemTab();

private:
    TSharedRef<class SDockTab> SpawnHelpSystemTab(const FSpawnTabArgs& Args);

private:
    TSharedPtr<class FDAHelpMenuExtender> MenuExtender;
};

