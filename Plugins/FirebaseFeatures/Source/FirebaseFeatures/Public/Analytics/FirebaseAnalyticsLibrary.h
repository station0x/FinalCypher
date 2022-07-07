// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FirebaseAnalyticsLibrary.generated.h"

DECLARE_DELEGATE_OneParam(FFirebaseAnalyticsCallback, const int32);
DECLARE_DELEGATE_TwoParams(FFirebaseAnalyticsStringCallback, const int32, const FString&);

UCLASS()
class FIREBASEFEATURES_API UFirebaseAnalyticsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics")
	static UPARAM(DisplayName = "Has Started") bool StartSession();

	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics")
	static void EndSession();

	/// Clears all analytics data for this app from the device and resets the app
	/// instance id.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics")
	static void ResetAnalyticsData();

	/// @brief Sets whether analytics collection is enabled for this app on this
	/// device.
	///
	/// This setting is persisted across app sessions. By default it is enabled.
	///
	/// @param[in] enabled true to enable analytics collection, false to disable.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics")
	static void SetAnalyticsCollectionEnabled(const bool bEnabled);

	/// @brief Log an event with one string parameter.
	///
	/// @param[in] name Name of the event to log. Should contain 1 to 40
	/// alphanumeric characters or underscores. The name must start with an
	/// alphabetic character. Some event names are reserved.
	/// @if cpp_examples
	/// See @ref event_names (%event_names.h) for the list of reserved event names.
	/// @endif
	/// The "firebase_" prefix is reserved and should not be used. Note that event
	/// names are case-sensitive and that logging two events whose names differ
	/// only in case will result in two distinct events.
	/// @param[in] parameter_name Name of the parameter to log.
	/// For more information, see @ref Parameter.
	/// @param[in] parameter_value Value of the parameter to log.
	///
	/// @if cpp_examples
	/// @see LogEvent(const char*, const Parameter*, size_t)
	/// @endif
	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics", DisplayName = "Log Event (String)")
	static void LogEventString(const FString& EventName, const FString& ParameterName, const FString& ParameterValue);

	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics", DisplayName = "Log Event (Float)")
	static void LogEventFloat(const FString& EventName, const FString& ParameterName, const float ParameterValue);
	
	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics", DisplayName = "Log Event (Int64)")
	static void LogEventInt64(const FString& EventName, const FString& ParameterName, const int64 ParameterValue);
	
	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics", DisplayName = "Log Event (Int32)")
	static void LogEventInt32(const FString& EventName, const FString& ParameterName, const int32 ParameterValue);

	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics", DisplayName = "Log Event")
	static void LogEvent(const FString& EventName);
	
	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics", DisplayName = "Log Event with Parameters")
	static void LogEventWithParameter(const FString& EventName, const TMap<FString, FString>& Parameters);

	/// @brief Set a user property to the given value.
	///
	/// Properties associated with a user allow a developer to segment users
	/// into groups that are useful to their application.  Up to 25 properties
	/// can be associated with a user.
	///
	/// Suggested property names are listed @ref user_property_names
	/// (%user_property_names.h) but you're not limited to this set. For example,
	/// the "gamertype" property could be used to store the type of player where
	/// a range of values could be "casual", "mid_core", or "core".
	///
	/// @param[in] name Name of the user property to set.  This must be a
	/// combination of letters and digits (matching the regular expression
	/// [a-zA-Z0-9] between 1 and 40 characters long starting with a letter
	/// [a-zA-Z] character.
	/// @param[in] property Value to set the user property to.  Set this
	/// argument to NULL or nullptr to remove the user property.  The value can be
	/// between 1 to 100 characters long.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics")
	static void SetUserProperty(const FString& Name, const FString& Property);

	/// @brief Sets the user ID property.
	///
	/// This feature must be used in accordance with
	/// <a href="https://www.google.com/policies/privacy">Google's Privacy
	/// Policy</a>
	///
	/// @param[in] user_id The user ID associated with the user of this app on this
	/// device.  The user ID must be non-empty and no more than 256 characters long.
	/// Setting user_id to NULL or nullptr removes the user ID.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics")
	static void SetUserId(const FString& UserId);
	
	/// @brief Sets the duration of inactivity that terminates the current session.
	///
	/// @note The default value is 1800000 (30 minutes).
	///
	/// @param milliseconds The duration of inactivity that terminates the current
	/// session.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics")
	static void SetSessionTimeoutDuration(const int64 Milliseconds);

	/// @brief Sets the current screen name and screen class, which specifies the
	/// current visual context in your app. This helps identify the areas in your
	/// app where users spend their time and how they interact with your app.
	///
	/// @param screen_name The name of the current screen. Set to nullptr to clear
	/// the current screen name. Limited to 100 characters.
	/// @param screen_class The name of the screen class. If you specify nullptr for
	/// this, it will use the default. On Android, the default is the class name of
	/// the current Activity. On iOS, the default is the class name of the current
	/// UIViewController. Limited to 100 characters.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Analytics", meta = (DeprecatedFunction, DeprecationMessage = "Use LogEvent with the ScreenView event to manually log screen changes."))
	static void SetCurrentScreen(const FString& ScreenName, const FString& ScreenClass);

	/// Get the instance ID from the analytics service.
	///
	/// @note This is *not* the same ID as the ID returned by
	/// @if cpp_examples
	/// firebase::instance_id::InstanceId.
	/// @else
	/// Firebase.InstanceId.FirebaseInstanceId.
	/// @endif
	///
	/// @returns Object which can be used to retrieve the analytics instance ID.
	static void GetAnalyticsInstanceId(const FFirebaseAnalyticsStringCallback& Callback);

	static TSharedPtr<class FFirebaseAnalyticsProvider> GetAnalyticsProvider();
};

