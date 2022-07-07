// Copyright Pandores Marketplace 2022. All Rights Reserved.
#include "Functions/FunctionsLibrary.h"
#include "FirebaseFeatures.h"

#if WITH_FIREBASE_FUNCTIONS
THIRD_PARTY_INCLUDES_START
#	include "firebase/functions.h"
#	include "firebase/functions/callable_reference.h"
THIRD_PARTY_INCLUDES_END
#endif

void UFirebaseFunctionsLibrary::UseFunctionsEmulator(const char* Origin)
{
#if WITH_FIREBASE_FUNCTIONS
	using namespace firebase;

	functions::Functions* const Functions = functions::Functions::GetInstance(FFirebaseFeaturesModule::GetCurrentFirebaseApp());

	check(Functions != nullptr);

	Functions->UseFunctionsEmulator(Origin);
#endif
}

void UFirebaseFunctionsLibrary::UseFunctionsEmulator(const FString& Name)
{
	UseFunctionsEmulator(TCHAR_TO_UTF8(*Name));
}

FFirebaseHttpsCallableReference UFirebaseFunctionsLibrary::GetHttpsCallable(const char* Name)
{
#if WITH_FIREBASE_FUNCTIONS
	using namespace firebase;
	functions::Functions* const Functions = functions::Functions::GetInstance(FFirebaseFeaturesModule::GetCurrentFirebaseApp());

	check(Functions != nullptr);

	return Functions->GetHttpsCallable(Name);
#else
	return {};
#endif
}

FFirebaseHttpsCallableReference UFirebaseFunctionsLibrary::GetHttpsCallable(const FString& Name)
{
	return GetHttpsCallable(TCHAR_TO_UTF8(*Name));
}
FFirebaseHttpsCallableReference UFirebaseFunctionsLibrary::GetHttpsCallableWithRegion(const FString& Name, const FString& Region)
{
	return GetHttpsCallableWithRegion(TCHAR_TO_UTF8(*Name), TCHAR_TO_UTF8(*Region));
}

FFirebaseHttpsCallableReference UFirebaseFunctionsLibrary::GetHttpsCallableWithRegion(const char* Name, const char* Region)
{
#if WITH_FIREBASE_FUNCTIONS
	using namespace firebase;

	functions::Functions* const Functions = functions::Functions::GetInstance(FFirebaseFeaturesModule::GetCurrentFirebaseApp(), Region);

	check(Functions != nullptr);

	return Functions->GetHttpsCallable(Name);
#else
	return {};
#endif
}
