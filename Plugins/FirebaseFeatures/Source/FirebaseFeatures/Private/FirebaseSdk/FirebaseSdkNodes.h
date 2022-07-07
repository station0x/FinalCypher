// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "FirebaseFeatures.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FirebaseSdkNodes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynMulOnFbInit);

UENUM()
enum class EFirebaseSDKCreateApp : uint8
{
	Success,
	Failed
};

UCLASS()
class UFirebaseSDKLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * Creates a new Firebase Application.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase|SDK", Meta = (ExpandEnumAsExecs = "Branches"))
	static void CreateFirebaseApp(const FString& AppName, const FFirebaseAppOptions& Options, EFirebaseSDKCreateApp& Branches);

	/**
	 * Switches the current Firebase App for the specified one.
	 * To switch back to the default app, an empty AppName must be specified.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase|SDK", Meta = (ExpandEnumAsExecs = "Branches"))
	static void SwitchFirebaseApp(const FString& AppName, EFirebaseSDKCreateApp& Branches);
};

UCLASS()
class UFirebaseInitializationProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMulOnFbInit FirebaseInitialized;

public:
	UFUNCTION(BlueprintCallable, Category = "Firebase", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Wait for Firebase SDK Initialization"))
	static UFirebaseInitializationProxy* WaitForInitialization();

	virtual void Activate() override;

private:
	void OnSdkInitialized();

};
