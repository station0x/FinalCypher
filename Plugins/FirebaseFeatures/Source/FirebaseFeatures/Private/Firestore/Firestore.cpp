// Copyright Pandores Marketplace 2022. All Rights Reserved.
#include "Firestore/Firestore.h"

THIRD_PARTY_INCLUDES_START
#	include "firebase/firestore.h"
#	include "firebase/firestore/write_batch.h"
#	include "firebase/firestore/field_value.h"
#	include "firebase/firestore/field_path.h"
THIRD_PARTY_INCLUDES_END

#include "Async/Async.h"

#include "Firestore/CollectionReference.h"
#include "Firestore/DocumentReference.h"
#include "Firestore/Query.h"

#if !UE_BUILD_SHIPPING
#	include "Misc/MessageDialog.h"
#endif



FFirestoreSetOptions FFirestoreSetOptions::Merge()
{
	FFirestoreSetOptions Opt;

#if WITH_FIREBASE_FIRESTORE
	*Opt.Options = firebase::firestore::SetOptions::Merge();
#endif // WITH_FIREBASE_FIRESTORE

	return Opt;
}

FFirestoreSetOptions FFirestoreSetOptions::MergeFields(const TArray<FString>& Fields)
{
	FFirestoreSetOptions Opt;

#if WITH_FIREBASE_FIRESTORE
	std::vector<std::string> RawFields;

	RawFields.reserve(Fields.Num());

	for (const auto& Field : Fields)
	{
		RawFields.push_back(TCHAR_TO_UTF8(*Field));
	}

	*Opt.Options = firebase::firestore::SetOptions::MergeFields(RawFields);
#endif // WITH_FIREBASE_FIRESTORE

	return Opt;
}

FFirestoreSetOptions FFirestoreSetOptions::MergeFieldPaths(const TArray<FFirestoreFieldPath>& Fields)
{
	FFirestoreSetOptions Opt;

#if WITH_FIREBASE_FIRESTORE
	std::vector<firebase::firestore::FieldPath> RawFields;

	RawFields.reserve(Fields.Num());

	for (const auto& Field : Fields)
	{
		RawFields.push_back(Field);
	}

	*Opt.Options = firebase::firestore::SetOptions::MergeFieldPaths(RawFields);
#endif // WITH_FIREBASE_FIRESTORE

	return Opt;
}
FFirestoreSetOptions::FFirestoreSetOptions()
#if WITH_FIREBASE_FIRESTORE
	: Options(new firebase::firestore::SetOptions())
#endif // WITH_FIREBASE_FIRESTORE
{
}

FFirestoreSetOptions::FFirestoreSetOptions(const FFirestoreSetOptions& Other) : FFirestoreSetOptions()
{
#if WITH_FIREBASE_FIRESTORE
	*Options = *Other.Options;
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreSetOptions::FFirestoreSetOptions(FFirestoreSetOptions&& Other) : FFirestoreSetOptions()
{
#if WITH_FIREBASE_FIRESTORE
	*Options = *Other.Options;
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreSetOptions::~FFirestoreSetOptions()
{

}

FFirestoreSetOptions& FFirestoreSetOptions::operator=(const FFirestoreSetOptions& Other)
{
#if WITH_FIREBASE_FIRESTORE
	*Options = *Other.Options.Get();
#endif // WITH_FIREBASE_FIRESTORE
	return *this;
}


FFirestoreTransaction::FFirestoreTransaction()
#if WITH_FIREBASE_FIRESTORE
	: Transaction(nullptr)
#endif // WITH_FIREBASE_FIRESTORE
{}

#if WITH_FIREBASE_FIRESTORE
FFirestoreTransaction::FFirestoreTransaction(firebase::firestore::Transaction* const InTransaction)
	: Transaction(InTransaction)
{
}
#endif // WITH_FIREBASE_FIRESTORE

FFirestoreTransaction::FFirestoreTransaction(const FFirestoreTransaction&) : FFirestoreTransaction()
{
}

FFirestoreTransaction::~FFirestoreTransaction()
{
}

void FFirestoreTransaction::Set(UFirestoreDocumentReference* const Document, 
	const TMap<FString, FFirestoreFieldValue>& Data,
	const FFirestoreSetOptions& Options)
{
#if WITH_FIREBASE_FIRESTORE
	if (Transaction && Document)
	{
		std::unordered_map<std::string, firebase::firestore::FieldValue> Values;

		Values.reserve(Data.Num());

		for (const auto& DataElem : Data)
		{
			Values.emplace(TCHAR_TO_UTF8(*DataElem.Key), DataElem.Value);
		}

		Transaction->Set(*Document->GetInternal(), MoveTemp(Values), Options);
	}
#endif // WITH_FIREBASE_FIRESTORE
}

void FFirestoreTransaction::Update(UFirestoreDocumentReference* const Document,
	const TMap<FString, FFirestoreFieldValue>& Data)
{
#if WITH_FIREBASE_FIRESTORE
	if (Transaction && Document)
	{
		std::unordered_map<std::string, firebase::firestore::FieldValue> Values;

		Values.reserve(Data.Num());

		for (const auto& DataElem : Data)
		{
			Values.emplace(TCHAR_TO_UTF8(*DataElem.Key), DataElem.Value);
		}

		Transaction->Update(*Document->GetInternal(), MoveTemp(Values));
	}
#endif // WITH_FIREBASE_FIRESTORE
}


void FFirestoreTransaction::Update(UFirestoreDocumentReference* const Document,
	const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data)
{
#if WITH_FIREBASE_FIRESTORE
	if (Transaction && Document)
	{
		std::unordered_map<firebase::firestore::FieldPath, firebase::firestore::FieldValue> Values;

		for (const auto& DataElem : Data)
		{
			Values.emplace(DataElem.Key, DataElem.Value);
		}
		
		Transaction->Update(*Document->GetInternal(), MoveTemp(Values));
	}
#endif // WITH_FIREBASE_FIRESTORE
}

void FFirestoreTransaction::Delete(UFirestoreDocumentReference* const Document)
{
#if WITH_FIREBASE_FIRESTORE
	if (Transaction)
	{
		Transaction->Delete(*Document->GetInternal());
	}
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreDocumentSnapshot FFirestoreTransaction::Get(UFirestoreDocumentReference* const Document,
	EFirestoreError& ErrorCode, FString& ErrorMessage)
{
#if WITH_FIREBASE_FIRESTORE
	firebase::firestore::DocumentSnapshot Snap;
	if (Transaction && Document)
	{
		firebase::firestore::Error Err = firebase::firestore::Error::kErrorOk;
		std::string ErrMsg;
		Snap = Transaction->Get(*Document->GetInternal(), &Err, &ErrMsg);

		ErrorCode = (EFirestoreError)Err;
		ErrorMessage = UTF8_TO_TCHAR(ErrMsg.c_str());
	}
	else
	{
		ErrorCode    = EFirestoreError::InvalidArgument;
		ErrorMessage = TEXT("Invalid parameters");
	}
	return Snap;
#else
	return FFirestoreDocumentSnapshot();
#endif // WITH_FIREBASE_FIRESTORE
}

FWriteBatch::FWriteBatch()
#if WITH_FIREBASE_FIRESTORE
	: Batch(new firebase::firestore::WriteBatch())
#endif // WITH_FIREBASE_FIRESTORE
{

}

FWriteBatch::~FWriteBatch()
{

}

FWriteBatch& FWriteBatch::operator=(const FWriteBatch& Other)
{
#if WITH_FIREBASE_FIRESTORE
	*Batch = *Other.Batch;
#endif // WITH_FIREBASE_FIRESTORE
	return *this;
}


FWriteBatch& FWriteBatch::Set(UFirestoreDocumentReference* const Document,
	const TMap<FString, FFirestoreFieldValue>& Data,
	const FFirestoreSetOptions& Options)
{
#if WITH_FIREBASE_FIRESTORE
	if (Document)
	{
		std::unordered_map<std::string, firebase::firestore::FieldValue> RawData;

		RawData.reserve(Data.Num());

		for (const auto& DataElem : Data)
		{
			RawData.emplace(TCHAR_TO_UTF8(*DataElem.Key), DataElem.Value);
		}

		Batch->Set(*Document->GetInternal(), MoveTemp(RawData), Options);
	}
#endif // WITH_FIREBASE_FIRESTORE

	return *this;
}

FWriteBatch& FWriteBatch::Update(UFirestoreDocumentReference* const Document,
	const TMap<FString, FFirestoreFieldValue>& Data)
{
#if WITH_FIREBASE_FIRESTORE
	if (Document)
	{
		std::unordered_map<std::string, firebase::firestore::FieldValue> RawData;

		RawData.reserve(Data.Num());

		for (const auto& DataElem : Data)
		{
			RawData.emplace(TCHAR_TO_UTF8(*DataElem.Key), DataElem.Value);
		}

		Batch->Update(*Document->GetInternal(), MoveTemp(RawData));
	}
#endif // WITH_FIREBASE_FIRESTORE

	return *this;
}


FWriteBatch& FWriteBatch::Update(UFirestoreDocumentReference* const Document,
	const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data)
{
#if WITH_FIREBASE_FIRESTORE
	if (Document)
	{
		std::unordered_map<firebase::firestore::FieldPath, firebase::firestore::FieldValue> RawData;

		RawData.reserve(Data.Num());

		for (const auto& DataElem : Data)
		{
			RawData.emplace(DataElem.Key, DataElem.Value);
		}

		Batch->Update(*Document->GetInternal(), MoveTemp(RawData));
	}
#endif // WITH_FIREBASE_FIRESTORE

	return *this;
}

FWriteBatch& FWriteBatch::Delete(UFirestoreDocumentReference* const Document)
{
#if WITH_FIREBASE_FIRESTORE
	Batch->Delete(*Document->GetInternal());
#endif // WITH_FIREBASE_FIRESTORE

	return *this;
}

void FWriteBatch::Commit(const FFirestoreCallback& Callback)
{
#if WITH_FIREBASE_FIRESTORE
	Batch->Commit().OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		const EFirestoreError Error = (EFirestoreError) Future.error();
		if (Error != EFirestoreError::Ok)
		{
			UE_LOG(LogFirestore, Error, TEXT("Failed to commit Write Batch. Code: %d. Message: %s"), 
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Error, Callback]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});
		}
	});
#endif // WITH_FIREBASE_FIRESTORE
}

#if WITH_FIREBASE_FIRESTORE
firebase::firestore::Firestore* UFirestore::GetFirestore()
{
#if !UE_BUILD_SHIPPING
	if (!FFirebaseFeaturesModule::GetCurrentFirebaseApp())
	{
		FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("Firestore", "GetFirestoreFailedApp", 
			"Failed to get Firebase application instance. Please, make sure your google-services.json is valid."));
		checkf(false, TEXT("Failed to get application instance."));
		return nullptr;
	}
#endif

	auto Instance = firebase::firestore::Firestore::GetInstance();
	if (!Instance)
	{
#if !UE_BUILD_SHIPPING
		FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("Firestore", "GetFirestoreFailed",
			"Failed to get Firestore instance. Please, make sure your google-services.json is valid."));
#endif
		checkf(false, TEXT("Failed to get firestore instance."));
		return nullptr;
	}

	return Instance;
}
#endif // WITH_FIREBASE_FIRESTORE

UFirestoreCollectionReference* UFirestore::GetCollection(const FString& CollectionPath)
{
	if (CollectionPath.IsEmpty())
	{
		UE_LOG(LogFirestore, Error, TEXT("Called GetCollection() with an empty collection path."));
		return nullptr;
	}

	if (CollectionPath.Contains(TEXT("//")))
	{
		UE_LOG(LogFirestore, Error, TEXT("CollectionPath can't contain a double slash (\"//\")."));
		return nullptr;
	}

	UFirestoreCollectionReference* const Collection = NewObject<UFirestoreCollectionReference>();

#if WITH_FIREBASE_FIRESTORE
	firebase::firestore::Firestore* const Firestore = GetFirestore();

	check(Firestore);
	check(Collection->GetQuery());

	Collection->GetQuery()->operator=(Firestore->Collection(TCHAR_TO_UTF8(*CollectionPath)));

#endif // WITH_FIREBASE_FIRESTORE

	return Collection;
}

UFirestoreDocumentReference* UFirestore::GetDocument(const FString& DocumentPath)
{
	if (DocumentPath.IsEmpty())
	{
		UE_LOG(LogFirestore, Error, TEXT("Called GetDocument() with an empty document path."));
		return nullptr;
	}

	if (DocumentPath.Contains(TEXT("//")))
	{
		UE_LOG(LogFirestore, Error, TEXT("DocumentPath can't contain a double slash (\"//\")."));
		return nullptr;
	}

	UFirestoreDocumentReference* const Document = NewObject<UFirestoreDocumentReference>();

#if WITH_FIREBASE_FIRESTORE
	firebase::firestore::Firestore* const Firestore = GetFirestore();

	check(Firestore);

	*Document->Reference = Firestore->Document(TCHAR_TO_UTF8(*DocumentPath));
#endif // WITH_FIREBASE_FIRESTORE

	return Document;
}

UFirestoreQuery* UFirestore::CollectionGroup(const FString& CollectionId)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE
	*Query->Reference = GetFirestore()->CollectionGroup(TCHAR_TO_UTF8(*CollectionId));
#endif // WITH_FIREBASE_FIRESTORE

	return Query;
}

FFirestoreSettings UFirestore::GetSettings()
{
	FFirestoreSettings Settings;

#if WITH_FIREBASE_FIRESTORE
	firebase::firestore::Settings RawSettings = GetFirestore()->settings();

	Settings.bPersistenceEnabled	= RawSettings.is_persistence_enabled();
	Settings.bSslEnabled			= RawSettings.is_ssl_enabled();
	Settings.Host					= UTF8_TO_TCHAR(RawSettings.host().c_str());
#endif // WITH_FIREBASE_FIRESTORE

	return Settings;
}

void UFirestore::SetSettings(const FFirestoreSettings& Settings)
{
#if WITH_FIREBASE_FIRESTORE
	firebase::firestore::Settings RawSettings;
	
	RawSettings.set_persistence_enabled	(Settings.bPersistenceEnabled);
	RawSettings.set_ssl_enabled			(Settings.bSslEnabled);
	RawSettings.set_host				(TCHAR_TO_UTF8(*Settings.Host));

	GetFirestore()->set_settings(RawSettings);
#endif // WITH_FIREBASE_FIRESTORE
}

void UFirestore::RunTransaction(const FFirestoreTransactionCallback& TransactionFunc, const FFirestoreCallback& Callback)
{
#if WITH_FIREBASE_FIRESTORE
	GetFirestore()->RunTransaction([TransactionFunc](firebase::firestore::Transaction& Trans, std::string& Error) -> firebase::firestore::Error
	{
		FFirestoreTransaction Transaction(&Trans);

		FString ErrorMessage;

		const firebase::firestore::Error Err = TransactionFunc.IsBound() ?
			(firebase::firestore::Error)TransactionFunc.Execute(Transaction, ErrorMessage) :
			firebase::firestore::Error::kErrorOk;

		Error = TCHAR_TO_UTF8(*ErrorMessage);

		return Err;
	}).OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		const EFirestoreError Error = (EFirestoreError)Future.error();
		if (Error != EFirestoreError::Ok)
		{
			UE_LOG(LogFirestore, Error, TEXT("Failed to run transaction. Code: %d. Message: %s"), 
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}
		
		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Callback, Error]()->void
			{
				Callback.ExecuteIfBound(Error);
			});
		}
	});
#endif // WITH_FIREBASE_FIRESTORE
}

void UFirestore::DisableNetwork(const FFirestoreCallback& Callback)
{
#if WITH_FIREBASE_FIRESTORE
	GetFirestore()->DisableNetwork().OnCompletion([Callback](const firebase::Future<void> & Future) -> void
	{
		const EFirestoreError Error = (EFirestoreError)Future.error();
		if (Error != EFirestoreError::Ok)
		{
			UE_LOG(LogFirestore, Error, TEXT("Failed to disable network. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});
		}
	});
#endif // WITH_FIREBASE_FIRESTORE
}

void UFirestore::EnableNetwork(const FFirestoreCallback& Callback)
{
#if WITH_FIREBASE_FIRESTORE
	GetFirestore()->EnableNetwork().OnCompletion([Callback](const firebase::Future<void> & Future) -> void
	{
		const EFirestoreError Error = (EFirestoreError)Future.error();
		if (Error != EFirestoreError::Ok)
		{
			UE_LOG(LogFirestore, Error, TEXT("Failed to enable network. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});
		}
	});
#endif // WITH_FIREBASE_FIRESTORE
}

void UFirestore::WaitForPendingWrites(const FFirestoreCallback& Callback)
{
#if WITH_FIREBASE_FIRESTORE
	GetFirestore()->WaitForPendingWrites().OnCompletion([Callback](const firebase::Future<void> & Future) -> void
	{
		const EFirestoreError Error = (EFirestoreError)Future.error();
		if (Error != EFirestoreError::Ok)
		{
			UE_LOG(LogFirestore, Error, TEXT("Failed to wait for pending writes. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});
		}
	});
#endif // WITH_FIREBASE_FIRESTORE
}

//void UFirestore::SetPersistenceEnabled(const bool bEnabled)
//{
//#if WITH_FIREBASE_FIRESTORE
//	auto* const Firestore = GetFirestore();
//
//	auto settings = Firestore->settings();
//
//	settings.set_persistence_enabled(bEnabled);
//
//	Firestore->set_settings(MoveTemp(settings));
//#endif
//}
