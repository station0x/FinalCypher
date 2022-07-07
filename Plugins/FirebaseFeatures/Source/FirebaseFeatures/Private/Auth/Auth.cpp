// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Auth/Auth.h"

#if WITH_FIREBASE_AUTH
THIRD_PARTY_INCLUDES_START
#	include "firebase/auth.h"
THIRD_PARTY_INCLUDES_END
#endif

#include "Auth/AuthHelper.h"
#include "Auth/User.h"
#include "Async/Async.h"

#include "FirebaseFeatures.h"

#if PLATFORM_ANDROID
#	include "Android/FirebaseAndroidHelper.h"
#endif

#if WITH_FIREBASE_AUTH
FFederatedAuthProvider::FFederatedAuthProvider(TSharedPtr<firebase::auth::FederatedOAuthProvider> Provider)
	: AuthProvider(Provider)
{
}
#endif

FFederatedAuthProvider::FFederatedAuthProvider()
{
}

FFederatedAuthProvider::FFederatedAuthProvider(const FFederatedAuthProvider& Other)
#if WITH_FIREBASE_AUTH
	: AuthProvider(Other.AuthProvider)
#endif
{
}

FFederatedAuthProvider::FFederatedAuthProvider(FFederatedAuthProvider&& Other)
#if WITH_FIREBASE_AUTH
	: AuthProvider(MoveTemp(Other.AuthProvider))
#endif
{
}

FFederatedAuthProvider& FFederatedAuthProvider::operator=(const FFederatedAuthProvider& Other)
{
#if WITH_FIREBASE_AUTH
	AuthProvider = Other.AuthProvider;
#endif
	return *this;
}

FFederatedAuthProvider& FFederatedAuthProvider::operator=(FFederatedAuthProvider&& Other)
{
#if WITH_FIREBASE_AUTH
	AuthProvider = MoveTemp(Other.AuthProvider);
#endif
	return *this;
}

FFederatedAuthProvider::~FFederatedAuthProvider()
{
}

FCredential::FCredential()
#if WITH_FIREBASE_AUTH
	: Credential(MakeUnique<firebase::auth::Credential>())
#endif
{
}

#if WITH_FIREBASE_AUTH
FCredential::FCredential(const firebase::auth::Credential* InCredential)
	: Credential(MakeUnique<firebase::auth::Credential>(InCredential ? *InCredential : firebase::auth::Credential{}))
{
}
#endif

#if WITH_FIREBASE_AUTH
FCredential::FCredential(firebase::auth::Credential InCredential)
	: Credential(MakeUnique<firebase::auth::Credential>(MoveTemp(InCredential)))
{

}
#endif

FCredential::FCredential(FCredential&& Other)
{
	*this = MoveTemp(Other);
}

FCredential::FCredential(const FCredential& Other) 
{
	*this = Other;
}

FCredential& FCredential::operator=(const FCredential& Other)
{
#if WITH_FIREBASE_AUTH
	if (Credential)
	{
		*Credential = *Other.Credential;
	}
	else
	{
		Credential = MakeUnique<firebase::auth::Credential>(*Other.Credential);
	}
#endif
	return *this;
}

FCredential& FCredential::operator=(FCredential&& Other)
{
#if WITH_FIREBASE_AUTH
	Credential = MoveTemp(Other.Credential);
#endif
	return *this;
}

FCredential::~FCredential()
{
}

FString FCredential::Provider() const
{
#if WITH_FIREBASE_AUTH
	return Credential ? UTF8_TO_TCHAR(Credential->provider().c_str()) : TEXT("");
#else
	return {};
#endif
}

bool FCredential::IsValid() const
{
#if WITH_FIREBASE_AUTH
	return Credential ? Credential->is_valid() : false;
#else
	return false;
#endif
}

#if WITH_FIREBASE_AUTH
firebase::auth::Auth* FAuth::GetAuth()
{
	firebase::auth::Auth* const Auth = firebase::auth::Auth::GetAuth(FFirebaseFeaturesModule::GetCurrentFirebaseApp());

	checkf(Auth != nullptr, TEXT("Failed to get auth."));

	return Auth;
}
#endif

UUser* FAuth::CurrentUser()
{
#if WITH_FIREBASE_AUTH
	firebase::auth::User* const Current = GetAuth()->current_user();
	if (!Current)
	{
		return nullptr;
	}

	UUser* const User = NewObject<UUser>();
	User->User = Current;

	return User;
#else
	return nullptr;
#endif
}

void FAuth::FetchProvidersForEmail(const FString& Email, const FFetchProvidersForEmailCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	GetAuth()->FetchProvidersForEmail(TCHAR_TO_UTF8(*Email)).OnCompletion([Callback](const firebase::Future<firebase::auth::Auth::FetchProvidersResult>& Future) -> void
	{
		const EFirebaseAuthError ErrorCode = (EFirebaseAuthError)Future.error();
		
		
		if (ErrorCode != EFirebaseAuthError::None)
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to fetch providers for email: Code %s. Message: %s"),
				ErrorCode, UTF8_TO_TCHAR(Future.error_message()));
		}
		
		if (Callback.IsBound())
		{
			TArray<FString> Providers;
			if (ErrorCode == EFirebaseAuthError::None)
			{
				for (const auto& Provider : Future.result()->providers)
				{
					Providers.Emplace(UTF8_TO_TCHAR(Provider.c_str()));
				}
			}

			AsyncTask(ENamedThreads::GameThread, [Callback, ErrorCode, Providers]() -> void
			{
				Callback.ExecuteIfBound(ErrorCode, Providers);
			});
		}
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

void FAuth::SignInWithCustomToken(const FString& Token, const FSignInUserCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	GetAuth()->SignInWithCustomToken(TCHAR_TO_UTF8(*Token)).OnCompletion([Callback](const firebase::Future<firebase::auth::User*>& Future) -> void
	{
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		if (Error != EFirebaseAuthError::None)
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to sign in with custom token. Code: %d. Message: %s"), Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Callback, Future]() -> void
			{
				UUser* User = nullptr;

				if (Future.result() && *Future.result())
				{
					User = NewObject<UUser>();
					User->User = *Future.result();
				}

				Callback.ExecuteIfBound((EFirebaseAuthError)Future.error(), User);
			});
		}
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, nullptr);
#endif
}

void FAuth::SignInWithCredential(const FCredential& Credential, const FSignInUserCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	if (!Credential.IsValid())
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Tried to sign in with invalid credentials."));
		Callback.ExecuteIfBound(EFirebaseAuthError::InvalidCredential, nullptr);
		return;
	}

	GetAuth()->SignInWithCredential(*Credential.Credential).OnCompletion([Callback](const firebase::Future<firebase::auth::User*>& Future) -> void
	{
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();

		if (Error != EFirebaseAuthError::None)
		{
			const FString ErrorMessage = FString::Printf(TEXT("Failed to sign in with credential. Code: %d. Message: %s"), 
				Future.error(), UTF8_TO_TCHAR(Future.error_message()));

			UE_LOG(LogFirebaseAuth, Error, TEXT("%s"), *ErrorMessage);

#if PLATFORM_ANDROID
			FirebaseUtils::NativeLog(FBLog_Error, ErrorMessage);
#endif
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Callback, Future]() -> void
			{
				UUser* User = nullptr;

				if (Future.result() && *Future.result())
				{
					User = NewObject<UUser>();
					User->User = *Future.result();
				}

				Callback.ExecuteIfBound((EFirebaseAuthError)Future.error(), User);
			});
		}
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, nullptr);
#endif
}

void FAuth::SignInWithProvider(const FFederatedAuthProvider& AuthProvider, const FSignInCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	if (!AuthProvider.AuthProvider)
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Passed and invalid AuthProvider to sign in with provider."));
		Callback.ExecuteIfBound(EFirebaseAuthError::NoSuchProvider, FSignInResult());
		return;
	}

	if (!GetAuth())
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Invalid Auth."));
		return;
	}

	GetAuth()->SignInWithProvider(AuthProvider.AuthProvider.Get()).OnCompletion([Callback](const firebase::Future<firebase::auth::SignInResult>& Future) -> void
	{
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();
		if (Error != EFirebaseAuthError::None)
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to sign in with provider. Code: %d. Message: %s"), Future.error(), UTF8_TO_TCHAR(Future.error_message()))
		}

		firebase::auth::SignInResult RawResult;
		
		if (Future.result())
		{
			RawResult = *Future.result();
		}

		AsyncTask(ENamedThreads::GameThread, [Callback, Error, RawResult = MoveTemp(RawResult)]() -> void
		{
			Callback.ExecuteIfBound(Error, FAuthHelper::ConvertSignInResult(&RawResult));
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

void FAuth::SignInAndRetrieveDataWithCredential(const FCredential& Credential, const FSignInCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	if (!Credential.Credential)
	{
		UE_LOG(LogFirebaseAuth, Error, TEXT("Passed an invalid credential to sign in and retrieve data with credential."));
		Callback.ExecuteIfBound(EFirebaseAuthError::InvalidCredential, FSignInResult());
		return;
	}

	GetAuth()->SignInAndRetrieveDataWithCredential(*Credential.Credential).OnCompletion([Callback](const firebase::Future<firebase::auth::SignInResult>& Future) -> void
	{
		if (Future.error() != (int32)EFirebaseAuthError::None)
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to sign in and retrieve data with credential. Code: %d. Message: %s"), Future.error(), UTF8_TO_TCHAR(Future.error_message()))
		}

		AsyncTask(ENamedThreads::GameThread, [Callback, Future]()->void
		{
			Callback.ExecuteIfBound((EFirebaseAuthError)Future.error(), FAuthHelper::ConvertSignInResult(Future.result() ? Future.result() : nullptr));
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, {});
#endif
}

void FAuth::SignInAnonymously(const FSignInUserCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	GetAuth()->SignInAnonymously().OnCompletion([Callback](const firebase::Future<firebase::auth::User*> Future) -> void
	{
		if (Future.error() != (int32)EFirebaseAuthError::None)
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to sign in anonymously. Code: %d. Message: %s"), Future.error(), UTF8_TO_TCHAR(Future.error_message()));
		}
		
		AsyncTask(ENamedThreads::GameThread, [Callback, Future]()->void
		{
			Callback.ExecuteIfBound((EFirebaseAuthError)Future.error(), FAuthHelper::ConvertUser(Future.result() ? *Future.result() : nullptr));
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, nullptr);
#endif
}

void FAuth::SignInWithEmailAndPassword(const FString& Email, const FString& Password, const FSignInUserCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	GetAuth()->SignInWithEmailAndPassword(TCHAR_TO_UTF8(*Email), TCHAR_TO_UTF8(*Password)).OnCompletion([Callback](const firebase::Future<firebase::auth::User*> Future) -> void
	{
		if (Future.error() != (int32)EFirebaseAuthError::None)
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to sign in with email and password. Code: %d. Message: %s"), Future.error(), UTF8_TO_TCHAR(Future.error_message()));
		}
		
		AsyncTask(ENamedThreads::GameThread, [Callback, Future]() -> void
		{
			Callback.ExecuteIfBound((EFirebaseAuthError)Future.error(), FAuthHelper::ConvertUser(Future.result() ? *Future.result() : nullptr));
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, nullptr);
#endif
}

void FAuth::CreateUserWithEmailAndPassword(const FString& Email, const FString& Password, const FSignInUserCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	GetAuth()->CreateUserWithEmailAndPassword(TCHAR_TO_UTF8(*Email), TCHAR_TO_UTF8(*Password)).OnCompletion([Callback](const firebase::Future<firebase::auth::User*> Future) -> void
	{
		if (Future.error() != (int32)EFirebaseAuthError::None)
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to sign in with email and password. Code: %d. Message: %s"), Future.error(), UTF8_TO_TCHAR(Future.error_message()));
		}
		
		AsyncTask(ENamedThreads::GameThread, [Callback, Future]() -> void
		{
			Callback.ExecuteIfBound((EFirebaseAuthError)Future.error(), FAuthHelper::ConvertUser(Future.result() ? *Future.result() : nullptr));
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable, nullptr);
#endif
}

void FAuth::SignOut()
{
#if WITH_FIREBASE_AUTH
	GetAuth()->SignOut();
#endif
}

void FAuth::SendPasswordResetEmail(const FString& Email, const FAuthCallback& Callback)
{
#if WITH_FIREBASE_AUTH
	GetAuth()->SendPasswordResetEmail(TCHAR_TO_UTF8(*Email)).OnCompletion([Callback](const firebase::Future<void> Future) -> void
	{
		const EFirebaseAuthError Error = (EFirebaseAuthError)Future.error();
		if (Error != EFirebaseAuthError::None)
		{
			UE_LOG(LogFirebaseAuth, Error, TEXT("Failed to send password reset email. Code: %d. Message: %s"), Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else
	Callback.ExecuteIfBound(EFirebaseAuthError::ApiNotAvailable);
#endif
}

FFederatedAuthProvider FAuth::CreateFederatedOAuthProvider(const FFederatedOAuthProviderData& ProviderData)
{
#if WITH_FIREBASE_AUTH
	firebase::auth::FederatedOAuthProviderData Data;

	for (const FString& Scope : ProviderData.Scopes)
	{
		Data.scopes.push_back(TCHAR_TO_UTF8(*Scope));
	}

	for (const auto& Parameter : ProviderData.CustomParameters)
	{
		Data.custom_parameters.insert({ TCHAR_TO_UTF8(*Parameter.Key), TCHAR_TO_UTF8(*Parameter.Key) });
	}

	Data.provider_id = TCHAR_TO_UTF8(*ProviderData.ProviderId);

	TSharedPtr<firebase::auth::FederatedOAuthProvider> Provider = MakeShared<firebase::auth::FederatedOAuthProvider>();

	Provider->SetProviderData(Data);

	return FFederatedAuthProvider(Provider);
#else
	return {};
#endif
}


