// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Firestore/FirestoreNodes.h"
#include "Firestore/DocumentReference.h"
#include "Firestore/CollectionReference.h"

FFirestoreSetOptions UFirestoreLibrary::Merge()
{
	return FFirestoreSetOptions::Merge();
}

FFirestoreSetOptions UFirestoreLibrary::MergeFields(const TArray<FString>& Fields)
{
	return FFirestoreSetOptions::MergeFields(Fields);
}

FFirestoreSetOptions UFirestoreLibrary::MergeFieldPaths(const TArray<FFirestoreFieldPath>& Fields)
{
	return FFirestoreSetOptions::MergeFieldPaths(Fields);
}

void UFirestoreLibrary::Set(FFirestoreTransaction& Transaction, UFirestoreDocumentReference* const Document, const TMap<FString, FFirestoreFieldValue>& Data,
	const FFirestoreSetOptions& options)
{
	Transaction.Set(Document, Data, options);
}

void UFirestoreLibrary::Update(FFirestoreTransaction& Transaction, UFirestoreDocumentReference* const Document,
	const TMap<FString, FFirestoreFieldValue>& Data)
{
	Transaction.Update(Document, Data);
}

void UFirestoreLibrary::UpdateFieldPath(FFirestoreTransaction& Transaction, UFirestoreDocumentReference* const Document,
	const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data)
{
	Transaction.Update(Document, Data);
}

void UFirestoreLibrary::Delete(FFirestoreTransaction& Transaction, UFirestoreDocumentReference* const Document)
{
	Transaction.Delete(Document);
}

FFirestoreDocumentSnapshot UFirestoreLibrary::Get(FFirestoreTransaction& Transaction, UFirestoreDocumentReference* const Document,
	EFirestoreError& ErrorCode, FString& ErrorMessage)
{
	return Transaction.Get(Document, ErrorCode, ErrorMessage);
}

FWriteBatch& UFirestoreLibrary::WriteBatch_Set(FWriteBatch& WriteBatch, UFirestoreDocumentReference* const Document,
	const TMap<FString, FFirestoreFieldValue>& Data,
	const FFirestoreSetOptions& Options)
{
	return WriteBatch.Set(Document, Data, Options);
}

FWriteBatch& UFirestoreLibrary::WriteBatch_Update(UPARAM(ref) FWriteBatch& WriteBatch, UFirestoreDocumentReference* const Document,
	const TMap<FString, FFirestoreFieldValue>& Data)
{
	return WriteBatch.Update(Document, Data);
}

FWriteBatch& UFirestoreLibrary::WriteBatch_UpdateFieldPath(UPARAM(ref) FWriteBatch& WriteBatch, UFirestoreDocumentReference* const Document,
	const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data)
{
	return WriteBatch.Update(Document, Data);
}

FWriteBatch& UFirestoreLibrary::WriteBatch_Delete(UPARAM(ref) FWriteBatch& WriteBatch, UFirestoreDocumentReference* const Document)
{
	return WriteBatch.Delete(Document);
}

FFirestoreFieldPath UFirestoreLibrary::DocumentId()
{
	return FFirestoreFieldPath::DocumentId();
}

FString UFirestoreLibrary::ToString(UPARAM(ref) const FFirestoreFieldPath& FieldPath)
{
	return FieldPath.ToString();
}

FFirestoreFieldPath UFirestoreLibrary::MakeFirestoreFieldPath(const TArray<FString>& FieldNames)
{
	return FFirestoreFieldPath(FieldNames);
}

FString UFirestoreLibrary::GetId(const FFirestoreDocumentSnapshot& DocumentSnapshot)
{
	return DocumentSnapshot.GetId();
}

UFirestoreDocumentReference* UFirestoreLibrary::GetReference(const FFirestoreDocumentSnapshot& DocumentSnapshot)
{
	return DocumentSnapshot.GetReference();
}

FFirestoreSnapshotMetadata UFirestoreLibrary::GetMetadata(const FFirestoreDocumentSnapshot& DocumentSnapshot)
{
	return DocumentSnapshot.GetMetadata();
}

bool UFirestoreLibrary::Exists(const FFirestoreDocumentSnapshot& DocumentSnapshot)
{
	return DocumentSnapshot.Exists();
}

TMap<FString, FFirestoreFieldValue> UFirestoreLibrary::GetData(
	const FFirestoreDocumentSnapshot& DocumentSnapshot,
	EFirestoreServerTimestampBehavior ServerTimestampBehavior)
{
	return DocumentSnapshot.GetData(ServerTimestampBehavior);
}

FFirestoreFieldValue UFirestoreLibrary::Get_Snapshot(
	const FFirestoreDocumentSnapshot& DocumentSnapshot,
	const FString& Field,
	EFirestoreServerTimestampBehavior ServerTimestampBehavior)
{
	return DocumentSnapshot.Get(Field, ServerTimestampBehavior);
}

FFirestoreFieldValue UFirestoreLibrary::GetWithFieldPath(
	const FFirestoreDocumentSnapshot& DocumentSnapshot,
	const FFirestoreFieldPath& Field,
	EFirestoreServerTimestampBehavior ServerTimestampBehavior)
{
	return DocumentSnapshot.Get(Field, ServerTimestampBehavior);
}

FString UFirestoreLibrary::ToString_Snapshot(const FFirestoreDocumentSnapshot& DocumentSnapshot)
{
	return DocumentSnapshot.ToString();
}

#define BROADCAST_RESULT(OnSuccess, ...)														\
	do {																						\
		(Error == EFirestoreError::Ok ? OnSuccess : OnError).Broadcast(Error, ## __VA_ARGS__);	\
		SetReadyToDestroy();																	\
	} while(0)


UWriteBatchCommitProxy* UWriteBatchCommitProxy::Commit(UPARAM(ref) FWriteBatch& WriteBatch)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	// Write batch gonna switch thread so we can call it here to avoid a shallow copy.
	WriteBatch.Commit(FFirestoreCallback::CreateUObject(Proxy, &ThisClass::OnActionOver));

	return Proxy;
}

void UWriteBatchCommitProxy::Activate()
{}

void UWriteBatchCommitProxy::OnActionOver(const EFirestoreError Error)
{
	BROADCAST_RESULT(OnCommitted);
}

UFirestoreRunTransactionProxy* UFirestoreRunTransactionProxy::RunTransaction(const FDynamicTransactionCallback& TransactionFunction)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->TransactionCallback = TransactionFunction;

	return Proxy;
}

void UFirestoreRunTransactionProxy::Activate()
{
	UFirestore::RunTransaction
	(
		FFirestoreTransactionCallback::CreateUObject(this, &ThisClass::TransactionMethod),
		FFirestoreCallback::CreateUObject(this, &ThisClass::OnActionOver)
	);
}

EFirestoreError UFirestoreRunTransactionProxy::TransactionMethod(const FFirestoreTransaction& Transaction, FString& ErrorMessage)
{
	return TransactionCallback.IsBound() ? TransactionCallback.Execute(Transaction, ErrorMessage) : EFirestoreError::Ok;
}

void UFirestoreRunTransactionProxy::OnActionOver(const EFirestoreError Error)
{
	BROADCAST_RESULT(OnRun);
}


UDisableNetworkProxy* UDisableNetworkProxy::DisableNetwork()
{
	return NewObject<ThisClass>();
}

void UDisableNetworkProxy::Activate()
{
	UFirestore::DisableNetwork(FFirestoreCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void UDisableNetworkProxy::OnActionOver(const EFirestoreError Error)
{
	BROADCAST_RESULT(OnDisabled);
}

UEnableNetworkProxy* UEnableNetworkProxy::DisableNetwork()
{
	return NewObject<ThisClass>();
}

void UEnableNetworkProxy::Activate()
{
	UFirestore::EnableNetwork(FFirestoreCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void UEnableNetworkProxy::OnActionOver(const EFirestoreError Error)
{
	BROADCAST_RESULT(OnEnabled);
}

UWaitForPendingWritesProxy* UWaitForPendingWritesProxy::DisableNetwork()
{
	return NewObject<ThisClass>();
}

void UWaitForPendingWritesProxy::Activate()
{
	UFirestore::WaitForPendingWrites(FFirestoreCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void UWaitForPendingWritesProxy::OnActionOver(const EFirestoreError Error)
{
	BROADCAST_RESULT(OnPendingWritesOver);
}

#define CHECK_REFERENCE(...)															\
	if (!Reference)																		\
	{																					\
		UE_LOG(LogFirestore, Error, TEXT("Passed a nullptr Document reference."));		\
		OnActionOver(EFirestoreError::InvalidArgument, ## __VA_ARGS__);					\
		return;																			\
	}

UDocumentReferenceGetProxy* UDocumentReferenceGetProxy::Get(UFirestoreDocumentReference* const DocumentReference, EFirestoreSource Source)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Reference = DocumentReference;
	Proxy->Src = Source;

	return Proxy;
}

void UDocumentReferenceGetProxy::Activate()
{
	CHECK_REFERENCE(FFirestoreDocumentSnapshot());

	Reference->Get(Src, FDocumentSnapshotCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void UDocumentReferenceGetProxy::OnActionOver(const EFirestoreError Error, const FFirestoreDocumentSnapshot& Snapshot)
{
	BROADCAST_RESULT(OnGot, Snapshot);
}

UDocumentReferenceSetProxy* UDocumentReferenceSetProxy::Set(UFirestoreDocumentReference* const DocumentReference,
	const TMap<FString, FFirestoreFieldValue>& Data,
	const FFirestoreSetOptions& Options)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Reference = DocumentReference;
	Proxy->SetData = Data;
	Proxy->SetOptions = Options;

	return Proxy;
}

void UDocumentReferenceSetProxy::Activate()
{
	CHECK_REFERENCE();

	Reference->Set(SetData, SetOptions, FFirestoreCallback::CreateUObject(this, &ThisClass::OnActionOver));
}


void UDocumentReferenceSetProxy::OnActionOver(const EFirestoreError Error)
{
	BROADCAST_RESULT(OnSet);
}


UDocumentReferenceUpdateProxy* UDocumentReferenceUpdateProxy::Update(UFirestoreDocumentReference* const DocumentReference, const TMap<FString, FFirestoreFieldValue>& Data)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Reference = DocumentReference;
	Proxy->UpdateData = Data;

	return Proxy;
}

void UDocumentReferenceUpdateProxy::Activate()
{
	CHECK_REFERENCE();

	Reference->Update(UpdateData, FFirestoreCallback::CreateUObject(this, &ThisClass::OnActionOver));
}


void UDocumentReferenceUpdateProxy::OnActionOver(const EFirestoreError Error)
{
	BROADCAST_RESULT(OnUpdated);
}

UDocumentReferenceUpdateWithPathProxy* UDocumentReferenceUpdateWithPathProxy::UpdateWithPath(UFirestoreDocumentReference* const DocumentReference, const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Reference = DocumentReference;
	Proxy->UpdateData = Data;

	return Proxy;
}

void UDocumentReferenceUpdateWithPathProxy::Activate()
{
	CHECK_REFERENCE();

	Reference->Update(UpdateData, FFirestoreCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void UDocumentReferenceUpdateWithPathProxy::OnActionOver(const EFirestoreError Error)
{
	BROADCAST_RESULT(OnUpdated);
}

UDocumentReferenceDeleteProxy* UDocumentReferenceDeleteProxy::Delete(UFirestoreDocumentReference* const DocumentReference)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Reference = DocumentReference;

	return Proxy;
}

void UDocumentReferenceDeleteProxy::Activate()
{
	CHECK_REFERENCE();

	Reference->Delete(FFirestoreCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void UDocumentReferenceDeleteProxy::OnActionOver(const EFirestoreError Error)
{
	BROADCAST_RESULT(OnDeleted);
}


UCollectionReferenceAddProxy* UCollectionReferenceAddProxy::Add(UFirestoreCollectionReference* const DocumentReference, TMap<FString, FFirestoreFieldValue> Data)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Reference = DocumentReference;
	Proxy->DataAdd = MoveTemp(Data);

	return Proxy;
}

void UCollectionReferenceAddProxy::Activate()
{
	if (!Reference)
	{
		UE_LOG(LogFirestore, Error, TEXT("Passed a nullptr Collection reference."));
		OnActionOver(EFirestoreError::InvalidArgument, nullptr);
		return;
	}

	Reference->Add(DataAdd, FFirestoreDocumentCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void UCollectionReferenceAddProxy::OnActionOver(const EFirestoreError Error, UFirestoreDocumentReference* const Document)
{
	BROADCAST_RESULT(OnAdded, Document);
}

#undef CHECK_REFERENCE
#undef BROADCAST_RESULT

UQueryGetProxy* UQueryGetProxy::Get(UFirestoreQuery* Query, const EFirestoreSource Source)
{
	UQueryGetProxy* const Proxy = NewObject<UQueryGetProxy>();

	Proxy->Query  = Query;
	Proxy->Source = Source;

	return Proxy;
}

void UQueryGetProxy::Activate()
{
	if (!Query)
	{
		UE_LOG(LogFirestore, Error, TEXT("Passed a nullptr Query to Get()."));
		OnTaskOver(EFirestoreError::InvalidArgument, {}, {});
		return;
	}

	Query->Get(Source,FFirestoreQueryCallback::CreateUObject(this, &ThisClass::OnTaskOver));
}

void UQueryGetProxy::OnTaskOver(const EFirestoreError Error, TArray<FFirestoreDocumentSnapshot> DocumentSnapshots, TArray<class UFirestoreDocumentChange*> Changes)
{
	(Error == EFirestoreError::Ok ? OnGot : OnError).Broadcast(Error, DocumentSnapshots, Changes);
	SetReadyToDestroy();
}
