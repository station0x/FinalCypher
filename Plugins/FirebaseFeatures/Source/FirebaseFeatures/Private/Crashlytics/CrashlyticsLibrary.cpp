// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Crashlytics/CrashlyticsLibrary.h"
#include "FirebaseFeatures.h"

#if PLATFORM_IOS && WITH_FIREBASE_CRASHLYTICS
#	import <FirebaseCrashlytics/FirebaseCrashlytics.h>
#elif PLATFORM_ANDROID
#	include "Android/AndroidJNI.h"
#	include "Android/AndroidApplication.h"
#elif WITH_EDITOR
#	include "Editor/UnrealEdEngine.h"
#	include "UnrealEdGlobals.h"
#endif

#if PLATFORM_ANDROID && WITH_FIREBASE_CRASHLYTICS
static jclass GetGameActivityClass(JNIEnv* const Env)
{
	jclass jActivityClass = AndroidJavaEnv::FindJavaClass(FirebaseFeaturesGameActivityPath);

	if (Env->ExceptionOccurred())
	{
		Env->ExceptionClear();
		UE_LOG(LogFirebaseCrashlytics, Error, TEXT("Failed to get UE4 Game Activity class."));

		if (jActivityClass)
		{
			Env->DeleteLocalRef(jActivityClass);
		}

		return nullptr;
	}

	return jActivityClass;
}

static jmethodID GetGameActivityMethodID(JNIEnv* const Env, const char* MethodName, const char* MethodSignature)
{
	jclass jGameActivity = GetGameActivityClass(Env);

	if (!jGameActivity)
	{
		return (jmethodID)0;
	}

	jmethodID jMethod = Env->GetMethodID(jGameActivity, MethodName, MethodSignature);

	if (Env->ExceptionOccurred())
	{
		Env->ExceptionClear();

		UE_LOG(LogFirebaseCrashlytics, Error, TEXT("Failed to get method %s with signature %s."),
			UTF8_TO_TCHAR(MethodName), UTF8_TO_TCHAR(MethodSignature));
		
		Env->DeleteLocalRef(jGameActivity);

		return (jmethodID)0;
	}
	
	Env->DeleteLocalRef(jGameActivity);

	return jMethod;
}

#define CALL_FIR_CRASHLYTICS_JAVA_METHOD(MethodName, MethodSignature, ReturnType, DefaultReturn, ...)				\
	do																												\
	{																												\
		JNIEnv* const Env = FAndroidApplication::GetJavaEnv();														\
																													\
		check(Env);																									\
																													\
		static jmethodID jMethod = GetGameActivityMethodID(Env, MethodName, MethodSignature);						\
																													\
		if (jMethod == 0)																							\
		{																											\
			UE_LOG(LogFirebaseCrashlytics, Error, TEXT("Failed to get method %s with signature %s. "),				\
				TEXT("Did you forget to enable Crashlytics?"),														\
					UTF8_TO_TCHAR(MethodName), UTF8_TO_TCHAR(MethodSignature));										\
																													\
			return DefaultReturn;																					\
		}																											\
																													\
		return Env->Call ## ReturnType ## Method(AndroidJavaEnv::GetGameActivityThis(), jMethod , ## __VA_ARGS__);	\
	} while (0)

#endif

void UCrashlyticsLibrary::CrashApplication()
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Crash requested."));

#if PLATFORM_IOS
	UE_LOG(LogFirebaseCrashlytics, Error, TEXT("Crashed the app with Crashlytics."));
	@[][1];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_CrashApplication", "()V", Void, void());
#elif WITH_EDITOR
	// In Editor, we try to just terminate PIE if possible.
	if (GUnrealEd && GUnrealEd->GetPlayInEditorSessionInfo())
	{
		UE_LOG(LogFirebaseCrashlytics, Error, TEXT("Crashed the application with Crashlytics."));
		GUnrealEd->RequestEndPlayMap();
	}
	else
	{
		UE_LOG(LogFirebaseCrashlytics, Fatal, TEXT("Crashed the application with Crashlytics."));
	}
#else
	UE_LOG(LogFirebaseCrashlytics, Fatal, TEXT("Crashed the application with Crashlytics."));
#endif
#endif
}

void UCrashlyticsLibrary::Log(const FString& Message)
{
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Log: %s."), *Message);

#if WITH_FIREBASE_CRASHLYTICS
#if PLATFORM_IOS
	[[FIRCrashlytics crashlytics] log: Message.GetNSString()];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_Log", "(Ljava/lang/String;)V", Void, void(), *FJavaHelper::ToJavaString(Env, Message));
#endif
#endif
}

bool UCrashlyticsLibrary::DidCrashDuringPreviousExecution()
{
#if WITH_FIREBASE_CRASHLYTICS
#if PLATFORM_IOS
	return [[FIRCrashlytics crashlytics] didCrashDuringPreviousExecution];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_DidCrashOnPreviousExecution", "()Z", Boolean, false);
#else
	return false;
#endif
#else
	return false;
#endif
}

void UCrashlyticsLibrary::SetCrashlyticsCollectionEnabled(bool bEnabled)
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Crashlytics collection enabled: %d."), (int32)bEnabled);

#if PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled: bEnabled];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_SetCrashlyticsCollectionEnabled", "(Z)V", Void, void(), (jboolean)bEnabled);
#endif
#endif
}

bool UCrashlyticsLibrary::IsCrashlyticsCollectionEnabled()
{
#if WITH_FIREBASE_CRASHLYTICS
#if PLATFORM_IOS
	return [[FIRCrashlytics crashlytics] isCrashlyticsCollectionEnabled];
#elif PLATFORM_ANDROID
	// Method not available on Android
	UE_LOG(LogFirebaseCrashlytics, Warning, TEXT("Method IsCrashlyticsCollectionEnabled called, but it is not available on Android."));
	return false;
#else
	return false;
#endif
#else
	return false;
#endif
}

void UCrashlyticsLibrary::DeleteUnsentReports()
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Deleting unset reports."));

#if PLATFORM_IOS
	[[FIRCrashlytics crashlytics] deleteUnsentReports];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_DeleteUnsentReports", "()V", Void, void());
#endif
#endif
}

void UCrashlyticsLibrary::SendUnsentReports()
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Sending unset reports."));

#if PLATFORM_IOS
	[[FIRCrashlytics crashlytics] sendUnsentReports];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_SendUnsetReports", "()V", Void, void());
#endif
#endif
}

void UCrashlyticsLibrary::RecordError(const FString& Domain, const FString& Message, const int64 Code)
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Record Error: Domain: %s, Code: %lld, Message: %s"), *Domain, Code, *Message);

#if PLATFORM_IOS
	NSError* const error = [[NSError alloc]
		initWithDomain:Domain.GetNSString()
		code : Code
		userInfo : @{ NSLocalizedDescriptionKey: Message.GetNSString() }];
	[[FIRCrashlytics crashlytics]recordError:error];

	[error release] ;
#elif PLATFORM_ANDROID
	FString ThrowableMessage;

	if (Domain.IsEmpty())
	{
		ThrowableMessage = Message;
	}
	else
	{
		ThrowableMessage = FString::Printf(TEXT("%s;%lld - %s"), *Domain, Code, *Message);
	}

	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_RecordException", "(Ljava/lang/String;)V", Void, void(), 
		*FJavaHelper::ToJavaString(Env, ThrowableMessage));
#endif
#endif
}

void UCrashlyticsLibrary::SetCustomVectorValue(const FString& Key, const FVector Value)
{
	SetCustomStringValue(Key, FString::Printf(TEXT("{ %f, %f, %f }"), Value.X, Value.Y, Value.Z));
}

void UCrashlyticsLibrary::SetCustomFloatValue(const FString& Key, const float Value)
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Custom value set: { Key=%s, Value=%f }"), *Key, Value);

#if PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCustomValue: FString::Printf(TEXT("%f"), Value).GetNSString() forKey: Key.GetNSString()];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_SetCustomKey", "(Ljava/lang/String;F)V", Void, void(), *FJavaHelper::ToJavaString(Env, Key), (jfloat)Value);
#endif
#endif
}

void UCrashlyticsLibrary::SetCustomInt64Value(const FString& Key, const int64 Value)
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Custom value set: { Key=%s, Value=%lld }"), *Key, Value);

#if PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCustomValue: FString::Printf(TEXT("%lld"), Value).GetNSString() forKey: Key.GetNSString()];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_SetCustomKey", "(Ljava/lang/String;J)V", Void, void(), *FJavaHelper::ToJavaString(Env, Key), (jlong)Value);
#endif
#endif
}

void UCrashlyticsLibrary::SetCustomInt32Value(const FString& Key, const int32 Value)
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Custom value set: { Key=%s, Value=%d }"), *Key, Value);

#if PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCustomValue: FString::Printf(TEXT("%d"), Value).GetNSString() forKey: Key.GetNSString()];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_SetCustomKey", "(Ljava/lang/String;I)V", Void, void(), *FJavaHelper::ToJavaString(Env, Key), (jint)Value);
#endif
#endif
}

void UCrashlyticsLibrary::SetCustomBoolValue(const FString& Key, const bool Value)
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Custom value set: { Key=%s, Value=%d }"), *Key, (int32)Value);

#if PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCustomValue: FString(Value ? TEXT("True") : TEXT("False")).GetNSString() forKey: Key.GetNSString()];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_SetCustomKey", "(Ljava/lang/String;Z)V", Void, void(), *FJavaHelper::ToJavaString(Env, Key), (jboolean)Value);
#endif
#endif
}

void UCrashlyticsLibrary::SetCustomStringValue(const FString& Key, const FString& Value)
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("Custom value set: { Key=%s, Value=%s }"), *Key, *Value);

#if PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setCustomValue: Value.GetNSString() forKey: Key.GetNSString()];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_SetCustomKey", "(Ljava/lang/String;Ljava/lang/String;)V", Void, void(), *FJavaHelper::ToJavaString(Env, Key), *FJavaHelper::ToJavaString(Env, Value));
#endif
#endif
}

void UCrashlyticsLibrary::SetUserID(const FString& UserID)
{
#if WITH_FIREBASE_CRASHLYTICS
	UE_LOG(LogFirebaseCrashlytics, Log, TEXT("User ID updated to %s."), *UserID);

#if PLATFORM_IOS
	[[FIRCrashlytics crashlytics] setUserID: UserID.GetNSString()];
#elif PLATFORM_ANDROID
	CALL_FIR_CRASHLYTICS_JAVA_METHOD("FIR_CL_SetUserId", "(Ljava/lang/String;)V", Void, void(), *FJavaHelper::ToJavaString(Env, UserID));
#endif
#endif
}

