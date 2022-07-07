// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Storage/StorageNodes.h"

#include "FirebaseFeatures.h"


void UStorageBlueprintLibrary::Pause(FFirebaseStorageController& Controller, EFirebaseStorageBlueprintExecutionPin& Branches)
{
#if WITH_FIREBASE_STORAGE
	Branches = Controller.Pause() ? EFirebaseStorageBlueprintExecutionPin::Success : EFirebaseStorageBlueprintExecutionPin::Failed;
#endif
}

void UStorageBlueprintLibrary::Resume(FFirebaseStorageController& Controller, EFirebaseStorageBlueprintExecutionPin& Branches)
{
#if WITH_FIREBASE_STORAGE
	Branches = Controller.Resume() ? EFirebaseStorageBlueprintExecutionPin::Success : EFirebaseStorageBlueprintExecutionPin::Failed;
#endif
}

void UStorageBlueprintLibrary::Cancel(FFirebaseStorageController& Controller, EFirebaseStorageBlueprintExecutionPin& Branches)
{
#if WITH_FIREBASE_STORAGE
	Branches = Controller.Cancel() ? EFirebaseStorageBlueprintExecutionPin::Success : EFirebaseStorageBlueprintExecutionPin::Failed;
#endif
}

bool UStorageBlueprintLibrary::IsPaused(FFirebaseStorageController& Controller)
{
#if WITH_FIREBASE_STORAGE
	return Controller.Cancel();
#else
	return false;
#endif
}

int64 UStorageBlueprintLibrary::BytesTransferred(const FFirebaseStorageController& Controller)
{
#if WITH_FIREBASE_STORAGE
	return Controller.BytesTransferred();
#else
	return 0;
#endif
}

int64 UStorageBlueprintLibrary::TotalByteCount(const FFirebaseStorageController& Controller)
{
#if WITH_FIREBASE_STORAGE
	return Controller.TotalByteCount();
#else
	return 0;
#endif
}

UFirebaseStorageReference* UStorageBlueprintLibrary::GetReference(const FFirebaseStorageController& Controller)
{
#if WITH_FIREBASE_STORAGE
	return Controller.GetReference();
#else
	return 0;
#endif
}

bool UStorageBlueprintLibrary::IsValid(const FFirebaseStorageController& Controller)
{
#if WITH_FIREBASE_STORAGE
	return Controller.IsValid();
#else
	return 0;
#endif
}

FString UStorageBlueprintLibrary::GetBucket(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetBucket();
#else
	return {};
#endif
}

void UStorageBlueprintLibrary::SetCachControl(UPARAM(ref) FFirebaseStorageMetadata& Metadata, const FString& CacheControl)
{
#if WITH_FIREBASE_STORAGE
	Metadata.SetCachControl(CacheControl);
#endif
}

FString UStorageBlueprintLibrary::GetCacheControl(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetCacheControl();
#else
	return {};
#endif
}

void UStorageBlueprintLibrary::SetContentDisposition(UPARAM(ref) FFirebaseStorageMetadata& Metadata, const FString& Disposition)
{
#if WITH_FIREBASE_STORAGE
	Metadata.SetContentDisposition(Disposition);
#endif
}

FString UStorageBlueprintLibrary::GetContentDisposition(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetContentDisposition();
#else
	return {};
#endif
}

void UStorageBlueprintLibrary::SetContentEncoding(UPARAM(ref) FFirebaseStorageMetadata& Metadata, const FString& Encoding)
{
#if WITH_FIREBASE_STORAGE
	Metadata.SetContentEncoding(Encoding);
#endif
}

FString UStorageBlueprintLibrary::GetContentEncoding(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetContentEncoding();
#else
	return {};
#endif
}

void UStorageBlueprintLibrary::SetContentLanguage(UPARAM(ref) FFirebaseStorageMetadata& Metadata, const FString& Language)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.SetContentLanguage(Language);
#endif
}

FString UStorageBlueprintLibrary::GetContentLanguage(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetContentLanguage();
#else
	return {};
#endif
}

void UStorageBlueprintLibrary::SetContentType(UPARAM(ref) FFirebaseStorageMetadata& Metadata, const FString& Type)
{
#if WITH_FIREBASE_STORAGE
	Metadata.SetContentType(Type);
#endif
}

FString UStorageBlueprintLibrary::ContentType(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.ContentType();
#else
	return {};
#endif
}

int64 UStorageBlueprintLibrary::GetCreationTime(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetCreationTime();
#else
	return 0;
#endif
}

TMap<FString, FString> UStorageBlueprintLibrary::GetCustomMetadata(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetCustomMetadata();
#else
	return {};
#endif
}

int64 UStorageBlueprintLibrary::GetGeneration(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetGeneration();
#else
	return 0;
#endif
}

int64 UStorageBlueprintLibrary::GetMetadataGeneration(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetMetadataGeneration();
#else
	return 0;
#endif
}

FString UStorageBlueprintLibrary::GetName(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetName();
#else
	return {};
#endif
}

FString UStorageBlueprintLibrary::GetPath(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetPath();
#else
	return {};
#endif
}

UFirebaseStorageReference* UStorageBlueprintLibrary::GetReference_Metadata(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetReference();
#else
	return 0;
#endif
}

int64 UStorageBlueprintLibrary::GetSizeBytes(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetSizeBytes();
#else
	return 0;
#endif
}

int64 UStorageBlueprintLibrary::GetUpdatedTime(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetUpdatedTime();
#else
	return 0;
#endif
}

bool UStorageBlueprintLibrary::IsValid_Metadata(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.IsValid();
#else
	return 0;
#endif
}

FString UStorageBlueprintLibrary::GetMd5Hash(const FFirebaseStorageMetadata& Metadata)
{
#if WITH_FIREBASE_STORAGE
	return Metadata.GetMd5Hash();
#else
	return {};
#endif
}

#define CHECK_REFERENCE(...)											\
	if (!_Reference)													\
	{																	\
		UE_LOG(LogFirebaseStorage, Error,								\
			TEXT("Passed a nullptr Storage reference to function."));	\
		OnActionOver(EFirebaseStorageError::Unknown, ## __VA_ARGS__);	\
		SetReadyToDestroy();											\
		return;															\
	}

#define CALL_CALLBACK(Name, ...) do										\
	{																	\
		(Error == EFirebaseStorageError::None ? Name : OnError)			\
			.Broadcast(Error, ## __VA_ARGS__);							\
		SetReadyToDestroy();											\
	} while(0)

UStorageDeleteProxy* UStorageDeleteProxy::Delete(UFirebaseStorageReference* StorageReference)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

#if WITH_FIREBASE_STORAGE
	Proxy->_Reference = StorageReference;
#endif

	return Proxy;
}

void UStorageDeleteProxy::Activate()
{
#if WITH_FIREBASE_STORAGE
	CHECK_REFERENCE();

	_Reference->Delete(FFirebaseStorageCallback::CreateUObject(this, &ThisClass::OnActionOver));
#else
	OnActionOver(EFirebaseStorageError::Unknown);
#endif
}

void UStorageDeleteProxy::OnActionOver(const EFirebaseStorageError Error)
{
	CALL_CALLBACK(OnDeleted);
}

UStorageGetFileProxy* UStorageGetFileProxy::GetFile(UFirebaseStorageReference* StorageReference, const FString& Path)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Reference = StorageReference;
	Proxy->_Path = Path;

	return Proxy;
}

void UStorageGetFileProxy::Activate()
{
#if WITH_FIREBASE_STORAGE
	CHECK_REFERENCE(0);
	
	_Reference->GetFile
	(
		_Path,
		Controller,
		FFirebaseStorageInt64Callback	  ::CreateUObject(this, &ThisClass::OnActionOver),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnProgressInternal),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnPausedInternal)
	);
#else
	OnActionOver(EFirebaseStorageError::Unknown, 0);
#endif
}

void UStorageGetFileProxy::OnActionOver(const EFirebaseStorageError Error, const int64 Bytes)
{
	CALL_CALLBACK(OnFileDownloaded, Bytes, Controller);
}


void UStorageGetFileProxy::OnPausedInternal(FFirebaseStorageController& ActionController)
{
#if WITH_FIREBASE_STORAGE
	OnPaused.Broadcast(EFirebaseStorageError::None, Controller.BytesTransferred(), Controller);
#endif
}

void UStorageGetFileProxy::OnProgressInternal(FFirebaseStorageController& ActionController)
{
#if WITH_FIREBASE_STORAGE
	OnProgress.Broadcast(EFirebaseStorageError::None, Controller.BytesTransferred(), Controller);
#endif
}

UStorageGetBytesProxy* UStorageGetBytesProxy::GetBytes(UFirebaseStorageReference* StorageReference, const int64 BufferSize)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Reference = StorageReference;
	Proxy->_BufferSize = BufferSize;

	return Proxy;
}

void UStorageGetBytesProxy::Activate()
{
#if WITH_FIREBASE_STORAGE
	CHECK_REFERENCE(TArray<uint8>());

	_Reference->GetBytes
	(
		_BufferSize,
		Controller,
		FFirebaseStorageBinaryCallback::CreateUObject(this, &ThisClass::OnActionOver),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnProgressInternal),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnPausedInternal)
	);
#else
	OnActionOver(EFirebaseStorageError::Unknown, {});
#endif
}

void UStorageGetBytesProxy::OnActionOver(const EFirebaseStorageError Error, const TArray<uint8>& Bytes)
{
	CALL_CALLBACK(OnFileDownloaded, Bytes, Controller);
}

void UStorageGetBytesProxy::OnPausedInternal(FFirebaseStorageController& ActionController)
{
	OnPaused.Broadcast(EFirebaseStorageError::None, TArray<uint8>(), Controller);
}

void UStorageGetBytesProxy::OnProgressInternal(FFirebaseStorageController& ActionController)
{
	OnProgress.Broadcast(EFirebaseStorageError::None, TArray<uint8>(), Controller);
}

UStorageGetDownloadUrlProxy* UStorageGetDownloadUrlProxy::GetDownloadUrl(UFirebaseStorageReference* StorageReference)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Reference = StorageReference;

	return Proxy;
}

void UStorageGetDownloadUrlProxy::Activate()
{
#if WITH_FIREBASE_STORAGE
	CHECK_REFERENCE(TEXT(""));

	_Reference->GetDownloadUrl(FFirebaseStorageStringCallback::CreateUObject(this, &ThisClass::OnActionOver));
#else
	OnActionOver(EFirebaseStorageError::Unknown, {});
#endif
}

void UStorageGetDownloadUrlProxy::OnActionOver(const EFirebaseStorageError Error, const FString& Url)
{
	CALL_CALLBACK(OnUrlReceived, Url);
}

UStorageGetMetadataProxy* UStorageGetMetadataProxy::GetMetadata(UFirebaseStorageReference* StorageReference)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Reference = StorageReference;

	return Proxy;
}

void UStorageGetMetadataProxy::Activate()
{
#if WITH_FIREBASE_STORAGE
	CHECK_REFERENCE(FFirebaseStorageMetadata());

	_Reference->GetMetadata(FFirebaseStorageMetadataCallback::CreateUObject(this, &ThisClass::OnActionOver));
#else
	OnActionOver(EFirebaseStorageError::Unknown, {});
#endif
}

void UStorageGetMetadataProxy::OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Metadata)
{
	CALL_CALLBACK(OnMetadataReceived, Metadata);
}

UStorageUpdateMetadataProxy* UStorageUpdateMetadataProxy::UpdateMetadata(UFirebaseStorageReference* StorageReference,
	const FFirebaseStorageMetadata& Metadata)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Reference = StorageReference;
	Proxy->_Metadata = Metadata;

	return Proxy;
}

void UStorageUpdateMetadataProxy::Activate()
{
#if WITH_FIREBASE_STORAGE
	CHECK_REFERENCE(FFirebaseStorageMetadata());

	_Reference->UpdateMetadata(_Metadata, FFirebaseStorageMetadataCallback::CreateUObject(this, &ThisClass::OnActionOver));
#else
	OnActionOver(EFirebaseStorageError::Unknown, {});
#endif
}

void UStorageUpdateMetadataProxy::OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Metadata)
{
	CALL_CALLBACK(OnMetadataUpdated, Metadata);
}

UStoragePutBytesProxy* UStoragePutBytesProxy::PutBytes(UFirebaseStorageReference* StorageReference, const TArray<uint8>& Bytes)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Reference = StorageReference;
	Proxy->_Bytes  = Bytes;

	return Proxy;
}

void UStoragePutBytesProxy::Activate()
{
#if WITH_FIREBASE_STORAGE
	CHECK_REFERENCE(FFirebaseStorageMetadata());

	_Reference->PutBytes
	(
		_Bytes,
		Controller,
		FFirebaseStorageMetadataCallback  ::CreateUObject(this, &ThisClass::OnActionOver),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnProgressInternal),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnPausedInternal)
	);
#else
	OnActionOver(EFirebaseStorageError::Unknown, {});
#endif
}

void UStoragePutBytesProxy::OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Meta)
{
	CALL_CALLBACK(OnBytesUploaded, Meta, Controller);
}

void UStoragePutBytesProxy::OnPausedInternal(FFirebaseStorageController& ActionController)
{
	OnPaused.Broadcast(EFirebaseStorageError::None, FFirebaseStorageMetadata(), Controller);
}

void UStoragePutBytesProxy::OnProgressInternal(FFirebaseStorageController& ActionController)
{
	OnProgress.Broadcast(EFirebaseStorageError::None, FFirebaseStorageMetadata(), Controller);
}


UStoragePutBytesWithMetadataProxy* UStoragePutBytesWithMetadataProxy::PutBytesWithMetadata(UFirebaseStorageReference* StorageReference, const TArray<uint8>& Bytes, const FFirebaseStorageMetadata& Metadata)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Reference = StorageReference;
	Proxy->_Bytes = Bytes;
	Proxy->_Meta = Metadata;

	return Proxy;
}

void UStoragePutBytesWithMetadataProxy::Activate()
{
#if WITH_FIREBASE_STORAGE
	CHECK_REFERENCE(FFirebaseStorageMetadata());

	_Reference->PutBytes
	(
		_Bytes,
		_Meta,
		Controller,
		FFirebaseStorageMetadataCallback::CreateUObject(this, &ThisClass::OnActionOver),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnProgressInternal),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnPausedInternal)
	);
#else
	OnActionOver(EFirebaseStorageError::Unknown, {});
#endif
}

void UStoragePutBytesWithMetadataProxy::OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Meta)
{
	CALL_CALLBACK(OnBytesUploaded, Meta, Controller);
}

void UStoragePutBytesWithMetadataProxy::OnPausedInternal(FFirebaseStorageController& ActionController)
{
	OnPaused.Broadcast(EFirebaseStorageError::None, FFirebaseStorageMetadata(), Controller);
}

void UStoragePutBytesWithMetadataProxy::OnProgressInternal(FFirebaseStorageController& ActionController)
{
	OnProgress.Broadcast(EFirebaseStorageError::None, FFirebaseStorageMetadata(), Controller);
}


UStoragePutFileProxy* UStoragePutFileProxy::PutFile(UFirebaseStorageReference* StorageReference, const FString& Path)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Reference = StorageReference;
	Proxy->_Path = Path;

	return Proxy;
}

void UStoragePutFileProxy::Activate()
{
#if WITH_FIREBASE_STORAGE
	CHECK_REFERENCE(FFirebaseStorageMetadata());

	_Reference->PutFile
	(
		_Path,
		Controller,
		FFirebaseStorageMetadataCallback::CreateUObject(this, &ThisClass::OnActionOver),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnProgressInternal),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnPausedInternal)
	);
#else
	OnActionOver(EFirebaseStorageError::Unknown, {});
#endif
}

void UStoragePutFileProxy::OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Meta)
{
	CALL_CALLBACK(OnFileUploaded, Meta, Controller);
}

void UStoragePutFileProxy::OnPausedInternal(FFirebaseStorageController& ActionController)
{
	OnPaused.Broadcast(EFirebaseStorageError::None, FFirebaseStorageMetadata(), Controller);
}

void UStoragePutFileProxy::OnProgressInternal(FFirebaseStorageController& ActionController)
{
	OnProgress.Broadcast(EFirebaseStorageError::None, FFirebaseStorageMetadata(), Controller);
}


UStoragePutFileWithMetadataProxy* UStoragePutFileWithMetadataProxy::PutFileWithMetadata(UFirebaseStorageReference* StorageReference,
	const FString& Path, const FFirebaseStorageMetadata& Metadata)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Reference = StorageReference;
	Proxy->_Path = Path;
	Proxy->_Meta = Metadata;

	return Proxy;
}

void UStoragePutFileWithMetadataProxy::Activate()
{
#if WITH_FIREBASE_STORAGE
	CHECK_REFERENCE(FFirebaseStorageMetadata());

	_Reference->PutFile
	(
		_Path,
		_Meta,
		Controller,
		FFirebaseStorageMetadataCallback  ::CreateUObject(this, &ThisClass::OnActionOver),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnProgressInternal),
		FFirebaseStorageControllerCallback::CreateUObject(this, &ThisClass::OnPausedInternal)
	);
#else
	OnActionOver(EFirebaseStorageError::Unknown, {});
#endif
}

void UStoragePutFileWithMetadataProxy::OnActionOver(const EFirebaseStorageError Error, const FFirebaseStorageMetadata& Meta)
{
	CALL_CALLBACK(OnFileUploaded, Meta, Controller);
}

void UStoragePutFileWithMetadataProxy::OnPausedInternal(FFirebaseStorageController& ActionController)
{
	OnPaused.Broadcast(EFirebaseStorageError::None, FFirebaseStorageMetadata(), Controller);
}

void UStoragePutFileWithMetadataProxy::OnProgressInternal(FFirebaseStorageController& ActionController)
{
	OnProgress.Broadcast(EFirebaseStorageError::None, FFirebaseStorageMetadata(), Controller);
}

#undef CALL_CALLBACK
#undef CHECK_REFERENCE
