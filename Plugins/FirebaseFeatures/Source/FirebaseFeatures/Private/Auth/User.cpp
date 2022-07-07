// Copyright Pandores Marketplace 2022. All Rights Reserved.
#include "Auth/User.h"

THIRD_PARTY_INCLUDES_START
#	include "firebase/auth/user.h"
THIRD_PARTY_INCLUDES_END

#include "Auth/AuthHelper.h"

#include "FirebaseFeatures.h"

#include "Async/Async.h"

#define USER_CHECK_VALIDITY(...) do																\
{																								\
	if (!User)																					\
	{																							\
		UE_LOG(LogFirebaseAuth, Warning, TEXT("Called a method on an invalid user object."));	\
		return __VA_ARGS__;																		\
	}																							\
} while (0)

#define LOG_FUTURE_ERROR(Text) do																\
{																								\
	if (Future.error() != (int32)EFirebaseAuthError::None)										\
	{																							\
		UE_LOG(LogFirebaseAuth, Error, TEXT(Text) TEXT(" Code: %d. Message: %s"),				\
			Future.error(), UTF8_TO_TCHAR(Future.error_message()));								\
	}																							\
} while(0)


#if WITH_FIREBASE_AUTH
FUserInfoInterface::FUserInfoInterface(firebase::auth::UserInfoInterface* const Info)
{
	if (!Info)
	{
		return;
	}

	Uid			= UTF8_TO_TCHAR(Info->uid			().c_str());
	Email		= UTF8_TO_TCHAR(Info->email			().c_str());
	DisplayName = UTF8_TO_TCHAR(Info->display_name	().c_str());
	PhotoUrl	= UTF8_TO_TCHAR(Info->photo_url		().c_str());
	ProviderId  = UTF8_TO_TCHAR(Info->provider_id	().c_str());
	PhoneNumber = UTF8_TO_TCHAR(Info->phone_number	().c_str());
}
#endif

UUser::UUser()
	: Super()
#if WITH_FIREBASE_AUTH
	, User(nullptr)
#endif
{
}

void UUser::GetToken(const bool bForceRefresh, FGetTokenCallback Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound, TEXT("")));

	User->GetToken(bForceRefresh).OnCompletion([Callback = MoveTemp(Callback)](const firebase::Future<std::string>& Future) mutable -> void
	{
		FString Token;

		LOG_FUTURE_ERROR("Failed to get user token.");

		if (Future.error() == (int32)EFirebaseAuthError::None)
		{
			Token = UTF8_TO_TCHAR(Future.result()->c_str());
		}

		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback = MoveTemp(Callback), Token = MoveTemp(Token), Error]()-> void
		{
			Callback.ExecuteIfBound(Error, Token);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

TArray<FUserInfoInterface> UUser::ProviderData()
{
	TArray<FUserInfoInterface> ProviderData;

#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY(ProviderData);

	for (const auto& Provider : User->provider_data())
	{
		ProviderData.Emplace(FUserInfoInterface(Provider));
	}
#endif
	return ProviderData;
}

void UUser::UpdateEmail(const FString& Email, const FUserVoidCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound));

	User->UpdateEmail(TCHAR_TO_UTF8(*Email)).OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to update email.");

		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error]()-> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable);
#endif
}

void UUser::UpdatePassword(const FString& Password, const FUserVoidCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound));

	User->UpdatePassword(TCHAR_TO_UTF8(*Password)).OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to update password.");
		
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error]()-> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable);
#endif
}

void UUser::Reauthenticate(const FCredential& Credential, const FUserVoidCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound));

	if (!Credential.Credential || !Credential.IsValid())
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Invalid credential passed to reauthenticate."));
		Callback.ExecuteIfBound(EFirebaseAuthError::InvalidCredential);
		return;
	}

	User->Reauthenticate(*Credential.Credential).OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to reauthenticate.");		
		
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error]()-> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable);
#endif
}

void UUser::ReauthenticateAndRetrieveData(const FCredential& Credential, const FSignInCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound, FSignInResult()));
	
	if (!Credential.Credential || !Credential.IsValid())
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Invalid credential passed to reauthenticate and retrieve data."));
		Callback.ExecuteIfBound(EFirebaseAuthError::InvalidCredential, FSignInResult());
		return;
	}

	User->ReauthenticateAndRetrieveData(*Credential.Credential).OnCompletion([Callback](const firebase::Future<firebase::auth::SignInResult>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to reauthenticate and retrieve data.");

		FSignInResult Result = FAuthHelper::ConvertSignInResult(Future.result());
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error, Result = MoveTemp(Result)]()-> void
		{
			Callback.ExecuteIfBound(Error, Result);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

void UUser::ReauthenticateWithProvider(const FFederatedAuthProvider& Provider, const FSignInCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound, FSignInResult()));

	if (!Provider.AuthProvider)
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Passed an invalid Provider to Reauthenticate With Provider."));
		Callback.ExecuteIfBound(EFirebaseAuthError::InvalidCredential, FSignInResult());
		return;
	}

	User->ReauthenticateWithProvider(Provider.AuthProvider.Get()).OnCompletion([Callback](const firebase::Future<firebase::auth::SignInResult>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to reauthenticate and retrieve data.");

		FSignInResult Result = FAuthHelper::ConvertSignInResult(Future.result());
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error, Result = MoveTemp(Result)]()-> void
		{
			Callback.ExecuteIfBound(Error, Result);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

void UUser::SendEmailVerification(const FUserVoidCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound));

	User->SendEmailVerification().OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to send email verification.");	
		
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error]()-> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable);
#endif
}

void UUser::UpdateUserProfile(const FUserProfile& Profile, const FUserVoidCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound));

	User->UpdateUserProfile(CONVERT_USER_PROFILE(&Profile)).OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to send email verification.");	
		
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error]()-> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable);
#endif
}

void UUser::LinkWithCredential(const FCredential& Credential, const FSignInUserCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound, nullptr));

	if (!Credential.Credential || !Credential.IsValid())
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Invalid credential passed to link with credential."));
		Callback.ExecuteIfBound(EFirebaseAuthError::InvalidCredential, nullptr);
		return;
	}

	User->LinkWithCredential(*Credential.Credential).OnCompletion([Callback](const firebase::Future<firebase::auth::User*>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to link with credential.");

		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error, Future]()-> void
		{
			UUser* NewUser = nullptr;
				
			if (Future.error() == (int32)EFirebaseAuthError::None && Future.result())
			{
				NewUser = NewObject<UUser>();
				NewUser->User = *Future.result();
			}

			Callback.ExecuteIfBound(Error, NewUser);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

void UUser::LinkAndRetrieveDataWithCredential(const FCredential& Credential, const FSignInCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound, FSignInResult()));

	if (!Credential.Credential || !Credential.IsValid())
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Invalid credential passed to link and retrieve data with credential."));
		Callback.ExecuteIfBound(EFirebaseAuthError::InvalidCredential, FSignInResult());
		return;
	}

	User->LinkAndRetrieveDataWithCredential(*Credential.Credential).OnCompletion([Callback](const firebase::Future<firebase::auth::SignInResult>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to link and retrieve data with credential.");

		FSignInResult Result = FAuthHelper::ConvertSignInResult(Future.result());
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error, Result = MoveTemp(Result)]()-> void
		{
			Callback.ExecuteIfBound(Error, Result);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

void UUser::LinkWithProvider(const FFederatedAuthProvider& Provider, const FSignInCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound, FSignInResult()));

	if (!Provider.AuthProvider)
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Passed an invalid provider to link with provider."));
		Callback.ExecuteIfBound(EFirebaseAuthError::InvalidCredential, FSignInResult());
		return;
	}

	User->LinkWithProvider(Provider.AuthProvider.Get()).OnCompletion([Callback](const firebase::Future<firebase::auth::SignInResult>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to link with provider and retrieve data with credential.");

		FSignInResult Result = FAuthHelper::ConvertSignInResult(Future.result());
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error, Result = MoveTemp(Result)]()-> void
		{
			Callback.ExecuteIfBound(Error, Result);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

void UUser::Unlink(const FString& Provider, const FSignInUserCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound, nullptr));

	User->Unlink(TCHAR_TO_UTF8(*Provider)).OnCompletion([Callback](const firebase::Future<firebase::auth::User*>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to unlink provider.");
		
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error, Future]()-> void
		{
			UUser* NewUser = nullptr;
				
			if (Future.error() == (int32)EFirebaseAuthError::None && Future.result())
			{
				NewUser = NewObject<UUser>();
				NewUser->User = *Future.result();
			}

			Callback.ExecuteIfBound(Error, NewUser);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

void UUser::UpdatePhoneNumberCredential(const FCredential& Credential, const FSignInUserCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound, nullptr));

	if (!Credential.Credential || !Credential.IsValid())
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Invalid credential passed to link and retrieve data with credential."));
		Callback.ExecuteIfBound(EFirebaseAuthError::InvalidCredential, nullptr);
		return;
	}

	User->UpdatePhoneNumberCredential(*Credential.Credential).OnCompletion([Callback](const firebase::Future<firebase::auth::User*>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to update phone number credential.");
		
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error, Future]()-> void
		{
			UUser* NewUser = nullptr;
				
			if (Future.error() == (int32)EFirebaseAuthError::None && Future.result())
			{
				NewUser = NewObject<UUser>();
				NewUser->User = *Future.result();
			}

			Callback.ExecuteIfBound(Error, NewUser);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

void UUser::Reload(const FAuthCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound));

	User->Reload().OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to reload user.");
		
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error]()-> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable);
#endif
}

void UUser::Delete(const FAuthCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	USER_CHECK_VALIDITY((void)Callback.ExecuteIfBound(EFirebaseAuthError::UserNotFound));

	User->Delete().OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		LOG_FUTURE_ERROR("Failed to delete user.");
		
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		AsyncTask(ENamedThreads::GameThread, [Callback, Error]()-> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable);
#endif
}

FUserMetadata UUser::Metadata() const
{
	FUserMetadata Meta;

#if WITH_FIREBASE_AUTH
	if (User)
	{
		const auto RawMeta = User->metadata();

		Meta.CreationTtimestamp  = RawMeta.creation_timestamp;
		Meta.LastSignInTimestamp = RawMeta.last_sign_in_timestamp;
	}
#endif

	return Meta;
}

bool UUser::IsEmailVerified() const
{
#if WITH_FIREBASE_AUTH
	return User ? User->is_email_verified() : false;
#else
	return false;
#endif
}

bool UUser::IsAnonymous() const
{
#if WITH_FIREBASE_AUTH
	return User ? User->is_anonymous() : false;
#else
	return false;
#endif
}

FString UUser::Uid()			const
{
#if WITH_FIREBASE_AUTH
	return User ? UTF8_TO_TCHAR(User->uid().c_str()) : TEXT("");
#else
	return {};
#endif
}

FString UUser::Email()			const
{
#if WITH_FIREBASE_AUTH
	return User ? UTF8_TO_TCHAR(User->email().c_str()) : TEXT("");
#else
	return {};
#endif
}

FString UUser::DisplayName()	const
{
#if WITH_FIREBASE_AUTH
	return User ? UTF8_TO_TCHAR(User->display_name().c_str()) : TEXT("");
#else
	return {};
#endif
}

FString UUser::PhotoUrl()		const
{
#if WITH_FIREBASE_AUTH
	return User ? UTF8_TO_TCHAR(User->photo_url().c_str()) : TEXT("");
#else
	return {};
#endif
}

FString UUser::ProviderId()		const
{
#if WITH_FIREBASE_AUTH
	return User ? UTF8_TO_TCHAR(User->provider_id().c_str()) : TEXT("");
#else
	return {};
#endif
}

FString UUser::PhoneNumber()	const
{
#if WITH_FIREBASE_AUTH
	return User ? UTF8_TO_TCHAR(User->phone_number().c_str()) : TEXT("");
#else
	return {};
#endif
}


#undef USER_CHECK_VALIDITY
#undef LOG_FUTURE_ERROR
