// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Firestore/FieldPath.h"

uint32 FIREBASEFEATURES_API GetTypeHash(const FFirestoreFieldPath& Value)
{
	return GetTypeHash(Value.ToString());
}

FFirestoreFieldPath::FFirestoreFieldPath()
#if WITH_FIREBASE_FIRESTORE
	: Path(new firebase::firestore::FieldPath())
#endif
{
}

FFirestoreFieldPath::FFirestoreFieldPath(const FFirestoreFieldPath& Other) : FFirestoreFieldPath()
{
#if WITH_FIREBASE_FIRESTORE
	*Path = *Other.Path;
#endif
}

FFirestoreFieldPath::FFirestoreFieldPath(const FFirestoreFieldPath&& Other) : FFirestoreFieldPath()
{
#if WITH_FIREBASE_FIRESTORE
	*Path = *Other.Path;
#endif
}

FFirestoreFieldPath::FFirestoreFieldPath(const TArray<FString>& FieldNames) : FFirestoreFieldPath()
{
#if WITH_FIREBASE_FIRESTORE
	std::vector<std::string> Fields;

	Fields.reserve(FieldNames.Num());

	for (const auto& Field : FieldNames)
	{
		Fields.push_back(TCHAR_TO_UTF8(*Field));
	}

	*Path = firebase::firestore::FieldPath(Fields);
#endif
}

FFirestoreFieldPath::~FFirestoreFieldPath()
{

}

FFirestoreFieldPath& FFirestoreFieldPath::operator=(const FFirestoreFieldPath& Other)
{
#if WITH_FIREBASE_FIRESTORE
	*Path = *Other.Path;
#endif
	return *this;
}

FFirestoreFieldPath FFirestoreFieldPath::DocumentId()
{
	FFirestoreFieldPath FieldPath;

#if WITH_FIREBASE_FIRESTORE
	*FieldPath.Path = firebase::firestore::FieldPath::DocumentId();
#endif

	return FieldPath;
}

FString FFirestoreFieldPath::ToString() const
{
#if WITH_FIREBASE_FIRESTORE
	return UTF8_TO_TCHAR(Path->ToString().c_str());
#else
	return TEXT("FIRESTORE_DISABLED");
#endif
}