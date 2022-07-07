// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseFeatures.h"

#if WITH_FIREBASE_STORAGE
THIRD_PARTY_INCLUDES_START
#	include "firebase/storage/storage_reference.h"
THIRD_PARTY_INCLUDES_END
#endif

#include "Storage/Storage.h"
#include "StorageReference.generated.h"

class UFirebaseStorageReference;
class FStorageListener;

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseStorageController
{
    GENERATED_BODY()
private:
    friend class UFirebaseStorageReference;
    friend class FStorageListener;
    friend class UStorageBlueprintLibrary;

public:
    FFirebaseStorageController();
    FFirebaseStorageController(const FFirebaseStorageController& Other);
    FFirebaseStorageController(FFirebaseStorageController&& Other);

    FFirebaseStorageController& operator=(const FFirebaseStorageController& Other);
    FFirebaseStorageController& operator=(FFirebaseStorageController&& Other);

#if WITH_FIREBASE_STORAGE
    /// Pauses the operation currently in progress.
    ///
    /// @returns True if the operation was successfully paused, false otherwise.
    bool Pause();

    /// @brief Resumes the operation that is paused.
    ///
    /// @returns True if the operation was successfully resumed, false otherwise.
    bool Resume();

    /// @brief Cancels the operation currently in progress.
    ///
    /// @returns True if the operation was successfully canceled, false otherwise.
    bool Cancel();

    /// @brief Returns true if the operation is paused.
    bool IsPaused() const;

    /// @brief Returns the number of bytes transferred so far.
    ///
    /// @returns The number of bytes transferred so far.
    int64 BytesTransferred() const;

    /// @brief Returns the total bytes to be transferred.
    ///
    /// @returns The total bytes to be transferred.  This will return -1 if
    /// the size of the transfer is unknown.
    int64 TotalByteCount() const;

    /// @brief Returns the StorageReference associated with this Controller.
    ///
    /// @returns The StorageReference associated with this Controller.
    UFirebaseStorageReference* GetReference() const;

    /// @brief Returns true if this Controller is valid, false if it is not
    /// valid. An invalid Controller is one that is not associated with an
    /// operation.
    ///
    /// @returns true if this Controller is valid, false if this Controller is
    /// invalid.
    bool IsValid() const;

private:
    /* The actual controller. */
    TSharedPtr<firebase::storage::Controller, ESPMode::ThreadSafe> Controller;
#endif
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseStorageMetadata
{
    GENERATED_BODY()
private:
    friend class UFirebaseStorageReference;

public:
    FFirebaseStorageMetadata();
    FFirebaseStorageMetadata(const FFirebaseStorageMetadata& Other);
    FFirebaseStorageMetadata(FFirebaseStorageMetadata&& Other);

    FFirebaseStorageMetadata& operator=(const FFirebaseStorageMetadata& Other);
    FFirebaseStorageMetadata& operator=(FFirebaseStorageMetadata&& Other);

    /// @brief Return the owning Google Cloud Storage bucket for the
    /// StorageReference.
    ///
    /// @returns The owning Google Cloud Storage bucket for the StorageReference.
    FString GetBucket() const;

    /// @brief Set the Cache Control setting of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc7234#section-5.2
    void SetCachControl(const FString& CacheControl);
    
    /// @brief Return the Cache Control setting of the StorageReference.
    ///
    /// @returns The Cache Control setting of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc7234#section-5.2
    FString GetCacheControl() const;

    /// @brief Set the content disposition of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc6266
    void SetContentDisposition(const FString& Disposition);

    /// @brief Return the content disposition of the StorageReference.
    ///
    /// @returns The content disposition of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc6266
    FString GetContentDisposition() const;

    /// @brief Set the content encoding for the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc2616#section-14.11
    void SetContentEncoding(const FString& Encoding);

    /// @brief Return the content encoding for the StorageReference.
    ///
    /// @returns The content encoding for the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc2616#section-14.11
    FString GetContentEncoding() const;

    /// @brief Set the content language for the StorageReference.
    ///
    /// This must be an ISO 639-1 two-letter language code.
    /// E.g. "zh", "es", "en".
    ///
    /// @see https://www.loc.gov/standards/iso639-2/php/code_list.php
    void SetContentLanguage(const FString& Language);

    /// @brief Return the content language for the StorageReference.
    ///
    /// @returns The content language for the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc2616#section-14.12
    FString GetContentLanguage() const;

    /// @brief Set the content type of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc2616#section-14.17
    void SetContentType(const FString& Type);

    /// @brief Return the content type of the StorageReference.
    ///
    /// @returns The content type of the StorageReference.
    ///
    /// @see https://tools.ietf.org/html/rfc2616#section-14.17
    FString ContentType() const;

    /// @brief Return the time the StorageReference was created in milliseconds
    /// since the epoch.
    ///
    /// @returns The time the StorageReference was created in milliseconds since
    /// the epoch.
    int64 GetCreationTime() const;

    /// @brief Return a map of custom metadata key value pairs.
    ///
    /// The pointer returned is only valid during the lifetime of the Metadata
    /// object that owns it.
    ///
    /// @returns The keys for custom metadata.
    TMap<FString, FString> GetCustomMetadata() const;

    /// @brief Return a version String indicating what version of the
    /// StorageReference.
    ///
    /// @returns A value indicating the version of the StorageReference.
    int64 GetGeneration() const;

    /// @brief Return a version String indicating the version of this
    /// StorageMetadata.
    ///
    /// @returns A value indicating the version of this StorageMetadata.
    int64 GetMetadataGeneration() const;

    /// @brief Return a simple name of the StorageReference object.
    ///
    /// @returns A simple name of the StorageReference object.
    FString GetName() const;

    /// @brief Return the path of the StorageReference object.
    ///
    /// @returns The path of the StorageReference object.
    FString GetPath() const;

    /// @brief Return the associated StorageReference to which this Metadata
    /// belongs.
    ///
    /// @returns The associated StorageReference to which this Metadata belongs.
    /// If this Metadata is invalid or is not associated with any file, an invalid
    /// StorageReference is returned.
    UFirebaseStorageReference* GetReference() const;

    /// @brief Return the stored Size in bytes of the StorageReference object.
    ///
    /// @returns The stored Size in bytes of the StorageReference object.
    int64 GetSizeBytes() const;

    /// @brief Return the time the StorageReference was last updated in
    /// milliseconds since the epoch.
    ///
    /// @return The time the StorageReference was last updated in milliseconds
    /// since the epoch.
    int64 GetUpdatedTime() const;

    /// @brief Returns true if this Metadata is valid, false if it is not
    /// valid. An invalid Metadata is returned when a method such as
    /// StorageReference::GetMetadata() completes with an error.
    ///
    /// @returns true if this Metadata is valid, false if this Metadata is
    /// invalid.
    bool IsValid() const;

    /// @brief MD5 hash of the data; encoded using base64.
    ///
    /// @returns MD5 hash of the data; encoded using base64.
    FString GetMd5Hash() const;

#if WITH_FIREBASE_STORAGE
    operator firebase::storage::Metadata& ();

    operator const firebase::storage::Metadata& () const;

private:
    void CreateInternal() const;
    TUniquePtr<firebase::storage::Metadata> Metadata;
#endif
};

DECLARE_DELEGATE_TwoParams(FFirebaseStorageInt64Callback, const EFirebaseStorageError, const int64);
DECLARE_DELEGATE_OneParam(FFirebaseStorageControllerCallback, FFirebaseStorageController&);
DECLARE_DELEGATE_TwoParams(FFirebaseStorageBinaryCallback, const EFirebaseStorageError, const TArray<uint8>&);
DECLARE_DELEGATE_TwoParams(FFirebaseStorageMetadataCallback, const EFirebaseStorageError, const FFirebaseStorageMetadata&);
DECLARE_DELEGATE_TwoParams(FFirebaseStorageStringCallback, const EFirebaseStorageError, const FString&);

UCLASS(BlueprintType)
class FIREBASEFEATURES_API UFirebaseStorageReference : public UObject
{
	GENERATED_BODY()
private:
	friend class UFirebaseStorage;
    friend struct FFirebaseStorageController;
    friend struct FFirebaseStorageMetadata;

public:
    /// @brief Gets a reference to a location relative to this one.
    ///
    /// @param[in] path Path relative to this reference's location.
    /// The pointer only needs to be valid during this call.
    ///
    /// @returns Child relative to this location.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|StorageReference")
    UPARAM(DisplayName = "Child") UFirebaseStorageReference* Child(const FString& path) const;

    /// @brief Deletes the object at the current path.
    void Delete(FFirebaseStorageCallback Callback);

    /// @brief Return the Google Cloud Storage bucket that holds this object.
    ///
    /// @returns The bucket.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|StorageReference")
    UPARAM(DisplayName = "Bucket") FString GetBucket();

    /// @brief Return the full path of the storage reference, not including
    /// the Google Cloud Storage bucket.
    ///
    /// @returns Full path to the storage reference, not including GCS bucket.
    /// For example, for the reference "gs://bucket/path/to/object.txt", the full
    /// path would be "path/to/object.txt".
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|StorageReference")
    UPARAM(DisplayName = "Full Path") FString GetFullPath();

    /// @brief Asynchronously downloads the object from this StorageReference.
    ///
    /// A byte array will be allocated large enough to hold the entire file in
    /// memory. Therefore, using this method will impact memory usage of your
    /// process.
    ///
    /// @param Path Path to local file on device to download into.
    /// @param[in] listener A listener that will respond to events on this read
    /// operation. If not nullptr, a listener that will respond to events on this
    /// read operation. The caller is responsible for allocating and deallocating
    /// the listener. The same listener can be used for multiple operations.
    /// @param[out] controller_out Controls the write operation, providing the
    /// ability to pause, resume or cancel an ongoing write operation. If not
    /// nullptr, this method will output a Controller here that you can use to
    /// control the write operation.
    void GetFile
    (
        const FString& Path, 
        FFirebaseStorageController& Controller, 
        const FFirebaseStorageInt64Callback& OnOver, 
        const FFirebaseStorageControllerCallback& OnProgress = FFirebaseStorageControllerCallback(),
        const FFirebaseStorageControllerCallback& OnPaused   = FFirebaseStorageControllerCallback()
    );

    /// @brief Asynchronously downloads the object from this StorageReference.
    ///
    /// A byte array will be allocated large enough to hold the entire file in
    /// memory. Therefore, using this method will impact memory usage of your
    /// process.
    /// @param[in] BufferSize The size of the byte buffer.
    /// @param[in] listener A listener that will respond to events on this read
    /// operation. If not nullptr, a listener that will respond to events on this
    /// read operation. The caller is responsible for allocating and deallocating
    /// the listener. The same listener can be used for multiple operations.
    /// @param[out] Controller Controls the write operation, providing the
    /// ability to pause, resume or cancel an ongoing write operation. 
    void GetBytes
    (
        const int64 BufferSize,
        FFirebaseStorageController& Controller,
        const FFirebaseStorageBinaryCallback& OnOver,
        const FFirebaseStorageControllerCallback& OnProgress = FFirebaseStorageControllerCallback(),
        const FFirebaseStorageControllerCallback& OnPaused   = FFirebaseStorageControllerCallback()
    );

    /// @brief Asynchronously retrieves a long lived download URL with a revokable
    /// token.
    ///
    /// This can be used to share the file with others, but can be revoked by a
    /// developer in the Firebase Console if desired.
    void GetDownloadUrl(const FFirebaseStorageStringCallback& Callback);

    /// @brief Retrieves metadata associated with an object at this
    /// StorageReference.
    void GetMetadata(const FFirebaseStorageMetadataCallback& Callback);

    /// @brief Updates the metadata associated with this StorageReference.
    ///
    /// @param Callback A Future result, which will complete when the operation either
    /// succeeds or fails. When the Future is completed, if its Error is
    /// None, the operation succeeded and the Metadata is returned.
    void UpdateMetadata(const FFirebaseStorageMetadata& Metadata, 
        const FFirebaseStorageMetadataCallback& Callback = FFirebaseStorageMetadataCallback());

    /// @brief Returns the short name of this object.
    ///
    /// @returns the short name of this object.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|StorageReference")
    UPARAM(DisplayName = "Name") FString GetName();

    /// @brief Returns a new instance of StorageReference pointing to the parent
    /// location or null if this instance references the root location.
    ///
    /// @returns The parent StorageReference.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|StorageReference")
    UPARAM(DisplayName = "Parent") UFirebaseStorageReference* GetParent();

    /// @brief Asynchronously uploads data to the currently specified
    /// StorageReference, without additional metadata.
    ///
    /// @param[in] Bytes A byte buffer to write data from. 
    /// @param[out] Controller Controls the write operation, providing the
    /// ability to pause, resume or cancel an ongoing write operation. 
    /// This method will output a Controller here that you can use to
    /// control the write operation.
    void PutBytes
    (
        const TArray<uint8>& Bytes,
        FFirebaseStorageController& Controller,
        const FFirebaseStorageMetadataCallback& OnUploadOver,
        const FFirebaseStorageControllerCallback& OnProgress = FFirebaseStorageControllerCallback(),
        const FFirebaseStorageControllerCallback& OnPaused = FFirebaseStorageControllerCallback()
    );
    
    // Same as the above but supports up to 2^64 bytes.
    void PutBytes
    (
        const TArray64<uint8>& Bytes,
        FFirebaseStorageController& Controller,
        const FFirebaseStorageMetadataCallback& OnUploadOver,
        const FFirebaseStorageControllerCallback& OnProgress = FFirebaseStorageControllerCallback(),
        const FFirebaseStorageControllerCallback& OnPaused = FFirebaseStorageControllerCallback()
    );

    /// @brief Asynchronously uploads data to the currently specified
    /// StorageReference, without additional metadata.
    ///
    /// @param Bytes A byte buffer to write data from. This buffer must be
    /// valid for the duration of the transfer.
    /// @param Metadata Metadata containing additional information (MIME type,
    /// etc.) about the object being uploaded.
    /// @param[out] Controller Controls the write operation, providing the
    /// ability to pause, resume or cancel an ongoing write operation. 
    /// This method will output a Controller here that you can use to
    /// control the write operation.
    void PutBytes
    (
        const TArray<uint8>& Bytes,
        const FFirebaseStorageMetadata& Metadata,
        FFirebaseStorageController& Controller,
        const FFirebaseStorageMetadataCallback& OnUploadOver = FFirebaseStorageMetadataCallback(),
        const FFirebaseStorageControllerCallback& OnProgress = FFirebaseStorageControllerCallback(),
        const FFirebaseStorageControllerCallback& OnPaused   = FFirebaseStorageControllerCallback()
    );

    /// @brief Asynchronously uploads data to the currently specified
    /// StorageReference, without additional metadata.
    ///
    /// @param[in] Path Path to local file on device to upload to Firebase
    /// Storage.
    /// @param[in] listener A listener that will respond to events on this read
    /// operation. If not nullptr, a listener that will respond to events on this
    /// write operation. The caller is responsible for allocating and deallocating
    /// the listener. The same listener can be used for multiple operations.
    /// @param[out] controller_out Controls the write operation, providing the
    /// ability to pause, resume or cancel an ongoing write operation. If not
    /// nullptr, this method will output a Controller here that you can use to
    /// control the write operation.
    void PutFile
    (
        const FString& Path, 
        FFirebaseStorageController& Controller,
        const FFirebaseStorageMetadataCallback&    OnUploadOver = FFirebaseStorageMetadataCallback(),
        const FFirebaseStorageControllerCallback & OnProgress   = FFirebaseStorageControllerCallback(),
        const FFirebaseStorageControllerCallback & OnPaused     = FFirebaseStorageControllerCallback()
    );

    /// @brief Asynchronously uploads data to the currently specified
    /// StorageReference, without additional metadata.
    ///
    /// @param[in] path Path to local file on device to upload to Firebase
    /// Storage.
    /// @param[in] metadata Metadata containing additional information (MIME type,
    /// etc.) about the object being uploaded.
    /// @param[in] listener A listener that will respond to events on this read
    /// operation. If not nullptr, a listener that will respond to events on this
    /// write operation. The caller is responsible for allocating and deallocating
    /// the listener. The same listener can be used for multiple operations.
    /// @param[out] controller_out Controls the write operation, providing the
    /// ability to pause, resume or cancel an ongoing write operation. If not
    /// nullptr, this method will output a Controller here that you can use to
    /// control the write operation.
    void PutFile
    (
        const FString& Path, 
        const FFirebaseStorageMetadata& Metadata,
        FFirebaseStorageController& Controller,
        const FFirebaseStorageMetadataCallback& OnUploadOver = FFirebaseStorageMetadataCallback(),
        const FFirebaseStorageControllerCallback& OnProgress = FFirebaseStorageControllerCallback(),
        const FFirebaseStorageControllerCallback& OnPaused = FFirebaseStorageControllerCallback()
    );

    /// @brief Returns true if this StorageReference is valid, false if it is not
    /// valid. An invalid StorageReference indicates that the reference is
    /// uninitialized (created with the default constructor) or that there was an
    /// error retrieving the reference.
    ///
    /// @returns true if this StorageReference is valid, false if this
    /// StorageReference is invalid.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Storage|StorageReference")
    UPARAM(DisplayName = "Is Valid") bool IsValid() const;


private:
#if WITH_FIREBASE_STORAGE
	firebase::storage::StorageReference Reference;
#endif
};

