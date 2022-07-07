// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Installations/FirebaseInstallations.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FirebaseInstallationsNode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDynMultInstallID, const FString&, ID, int32, ErrorCode, const FString&, ErrorMessage);

UCLASS()
class UFirebaseGetInstallationIDProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Installations ID received.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultInstallID Done;

	/**
	 * Failed to get the installations ID.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultInstallID Failed;

public:

	/**
	 * The method creates or retrieves an installation ID. The installation ID is a stable identifier
	 * that uniquely identifies the app instance. NOTE: If the application already has an existing
	 * FirebaseInstanceID then the InstanceID identifier will be used.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Installations", meta = (BlueprintInternalUseOnly = "true"))
	static UFirebaseGetInstallationIDProxy* GetInstallationID();

	virtual void Activate() override;

private:
	void OnTaskOver(FString ID , int32 Error, FString ErrorMessage);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDynMultInstallAuthToken, const FFirebaseInstallationsAuthToken&, AuthToken, int32, ErrorCode, const FString&, ErrorMessage);

UCLASS()
class UFirebaseGetInstallationAuthTokenProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Auth Token received.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultInstallAuthToken Done;

	/**
	 * Failed to get the installation Auth Token.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultInstallAuthToken Failed;

public:

	/**
	 * Retrieves (locally or from the server depending on `forceRefresh` value) a valid authorization
	 * token. An existing token may be invalidated or expire, so it is recommended to fetch the auth
	 * token before each server request. This method should be used with `forceRefresh == YES` when e.g.
	 * a request with the previously fetched auth token failed with "Not Authorized" error.
	 * @param forceRefresh If `YES` then the locally cached auth token will be ignored and a new one
	 * will be requested from the server. If `NO`, then the locally cached auth token will be returned
	 * if exists and has not expired yet.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Installations", meta = (BlueprintInternalUseOnly = "true"))
	static UFirebaseGetInstallationAuthTokenProxy* GetInstallationAuthToken(const bool bForceRefresh);

	virtual void Activate() override;

private:
	void OnTaskOver(FFirebaseInstallationsAuthToken Token, int32 Error, FString ErrorMessage);

	bool bForceRefresh;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultInstallDelete, int32, Error, const FString&, ErrorMessage);

UCLASS()
class UFirebaseDeleteInstallationProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Installation data deleted.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultInstallDelete Done;

	/**
	 * Failed to delete the installation data.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultInstallDelete Failed;

public:

	/**
	 * Deletes all the installation data including the unique identifier, auth tokens and
	 * all related data on the server side. A network connection is required for the method to
	 * succeed. If fails, the existing installation data remains untouched.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Installations", meta = (BlueprintInternalUseOnly = "true"))
	static UFirebaseDeleteInstallationProxy* DeleteInstallation();

	virtual void Activate() override;

private:
	void OnTaskOver(int32 Error, FString ErrorMessage);
};
