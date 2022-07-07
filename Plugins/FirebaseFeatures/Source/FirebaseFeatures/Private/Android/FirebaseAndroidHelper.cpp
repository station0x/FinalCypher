// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "FirebaseAndroidHelper.h"
#include "FirebaseFeatures.h"

jclass FirebaseUtils::GetGameActivityClass()
{
	static jobject GameActivity = nullptr;

	if (GameActivity == nullptr)
	{
		JNIEnv* const Env = AndroidJavaEnv::GetJavaEnv();

		check(Env);

		jclass jActivityClass = AndroidJavaEnv::FindJavaClass(FirebaseFeaturesGameActivityPath);
		check(jActivityClass);

		GameActivity = (jclass)Env->NewGlobalRef((jobject)jActivityClass);
		Env->DeleteLocalRef(jActivityClass);
	}

	return (jclass)GameActivity;
}


void FirebaseUtils::NativeLog(const int32 Verbosity, const FString& Message)
{
	CALL_ACTIVITY_VOID_METHOD("AndroidThunk_FirebaseFeatures_nativeLog", "(ILjava/Lang/String;)V", 
		(jint)Verbosity, *FJavaHelper::ToJavaString(AndroidJavaEnv::GetJavaEnv(), Message));
}
