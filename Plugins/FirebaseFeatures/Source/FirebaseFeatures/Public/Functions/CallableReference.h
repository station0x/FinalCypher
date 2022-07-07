// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseFeatures.h"
#include "FirebaseSdk/FirebaseVariant.h"
#include "FirebaseSdk/FirebaseErrors.h"
#include "CallableReference.generated.h"

namespace firebase { namespace functions { class HttpsCallableReference; } };

DECLARE_DELEGATE_TwoParams(FFunctionsCallCallback, const EFirebaseFunctionsError, const FFirebaseVariant&);

/// Represents a reference to a Cloud Functions object.
/// Developers can call HTTPS Callable Functions.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseHttpsCallableReference
{
	GENERATED_BODY()
public:
#if WITH_FIREBASE_FUNCTIONS
	FFirebaseHttpsCallableReference(const firebase::functions::HttpsCallableReference& InReference);
#endif

	FFirebaseHttpsCallableReference();
	FFirebaseHttpsCallableReference(const FFirebaseHttpsCallableReference&  Other);
	FFirebaseHttpsCallableReference(FFirebaseHttpsCallableReference&& Other);
	~FFirebaseHttpsCallableReference();

	FFirebaseHttpsCallableReference& operator=(const FFirebaseHttpsCallableReference& Other);
	FFirebaseHttpsCallableReference& operator=(FFirebaseHttpsCallableReference&& Other);

	/// @brief Calls the function.
	void Call(const FFunctionsCallCallback& Callback = FFunctionsCallCallback());

	/// @brief Calls the function.
	///
	/// @param[in] Data The params to pass to the function.
	/// @returns The result of the call;
	void Call(const FFirebaseVariant& Data, const FFunctionsCallCallback& Callback = FFunctionsCallCallback());

	/// @brief Returns true if this HttpsCallableReference is valid, false if it
	/// is not valid. An invalid HttpsCallableReference indicates that the
	/// reference is uninitialized (created with the default constructor) or that
	/// there was an error retrieving the reference.
	///
	/// @returns true if this HttpsCallableReference is valid, false if this
	/// HttpsCallableReference is invalid.
	bool IsValid() const;

	/// @brief Returns true if this HttpsCallableReference is valid, false if it
	/// is not valid. An invalid HttpsCallableReference indicates that the
	/// reference is uninitialized (created with the default constructor) or that
	/// there was an error retrieving the reference.
	///
	/// @returns true if this HttpsCallableReference is valid, false if this
	/// HttpsCallableReference is invalid.
	operator bool() const;

	/// @brief Calls the function.
	void operator()(const FFunctionsCallCallback& Callback = FFunctionsCallCallback());

	/// @brief Calls the function.
	///
	/// @param[in] Data The params to pass to the function.
	void operator()(const FFirebaseVariant& Data, const FFunctionsCallCallback& Callback = FFunctionsCallCallback());

private:
#if WITH_FIREBASE_FUNCTIONS
	TUniquePtr<firebase::functions::HttpsCallableReference> Reference;
#endif
};
