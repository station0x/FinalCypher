// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseSdk/FirebaseErrors.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Storage.generated.h"

class UFirebaseStorageReference;

DECLARE_DELEGATE_OneParam(FFirebaseStorageCallback, const EFirebaseStorageError);

UCLASS()
class FIREBASEFEATURES_API UFirebaseStorage : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/// @brief Get the URL that this Storage was created with.
	///
	/// @returns The URL this Storage was created with, or an empty
	/// string if this Storage was created with default parameters.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage")
	static UPARAM(DisplayName = "URL") FString GetUrl();

	/// @brief Get a StorageReference to the root of the database.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage")
	static UPARAM(DisplayName = "Reference") UFirebaseStorageReference* GetReference();

	/// @brief Get a StorageReference for the specified path.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage")
	static UPARAM(DisplayName = "Reference") UFirebaseStorageReference* GetReferenceFromPath(const FString& Path);

	/// @brief Get a StorageReference for the provided URL.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage")
	static UPARAM(DisplayName = "Reference") UFirebaseStorageReference* GetReferenceFromUrl(const FString& Url);

	/// @brief Returns the maximum time in seconds to retry a download if a
	/// failure occurs.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage")
	static UPARAM(DisplayName = "Time") float GetMaxDownloadRetryTime();

	/// @brief Sets the maximum time to retry a download if a failure occurs.
	/// Defaults to 600 seconds (10 minutes).
	UFUNCTION(BlueprintCallable, Category = "Firebase|Storage")
	static void SetMaxDownloadRetryTime(float MaxTransferRetrySeconds);

	/// @brief Returns the maximum time to retry an upload if a failure occurs.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage")
	static UPARAM(DisplayName = "Time") float GetMaxUploadRetryTime();
	
	/// @brief Sets the maximum time to retry an upload if a failure occurs.
	/// Defaults to 600 seconds (10 minutes).
	UFUNCTION(BlueprintCallable, Category = "Firebase|Storage")
	static void SetMaxUploadRetryTime(float MaxTransferRetrySeconds);

	/// @brief Returns the maximum time to retry operations other than upload
	/// and download if a failure occurs.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage")
	static UPARAM(DisplayName = "Time") float GetMaxOperationRetryTime();

	/// @brief Sets the maximum time to retry operations other than upload and
	/// download if a failure occurs. Defaults to 120 seconds (2 minutes).
	UFUNCTION(BlueprintCallable, Category = "Firebase|Storage")
	static void SetMaxOperationRetryTime(float MaxTransferRetrySeconds);
};

