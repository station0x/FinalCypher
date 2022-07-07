// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseSdk/FirebaseVariant.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RemoteConfig.generated.h"

UENUM()
enum class EFirebaseConfigFetchActivationBranch : uint8
{
	Activated,
	NotActivated
};

/// @brief Describes the source a config value was retrieved from.
UENUM(BlueprintType)
enum class EFirebaseValueSource : uint8
{
	/// The value was not specified and no default was specified, so a static
	/// value (0 for numeric values, an empty string for strings) was returned.
	StaticValue,
	/// The value was found in the remote data store, and returned.
	RemoteValue,
	/// The value was not specified, so the specified default value was
	/// returned instead.
	DefaultValue,
};

/// @brief Describes the most recent fetch request status.
UENUM(BlueprintType)
enum class EFirebaseLastFetchStatus : uint8
{
	/// The most recent fetch was a success, and its data is ready to be
	/// applied, if you have not already done so.
	Success,
	/// The most recent fetch request failed.
	Failure,
	/// The most recent fetch is still in progress.
	Pending,
};

/// @brief Describes the most recent fetch failure.
UENUM(BlueprintType)
enum class EFirebaseFetchFailureReason : uint8
{
	/// The fetch has not yet failed.
	Invalid,
	/// The most recent fetch failed because it was throttled by the server.
	/// (You are sending too many fetch requests in too short a time.)
	Throttled,
	/// The most recent fetch failed for an unknown reason.
	Error,
};

/// @brief Keys of API settings.
///
/// @see SetConfigSetting
/// @see GetConfigSetting
UENUM(BlueprintType)
enum class EFirebaseConfigSetting : uint8
{
	/// Set the value associated with this key to "1" to enable developer mode
	/// (i.e disable throttling) and "0" to disable.
	SettingDeveloperMode,
};

/// @brief Describes the most recent fetch failure.
UENUM(BlueprintType)
enum class EFetchFailureReason : uint8
{
	/// The fetch has not yet failed.
	Invalid,
	
	/// The most recent fetch failed because it was throttled by the server.
	/// (You are sending too many fetch requests in too short a time.)
	Throttled,
	
	/// The most recent fetch failed for an unknown reason.
	Error,
};

/// @brief Describes the state of the most recent Fetch() call.
/// Normally returned as a result of the GetInfo() function.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseConfigInfo
{
	GENERATED_BODY()
public:
	/// @brief The time (in milliseconds since the epoch) that the last fetch
	/// operation completed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|RemoteConfig")
	int64 FetchTime = 0;

	/// @brief The status of the last fetch request.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|RemoteConfig")
	EFirebaseLastFetchStatus LastFetchStatus = EFirebaseLastFetchStatus::Failure;

	/// @brief The reason the most recent fetch failed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|RemoteConfig")
	EFirebaseFetchFailureReason LastFetchFailureReason = EFirebaseFetchFailureReason::Invalid;

	/// @brief The time (in milliseconds since the epoch) when the refreshing of
	/// Remote Config data is throttled.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|RemoteConfig")
	int64 ThrottledEndTime = 0;
};


/// @brief Describes a retrieved value.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseValueInfo
{
	GENERATED_BODY()
public:
	/// Where the config value was retrieved from (Default Config or Active
	/// Config).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|RemoteConfig")
	EFirebaseValueSource Source = EFirebaseValueSource::DefaultValue;

	/// If <code>true</code> this indicates conversion to the requested type
	/// succeeded, otherwise conversion failed so the static value for the
	/// requested type was retrieved instead.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|RemoteConfig")
	bool bConversionSuccessful = false;
};

/// @brief Describes a mapping of a key to a string value. Used to set default
/// values.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseConfigKeyValue 
{
	GENERATED_BODY()
public:
	/// The lookup key string.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|RemoteConfig")
	FString Key;

	/// The value string to be stored.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|RemoteConfig")
	FString Value;
};

/// @brief Describes a mapping of a key to a value of any type. Used to set
/// default values.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseConfigKeyValueVariant
{
	GENERATED_BODY()
public:
	/// The lookup key string.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|RemoteConfig")
	FString Key;

	/// The value to be stored. The type of the Variant determines the type of
	/// default data for the given key.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|RemoteConfig")
	FFirebaseVariant Value;
};

DECLARE_DELEGATE_OneParam(FRemoteConfigCallback, int32 /* Error */);

UCLASS()
class FIREBASEFEATURES_API UFirebaseRemoteConfig : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/// @brief Terminate the RemoteConfig API.
	///
	/// Call this method to free resources associated with the Remote Config API.
	UFUNCTION(BlueprintCallable, Category = "Firebase|RemoteConfig")
	static void Terminate();

#if PLATFORM_ANDROID
	/// @brief Sets the default values, using an XML resource.
	///
	/// @note This method is specific to the Android implementation.
	///
	/// This completely overwrites all previous default values.
	///
	/// @param[in] defaults_resource_id Id for the XML resource, which should be in
	/// your applications res/xml folder.
	static void SetDefaults(const int32 DefaultsResourceId);
#endif // PLATFORM_ANDROID


	/// @brief Sets the default values based on a mapping of string to Variant.
	/// This allows you to specify defaults of type other than string.
	///
	/// The type of each Variant in the map determines the type of data for which
	/// you are providing a default. For example, boolean values can be retrieved
	/// with GetBool(), integer values can be retrieved with GetLong(), double
	/// values can be retrieved with GetDouble(), string values can be retrieved
	/// with GetString(), and binary data can be retrieved with GetData(). Aggregate
	/// Variant types are not allowed.
	///
	/// @see firebase::Variant for more information on how to create a Variant of
	/// each type.
	///
	/// @note This completely overrides all previous values.
	///
	/// @param Defaults Array of ConfigKeyValueVariant, representing the new set of
	/// defaults to apply. If the same key is specified multiple times, the
	/// value associated with the last duplicate key is applied.
	/// @param number_of_defaults Number of elements in the defaults array.
	UFUNCTION(BlueprintCallable, Category = "Firebase|RemoteConfig")
	static void SetDefaultsWithVariant(const TArray<FFirebaseConfigKeyValueVariant>& Defaults);


	/// @brief Sets the default values based on a string map.
	///
	/// @note This completely overrides all previous values.
	///
	/// @param Defaults Array of ConfigKeyValue, representing the new set of
	/// defaults to apply. If the same key is specified multiple times, the
	/// value associated with the last duplicate key is applied.
	/// @param number_of_defaults Number of elements in the defaults array.
	UFUNCTION(BlueprintCallable, Category = "Firebase|RemoteConfig")
	static void SetDefaults(const TArray<FFirebaseConfigKeyValue>& Defaults);

	/// @brief Retrieve an internal configuration setting.
	///
	/// @param[in] Defaults Setting to retrieve.
	///
	/// @return The value of the config
	///
	/// @see SetConfigSetting
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig", Meta = (DeprecatedFunction))
	static UPARAM(DisplayName = "Config Setting") FString GetConfigSetting(EFirebaseConfigSetting Defaults);

	/// @brief Sets an internal configuration setting.
	///
	/// @param[in] setting Setting to set.
	/// @param[in] value Value to apply to the setting.
	///
	/// @see ConfigSetting
	UFUNCTION(BlueprintCallable, Category = "Firebase|RemoteConfig", Meta = (DeprecatedFunction))
	static void SetConfigSetting(EFirebaseConfigSetting Setting, const FString& Value);

	/// @brief Returns the value associated with a key, converted to a bool.
	///
	/// Values of "1", "true", "t", "yes", "y" and "on" are interpreted (case
	/// insensitive) as <code>true</code> and "0", "false", "f", "no", "n", "off",
	/// and empty strings are interpreted (case insensitive) as <code>false</code>.
	///
	/// @param[in] key Key of the value to be retrieved.
	///
	/// @return Value associated with the specified key converted to a boolean
	/// value.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Value") bool GetBoolean(const FString& Key);

	/// @brief Returns the value associated with a key, converted to a bool.
	///
	/// Values of "1", "true", "t", "yes", "y" and "on" are interpreted (case
	/// insensitive) as <code>true</code> and "0", "false", "f", "no", "n", "off",
	/// and empty strings are interpreted (case insensitive) as <code>false</code>.
	///
	/// @param[in] key Key of the value to be retrieved.
	/// @param[out] info A return value, specifying the source of the returned
	/// value.
	///
	/// @return Value associated with the specified key converted to a boolean
	/// value.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Value") bool GetBooleanWithInfo(const FString& Key, FFirebaseValueInfo& Info);


	/// @brief Returns the value associated with a key, converted to a 8-bit
	/// integer.
	///
	/// @param[in] key Key of the value to be retrieved.
	///
	/// @return Value associated with the specified key converted to a 8-bit
	/// integer.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Value") uint8 GetByte(const FString& Key);

	/// @brief Returns the value associated with a key, converted to a 32-bit
	/// integer.
	///
	/// @param[in] key Key of the value to be retrieved.
	///
	/// @return Value associated with the specified key converted to a 32-bit
	/// integer.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Value") int32 GetInt32(const FString& Key);

	/// @brief Returns the value associated with a key, converted to a 64-bit
	/// integer.
	///
	/// @param[in] key Key of the value to be retrieved.
	///
	/// @return Value associated with the specified key converted to a 64-bit
	/// integer.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Value") int64 GetInt64(const FString& Key);

	/// @brief Returns the value associated with a key, converted to a 64-bit
	/// integer.
	///
	/// @param[in] key Key of the value to be retrieved.
	/// @param[out] Info A return value, specifying the source of the returned
	/// value.
	///
	/// @return Value associated with the specified key converted to a 64-bit
	/// integer.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Value") int64 GetInt64WithInfo(const FString& Key, FFirebaseValueInfo& Info);

	/// @brief Returns the value associated with a key, converted to a double.
	///
	/// @param[in] key Key of the value to be retrieved.
	///
	/// @return Value associated with the specified key converted to a double.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Value") float GetFloat(const FString& Key);

	/// @brief Returns the value associated with a key, converted to a double.
	///
	/// @param[in] key Key of the value to be retrieved.
	/// @param[out] info A return value, specifying the source of the returned
	/// value.
	///
	/// @return Value associated with the specified key converted to a double.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Value") float GetFloatWithInfo(const FString& Key, FFirebaseValueInfo& Info);

	/// @brief Returns the value associated with a key, converted to a string.
	///
	/// @param[in] key Key of the value to be retrieved.
	///
	/// @return Value as a string associated with the specified key.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Value") FString GetString(const FString& Key);

	/// @brief Returns the value associated with a key, converted to a string.
	///
	/// @param[in] key Key of the value to be retrieved.
	/// @param[out] info A return value, specifying the source of the returned
	/// value.
	///
	/// @return Value as a string associated with the specified key.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Value") FString GetStringWithInfo(const FString& Key, FFirebaseValueInfo& Info);

	/// @brief Returns the value associated with a key, as a vector of raw
	/// byte-data.
	///
	/// @param[in] key Key of the value to be retrieved.
	///
	/// @return Vector of bytes.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Data") TArray<uint8> GetData(const FString& Key);

	/// @brief Returns the value associated with a key, as a vector of raw
	/// byte-data.
	///
	/// @param[in] key Key of the value to be retrieved.
	/// @param[out] info A return value, specifying the source of the returned
	/// value.
	///
	/// @return Vector of bytes.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Data") TArray<uint8> GetDataWithInfo(const FString& Key, FFirebaseValueInfo& Info);

	/// @brief Gets the set of keys that start with the given prefix.
	///
	/// @param[in] prefix The key prefix to look for. If empty or null, this
	/// method will return all keys.
	///
	/// @return Set of Remote Config parameter keys that start with the specified
	/// prefix. Will return an empty set if there are no keys with the given
	/// prefix.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Keys") TArray<FString> GetKeysByPrefix(const FString& Prefix);

	/// @brief Gets the set of all keys.
	///
	/// @return Set of all Remote Config parameter keys.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Keys") TArray<FString> GetKeys();

	/// @brief Fetches config data from the server.
	///
	/// @note This does not actually apply the data or make it accessible,
	/// it merely retrieves it and caches it.  To accept and access the newly
	/// retrieved values, you must call @ref ActivateFetched().
	/// Note that this function is asynchronous, and will normally take an
	/// unspecified amount of time before completion.
	static void Fetch(const FRemoteConfigCallback& Callback = FRemoteConfigCallback());

	/// @brief Fetches config data from the server.
	///
	/// @note This does not actually apply the data or make it accessible,
	/// it merely retrieves it and caches it.  To accept and access the newly
	/// retrieved values, you must call @ref ActivateFetched().
	/// Note that this function is asynchronous, and will normally take an
	/// unspecified amount of time before completion.
	///
	/// @param[in] cache_expiration_in_seconds The number of seconds to keep
	/// previously fetch data available.  If cached data is available that is newer
	/// than cache_expiration_in_seconds, then the function returns immediately
	/// and does not fetch any data. A cache_expiration_in_seconds of zero will
	/// always cause a fetch.
	static void Fetch(const int64 CacheExpirationInSeconds, const FRemoteConfigCallback& Callback = FRemoteConfigCallback());

	/// @brief Applies the most recently fetched data, so that its values can be
	/// accessed.
	///
	/// Calls to @ref GetLong(), @ref GetDouble(), @ref GetString() and
	/// @ref GetData() will not reflect the new data retrieved by @ref Fetch()
	/// until @ref ActivateFetched() is called.  This gives the developer control
	/// over when newly fetched data is visible to their application.
	///
	/// @return true if a previously fetch configuration was activated, false
	/// if a fetched configuration wasn't found or the configuration was previously
	/// activated.
	UFUNCTION(BlueprintCallable, Category = "Firebase|RemoteConfig", Meta = (ExpandEnumAsExecs = "Branch"))
	static void ActivateFetched(EFirebaseConfigFetchActivationBranch& Branch);
	static bool ActivateFetched();

	/// @brief Returns information about the last fetch request, in the form
	/// of a ConfigInfo struct.
	///
	/// @return A ConfigInfo struct, containing fields reflecting the state
	/// of the most recent fetch request.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|RemoteConfig")
	static UPARAM(DisplayName = "Info") FFirebaseConfigInfo GetInfo();
};