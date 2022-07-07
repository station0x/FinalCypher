// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Storage/Storage.h"
#include "FirebaseFeatures.h"

#if WITH_FIREBASE_STORAGE
THIRD_PARTY_INCLUDES_START
#	include "firebase/storage.h"
#	include "firebase/storage/storage_reference.h"
THIRD_PARTY_INCLUDES_END
#endif

#include "Storage/StorageReference.h"

#if WITH_FIREBASE_STORAGE
static firebase::storage::Storage* GetStorage()
{
	check(FFirebaseFeaturesModule::GetCurrentFirebaseApp());

	auto* const StorageInstance = firebase::storage::Storage::GetInstance(FFirebaseFeaturesModule::GetCurrentFirebaseApp());

	checkf(StorageInstance, TEXT("Firebase Storage instance creation failed."));

	return StorageInstance;
}
#endif

FString UFirebaseStorage::GetUrl()
{
#if WITH_FIREBASE_STORAGE
	return UTF8_TO_TCHAR(GetStorage()->url().c_str());
#else
	return {};
#endif
}

UFirebaseStorageReference* UFirebaseStorage::GetReference()
{
#if WITH_FIREBASE_STORAGE
	UFirebaseStorageReference* const Reference = NewObject<UFirebaseStorageReference>();

	Reference->Reference = GetStorage()->GetReference();

	return Reference;
#else
	return nullptr;
#endif
}

UFirebaseStorageReference* UFirebaseStorage::GetReferenceFromPath(const FString& Path)
{
#if WITH_FIREBASE_STORAGE
	UFirebaseStorageReference* const Reference = NewObject<UFirebaseStorageReference>();

	Reference->Reference = GetStorage()->GetReference(TCHAR_TO_UTF8(*Path));

	return Reference;
#else
	return nullptr;
#endif
}

UFirebaseStorageReference* UFirebaseStorage::GetReferenceFromUrl(const FString& Url)
{
#if WITH_FIREBASE_STORAGE
	UFirebaseStorageReference* const Reference = NewObject<UFirebaseStorageReference>();

	Reference->Reference = GetStorage()->GetReferenceFromUrl(TCHAR_TO_UTF8(*Url));

	return Reference;
#else
	return nullptr;
#endif
}

float UFirebaseStorage::GetMaxDownloadRetryTime()
{
#if WITH_FIREBASE_STORAGE
	return (float)GetStorage()->max_download_retry_time();
#else
	return 0.f;
#endif
}

void  UFirebaseStorage::SetMaxDownloadRetryTime(float MaxTransferRetrySeconds)
{
#if WITH_FIREBASE_STORAGE
	GetStorage()->set_max_download_retry_time((double)MaxTransferRetrySeconds);
#endif
}

float UFirebaseStorage::GetMaxUploadRetryTime()
{
#if WITH_FIREBASE_STORAGE
	return (float)GetStorage()->max_upload_retry_time();
#else
	return 0.f;
#endif
}

void  UFirebaseStorage::SetMaxUploadRetryTime(float MaxTransferRetrySeconds)
{
#if WITH_FIREBASE_STORAGE
	GetStorage()->set_max_upload_retry_time((double)MaxTransferRetrySeconds);
#endif
}

float UFirebaseStorage::GetMaxOperationRetryTime()
{
#if WITH_FIREBASE_STORAGE
	return (float)GetStorage()->max_operation_retry_time();
#else
	return 0.f;
#endif
}

void  UFirebaseStorage::SetMaxOperationRetryTime(float MaxTransferRetrySeconds)
{
#if WITH_FIREBASE_STORAGE
	GetStorage()->set_max_operation_retry_time((double)MaxTransferRetrySeconds);
#endif
}
