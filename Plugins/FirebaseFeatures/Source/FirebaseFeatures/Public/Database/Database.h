// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Database.generated.h"

namespace firebase { namespace database { class Database; class DatabaseReference; }; };

class UDatabaseReference;

/// @brief Levels used when logging messages.
UENUM(BlueprintType)
enum class EDatabaseLogLevel  : uint8
{
	/// Verbose Log Level
	Verbose = 0,
	/// Debug Log Level
	Debug,
	/// Info Log Level
	Info,
	/// Warning Log Level
	Warning,
	/// Error Log Level
	Error,
	/// Assert Log Level
	Assert
};

/// Error code returned by Firebase Realtime Database C++ functions.
UENUM(BlueprintType)
enum class EFirebaseDatabaseError : uint8
{
	/// The operation was a success, no error occurred.
	None = 0,
	/// The operation had to be aborted due to a network disconnect.
	Disconnected,
	/// The supplied auth token has expired.
	ExpiredToken,
	/// The specified authentication token is invalid.
	InvalidToken,
	/// The transaction had too many retries.
	MaxRetries,
	/// The operation could not be performed due to a network error.
	NetworkError,
	/// The server indicated that this operation failed.
	OperationFailed,
	/// The transaction was overridden by a subsequent set.
	OverriddenBySet,
	/// This client does not have permission to perform this operation.
	PermissionDenied,
	/// The service is unavailable.
	Unavailable,
	/// An unknown error occurred.
	UnknownError,
	/// The write was canceled locally.
	WriteCanceled,
	/// You specified an invalid Variant type for a field. For example,
	/// a DatabaseReference's Priority and the keys of a Map must be of
	/// scalar type (MutableString, StaticString, Int64, Double).
	InvalidVariantType,
	/// An operation that conflicts with this one is already in progress. For
	/// example, calling SetValue and SetValueAndPriority on a DatabaseReference
	/// is not allowed.
	ConflictingOperationInProgress,
	/// The transaction was aborted, because the user's DoTransaction function
	/// returned kTransactionResultAbort instead of kTransactionResultSuccess.
	TransactionAbortedByUser,
};

/// @brief Entry point for the Firebase Realtime Database C++ SDK.
/// To use the SDK, call UDatabase::GetInstance() to obtain
/// an instance of Database, then use GetReference() to obtain references to
/// child paths within the database. From there you can set data via
/// UQuery::SetValue(), get data via UQuery::GetValue(), attach listeners, and
/// more.
UCLASS()
class FIREBASEFEATURES_API UDatabase : public UObject
{
	GENERATED_BODY()

private:
	typedef firebase::database::Database			TDatabase;
	typedef firebase::database::DatabaseReference	TDatabaseReference;

	friend class UDatabaseReference;

public:

	/// @brief Get an instance of Database corresponding to your App.
	/// Firebase Realtime Database uses firebase::App to communicate with Firebase
	/// Authentication to authenticate users to the Database server backend.
	/// If you call GetInstance() multiple times, you will get
	/// the same instance of Database.
	/// @returns An instance of Database corresponding to the given App.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database", meta = (DisplayName = "Get Instance"))
	static UPARAM(DisplayName = "Database") UDatabase* GetInstance();

	/// Gets a reference to the Database instance.
	/// Short method for GetInstance()->GetReference().
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database", meta = (DisplayName = "Get Instance Reference"))
	static UPARAM(DisplayName = "Reference") UDatabaseReference* GetInstanceReference();

	/// @brief Gets an instance of FirebaseDatabase for the specified URL.
	/// If you call GetInstance() multiple times with the same App and URL, you
	/// will get the same instance of Database
	/// @param[in] Url The URL of your Firebase Realtime Database. This overrides
	/// any url specified in the App options.
	/// @returns An instance of Database corresponding to the given App and URL.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database", meta = (DisplayName = "Get Instance from URL"))
	static UPARAM(DisplayName = "Database") UDatabase* GetInstanceFromUrl(const FString& Url);

	/// @brief Get a DatabaseReference to the root of the database.
	/// @returns A DatabaseReference to the root of the database.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database")
	UPARAM(DisplayName = "Reference") UDatabaseReference* GetReference() const;
	
	/// @brief Get a UDatabaseReference for the specified path.
	///
	/// @returns A DatabaseReference to the specified path in the database.
	/// If you specified an invalid path, the reference's
	/// DatabaseReference::IsValid() will return false.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database")
	UPARAM(DisplayName = "Reference") UDatabaseReference* GetReferenceFromPath(const FString& Path) const;

	/// @brief Get a DatabaseReference for the provided URL, which must belong to
	/// the database URL this instance is already connected to.
	/// @returns A DatabaseReference to the specified path in the database.
	/// If you specified an invalid path, the reference's
	/// DatabaseReference::IsValid() will return false.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database")
	UPARAM(DisplayName = "Reference") UDatabaseReference* GetReferenceFromUrl(const FString& Url) const;

	/// @brief Get the URL that this Database was created with.
	/// @returns The URL this Database was created with, or an empty string if
	/// this Database was created with default parameters. 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database", meta = (DisplayName = "Get URL"))
	UPARAM(DisplayName = "URL") FString GetUrl();

	/// @brief Shuts down the connection to the Firebase Realtime Database
	/// backend until GoOnline() is called.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database", meta = (DisplayName = "Go Offline"))
	void GoOffline();

	/// @brief Resumes the connection to the Firebase Realtime Database backend
	/// after a previous GoOffline() call.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database", meta = (DisplayName = "Go Online"))
	void GoOnline();

	/// @brief Purge all pending writes to the Firebase Realtime Database server.
	/// The Firebase Realtime Database client automatically queues writes and
	/// sends them to the server at the earliest opportunity, depending on network
	/// connectivity. In some cases (e.g. offline usage) there may be a large
	/// number of writes waiting to be sent. Calling this method will purge all
	/// outstanding writes so they are abandoned. All writes will be purged,
	/// including transactions and onDisconnect() writes. The writes will be
	/// rolled back locally, perhaps triggering events for affected event
	/// listeners, and the client will not (re-)send them to the Firebase backend.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database", meta = (DisplayName = "Purge Outstanding Writes"))
	void PurgeOutstandingWrites();

	/// @brief Sets whether pending write data will persist between application
	/// exits.
	/// The Firebase Database client will cache synchronized data and keep track
	/// of all writes you've initiated while your application is running. It
	/// seamlessly handles intermittent network connections and re-sends write
	/// operations when the network connection is restored. However by default
	/// your write operations and cached data are only stored in-memory and will
	/// be lost when your app restarts. By setting this value to `true`, the data
	/// will be persisted to on-device (disk) storage and will thus be available
	/// again when the app is restarted (even when there is no network
	/// connectivity at that time).
	/// @note SetPersistenceEnabled should be called before creating any instances
	/// of DatabaseReference, and only needs to be called once per application.
	/// @param[in] enabled Set this to true to persist write data to on-device
	/// (disk) storage, or false to discard pending writes when the app exits.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database", meta = (DisplayName = "Set Persistence Enabled"))
	void SetPersistenceEnabled(const bool bEnabled);

	/// Set the log verbosity of this Database instance.
	/// The log filtering is cumulative with Firebase App. That is, this library's
	/// log messages will only be displayed if they are not filtered out by this
	/// library's log level setting and by Firebase App's log level setting
	/// @note On Android this can only be set before any operations have been
	/// performed with the object.
	/// @param[in] log_level Log level, by default this is set to kLogLevelInfo.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database", meta = (DisplayName = "Set Log Level"))
	void SetLogLevel(const EDatabaseLogLevel DatabaseLogLevel);

private:
	static TDatabase* GetDatabase(const FString& Url);
	TDatabase*		  GetDatabase() const;

	FString Url;
};

