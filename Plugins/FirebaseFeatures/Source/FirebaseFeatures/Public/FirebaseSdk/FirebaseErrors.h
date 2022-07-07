// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseErrors.generated.h"

UENUM(BlueprintType)
enum class EFirebaseAuthError : uint8
{
	/// Success.
	None = 0,

	/// Function will be implemented in a later revision of the API.
	Unimplemented = 255,

	/// This indicates an internal error.
	/// Common error code for all API Methods.
	Failure = 1,

	/// Indicates a validation error with the custom token.
	/// This error originates from "bring your own auth" methods.
	InvalidCustomToken,

	/// Indicates the service account and the API key belong to different
	/// projects.
	/// Caused by "Bring your own auth" methods.
	CustomTokenMismatch,

	/// Indicates the IDP token or requestUri is invalid.
	/// Caused by "Sign in with credential" methods.
	InvalidCredential,

	/// Indicates the user's account is disabled on the server.
	/// Caused by "Sign in with credential" methods.
	UserDisabled,

	/// Indicates an account already exists with the same email address but using
	/// different sign-in credentials. Account linking is required.
	/// Caused by "Sign in with credential" methods.
	AccountExistsWithDifferentCredentials,

	/// Indicates the administrator disabled sign in with the specified identity
	/// provider.
	/// Caused by "Set account info" methods.
	OperationNotAllowed,

	/// Indicates the email used to attempt a sign up is already in use.
	/// Caused by "Set account info" methods.
	EmailAlreadyInUse,

	/// Indicates the user has attemped to change email or password more than 5
	/// minutes after signing in, and will need to refresh the credentials.
	/// Caused by "Set account info" methods.
	RequiresRecentLogin,

	/// Indicates an attempt to link with a credential that has already been
	/// linked with a different Firebase account.
	/// Caused by "Set account info" methods.
	CredentialAlreadyInUse,

	/// Indicates an invalid email address.
	/// Caused by "Sign in with password" methods.
	InvalidEmail,

	/// Indicates the user attempted sign in with a wrong password.
	/// Caused by "Sign in with password" methods.
	WrongPassword,

	/// Indicates that too many requests were made to a server method.
	/// Common error code for all API methods.
	TooManyRequests,

	/// Indicates the user account was not found.
	/// Send password request email error code.
	/// Common error code for all API methods.
	UserNotFound,

	/// Indicates an attempt to link a provider to which the account is already
	/// linked.
	/// Caused by "Link credential" methods.
	ProviderAlreadyLinked,

	/// Indicates an attempt to unlink a provider that is not linked.
	/// Caused by "Link credential" methods.
	NoSuchProvider,

	/// Indicates user's saved auth credential is invalid, the user needs to sign
	/// in again.
	/// Caused by requests with an STS id token.
	InvalidUserToken,

	/// Indicates the saved token has expired.
	/// For example, the user may have changed account password on another device.
	/// The user needs to sign in again on the device that made this request.
	/// Caused by requests with an STS id token.
	UserTokenExpired,

	/// Indicates a network error occurred (such as a timeout, interrupted
	/// connection, or unreachable host). These types of errors are often
	/// recoverable with a retry.
	/// Common error code for all API Methods.
	NetworkRequestFailed,

	/// Indicates an invalid API key was supplied in the request.
	/// For Android these should no longer occur (as of 2016 v3).
	/// Common error code for all API Methods.
	InvalidApiKey,

	/// Indicates the App is not authorized to use Firebase Authentication with
	/// the provided API Key.
	/// Common error code for all API Methods.
	/// On Android this error should no longer occur (as of 2016 v3).
	/// Common error code for all API Methods.
	AppNotAuthorized,

	/// Indicates that an attempt was made to reauthenticate with a user which is
	/// not the current user.
	UserMismatch,

	/// Indicates an attempt to set a password that is considered too weak.
	WeakPassword,

	/// Internal api usage error code when there is no signed-in user
	/// and getAccessToken is called.
	///
	/// @note This error is only reported on Android.
	NoSignedInUser,

	/// This can happen when certain methods on App are performed, when the auth
	/// API is not loaded.
	///
	/// @note This error is only reported on Android.
	ApiNotAvailable,

	/// Indicates the out-of-band authentication code is expired.
	ExpiredActionCode,

	/// Indicates the out-of-band authentication code is invalid.
	InvalidActionCode,

	/// Indicates that there are invalid parameters in the payload during a
	/// "send password reset email" attempt.
	InvalidMessagePayload,

	/// Indicates that an invalid phone number was provided.
	/// This is caused when the user is entering a phone number for verification.
	InvalidPhoneNumber,

	/// Indicates that a phone number was not provided during phone number
	/// verification.
	///
	/// @note This error is iOS-specific.
	MissingPhoneNumber,

	/// Indicates that the recipient email is invalid.
	InvalidRecipientEmail,

	/// Indicates that the sender email is invalid during a "send password reset
	/// email" attempt.
	InvalidSender,

	/// Indicates that an invalid verification code was used in the
	/// verifyPhoneNumber request.
	InvalidVerificationCode,

	/// Indicates that an invalid verification ID was used in the
	/// verifyPhoneNumber request.
	InvalidVerificationId,

	/// Indicates that the phone auth credential was created with an empty
	/// verification code.
	MissingVerificationCode,

	/// Indicates that the phone auth credential was created with an empty
	/// verification ID.
	MissingVerificationId,

	/// Indicates that an email address was expected but one was not provided.
	MissingEmail,

	/// Represents the error code for when an application attempts to create an
	/// email/password account with an empty/null password field.
	///
	/// @note This error is only reported on Android.
	MissingPassword,

	/// Indicates that the project's quota for this operation (SMS messages,
	/// sign-ins, account creation) has been exceeded. Try again later.
	QuotaExceeded,

	/// Thrown when one or more of the credentials passed to a method fail to
	/// identify and/or authenticate the user subject of that operation. Inspect
	/// the error message to find out the specific cause.
	/// @note This error is only reported on Android.
	RetryPhoneAuth,

	/// Indicates that the SMS code has expired.
	SessionExpired,

	/// Indicates that the app could not be verified by Firebase during phone
	/// number authentication.
	///
	/// @note This error is iOS-specific.
	AppNotVerified,

	/// Indicates a general failure during the app verification flow.
	///
	/// @note This error is iOS-specific.
	AppVerificationFailed,

	/// Indicates that the reCAPTCHA token is not valid.
	///
	/// @note This error is iOS-specific.
	CaptchaCheckFailed,

	/// Indicates that an invalid APNS device token was used in the verifyClient
	/// request.
	///
	/// @note This error is iOS-specific.
	InvalidAppCredential,

	/// Indicates that the APNS device token is missing in the verifyClient
	/// request.
	///
	/// @note This error is iOS-specific.
	MissingAppCredential,

	/// Indicates that the clientID used to invoke a web flow is invalid.
	///
	/// @note This error is iOS-specific.
	InvalidClientId,

	/// Indicates that the domain specified in the continue URI is not valid.
	///
	/// @note This error is iOS-specific.
	InvalidContinueUri,

	/// Indicates that a continue URI was not provided in a request to the backend
	/// which requires one.
	MissingContinueUri,

	/// Indicates an error occurred while attempting to access the keychain.
	/// Common error code for all API Methods.
	///
	/// @note This error is iOS-specific.
	KeychainError,

	/// Indicates that the APNs device token could not be obtained. The app may
	/// not have set up remote notification correctly, or may have failed to
	/// forward the APNs device token to FIRAuth if app delegate swizzling is
	/// disabled.
	///
	/// @note This error is iOS-specific.
	MissingAppToken,

	/// Indicates that the iOS bundle ID is missing when an iOS App Store ID is
	/// provided.
	///
	/// @note This error is iOS-specific.
	MissingIosBundleId,

	/// Indicates that the app fails to forward remote notification to FIRAuth.
	///
	/// @note This error is iOS-specific.
	NotificationNotForwarded,

	/// Indicates that the domain specified in the continue URL is not white-
	/// listed in the Firebase console.
	///
	/// @note This error is iOS-specific.
	UnauthorizedDomain,

	/// Indicates that an attempt was made to present a new web context while one
	/// was already being presented.
	WebContextAlreadyPresented,

	/// Indicates that the URL presentation was cancelled prematurely by the user.
	WebContextCancelled,

	/// Indicates that Dynamic Links in the Firebase Console is not activated.
	DynamicLinkNotActivated,

	/// Indicates that the operation was cancelled.
	Cancelled,

	/// Indicates that the provider id given for the web operation is invalid.
	InvalidProviderId,

	/// Indicates that an internal error occurred during a web operation.
	WebInternalError,

	/// Indicates that 3rd party cookies or data are disabled, or that there was
	/// a problem with the browser.
	WebStorateUnsupported,

	/// Indicates that the provided tenant ID does not match the Auth instance's
	/// tenant ID.
	TenantIdMismatch,

	/// Indicates that a request was made to the backend with an associated tenant
	/// ID for an operation that does not support multi-tenancy.
	UnsupportedTenantOperation,

	/// Indicates that an FDL domain used for an out of band code flow is either
	/// not configured or is unauthorized for the current project.
	InvalidLinkDomain,

	/// Indicates that credential related request data is invalid. This can occur
	/// when there is a project number mismatch (sessionInfo, spatula header,
	/// temporary proof),
	/// an incorrect temporary proof phone number, or during game center sign in
	/// when the user is
	/// already signed into a different game center account.
	RejectedCredential,

	/// Indicates that the phone number provided in the MFA sign in flow to be
	/// verified does not correspond to a phone second factor for the user.
	PhoneNumberNotFound,

	/// Indicates that a request was made to the backend with an invalid tenant
	/// ID.
	InvalidTenantId,

	/// Indicates that a request was made to the backend without a valid client
	/// identifier.
	MissingClientIdentifier,

	/// Indicates that a second factor challenge request was made without proof of
	/// a successful first factor sign-in.
	MissingMultiFactorSession,

	/// Indicates that a second factor challenge request was made where a second
	/// factor identifier was not provided.
	MissingMultiFactorInfo,

	/// Indicates that a second factor challenge request was made containing an
	/// invalid proof of first factor sign-in.
	InvalidMultiFactorSession,

	/// Indicates that the user does not have a second factor matching the
	/// provided identifier.
	MultiFactorInfoNotFound,

	/// Indicates that a request was made that is restricted to administrators
	/// only.
	AdminRestrictedOperation,

	/// Indicates that the user's email must be verified to perform that request.
	UnverifiedEmail,

	/// Indicates that the user is trying to enroll a second factor that already
	/// exists on their account.
	SecondFactorAlreadyEnrolled,

	/// Indicates that the user has reached the maximum number of allowed second
	/// factors and is attempting to enroll another one.
	MaximumSecondFactorCountExceeded,

	/// Indicates that a user either attempted to enroll in 2FA with an
	/// unsupported first factor or is enrolled and attempts a first factor sign
	/// in that is not supported for 2FA users.
	UnsupportedFirstFactor,

	/// Indicates that a second factor users attempted to change their email with
	/// updateEmail instead of verifyBeforeUpdateEmail.
	EmailChangeNeedsVerification,

};

UENUM(BlueprintType)
enum class EFirestoreError : uint8
{
  /** The operation completed successfully. */
  // Note: NSError objects will never have a code with this value.
  Ok = 0,

  /** The operation was cancelled (typically by the caller). */
  Cancelled = 1,

  /** Unknown error or an error from a different error domain. */
  Unknown = 2,

  /**
   * Client specified an invalid argument. Note that this differs from
   * FailedPrecondition. InvalidArgument indicates arguments that are
   * problematic regardless of the state of the system (e.g., an invalid field
   * name).
   */
  InvalidArgument = 3,

  /**
   * Deadline expired before operation could complete. For operations that
   * change the state of the system, this error may be returned even if the
   * operation has completed successfully. For example, a successful response
   * from a server could have been delayed long enough for the deadline to
   * expire.
   */
  DeadlineExceeded = 4,

  /** Some requested document was not found. */
  NotFound = 5,

  /** Some document that we attempted to create already exists. */
  AlreadyExists = 6,

  /** The caller does not have permission to execute the specified operation. */
  PermissionDenied = 7,

  /**
   * Some resource has been exhausted, perhaps a per-user quota, or perhaps the
   * entire file system is out of space.
   */
  ResourceExhausted = 8,

  /**
   * Operation was rejected because the system is not in a state required for
   * the operation's execution.
   */
  FailedPrecondition = 9,

  /**
   * The operation was aborted, typically due to a concurrency issue like
   * transaction aborts, etc.
   */
  Aborted = 10,

  /** Operation was attempted past the valid range. */
  OutOfRange = 11,

  /** Operation is not implemented or not supported/enabled. */
  Unimplemented = 12,

  /**
   * Internal errors. Means some invariants expected by underlying system has
   * been broken. If you see one of these errors, something is very broken.
   */
  Internal = 13,

  /**
   * The service is currently unavailable. This is a most likely a transient
   * condition and may be corrected by retrying with a backoff.
   */
  Unavailable = 14,

  /** Unrecoverable data loss or corruption. */
  DataLoss = 15,

  /**
   * The request does not have valid authentication credentials for the
   * operation.
   */
  Unauthenticated = 16
};

/// Error code returned by Cloud Functions C++ functions.
/// Standard gRPC error codes, as defined in:
/// https://github.com/googleapis/googleapis/blob/master/google/rpc/code.proto
UENUM(BlueprintType)
enum class EFirebaseFunctionsError : uint8
{
	// Not an error; returned on success
	//
	// HTTP Mapping: 200 OK
	None = 0,

	// The operation was cancelled, typically by the caller.
	//
	// HTTP Mapping: 499 Client Closed Request
	Cancelled = 1,

	// Unknown error.  For example, this error may be returned when
	// a `Status` value received from another address space belongs to
	// an error space that is not known in this address space.  Also
	// errors raised by APIs that do not return enough error information
	// may be converted to this error.
	//
	// HTTP Mapping: 500 Internal Server Error
	Unknown = 2,

	// The client specified an invalid argument.  Note that this differs
	// from `FAILED_PRECONDITION`.  `INVALID_ARGUMENT` indicates arguments
	// that are problematic regardless of the state of the system
	// (e.g., a malformed file name).
	//
	// HTTP Mapping: 400 Bad Request
	InvalidArgument = 3,

	// The deadline expired before the operation could complete. For operations
	// that change the state of the system, this error may be returned
	// even if the operation has completed successfully.  For example, a
	// successful response from a server could have been delayed long
	// enough for the deadline to expire.
	//
	// HTTP Mapping: 504 Gateway Timeout
	DeadlineExceeded = 4,

	// Some requested entity (e.g., file or directory) was not found.
	//
	// Note to server developers: if a request is denied for an entire class
	// of users, such as gradual feature rollout or undocumented whitelist,
	// `NOT_FOUND` may be used. If a request is denied for some users within
	// a class of users, such as user-based access control, `PERMISSION_DENIED`
	// must be used.
	//
	// HTTP Mapping: 404 Not Found
	NotFound = 5,

	// The entity that a client attempted to create (e.g., file or directory)
	// already exists.
	//
	// HTTP Mapping: 409 Conflict
	AlreadyExists = 6,

	// The caller does not have permission to execute the specified
	// operation. `PERMISSION_DENIED` must not be used for rejections
	// caused by exhausting some resource (use `RESOURCE_EXHAUSTED`
	// instead for those errors). `PERMISSION_DENIED` must not be
	// used if the caller can not be identified (use `UNAUTHENTICATED`
	// instead for those errors). This error code does not imply the
	// request is valid or the requested entity exists or satisfies
	// other pre-conditions.
	//
	// HTTP Mapping: 403 Forbidden
	PermissionDenied = 7,

	// The request does not have valid authentication credentials for the
	// operation.
	//
	// HTTP Mapping: 401 Unauthorized
	Unauthenticated = 16,

	// Some resource has been exhausted, perhaps a per-user quota, or
	// perhaps the entire file system is out of space.
	//
	// HTTP Mapping: 429 Too Many Requests
	ResourceExhausted = 8,

	// The operation was rejected because the system is not in a state
	// required for the operation's execution.  For example, the directory
	// to be deleted is non-empty, an rmdir operation is applied to
	// a non-directory, etc.
	//
	// Service implementors can use the following guidelines to decide
	// between `FAILED_PRECONDITION`, `ABORTED`, and `UNAVAILABLE`:
	//  (a) Use `UNAVAILABLE` if the client can retry just the failing call.
	//  (b) Use `ABORTED` if the client should retry at a higher level
	//      (e.g., when a client-specified test-and-set fails, indicating the
	//      client should restart a read-modify-write sequence).
	//  (c) Use `FAILED_PRECONDITION` if the client should not retry until
	//      the system state has been explicitly fixed.  E.g., if an "rmdir"
	//      fails because the directory is non-empty, `FAILED_PRECONDITION`
	//      should be returned since the client should not retry unless
	//      the files are deleted from the directory.
	//
	// HTTP Mapping: 400 Bad Request
	FailedPrecondition = 9,

	// The operation was aborted, typically due to a concurrency issue such as
	// a sequencer check failure or transaction abort.
	//
	// See the guidelines above for deciding between `FAILED_PRECONDITION`,
	// `ABORTED`, and `UNAVAILABLE`.
	//
	// HTTP Mapping: 409 Conflict
	Aborted = 10,

	// The operation was attempted past the valid range.  E.g., seeking or
	// reading past end-of-file.
	//
	// Unlike `INVALID_ARGUMENT`, this error indicates a problem that may
	// be fixed if the system state changes. For example, a 32-bit file
	// system will generate `INVALID_ARGUMENT` if asked to read at an
	// offset that is not in the range [0,2^32-1], but it will generate
	// `OUT_OF_RANGE` if asked to read from an offset past the current
	// file size.
	//
	// There is a fair bit of overlap between `FAILED_PRECONDITION` and
	// `OUT_OF_RANGE`.  We recommend using `OUT_OF_RANGE` (the more specific
	// error) when it applies so that callers who are iterating through
	// a space can easily look for an `OUT_OF_RANGE` error to detect when
	// they are done.
	//
	// HTTP Mapping: 400 Bad Request
	OutOfRange = 11,

	// The operation is not implemented or is not supported/enabled in this
	// service.
	//
	// HTTP Mapping: 501 Not Implemented
	Unimplemented = 12,

	// Internal errors.  This means that some invariants expected by the
	// underlying system have been broken.  This error code is reserved
	// for serious errors.
	//
	// HTTP Mapping: 500 Internal Server Error
	Internal = 13,

	// The service is currently unavailable.  This is most likely a
	// transient condition, which can be corrected by retrying with
	// a backoff.
	//
	// See the guidelines above for deciding between `FAILED_PRECONDITION`,
	// `ABORTED`, and `UNAVAILABLE`.
	//
	// HTTP Mapping: 503 Service Unavailable
	Unavailable = 14,

	// Unrecoverable data loss or corruption.
	//
	// HTTP Mapping: 500 Internal Server Error
	DataLoss = 15
};


/// Error code returned by Cloud Storage C++ functions.
UENUM(BlueprintType)
enum class EFirebaseStorageError : uint8
{
	/// The operation was a success, no error occurred.
	None = 0,
	/// An unknown error occurred.
	Unknown,
	/// No object exists at the desired reference.
	ObjectNotFound,
	/// No bucket is configured for Cloud Storage.
	BucketNotFound,
	/// No project is configured for Cloud Storage.
	ProjectNotFound,
	/// Quota on your Cloud Storage bucket has been exceeded.
	QuotaExceeded,
	/// User is unauthenticated.
	Unauthenticated,
	/// User is not authorized to perform the desired action.
	Unauthorized,
	/// The maximum time limit on an operation (upload, download, delete, etc.)
	/// has been exceeded.
	RetryLimitExceeded,
	/// File on the client does not match the checksum of the file received by the
	/// server.
	NonMatchingChecksum,
	/// Size of the downloaded file exceeds the amount of memory allocated for the
	/// download.
	DownloadSizeExceeded,
	/// User cancelled the operation.
	Cancelled,
};


