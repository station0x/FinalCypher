// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CrashlyticsLibrary.generated.h"

/**
 * Crashlytics library.
 * Crashlytics is still a work in progress.
*/
UCLASS()
class FIREBASEFEATURES_API UCrashlyticsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * Crashes the application.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics")
	static void CrashApplication();

	/**
	 * Adds logging that is sent with your crash data. The logging does not appear  in the
	 * system.log and is only visible in the Crashlytics dashboard.
	 *
	 * @param Message Message to log
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics")
	static void Log(const FString& Message);

	/**
	 * Returns whether the app crashed during the previous execution.
	 * @return Whether the app crashed during the previous execution.
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Crashlytics")
	static UPARAM(DisplayName = "Did Crash") bool DidCrashDuringPreviousExecution();

	/**
	 * Enables/disables automatic data collection.
	 *
	 * Calling this method overrides both the FirebaseCrashlyticsCollectionEnabled flag in your
	 * App's Info.plist and FIRApp's isDataCollectionDefaultEnabled flag.
	 *
	 * When you set a value for this method, it persists across runs of the app.
	 *
	 * The value does not apply until the next run of the app. If you want to disable data
	 * collection without rebooting, add the FirebaseCrashlyticsCollectionEnabled flag to your app's
	 * Info.plist.
	 * *
	 * @param bEnabled Determines whether automatic data collection is enabled
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics")
	static void SetCrashlyticsCollectionEnabled(bool bEnabled);

	/**
	 * /!\ METHOD NOT AVAILABLE ON ANDROID /!\
	 * 
	 * Indicates whether or not automatic data collection is enabled
	 *
	 * This method uses three ways to decide whether automatic data collection is enabled,
	 * in order of priority:
	 *  - If setCrashlyticsCollectionEnabled is called with a value, use it
	 *  - If the FirebaseCrashlyticsCollectionEnabled key is in your app's Info.plist, use it
	 *  - Otherwise, use the default isDataCollectionDefaultEnabled in FIRApp
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Crashlytics")
	static UPARAM(DisplayName = "Is Enabled") bool IsCrashlyticsCollectionEnabled();

	/**
	* Records a user ID (identifier) that's associated with subsequent fatal and non-fatal reports.
	*
	* If you want to associate a crash with a specific user, we recommend specifying an arbitrary
	* string (e.g., a database, ID, hash, or other value that you can index and query, but is
	* meaningless to a third-party observer). This allows you to facilitate responses for support
	* requests and reach out to users for more information.
	*
	* @param UserID An arbitrary user identifier string that associates a user to a record in your
	* system.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics")
	static void SetUserID(const FString& UserID);

	/**
	 * Sets a custom key and value to be associated with subsequent fatal and non-fatal reports.
	 * When setting an object value.
	 *
	 * @param Key A unique key
	 * @param Value The value to be associated with the key
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics")
	static void SetCustomStringValue(const FString& Key, const FString& Value);

	/**
	 * Sets a custom key and value to be associated with subsequent fatal and non-fatal reports.
	 * When setting an object value.
	 *
	 * @param Key A unique key
	 * @param Value The value to be associated with the key
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics")
	static void SetCustomBoolValue(const FString& Key, const bool Value);

	/**
	 * Sets a custom key and value to be associated with subsequent fatal and non-fatal reports.
	 * When setting an object value.
	 *
	 * @param Key A unique key
	 * @param Value The value to be associated with the key
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics", Meta = (DisplayName = "Set Custom Int32 Value"))
	static void SetCustomInt32Value(const FString& Key, const int32 Value);

	/**
	 * Sets a custom key and value to be associated with subsequent fatal and non-fatal reports.
	 * When setting an object value.
	 *
	 * @param Key A unique key
	 * @param Value The value to be associated with the key
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics", Meta = (DisplayName = "Set Custom Int64 Value"))
	static void SetCustomInt64Value(const FString& Key, const int64 Value);

	/**
	 * Sets a custom key and value to be associated with subsequent fatal and non-fatal reports.
	 * When setting an object value.
	 *
	 * @param Key A unique key
	 * @param Value The value to be associated with the key
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics", Meta = (DisplayName = "Set Custom Float Value"))
	static void SetCustomFloatValue(const FString& Key, const float Value);

	/**
	 * Sets a custom key and value to be associated with subsequent fatal and non-fatal reports.
	 * When setting an object value.
	 *
	 * @param Key A unique key
	 * @param Value The value to be associated with the key
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics")
	static void SetCustomVectorValue(const FString& Key, const FVector Value);

	/**
	 * Records a non-fatal event described by an error object. The events are
	 * grouped and displayed similarly to crashes. Keep in mind that this method can be expensive.
	 * The total number of errors that can be recorded during your app's life-cycle is limited by a
	 * fixed-size circular buffer. If the buffer is overrun, the oldest data is dropped. Errors are
	 * relayed to Crashlytics on a subsequent launch of your application.
	 *
	 * @param Domain Domain of the error.
	 * @param Message Message of the error.
	 * @param Code Code of the error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics")
	static void RecordError(const FString& Domain, const FString& Message, const int64 Code);

	/**
	 * Enqueues any unsent reports on the device to upload to Crashlytics.
	 *
	 * This method only applies if automatic data collection is disabled.
	 *
	 * When automatic data collection is enabled, Crashlytics automatically uploads and deletes reports
	 * at startup, so this method is ignored.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics")
	static void SendUnsentReports();

	/**
	 * Deletes any unsent reports on the device.
	 *
	 * This method only applies if automatic data collection is disabled.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Crashlytics")
	static void DeleteUnsentReports();
};

