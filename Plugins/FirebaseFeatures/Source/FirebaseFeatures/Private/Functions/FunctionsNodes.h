// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Functions/CallableReference.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FunctionsNodes.generated.h"

UCLASS()
class UFirebaseFunctionsBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/// @brief Returns true if this HttpsCallableReference is valid, false if it
	/// is not valid. An invalid HttpsCallableReference indicates that the
	/// reference is uninitialized (created with the default constructor) or that
	/// there was an error retrieving the reference.
	///
	/// @returns true if this HttpsCallableReference is valid, false if this
	/// HttpsCallableReference is invalid.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Functions|HttpsCallable")
	static UPARAM(DisplayName = "Is Valid") bool IsValid(UPARAM(ref) const FFirebaseHttpsCallableReference& HttpsCallable);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultFunctionsCallResult, const EFirebaseFunctionsError, Error, const FFirebaseVariant&, Result);

UCLASS()
class UCallHttpsFunctionsProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFunctionsCallResult OnCalled;
	UPROPERTY(BlueprintAssignable)
	FDynMultFunctionsCallResult OnError;

public:
	virtual void Activate();

	/// @brief Calls the function.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore", meta = (BlueprintInternalUseOnly = "true"))
	static UCallHttpsFunctionsProxy* Call(UPARAM(ref) FFirebaseHttpsCallableReference& HttpsCallable);

private:
	void OnActionOver(const EFirebaseFunctionsError Error, const FFirebaseVariant& Result);

	FFirebaseHttpsCallableReference Callable;
};


UCLASS()
class UCallHttpsFunctionsWithDataProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFunctionsCallResult OnCalled;
	UPROPERTY(BlueprintAssignable)
	FDynMultFunctionsCallResult OnError;

public:
	virtual void Activate();

	/// @brief Calls the function.
	///
	/// @param Data The params to pass to the function.
	/// @returns The result of the call;
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore", meta = (BlueprintInternalUseOnly = "true"))
	static UCallHttpsFunctionsWithDataProxy* CallWithData(UPARAM(ref) FFirebaseHttpsCallableReference& HttpsCallable, const FFirebaseVariant& Data);

private:
	void OnActionOver(const EFirebaseFunctionsError Error, const FFirebaseVariant& Result);

	FFirebaseHttpsCallableReference Callable;
	FFirebaseVariant CallData;
};


