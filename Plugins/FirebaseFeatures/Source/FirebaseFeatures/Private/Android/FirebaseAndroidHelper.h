// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseFeatures.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJavaEnv.h"

enum EFirebaseAndroidLogCategory : int32
{
	FBLog_Info = 0,
	FBLog_Warn = 1,
	FBLog_Error = 2,
	FBLog_Debug = 3
};

namespace FirebaseUtils
{
	jclass GetGameActivityClass();

	void NativeLog(const int32 Verbosity, const FString& Message);
}

#define CALL_ACTIVITY_METHOD(Name, Signature, Type, Default, ...)															\
	[&]()																													\
	{																														\
		JNIEnv* const Env = AndroidJavaEnv::GetJavaEnv();																	\
																															\
		if (!Env)																											\
		{																													\
			UE_LOG(LogFirebaseSdk, Error, TEXT("Failed to get Env."));														\
			return Default;																									\
		}																													\
																															\
		static jmethodID jMethod = Env->GetMethodID(FirebaseUtils::GetGameActivityClass(), Name, Signature);				\
																															\
		if (Env->ExceptionCheck())																							\
		{																													\
			UE_LOG(LogFirebaseSdk, Error, TEXT("Failed to get method \"") TEXT(Name) TEXT(Signature) TEXT("\"."));			\
			Env->ExceptionDescribe();																						\
			Env->ExceptionClear();																							\
			return Default;																									\
		}																													\
																															\
		auto Result = Env->Call ## Type ## Method(AndroidJavaEnv::GetGameActivityThis(), jMethod, ## __VA_ARGS__);			\
		if (Env->ExceptionCheck())																							\
		{																													\
			UE_LOG(LogFirebaseSdk, Error, TEXT("Exception thrown while calling method."));									\
			Env->ExceptionDescribe();																						\
			Env->ExceptionClear();																							\
			return Default;																									\
		}																													\
		return Result;																										\
	}()		

#define CALL_ACTIVITY_STRING_METHOD(Name, Signature, ...) \
	(FJavaHelper::FStringFromLocalRef(AndroidJavaEnv::GetJavaEnv(), (jstring)CALL_ACTIVITY_METHOD(Name, Signature, Object, (jobject)nullptr, ## __VA_ARGS__)))

#define CALL_ACTIVITY_BOOL_METHOD(Name, Signature, ...) \
	(bool)CALL_ACTIVITY_METHOD(Name, Signature, Boolean, (jboolean)false, ## __VA_ARGS__)

#define CALL_ACTIVITY_VOID_METHOD(Name, Signature, ...)																		\
	[&]()																													\
	{																														\
		JNIEnv* const Env = AndroidJavaEnv::GetJavaEnv();																	\
																															\
		if (!Env)																											\
		{																													\
			UE_LOG(LogFirebaseSdk, Error, TEXT("Failed to get Env."));														\
			return;																											\
		}																													\
																															\
		static jmethodID jMethod = Env->GetMethodID(FirebaseUtils::GetGameActivityClass(), Name, Signature);				\
																															\
		if (Env->ExceptionCheck())																							\
		{																													\
			UE_LOG(LogFirebaseSdk, Error, TEXT("Failed to get method \"") TEXT(Name) TEXT(Signature) TEXT("\"."));			\
			Env->ExceptionDescribe();																						\
			Env->ExceptionClear();																							\
			return;																											\
		}																													\
																															\
		Env->CallVoidMethod(AndroidJavaEnv::GetGameActivityThis(), jMethod, ## __VA_ARGS__);								\
																															\
		if (Env->ExceptionCheck())																							\
		{																													\
			UE_LOG(LogFirebaseSdk, Error, TEXT("Exception thrown while calling method."));									\
			Env->ExceptionDescribe();																						\
			Env->ExceptionClear();																							\
		}																													\
	}()		

#define CALL_ACTIVITY_OBJECT_METHOD(Name, Signature, ...) \
	CALL_ACTIVITY_METHOD(Name, Signature, Object, (jobject)NULL, ## __VA_ARGS__)

