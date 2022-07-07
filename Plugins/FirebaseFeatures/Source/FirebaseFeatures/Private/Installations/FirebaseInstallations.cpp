// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Installations/FirebaseInstallations.h"
#include "Async/Async.h"

#if PLATFORM_IOS
#	import <FirebaseInstallations/FirebaseInstallations.h>
#elif PLATFORM_ANDROID
#	include "Android/FirebaseAndroidHelper.h"
#endif

DECLARE_LOG_CATEGORY_CLASS(LogFirebaseInstallations, Log, All);

#if PLATFORM_ANDROID
extern "C"
JNIEXPORT void JNICALL
Java_com_epicgames_ue4_GameActivity_nativeFirInstDeleteCallback(JNIEnv * env, jobject thiz, jlong Data, jint Code, jstring Message)
{
	if (!Data)
	{
		return;
	}

	FFirebaseInstallationDeleteCallback* Callback = (FFirebaseInstallationDeleteCallback*)Data;

	AsyncTask(ENamedThreads::GameThread, [Callback, Code = (int32)Code, Message = FJavaHelper::FStringFromParam(env, Message)]() mutable -> void
	{
		Callback->ExecuteIfBound(Code, MoveTemp(Message));
		delete Callback;
	});
}

extern "C"
JNIEXPORT void JNICALL
Java_com_epicgames_ue4_GameActivity_nativeFirInstGetTokenCallback(JNIEnv * env, jobject thiz, jlong Data, jstring token, jlong expiration, jint Code, jstring Message)
{
	if (!Data)
	{
		return;
	}

	FFirebaseInstallationAuthTokenCallback* Callback = (FFirebaseInstallationAuthTokenCallback*)Data;

	FFirebaseInstallationsAuthToken Token;

	Token.AuthToken = FJavaHelper::FStringFromParam(env, token);
	Token.Expiration = FDateTime::FromUnixTimestamp((int64)expiration);

	AsyncTask(ENamedThreads::GameThread, [Token = MoveTemp(Token), Callback, Code = (int32)Code, Message = FJavaHelper::FStringFromParam(env, Message)]() mutable -> void
	{
		Callback->ExecuteIfBound(MoveTemp(Token), Code, MoveTemp(Message));
		delete Callback;
	});
}

extern "C"
JNIEXPORT void JNICALL
Java_com_epicgames_ue4_GameActivity_nativeFirInstGetIDCallback(JNIEnv * env, jobject thiz, jlong Data, jstring instId, jint Code, jstring Message)
{
	if (!Data)
	{
		return;
	}

	FFirebaseInstallationIDCallback* Callback = (FFirebaseInstallationIDCallback*)Data;

	AsyncTask(ENamedThreads::GameThread, [InstId = FJavaHelper::FStringFromParam(env, instId), Callback, Code = (int32)Code, Message = FJavaHelper::FStringFromParam(env, Message)]() mutable -> void
	{
		Callback->ExecuteIfBound(MoveTemp(InstId), Code, MoveTemp(Message));
		delete Callback;
	});
}
#endif

#if PLATFORM_IOS
static void ExecuteAuthTokenCallback(FIRInstallationsAuthTokenResult* tokenResult, NSError* error, FFirebaseInstallationAuthTokenCallback&& Callback)
{
	FFirebaseInstallationsAuthToken Token;
	int32 ErrorCode = 0;
	FString ErrorMessage;

	if (tokenResult != nil)
	{
		Token.AuthToken = tokenResult.authToken;
		Token.Expiration = FDateTime::FromUnixTimestamp([tokenResult.expirationDate timeIntervalSince1970]);
	}

	if (error != nil)
	{
		ErrorCode = error.code;
		ErrorMessage = error.description;

		UE_LOG(LogFirebaseInstallations, Error, TEXT("Failed to get auth token: %d - %s"), ErrorCode, *ErrorMessage);
	}

	AsyncTask(ENamedThreads::GameThread, [Callback = MoveTemp(Callback), Token = MoveTemp(Token), ErrorCode, ErrorMessage = MoveTemp(ErrorMessage)]() mutable -> void
	{
		Callback.ExecuteIfBound(MoveTemp(Token), ErrorCode, MoveTemp(ErrorMessage));
	});
}
#endif

void UFirebaseInstallations::GetInstallationID(FFirebaseInstallationIDCallback Callback)
{
#if PLATFORM_IOS
	[[FIRInstallations installations] installationIDWithCompletion: [Callback = MoveTemp(Callback)](
		NSString* __nullable identifier,
		NSError* __nullable error) mutable -> void
	{
		FString Identifier = identifier;
		FString ErrorMessage;
		int32 ErrorCode = 0;
		
		if (error != nil)
		{
			ErrorCode = error.code;
			ErrorMessage = error.description;

			UE_LOG(LogFirebaseInstallations, Error, TEXT("Failed to get installation ID: %d - %s"), ErrorCode, *ErrorMessage);
		}

		AsyncTask(ENamedThreads::GameThread, [Callback = MoveTemp(Callback), Identifier = MoveTemp(Identifier), 
			ErrorCode, ErrorMessage = MoveTemp(ErrorMessage)]() mutable -> void
		{
			Callback.ExecuteIfBound(MoveTemp(Identifier), ErrorCode, MoveTemp(ErrorMessage));
		});
	}];
#elif PLATFORM_ANDROID
	CALL_ACTIVITY_VOID_METHOD("FIR_INST_getID", "(J)V", (jlong)new FFirebaseInstallationIDCallback(MoveTemp(Callback)));
#else
	Callback.ExecuteIfBound({}, 1, TEXT("Platform doesn't support installations."));
#endif
}

void UFirebaseInstallations::GetAuthToken(FFirebaseInstallationAuthTokenCallback Callback)
{
	
#if PLATFORM_IOS
	[[FIRInstallations installations] authTokenWithCompletion: [Callback = MoveTemp(Callback)] (
		FIRInstallationsAuthTokenResult* __nullable tokenResult, NSError* __nullable error) mutable -> void
	{
		ExecuteAuthTokenCallback(tokenResult, error, MoveTemp(Callback));
	}];
#elif PLATFORM_ANDROID
	GetAuthToken(false, MoveTemp(Callback));
#else
	Callback.ExecuteIfBound({}, 1, TEXT("Platform doesn't support installations."));
#endif
}

void UFirebaseInstallations::GetAuthToken(const bool bForceRefresh, FFirebaseInstallationAuthTokenCallback Callback)
{
#if PLATFORM_IOS
	[[FIRInstallations installations] authTokenForcingRefresh: bForceRefresh
												   completion: [Callback = MoveTemp(Callback)] (
		FIRInstallationsAuthTokenResult* __nullable tokenResult, NSError* __nullable error) mutable -> void
	{
		ExecuteAuthTokenCallback(tokenResult, error, MoveTemp(Callback));
	}];
#elif PLATFORM_ANDROID
	CALL_ACTIVITY_VOID_METHOD("FIR_INST_getToken", "(ZJ)V", (jboolean)bForceRefresh, (jlong)new FFirebaseInstallationAuthTokenCallback(MoveTemp(Callback)));
#else
	Callback.ExecuteIfBound({}, 1, TEXT("Platform doesn't support installations."));
#endif
}

void UFirebaseInstallations::Delete(FFirebaseInstallationDeleteCallback Callback)
{
#if PLATFORM_IOS
	[[FIRInstallations installations] deleteWithCompletion: [Callback = MoveTemp(Callback)](NSError* __nullable error) mutable -> void
	{
		int32 ErrorCode = 0;
		FString ErrorMessage = error.description;

		if (error != nil)
		{
			ErrorCode = error.code;
			ErrorMessage = error.description;

			UE_LOG(LogFirebaseInstallations, Error, TEXT("Failed to delete installation: %d - %s"), ErrorCode, *ErrorMessage);
		}

		AsyncTask(ENamedThreads::GameThread, [Callback = MoveTemp(Callback), ErrorCode, ErrorMessage = MoveTemp(ErrorMessage)]() mutable -> void
		{
			Callback.ExecuteIfBound(ErrorCode, MoveTemp(ErrorMessage));
		});
	}];
#elif PLATFORM_ANDROID
	CALL_ACTIVITY_VOID_METHOD("FIR_INST_delete", "(J)V", (jlong)new FFirebaseInstallationDeleteCallback(MoveTemp(Callback)));
#else
	Callback.ExecuteIfBound(1, TEXT("Platform doesn't support installations."));
#endif
}
