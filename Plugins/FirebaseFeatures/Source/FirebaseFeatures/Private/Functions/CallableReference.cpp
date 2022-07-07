// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Functions/CallableReference.h"
#include "FirebaseFeatures.h"

#if WITH_FIREBASE_FUNCTIONS
THIRD_PARTY_INCLUDES_START
#	include "firebase/functions/callable_reference.h"
#	include "firebase/functions/callable_result.h"
THIRD_PARTY_INCLUDES_END
#endif

#include "Async/Async.h"


FFirebaseHttpsCallableReference::FFirebaseHttpsCallableReference()
#if WITH_FIREBASE_FUNCTIONS
	: Reference(MakeUnique<firebase::functions::HttpsCallableReference>())
#endif
{
}

#if WITH_FIREBASE_FUNCTIONS
FFirebaseHttpsCallableReference::FFirebaseHttpsCallableReference(const firebase::functions::HttpsCallableReference& InReference)
	: Reference(MakeUnique<firebase::functions::HttpsCallableReference>(InReference))
{
}
#endif

FFirebaseHttpsCallableReference::FFirebaseHttpsCallableReference(const FFirebaseHttpsCallableReference& Other)
	: FFirebaseHttpsCallableReference()
{
#if WITH_FIREBASE_FUNCTIONS
	*Reference = *Other.Reference;
#endif
}

FFirebaseHttpsCallableReference::FFirebaseHttpsCallableReference(FFirebaseHttpsCallableReference&& Other)
	: FFirebaseHttpsCallableReference()
{
#if WITH_FIREBASE_FUNCTIONS
	*Reference = MoveTemp(*Other.Reference);
#endif
}

FFirebaseHttpsCallableReference::~FFirebaseHttpsCallableReference()
{
#if WITH_FIREBASE_FUNCTIONS
	Reference.Reset();
#endif
}

FFirebaseHttpsCallableReference& FFirebaseHttpsCallableReference::operator=(const FFirebaseHttpsCallableReference& Other)
{
#if WITH_FIREBASE_FUNCTIONS
	*Reference = *Other.Reference;
#endif
	return *this;
}

FFirebaseHttpsCallableReference& FFirebaseHttpsCallableReference::operator=(FFirebaseHttpsCallableReference&& Other)
{
#if WITH_FIREBASE_FUNCTIONS
	*Reference = MoveTemp(*Other.Reference);
#endif
	return *this;
}

bool FFirebaseHttpsCallableReference::IsValid() const
{
#if WITH_FIREBASE_FUNCTIONS
	return Reference && Reference->is_valid();
#else
	return false;
#endif
}

void FFirebaseHttpsCallableReference::Call(const FFunctionsCallCallback& Callback)
{
#if WITH_FIREBASE_FUNCTIONS
	using namespace firebase;
	
	functions::HttpsCallableReference* Copy = new functions::HttpsCallableReference(*Reference);

	Copy->Call().OnCompletion([Copy, Callback](const Future<functions::HttpsCallableResult>& Future) -> void
	{
		const EFirebaseFunctionsError Error = (EFirebaseFunctionsError)Future.error();
		if (Error != EFirebaseFunctionsError::None)
		{
			UE_LOG(LogFirebaseFunctions, Error, TEXT("Function call failed. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		functions::HttpsCallableResult Result = Future.result() ? *Future.result() : functions::HttpsCallableResult();

		AsyncTask(ENamedThreads::GameThread, [Copy, Callback, Error, Result = MoveTemp(Result)]() -> void
		{
			Callback.ExecuteIfBound(Error, Result.data());
			delete Copy;
		});
	});
#endif
}

#if WITH_FIREBASE_FUNCTIONS
static void CallInternal(firebase::functions::HttpsCallableReference& Ref, const FFirebaseVariant& Data, const FFunctionsCallCallback& Callback)
{
	firebase::functions::HttpsCallableReference* Copy = new firebase::functions::HttpsCallableReference(Ref);

	Copy->Call(Data.GetRawVariant()).OnCompletion([Copy, Callback](const firebase::Future<firebase::functions::HttpsCallableResult>& Future) mutable -> void
	{
		const EFirebaseFunctionsError Error = (EFirebaseFunctionsError)Future.error();
		if (Error != EFirebaseFunctionsError::None)
		{
			UE_LOG(LogFirebaseFunctions, Error, TEXT("Function call failed. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		firebase::Variant Result = Future.result() ? Future.result()->data() : firebase::Variant::Null();

		AsyncTask(ENamedThreads::GameThread, [Copy, Callback, Error, Result = MoveTemp(Result)]() -> void
		{
			Callback.ExecuteIfBound(Error, Result);
			delete Copy;
		});
	});
}
#endif

void FFirebaseHttpsCallableReference::Call(const FFirebaseVariant& Data, const FFunctionsCallCallback& Callback)
{
#if WITH_FIREBASE_FUNCTIONS
	CallInternal(*Reference, Data, Callback);
#endif
}

FFirebaseHttpsCallableReference::operator bool() const
{
	return IsValid();
}

void FFirebaseHttpsCallableReference::operator()(const FFunctionsCallCallback& Callback)
{
	Call(Callback);
}

void FFirebaseHttpsCallableReference::operator()(const FFirebaseVariant& Data, const FFunctionsCallCallback& Callback)
{
	Call(Data, Callback);
}

