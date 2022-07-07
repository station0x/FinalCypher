// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Storage/StorageReference.h"
#include "StorageNodes.generated.h"

UENUM()
enum class EFirebaseStorageBlueprintExecutionPin : uint8
{
    Success,
    Failed
};

UCLASS()
class UStorageBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

    /// @brief Pauses the operation currently in progress.
    ///
    /// @returns True if the operation was successfully paused, false otherwise.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|Controller", Meta = (ExpandEnumAsExecs = "Branches"))
    static void Pause(UPARAM(ref) FFirebaseStorageController& Controller, EFirebaseStorageBlueprintExecutionPin& Branches);

    /// @brief Resumes the operation that is paused.
    ///
    /// @returns True if the operation was successfully resumed, false otherwise.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|Controller", Meta = (ExpandEnumAsExecs = "Branches"))
    static void Resume(UPARAM(ref) FFirebaseStorageController& Controller, EFirebaseStorageBlueprintExecutionPin& Branches);

    /// @brief Cancels the operation currently in progress.
    ///
    /// @returns True if the operation was successfully canceled, false otherwise.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|Controller", Meta = (ExpandEnumAsExecs = "Branches"))
    static void Cancel(UPARAM(ref) FFirebaseStorageController& Controller, EFirebaseStorageBlueprintExecutionPin& Branches);

    /// @brief Returns true if the operation is paused.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Controller")
    static UPARAM(DisplayName = "Is Paused") bool IsPaused(UPARAM(ref) FFirebaseStorageController& Controller);

    /// @brief Returns the number of bytes transferred so far.
    ///
    /// @returns The number of bytes transferred so far.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Controller")
    static UPARAM(DisplayName = "Transferred") int64 BytesTransferred(const FFirebaseStorageController& Controller);

    /// @brief Returns the total bytes to be transferred.
    ///
    /// @returns The total bytes to be transferred.  This will return -1 if
    /// the size of the transfer is unknown.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Controller")
    static UPARAM(DisplayName = "Total") int64 TotalByteCount(const FFirebaseStorageController& Controller);

    /// @brief Returns the StorageReference associated with this Controller.
    ///
    /// @returns The StorageReference associated with this Controller.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Controller")
    static UPARAM(DisplayName = "Storage Reference") UFirebaseStorageReference* GetReference(const FFirebaseStorageController& Controller);

    /// @brief Returns true if this Controller is valid, false if it is not
    /// valid. An invalid Controller is one that is not associated with an
    /// operation.
    ///
    /// @returns true if this Controller is valid, false if this Controller is
    /// invalid.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Controller")
    static UPARAM(DisplayName = "IsValid") bool IsValid(const FFirebaseStorageController& Controller);

    /// @brief Return the owning Google Cloud Storage bucket for the
    /// StorageReference.
    ///
    /// @returns The owning Google Cloud Storage bucket for the StorageReference.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Bucket") FString GetBucket(const FFirebaseStorageMetadata& Metadata);

    /// @brief Set the Cache Control setting of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc7234#section-5.2
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|Metadata")
    static void SetCachControl(UPARAM(ref) FFirebaseStorageMetadata& Metadata, const FString& CacheControl);

    /// @brief Return the Cache Control setting of the StorageReference.
    ///
    /// @returns The Cache Control setting of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc7234#section-5.2
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Cache Control") FString GetCacheControl(const FFirebaseStorageMetadata& Metadata);

    /// @brief Set the content disposition of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc6266
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|Metadata")
    static void SetContentDisposition(UPARAM(ref) FFirebaseStorageMetadata& Metadata, const FString& Disposition);

    /// @brief Return the content disposition of the StorageReference.
    ///
    /// @returns The content disposition of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc6266
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Disposition") FString GetContentDisposition(const FFirebaseStorageMetadata& Metadata);

    /// @brief Set the content encoding for the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc2616#section-14.11
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|Metadata")
    static void SetContentEncoding(UPARAM(ref) FFirebaseStorageMetadata& Metadata, const FString& Encoding);

    /// @brief Return the content encoding for the StorageReference.
    ///
    /// @returns The content encoding for the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc2616#section-14.11
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Encoding") FString GetContentEncoding(const FFirebaseStorageMetadata& Metadata);

    /// @brief Set the content language for the StorageReference.
    ///
    /// This must be an ISO 639-1 two-letter language code.
    /// E.g. "zh", "es", "en".
    ///
    /// @see https://www.loc.gov/standards/iso639-2/php/code_list.php
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|Metadata")
    static void SetContentLanguage(UPARAM(ref) FFirebaseStorageMetadata& Metadata, const FString& Language);

    /// @brief Return the content language for the StorageReference.
    ///
    /// @returns The content language for the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc2616#section-14.12
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Language") FString GetContentLanguage(const FFirebaseStorageMetadata& Metadata);

    /// @brief Set the content type of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc2616#section-14.17
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|Metadata")
    static void SetContentType(UPARAM(ref) FFirebaseStorageMetadata& Metadata, const FString& Type);

    /// @brief Return the content type of the StorageReference.
    ///
    /// @returns The content type of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc2616#section-14.17
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Content Type") FString ContentType(const FFirebaseStorageMetadata& Metadata);

    /// @brief Return the time the StorageReference was created in milliseconds
    /// since the epoch.
    ///
    /// @returns The time the StorageReference was created in milliseconds since
    /// the epoch.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Creation Time") int64 GetCreationTime(const FFirebaseStorageMetadata& Metadata);

    /// @brief Return a map of custom metadata key value pairs.
    ///
    /// The pointer returned is only valid during the lifetime of the Metadata
    /// object that owns it.
    ///
    /// @returns The keys for custom metadata.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Custom Metadata") TMap<FString, FString> GetCustomMetadata(const FFirebaseStorageMetadata& Metadata);

    /// @brief Return a version String indicating what version of the
    /// StorageReference.
    ///
    /// @returns A value indicating the version of the StorageReference.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Generation") int64 GetGeneration(const FFirebaseStorageMetadata& Metadata);

    /// @brief Return a version String indicating the version of this
    /// StorageMetadata.
    ///
    /// @returns A value indicating the version of this StorageMetadata.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Generation") int64 GetMetadataGeneration(const FFirebaseStorageMetadata& Metadata);

    /// @brief Return a simple name of the StorageReference object.
    ///
    /// @returns A simple name of the StorageReference object.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Name") FString GetName(const FFirebaseStorageMetadata& Metadata);

    /// @brief Return the path of the StorageReference object.
    ///
    /// @returns The path of the StorageReference object.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Path") FString GetPath(const FFirebaseStorageMetadata& Metadata);

    /// @brief Return the associated StorageReference to which this Metadata
    /// belongs.
    ///
    /// @returns The associated StorageReference to which this Metadata belongs.
    /// If this Metadata is invalid or is not associated with any file, an invalid
    /// StorageReference is returned.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata", meta = (DisplayName = "Get Reference"))
    static UPARAM(DisplayName = "Storage Reference") UFirebaseStorageReference* GetReference_Metadata(const FFirebaseStorageMetadata& Metadata);

    /// @brief Return the stored Size in bytes of the StorageReference object.
    ///
    /// @returns The stored Size in bytes of the StorageReference object.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Size") int64 GetSizeBytes(const FFirebaseStorageMetadata& Metadata);

    /// @brief Return the time the StorageReference was last updated in
    /// milliseconds since the epoch.
    ///
    /// @return The time the StorageReference was last updated in milliseconds
    /// since the epoch.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Updated Time") int64 GetUpdatedTime(const FFirebaseStorageMetadata& Metadata);

    /// @brief Returns true if this Metadata is valid, false if it is not
    /// valid. An invalid Metadata is returned when a method such as
    /// StorageReference::GetMetadata() completes with an error.
    ///
    /// @returns true if this Metadata is valid, false if this Metadata is
    /// invalid.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata", meta = (DisplayName = "Is Valid"))
    static UPARAM(DisplayName = "Is Valid") bool IsValid_Metadata(const FFirebaseStorageMetadata& Metadata);

    /// @brief MD5 hash of the data; encoded using base64.
    ///
    /// @returns MD5 hash of the data; encoded using base64.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|Metadata")
    static UPARAM(DisplayName = "Hash") FString GetMd5Hash(const FFirebaseStorageMetadata& Metadata);
};

UCLASS()
class UFirebaseStorageNodeProxyBase : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
protected:
    UPROPERTY()
    UFirebaseStorageReference* _Reference;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynMultStorageVoid, const EFirebaseStorageError, Error);

UCLASS()
class UStorageDeleteProxy final : public UFirebaseStorageNodeProxyBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDynMultStorageVoid OnDeleted;
    UPROPERTY(BlueprintAssignable)
    FDynMultStorageVoid OnError;
public:
    virtual void Activate();

    /// @brief Deletes the object at the current path.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|StorageReference", meta = (BlueprintInternalUseOnly = "true"))
    static UStorageDeleteProxy* Delete(UFirebaseStorageReference* StorageReference);

private:
    void OnActionOver(const EFirebaseStorageError Error);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDynMultDownloadCallback, 
        const EFirebaseStorageError, Error,    
        const int64, BytesRead, 
        const FFirebaseStorageController&, Controller);

UCLASS()
class UStorageGetFileProxy final : public UFirebaseStorageNodeProxyBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDynMultDownloadCallback OnFileDownloaded;
    
    UPROPERTY(BlueprintAssignable)
    FDynMultDownloadCallback OnProgress;

    UPROPERTY(BlueprintAssignable)
    FDynMultDownloadCallback OnPaused;

    UPROPERTY(BlueprintAssignable)
    FDynMultDownloadCallback OnError;
public:
    virtual void Activate();

    /// @brief Asynchronously downloads the object from this StorageReference.
    ///
    /// A byte array will be allocated large enough to hold the entire file in
    /// memory. Therefore, using this method will impact memory usage of your
    /// process.
    ///
    /// @param Path Path to local file on device to download into.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|StorageReference", meta = (BlueprintInternalUseOnly = "true"))
    static UStorageGetFileProxy* GetFile(UFirebaseStorageReference* StorageReference, const FString& Path);

private:
    void OnActionOver(const EFirebaseStorageError Error, const int64 Bytes);
    void OnPausedInternal(FFirebaseStorageController& Controller);
    void OnProgressInternal(FFirebaseStorageController& Controller);

    FString _Path;
    FFirebaseStorageController Controller;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDynMultDownloadBytesCallback, 
        const EFirebaseStorageError, Error,    
        const TArray<uint8>, BytesRead, 
        const FFirebaseStorageController&, Controller);

UCLASS()
class UStorageGetBytesProxy final : public UFirebaseStorageNodeProxyBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDynMultDownloadBytesCallback OnFileDownloaded;
    
    UPROPERTY(BlueprintAssignable)
    FDynMultDownloadBytesCallback OnProgress;

    UPROPERTY(BlueprintAssignable)
    FDynMultDownloadBytesCallback OnPaused;

    UPROPERTY(BlueprintAssignable)
    FDynMultDownloadBytesCallback OnError;
public:
    virtual void Activate();

    /// @brief Asynchronously downloads the object from this StorageReference.
    ///
    /// A byte array will be allocated large enough to hold the entire file in
    /// memory. Therefore, using this method will impact memory usage of your
    /// process.
    /// @param[in] BufferSize The size of the byte buffer.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|StorageReference", meta = (BlueprintInternalUseOnly = "true"))
    static UStorageGetBytesProxy* GetBytes(UFirebaseStorageReference* StorageReference, const int64 BufferSize);

private:
    void OnActionOver(const EFirebaseStorageError Error, const TArray<uint8>& Bytes);
    void OnPausedInternal(FFirebaseStorageController& Controller);
    void OnProgressInternal(FFirebaseStorageController& Controller);

    int64 _BufferSize;
    FFirebaseStorageController Controller;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultStorageString, const EFirebaseStorageError, Error, const FString&, URL);

UCLASS()
class UStorageGetDownloadUrlProxy final : public UFirebaseStorageNodeProxyBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDynMultStorageString OnUrlReceived;

    UPROPERTY(BlueprintAssignable)
    FDynMultStorageString OnError;
public:
    virtual void Activate();


    /// @brief Asynchronously retrieves a long lived download URL with a revokable
    /// token.
    ///
    /// This can be used to share the file with others, but can be revoked by a
    /// developer in the Firebase Console if desired.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|StorageReference", meta = (BlueprintInternalUseOnly = "true"))
    static UStorageGetDownloadUrlProxy* GetDownloadUrl(UFirebaseStorageReference* StorageReference);

private:
    void OnActionOver(const EFirebaseStorageError Error, const FString& Url);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultStorageMetadata, 
    const EFirebaseStorageError, Error, 
    const FFirebaseStorageMetadata&, Metadata);

UCLASS()
class UStorageGetMetadataProxy final : public UFirebaseStorageNodeProxyBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDynMultStorageMetadata OnMetadataReceived;

    UPROPERTY(BlueprintAssignable)
    FDynMultStorageMetadata OnError;
public:
    virtual void Activate();

    /// @brief Retrieves metadata associated with an object at this
    /// StorageReference.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|StorageReference", meta = (BlueprintInternalUseOnly = "true"))
    static UStorageGetMetadataProxy* GetMetadata(UFirebaseStorageReference* StorageReference);

private:
    void OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Metadata);
};

UCLASS()
class UStorageUpdateMetadataProxy final : public UFirebaseStorageNodeProxyBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDynMultStorageMetadata OnMetadataUpdated;

    UPROPERTY(BlueprintAssignable)
    FDynMultStorageMetadata OnError;
public:
    virtual void Activate();


    /// @brief Updates the metadata associated with this StorageReference.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|StorageReference", meta = (BlueprintInternalUseOnly = "true"))
    static UStorageUpdateMetadataProxy* UpdateMetadata(UFirebaseStorageReference* StorageReference, 
        const FFirebaseStorageMetadata& Metadata);

private:
    void OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Metadata);

    FFirebaseStorageMetadata _Metadata;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDynMultUploadBytesCallback, 
        const EFirebaseStorageError, Error,    
        const FFirebaseStorageMetadata, Metadata, 
        const FFirebaseStorageController&, Controller);

UCLASS()
class UStoragePutBytesProxy final : public UFirebaseStorageNodeProxyBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnBytesUploaded;
    
    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnProgress;

    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnPaused;

    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnError;
public:
    virtual void Activate();

    /// @brief Asynchronously uploads data to the currently specified
    /// StorageReference, without additional metadata.
    ///
    /// @param[in] buffer A byte buffer to write data from.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|StorageReference", meta = (BlueprintInternalUseOnly = "true"))
    static UStoragePutBytesProxy* PutBytes(UFirebaseStorageReference* StorageReference, const TArray<uint8> & Bytes);

private:
    void OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Meta);
    void OnPausedInternal(FFirebaseStorageController& Controller);
    void OnProgressInternal(FFirebaseStorageController& Controller);

    TArray<uint8> _Bytes;
    FFirebaseStorageController Controller;
};

UCLASS()
class UStoragePutBytesWithMetadataProxy final : public UFirebaseStorageNodeProxyBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnBytesUploaded;
    
    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnProgress;

    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnPaused;

    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnError;
public:
    virtual void Activate();

    /// @brief Asynchronously uploads data to the currently specified
    /// StorageReference.
    ///
    /// @param[in] Bytes The data to upload.
    /// @param Metadata Metadata containing additional information (MIME type,
    /// etc.) about the object being uploaded.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|StorageReference", meta = (BlueprintInternalUseOnly = "true"))
    static UStoragePutBytesWithMetadataProxy* PutBytesWithMetadata(UFirebaseStorageReference* StorageReference, 
        const TArray<uint8> & Bytes, const FFirebaseStorageMetadata& Metadata);

private:
    void OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Meta);
    void OnPausedInternal(FFirebaseStorageController& Controller);
    void OnProgressInternal(FFirebaseStorageController& Controller);

    TArray<uint8> _Bytes;
    FFirebaseStorageMetadata _Meta;
    FFirebaseStorageController Controller;
};

UCLASS()
class UStoragePutFileProxy final : public UFirebaseStorageNodeProxyBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnFileUploaded;
    
    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnProgress;

    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnPaused;

    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnError;
public:
    virtual void Activate();

    /// @brief Asynchronously uploads data to the currently specified
    /// StorageReference, without additional metadata.
    ///
    /// @param Path Path to local file on device to upload to Firebase
    /// Storage.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|StorageReference", meta = (BlueprintInternalUseOnly = "true"))
    static UStoragePutFileProxy* PutFile(UFirebaseStorageReference* StorageReference, const FString& Path);

private:
    void OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Meta);
    void OnPausedInternal(FFirebaseStorageController& Controller);
    void OnProgressInternal(FFirebaseStorageController& Controller);

    FString _Path;
    FFirebaseStorageController Controller;
};

UCLASS()
class UStoragePutFileWithMetadataProxy final : public UFirebaseStorageNodeProxyBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnFileUploaded;
    
    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnProgress;

    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnPaused;

    UPROPERTY(BlueprintAssignable)
    FDynMultUploadBytesCallback OnError;
public:
    virtual void Activate();

    /// @brief Asynchronously uploads data to the currently specified
    /// StorageReference, without additional metadata.
    ///
    /// @param Path Path to local file on device to upload to Firebase
    /// Storage.
    UFUNCTION(BlueprintCallable, Category = "Firebase|Storage|StorageReference", meta = (BlueprintInternalUseOnly = "true"))
    static UStoragePutFileWithMetadataProxy* PutFileWithMetadata(UFirebaseStorageReference* StorageReference, 
        const FString& Path, const FFirebaseStorageMetadata& Metadata);

private:
    void OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Meta);
    void OnPausedInternal(FFirebaseStorageController& Controller);
    void OnProgressInternal(FFirebaseStorageController& Controller);

    FString _Path;
    FFirebaseStorageMetadata _Meta;
    FFirebaseStorageController Controller;
};





