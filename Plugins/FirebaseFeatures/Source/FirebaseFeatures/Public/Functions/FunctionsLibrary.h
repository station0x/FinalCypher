// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Functions/CallableReference.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FunctionsLibrary.generated.h"

/// @brief Entry point for the Firebase C++ SDK for Cloud Functions.
///
/// To use the SDK, use GetHttpsCallable() to obtain
/// references to callable functions. From there you can call them with
/// FFirebaseHttpsCallableReference::Call().
UCLASS()
class FIREBASEFEATURES_API UFirebaseFunctionsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/// @brief Sets an origin for a Cloud Functions emulator to use.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Functions")
	static void UseFunctionsEmulator(const FString& Origin);
	static void UseFunctionsEmulator(const char* Origin);

	/// @brief Get a FunctionsReference for the specified path.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Functions")
	static UPARAM(DisplayName = "HTTPS Callable") FFirebaseHttpsCallableReference GetHttpsCallable(const FString& Name);
	static FFirebaseHttpsCallableReference GetHttpsCallable(const char* Name);
	
	/// @brief Get a FunctionsReference for the specified path.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Functions")
	static UPARAM(DisplayName = "HTTPS Callable") FFirebaseHttpsCallableReference GetHttpsCallableWithRegion(const FString& Name, const FString& Region);
	static FFirebaseHttpsCallableReference GetHttpsCallableWithRegion(const char* Name, const char* Region);
};
