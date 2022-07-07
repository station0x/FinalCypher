// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FirebaseInstallations.generated.h"

/**
 * Represents a result of the installation auth token request.
*/
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseInstallationsAuthToken
{
	GENERATED_BODY()
public:
	/**
	 * The installation auth token string.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Firebase|Installations")
	FString AuthToken;

	/**
	 * The installation auth token expiration date.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Firebase|Installations")
	FDateTime Expiration;
};

DECLARE_DELEGATE_ThreeParams(FFirebaseInstallationIDCallback, FString /* ID */, int32 /* Error */, FString /* ErrorMessage */);

DECLARE_DELEGATE_ThreeParams(FFirebaseInstallationAuthTokenCallback, FFirebaseInstallationsAuthToken /* AuthToken */, int32 /* Error */, FString /* ErrorMessage */);

DECLARE_DELEGATE_TwoParams(FFirebaseInstallationDeleteCallback, int32 /* Error */, FString /* ErrorMessage */);

UCLASS()
class FIREBASEFEATURES_API UFirebaseInstallations : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * The method creates or retrieves an installation ID. The installation ID is a stable identifier
	 * that uniquely identifies the app instance. NOTE: If the application already has an existing
	 * FirebaseInstanceID then the InstanceID identifier will be used.
	 * @param completion A completion handler which is invoked when the operation completes. See
	 * `InstallationsIDHandler` for additional details.
	 */
	static void GetInstallationID(FFirebaseInstallationIDCallback Callback);

	/**
	 * Retrieves (locally if it exists or from the server) a valid authorization token. An existing
	 * token may be invalidated or expired, so it is recommended to fetch the auth token before each
	 * server request. The method does the same as `Installations.authTokenForcingRefresh(:,
	 * completion:)` with forcing refresh `NO`.
	 * @param Callback A completion handler which is invoked when the operation completes. 
	 */
	static void GetAuthToken(FFirebaseInstallationAuthTokenCallback Callback);

	/**
	 * Retrieves (locally or from the server depending on `forceRefresh` value) a valid authorization
	 * token. An existing token may be invalidated or expire, so it is recommended to fetch the auth
	 * token before each server request. This method should be used with `forceRefresh == YES` when e.g.
	 * a request with the previously fetched auth token failed with "Not Authorized" error.
	 * @param forceRefresh If `YES` then the locally cached auth token will be ignored and a new one
	 * will be requested from the server. If `NO`, then the locally cached auth token will be returned
	 * if exists and has not expired yet.
	 * @param Callback A completion handler which is invoked when the operation completes. 
	 */
	static void GetAuthToken(const bool bForceRefresh, FFirebaseInstallationAuthTokenCallback Callback);

	/**
	 * Deletes all the installation data including the unique identifier, auth tokens and
	 * all related data on the server side. A network connection is required for the method to
	 * succeed. If fails, the existing installation data remains untouched.
	 * @param Callback A completion handler which is invoked when the operation completes. `error ==
	 * 0` indicates success.
	 */
	static void Delete(FFirebaseInstallationDeleteCallback Callback = {});
};

