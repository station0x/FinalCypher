// Corentin GUILLAUME 2020  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Windows/MinWindows.h"
#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <sstream>
#include <string>
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CompilerBoosterBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class COMPILERBOOSTER_API UCompilerBoosterBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		UFUNCTION(BlueprintCallable, Category = "ProcessFunctions", meta = (WorldContext = WorldContextObject))
		static void ProcessPriority(const int P1PrioIndex, const FString P1Name, const int P2PrioIndex, const FString P2Name, const bool P1SetPrio, const bool P2SetPrio,  bool &ProcessFound);

	UFUNCTION(BlueprintCallable, Category = "ProcessFunctions", meta = (WorldContext = WorldContextObject))
		static void IsShaderCompiling(bool &P1Found, bool &P2Found, const FString P1Name, const FString P2Name, FString &P1Prio, FString &P2Prio);
};
