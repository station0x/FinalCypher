// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FirebasePerformanceLibrary.generated.h"

#if PLATFORM_ANDROID
#	include <jni.h>
#endif

#if PLATFORM_IOS
@class FIRTrace;
#endif

/**
 * FFirebaseTrace objects contain information about a "Trace", which is a sequence of steps. Traces can be
 * used to measure the time taken for a sequence of steps.
 * Traces also include "Counters". Counters are used to track information which is cumulative in
 * nature (e.g., Bytes downloaded). Counters are scoped to an FFirebaseTrace object.
 */
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseTrace
{
	GENERATED_BODY()
public:
	/**
	 * Gets the name of the trace.
	 * @return The name of the trace.
	 */
	FString GetName() const;

	/**
	 * Starts the trace.
	 */
	void Start();

	/**
	 * Stops the trace if the trace is active.
	 */
	void Stop();

	/**
	 * Atomically increments the metric for the provided metric name with the provided value. If it is a
	 * new metric name, the metric value will be initialized to the value. Does nothing if the trace
	 * has not been started or has already been stopped.
	 *
	 * @param metricName The name of the metric to increment.
	 * @param incrementValue The value to increment the metric by.
	 */
	void IncrementMetric(const FString& MetricName, const int64 ByValue);

	/**
	 * Gets the value of the metric for the provided metric name. If the metric doesn't exist, a 0 is
	 * returned.
	 *
	 * @param Name The name of metric whose value to get.
	 * @return The value of the given metric or 0 if it hasn't yet been set.
	 */
	int64 GetMetricValue(const FString& MetricName) const;

	/**
	 * Sets the value of the metric for the provided metric name to the provided value. Does nothing if
	 * the trace has not been started or has already been stopped.
	 *
	 * @param Name The name of the metric to set.
	 * @param Value The value to set the metric to.
	 */
	void SetMetricValue(const FString& MetricName, const int64 Value);

public:
	FFirebaseTrace();
	FFirebaseTrace(const FFirebaseTrace& Other);
	FFirebaseTrace(FFirebaseTrace&& Other);

	FFirebaseTrace& operator=(const FFirebaseTrace& Other);
	FFirebaseTrace& operator=(FFirebaseTrace&& Other);

	~FFirebaseTrace();

#if PLATFORM_IOS 
	FFirebaseTrace(FIRTrace* Native);
#elif PLATFORM_ANDROID
	FFirebaseTrace(jobject Native);
#else
	FFirebaseTrace(FString InName);
#endif

private:
#if PLATFORM_IOS
	FIRTrace* Trace;
#elif PLATFORM_ANDROID
	jobject Trace;
#else
	FString Name;
	TMap<FString, int64> Metrics;
#endif
};

UCLASS()
class FIREBASEFEATURES_API UFirebasePerformanceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * Creates an instance of FFirebaseTrace. This API does not start the trace. To start the trace, use the
	 * start() API on the returned |FFirebaseTrace| object. The |TraceName| cannot be an empty string.
	 *
	 * @param TraceName The name of the Trace.
	 * @return The FFirebaseTrace object.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Performance")
	static UPARAM(DisplayName = "Trace") FFirebaseTrace CreateTrace(const FString& TraceName);

	/**
	 * Creates an instance of FFirebaseTrace after creating the shared instance of FIRPerformance. The trace
	 * will automatically be started on a successful creation of the instance. The |name| of the trace
	 * cannot be an empty string.
	 *
	 * @param TraceName The name of the Trace.
	 * @return The FFirebaseTrace object.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Performance")
	static UPARAM(DisplayName = "Trace") FFirebaseTrace CreateAndStartTrace(const FString& TraceName);

	/**
	 * Controls the instrumentation of the app to capture performance data. Setting this value to NO has
	 * immediate effect only if it is done so before calling [FIRApp configure]. Otherwise it takes
	 * effect after the app starts again the next time.
	 *
	 * If set to NO, the app will not be instrumented to collect performance
	 * data (in scenarios like app_start, networking monitoring). Default is YES.
	 *
	 * This setting is persisted, and is applied on future invocations of your application. Once
	 * explicitly set, it overrides any settings in your Info.plist.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Performance")
	static void SetInstrumentationEnabled(const bool bEnabled);

	/**
	 * Controls the capture of performance data. When this value is set to NO, none of the performance
	 * data will sent to the server. Default is YES.
	 *
	 * This setting is persisted, and is applied on future invocations of your application. Once
	 * explicitly set, it overrides any settings in your Info.plist.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Performance")
	static void SetDataCollectionEnabled(const bool bEnabled);

public:

	/**
	 * Gets the name of the trace. Available on iOS only.
	 * @return The name of the trace.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Performance", Meta = (DisplayName = "Get Name"))
	static UPARAM(DisplayName = "Name") FString Blueprint_GetName(UPARAM(ref) const FFirebaseTrace& Trace);

	/**
	 * Starts the trace.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Performance", Meta = (DisplayName = "Start Trace"))
	static void Blueprint_Start(UPARAM(ref) FFirebaseTrace& Trace);

	/**
	 * Stops the trace if the trace is active.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Performance", Meta = (DisplayName = "Stop Trace"))
	static void Blueprint_Stop(UPARAM(ref) FFirebaseTrace& Trace);

	/**
	 * Atomically increments the metric for the provided metric name with the provided value. If it is a
	 * new metric name, the metric value will be initialized to the value. Does nothing if the trace
	 * has not been started or has already been stopped.
	 *
	 * @param metricName The name of the metric to increment.
	 * @param incrementValue The value to increment the metric by.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Performance", Meta = (DisplayName = "Increment Metric"))
	static void Blueprint_IncrementMetric(UPARAM(ref) FFirebaseTrace& Trace, const FString& MetricName = TEXT(""), const int64 ByValue = 1);

	/**
	 * Gets the value of the metric for the provided metric name. If the metric doesn't exist, a 0 is
	 * returned.
	 *
	 * @param Name The name of metric whose value to get.
	 * @return The value of the given metric or 0 if it hasn't yet been set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Performance", Meta = (DisplayName = "Get Metric Value"))
	static UPARAM(DisplayName = "Value") int64 Blueprint_GetMetricValue(UPARAM(ref) const FFirebaseTrace& Trace, const FString& MetricName);

	/**
	 * Sets the value of the metric for the provided metric name to the provided value. Does nothing if
	 * the trace has not been started or has already been stopped.
	 *
	 * @param Name The name of the metric to set.
	 * @param Value The value to set the metric to.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Performance", Meta = (DisplayName = "Set Metric Value"))
	static void Blueprint_SetMetricValue(UPARAM(ref) FFirebaseTrace& Trace, const FString& MetricName, const int64 Value);

};