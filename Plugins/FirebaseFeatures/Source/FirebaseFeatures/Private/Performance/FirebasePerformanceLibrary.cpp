// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Performance/FirebasePerformanceLibrary.h"
#include "FirebaseFeatures.h"

#if WITH_FIREBASE_PERFORMANCE
#	if PLATFORM_IOS
#		import <FirebasePerformance/FirebasePerformance.h>
#	elif PLATFORM_ANDROID
#		include <jni.h>
#		include "Android/AndroidJavaEnv.h"
#		include "Android/AndroidJNI.h"
#		include "Android/AndroidApplication.h"
#	endif
#endif

#if WITH_FIREBASE_PERFORMANCE && PLATFORM_ANDROID
static jclass GjGameActivityClass = nullptr;

void InitializeFirebasePerformance()
{
	if (!GjGameActivityClass)
	{
		JNIEnv* const Env = AndroidJavaEnv::GetJavaEnv();

		check(Env);

		jclass jActivityClass = AndroidJavaEnv::FindJavaClass(FirebaseFeaturesGameActivityPath);
		check(jActivityClass);

		GjGameActivityClass = (jclass)Env->NewGlobalRef((jobject)jActivityClass);
		Env->DeleteLocalRef(jActivityClass);
	}
}

void TerminateFirebasePerformance()
{
	if (GjGameActivityClass)
	{
		JNIEnv* const Env = AndroidJavaEnv::GetJavaEnv();

		check(Env);

		Env->DeleteGlobalRef((jobject)GjGameActivityClass);

		GjGameActivityClass = nullptr;
	}
}

#define CALL_PERFORMANCE(FuncName, FuncSignature, ReturnType, DefaultReturn, ...)											\
	do																														\
	{																														\
		if (!GjGameActivityClass)																							\
		{																													\
			InitializeFirebasePerformance();																				\
		}																													\
																															\
		JNIEnv* const Env = AndroidJavaEnv::GetJavaEnv();																	\
																															\
		check(Env);																											\
																															\
		static jmethodID jMethod = Env->GetMethodID(GjGameActivityClass, FuncName, FuncSignature);							\
																															\
		if (jMethod == 0)																									\
		{																													\
			UE_LOG(LogFirebasePerformance, Error, TEXT("Failed to get method ") TEXT(FuncName) TEXT(" with signature ")		\
				TEXT(FuncSignature) TEXT(". Did you forget to enable Firebase Performance?"));								\
			return DefaultReturn;																							\
		}																													\
																															\
		return Env->Call ## ReturnType ## Method(AndroidJavaEnv::GetGameActivityThis(), jMethod , ## __VA_ARGS__);			\
	} while(0)
#endif


#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS 
FFirebaseTrace::FFirebaseTrace(FIRTrace* Native)
	: Trace([Native retain])
{
}

#elif PLATFORM_ANDROID
FFirebaseTrace::FFirebaseTrace(jobject Native)
{
	JNIEnv* const Env = AndroidJavaEnv::GetJavaEnv();

	check(Env);

	Trace = Env->NewGlobalRef(Native);

	Env->DeleteLocalRef(Native);
}
#else
FFirebaseTrace::FFirebaseTrace(FString InName)
	: Name(MoveTemp(InName))
{
}
#endif
#endif

FFirebaseTrace::FFirebaseTrace()
#if PLATFORM_IOS
	: Trace(nil)
#elif PLATFORM_ANDROID
	: Trace(nullptr)
#endif
{

}

FFirebaseTrace::~FFirebaseTrace()
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	[Trace release], Trace = nil;
#elif PLATFORM_ANDROID
	JNIEnv* const Env = AndroidJavaEnv::GetJavaEnv();

	check(Env);

	Env->DeleteGlobalRef(Trace);
	Trace = nullptr;
#endif
#endif
}

FFirebaseTrace::FFirebaseTrace(const FFirebaseTrace& Other)
{
	*this = Other;
}

FFirebaseTrace::FFirebaseTrace(FFirebaseTrace&& Other)
{
	*this = MoveTemp(Other);
}

FFirebaseTrace& FFirebaseTrace::operator=(FFirebaseTrace&& Other)
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	Trace = Other.Trace;
	Other.Trace = nil;
#elif PLATFORM_ANDROID
	Trace = Other.Trace;
	Other.Trace = nullptr;
#else
	Metrics = MoveTemp(Other.Metrics);
#endif
#endif

	return *this;
}

FFirebaseTrace& FFirebaseTrace::operator=(const FFirebaseTrace& Other)
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	Trace = [Other.Trace retain];
#elif PLATFORM_ANDROID
	JNIEnv* const Env = AndroidJavaEnv::GetJavaEnv();

	check(Env);

	Trace = Env->NewGlobalRef(Other.Trace);
#else
	Metrics = Other.Metrics;
#endif
#endif

	return *this;
}

FString FFirebaseTrace::GetName() const
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	return Trace == nil ? TEXT("INVALID_TRACE") : FString([Trace name]);
#elif PLATFORM_ANDROID
	return TEXT("");
#else
	return Name;
#endif
#else
	return FString{};
#endif
}

void FFirebaseTrace::Start()
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	if (Trace != nil)
	{
		[Trace start];
	}
#elif PLATFORM_ANDROID
	CALL_PERFORMANCE("FIR_PR_StartTrace", "(Lcom/google/firebase/perf/metrics/Trace;)V", Void, void(), Trace);
#else
	UE_LOG(LogFirebasePerformance, Log, TEXT("Started trace %s execution."), *Name);
#endif
#endif
}

void FFirebaseTrace::Stop()
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	if (Trace != nil)
	{
		[Trace stop];
	}
#elif PLATFORM_ANDROID
	CALL_PERFORMANCE("FIR_PR_StopTrace", "(Lcom/google/firebase/perf/metrics/Trace;)V", Void, void(), Trace);
#else
	UE_LOG(LogFirebasePerformance, Log, TEXT("Stopped trace %s execution."), *Name);
#endif
#endif
}

void FFirebaseTrace::IncrementMetric(const FString& MetricName, const int64 ByValue)
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	if (Trace != nil)
	{
		[Trace incrementMetric: MetricName.GetNSString()
                         byInt: ByValue];
	}
#elif PLATFORM_ANDROID
	CALL_PERFORMANCE("FIR_PR_IncrementMetric", "(Lcom/google/firebase/perf/metrics/Trace;Ljava/lang/String;J)V", Void,
		void(), Trace, *FJavaHelper::ToJavaString(Env, MetricName), (jlong)ByValue);
#else
	Metrics.FindOrAdd(MetricName, 0LL) += ByValue;
#endif
#endif
}

int64 FFirebaseTrace::GetMetricValue(const FString& MetricName) const
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	if (Trace != nil)
	{
		return [Trace valueForIntMetric: MetricName.GetNSString()] ;
	}
	return 0LL;
#elif PLATFORM_ANDROID
	CALL_PERFORMANCE("FIR_PR_GetMetricValue", "(Lcom/google/firebase/perf/metrics/Trace;Ljava/lang/String;)J", Long,
		0LL, Trace, *FJavaHelper::ToJavaString(Env, MetricName));
#else
	return Metrics.Contains(MetricName) ? Metrics[MetricName] : 0LL;
#endif
#else
	return 0LL;
#endif
}

void FFirebaseTrace::SetMetricValue(const FString& MetricName, const int64 Value)
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	if (Trace != nil)
	{
		[Trace setIntValue: Value
                 forMetric: MetricName.GetNSString()];
	}
#elif PLATFORM_ANDROID
	CALL_PERFORMANCE("FIR_PR_SetMetricValue", "(Lcom/google/firebase/perf/metrics/Trace;Ljava/lang/String;J)V", Void,
		void(), Trace, *FJavaHelper::ToJavaString(Env, MetricName), (jlong)Value);
#else
	Metrics.Add(MetricName, Value);
#endif
#endif
}

FFirebaseTrace UFirebasePerformanceLibrary::CreateTrace(const FString& TraceName)
{
#if WITH_FIREBASE_PERFORMANCE
	UE_LOG(LogFirebasePerformance, Log, TEXT("Creating new trace named %s."), *TraceName);

#if PLATFORM_IOS
	if (TraceName.IsEmpty())
	{
		UE_LOG(LogFirebasePerformance, Warning, TEXT("Trace name was empty. Using default trace name."));
		return [FIRPerformance.sharedInstance traceWithName: @"INVALID_TRACE_NAME"];
	}
	return [FIRPerformance.sharedInstance traceWithName: TraceName.GetNSString()];
#elif PLATFORM_ANDROID
	CALL_PERFORMANCE("FIR_PR_NewTrace", "(Ljava/lang/String;)Lcom/google/firebase/perf/metrics/Trace;", Object,
		FFirebaseTrace(), *FJavaHelper::ToJavaString(Env, TraceName));
#else
	return TraceName;
#endif
#else
	return FFirebaseTrace();
#endif
}

FFirebaseTrace UFirebasePerformanceLibrary::CreateAndStartTrace(const FString& TraceName)
{
#if WITH_FIREBASE_PERFORMANCE
	UE_LOG(LogFirebasePerformance, Log, TEXT("Creating and starting new trace named %s."), *TraceName);

#if PLATFORM_IOS
	if (TraceName.IsEmpty())
	{
		UE_LOG(LogFirebasePerformance, Warning, TEXT("Trace name was empty. Using default trace name."));
		return [FIRPerformance startTraceWithName: @"INVALID_TRACE_NAME"];
	}
	return [FIRPerformance startTraceWithName: TraceName.GetNSString()];
#elif PLATFORM_ANDROID
	CALL_PERFORMANCE("FIR_PR_StartTrace", "(Ljava/lang/String;)Lcom/google/firebase/perf/metrics/Trace;", Object, 
			FFirebaseTrace(), *FJavaHelper::ToJavaString(Env, TraceName));
#else
	return TraceName;
#endif
#else
	return FFirebaseTrace();
#endif
}

void UFirebasePerformanceLibrary::SetInstrumentationEnabled(const bool bEnabled)
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	[FIRPerformance sharedInstance].instrumentationEnabled = bEnabled;
#elif PLATFORM_ANDROID

#else

#endif
#endif
}

void UFirebasePerformanceLibrary::SetDataCollectionEnabled(const bool bEnabled)
{
#if WITH_FIREBASE_PERFORMANCE
#if PLATFORM_IOS
	[FIRPerformance sharedInstance].dataCollectionEnabled = bEnabled;
#elif PLATFORM_ANDROID
	CALL_PERFORMANCE("FIR_PR_SetPerformanceCollectionEnabled", "(Z)V", Void,
		void(), (jboolean)bEnabled);
#else

#endif
#endif
}

FString UFirebasePerformanceLibrary::Blueprint_GetName(UPARAM(ref) const FFirebaseTrace& Trace)
{
	return Trace.GetName();
}

void UFirebasePerformanceLibrary::Blueprint_Start(UPARAM(ref) FFirebaseTrace& Trace)
{
	Trace.Start();
}

void UFirebasePerformanceLibrary::Blueprint_Stop(UPARAM(ref) FFirebaseTrace& Trace)
{
	Trace.Stop();
}

void UFirebasePerformanceLibrary::Blueprint_IncrementMetric(UPARAM(ref) FFirebaseTrace& Trace, const FString& MetricName, const int64 ByValue)
{
	Trace.IncrementMetric(MetricName, ByValue);
}

int64 UFirebasePerformanceLibrary::Blueprint_GetMetricValue(UPARAM(ref) const FFirebaseTrace& Trace, const FString& MetricName)
{
	return Trace.GetMetricValue(MetricName);
}

void UFirebasePerformanceLibrary::Blueprint_SetMetricValue(UPARAM(ref) FFirebaseTrace& Trace, const FString& MetricName, const int64 Value)
{
	Trace.SetMetricValue(MetricName, Value);
}



