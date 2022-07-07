// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Functions/FunctionsNodes.h"

bool UFirebaseFunctionsBlueprintLibrary::IsValid(const FFirebaseHttpsCallableReference& HttpsCallable)
{
#if WITH_FIREBASE_FUNCTIONS
	return HttpsCallable.IsValid();
#else
	return false;
#endif
}

UCallHttpsFunctionsProxy* UCallHttpsFunctionsProxy::Call(FFirebaseHttpsCallableReference& HttpsCallable)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

#if WITH_FIREBASE_FUNCTIONS
	Proxy->Callable = HttpsCallable;
#endif

	return Proxy;
}

void UCallHttpsFunctionsProxy::Activate()
{
#if WITH_FIREBASE_FUNCTIONS
	if (!Callable.IsValid())
	{
		UE_LOG(LogFirebaseFunctions, Error, TEXT("Tried to call a Cloud Function with an invalid Callable Reference."));
		OnActionOver(EFirebaseFunctionsError::Cancelled, {});
		return;
	}

	Callable.Call(FFunctionsCallCallback::CreateUObject(this, &ThisClass::OnActionOver));
#else
	OnActionOver(EFirebaseFunctionsError::Unavailable, {});
#endif
}

void UCallHttpsFunctionsProxy::OnActionOver(const EFirebaseFunctionsError Error, const FFirebaseVariant& Result)
{
	(Error == EFirebaseFunctionsError::None ? OnCalled : OnError).Broadcast(Error, Result);
	SetReadyToDestroy();
}

UCallHttpsFunctionsWithDataProxy* UCallHttpsFunctionsWithDataProxy::CallWithData(FFirebaseHttpsCallableReference& HttpsCallable, const FFirebaseVariant& Data)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

#if WITH_FIREBASE_FUNCTIONS
	Proxy->Callable = HttpsCallable;
	Proxy->CallData = Data;
#endif

	return Proxy;
}

void UCallHttpsFunctionsWithDataProxy::Activate()
{
#if WITH_FIREBASE_FUNCTIONS
	if (!Callable.IsValid())
	{
		UE_LOG(LogFirebaseFunctions, Error, TEXT("Tried to call a Cloud Function with an invalid Callable Reference."));
		OnActionOver(EFirebaseFunctionsError::Cancelled, {});
		return;
	}

	Callable.Call(CallData, FFunctionsCallCallback::CreateUObject(this, &ThisClass::OnActionOver));
#else
	OnActionOver(EFirebaseFunctionsError::Unavailable, {});
#endif
}

void UCallHttpsFunctionsWithDataProxy::OnActionOver(const EFirebaseFunctionsError Error, const FFirebaseVariant& Result)
{
	(Error == EFirebaseFunctionsError::None ? OnCalled : OnError).Broadcast(Error, Result);
	SetReadyToDestroy();
}
